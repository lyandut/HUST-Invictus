#include <stdlib.h>
#include <stdio.h>
#include "Paxos/Acceptor.h"
#include "Paxos/Proposer.h"
#include "lib/Thread.h"
#include "lib/Lock.h"
#include "lib/mapi.h"
#include "lib/atom.h"
#include "lib/Logger.h"

paxos::Proposer p[5];
paxos::Acceptor a[11];
mdk::Mutex l[11];
int finishedCount = 0;
int finalValue = -1;
bool isFinished = false;
mdk::uint64 g_start;
mdk::Logger g_log;

void* Proposer(void *id)
{
	mdk::Logger log;
	char logName[256];
	sprintf( logName, "Proposer%d", (long)id );
	log.SetLogName(logName);
	log.SetMaxLogSize(10);
	log.SetMaxExistDay(30);
	log.SetPrintLog(false);

	paxos::Proposer &proposer = p[(long)id];
	paxos::PROPOSAL value = proposer.GetProposal();
	paxos::PROPOSAL lastValue;


	int acceptorId[11];
	int count = 0;

	mdk::uint64 start = mdk::MillTime();
	while ( true )
	{
		value = proposer.GetProposal();//拿到提议
		log.Info("Info", "Proposer%d号开始(Propose阶段):提议=[编号:%d，提议:%d]\n", 
			(long)id, value.serialNum, value.value);
		count = 0;
		int i = 0;
		for (i = 0; i < 11; i++ )
		{
		/*
			* 发送消息到第i个acceptor
			* 经过一定时间达到acceptor，sleep(随机时间)模拟
			* acceptor处理消息，mAcceptors[i].Propose()
			* 回应proposer
			* 经过一定时间proposer收到回应，sleep(随机时间)模拟
			* proposer处理回应mProposer.proposed(ok, lastValue)
		*/
			mdk::m_sleep(rand()%500);//经过随机时间，消息到达了mAcceptors[i]
			//处理消息
			l[i].Lock();
			bool ok = a[i].Propose(value.serialNum, lastValue);
			l[i].Unlock();
			mdk::m_sleep(rand()%500);//经过随机时间,消息到达Proposer
			//处理Propose回应
			if ( !proposer.Proposed(ok, lastValue) ) //重新开始Propose阶段
			{
				mdk::m_sleep(1000);//为了降低活锁，多等一会让别的proposer有机会完成自己的2阶段批准
				break;
			}
			paxos::PROPOSAL curValue = proposer.GetProposal();//拿到提议
			if ( curValue.value != value.value )//acceptor本次回应可能推荐了一个提议
			{
				log.Info("Info", "Proposer%d号修改了提议:提议=[编号:%d，提议:%d]\n", 
					(long)id, curValue.serialNum, curValue.value);
				break;
			}
			acceptorId[count++] = i;//记录愿意投票的acceptor
			if ( proposer.StartAccept() ) break;
		}
		//检查有没有达到Accept开始条件，如果没有表示要重新开始Propose阶段
		if ( !proposer.StartAccept() ) continue;

		//开始Accept阶段
		//发送Accept消息到所有愿意投票的acceptor
		value = proposer.GetProposal();
		log.Info("Info", "Proposer%d号开始(Accept阶段):提议=[编号:%d，提议:%d]\n", 
			(long)id, value.serialNum, value.value);
		for (i = 0; i < count; i++ )
		{
			//发送accept消息到acceptor
			//减少accept阶段等待时间，加快收敛
			mdk::m_sleep(rand()%200);//经过随机时间,accept消息到达acceptor
			//处理accept消息
			l[acceptorId[i]].Lock();
			bool ok = a[acceptorId[i]].Accept(value);
			l[acceptorId[i]].Unlock();
			mdk::m_sleep(rand()%200);//经过随机时间,accept回应到达proposer
			//处理accept回应
			if ( !proposer.Accepted(ok) ) //重新开始Propose阶段
			{
				mdk::m_sleep(1000);//为了降低活锁，多等一会让别的proposer有机会完成自己的2阶段批准
				break;
			}
			if ( proposer.IsAgree() )//成功批准了提议
			{
				start = mdk::MillTime() - start;
				log.Info("Info", "Proposer%d号的提议被批准,用时%lluMS:最终提议 = [编号:%d，提议:%d]\n", (long)id, start, value.serialNum, value.value);
				g_log.Info("Info", "Proposer%d号的提议被批准,用时%lluMS:最终提议 = [编号:%d，提议:%d]\n", (long)id, start, value.serialNum, value.value);
				if(finalValue == -1) finalValue = value.value;
				else if(finalValue != value.value) finalValue = 0;
				if ( 4 == mdk::AtomAdd(&finishedCount, 1) )
				{
					isFinished = true;
					g_start = mdk::MillTime() - g_start;
					if(finalValue > 0){
						g_log.Info("Info", "Paxos完成，用时%lluMS，最终通过提议值为：%d\n", g_start, finalValue);
					}
					else{
						g_log.Info("Info", "Paxos完成，用时%lluMS，最终结果不一致！\n", g_start);
					}
				}
				return NULL;
			}
		}
	}
	return NULL;
}

//Paxos过程模拟演示程序
int main(int argc, char* argv[])
{
	int i = 0;
	g_log.SetLogName("Paxos");
	g_log.SetMaxLogSize(10);
	g_log.SetMaxExistDay(30);
	g_log.SetPrintLog(true);
	g_log.Info("Info", "5个Proposer, 11个Acceptor准备进行Paxos\n"
		"每个Proposer独立线程，Acceptor不需要线程\n"
		"Proposer编号从0-10,编号为i的Proposer初始提议编号和提议值是（i+1, i+1）\n"
		"Proposer每次重新提议会将提议编号增加5\n"
		"Proposer被批准后结束线程,其它线程继续投票最终，全部批准相同的值，达成一致。\n");
	g_start = mdk::MillTime();
	g_log.Info("Info", "Paxos开始\n" );
	paxos::PROPOSAL value;

	for ( i = 0; i < 5; i++ ) 
	{
		p[i].SetPlayerCount(5, 11);
		value.serialNum = value.value = i + 1;
		p[i].StartPropose(value);
	}

	mdk::Thread t[5];
	for ( i = 0; i < 5; i++ ) t[i].Run(Proposer, (void*)i);
	//for ( i = 0; i < 5; i++ ) t[i].WaitStop();
	while(true){
		if(isFinished) break;
		mdk::m_sleep(500);
	}
	return 0;
}
