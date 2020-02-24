#ifndef PROPOSER_H
#define PROPOSER_H

#include <ctime>
#include "PaxosData.h"

namespace paxos
{

//提议者
class Proposer
{
public:
	Proposer();
	Proposer(short proposerCount, short acceptorCount);
	virtual ~Proposer();
	//设置参与者数量
	void SetPlayerCount(short proposerCount, short acceptorCount);
	//开始Propose阶段
	void StartPropose(PROPOSAL &value);
	//取得提议
	PROPOSAL& GetProposal();
	//提议被投票，Proposed失败则重新开始Propose阶段
	bool Proposed(bool ok, PROPOSAL &lastAcceptValue);
	//开始Accept阶段,满足条件成功开始accept阶段返回ture，不满足开始条件返回false
	bool StartAccept();
	//提议被接受，Accepted失败则重新开始Propose阶段
	bool Accepted(bool ok);
	//提议被批准
	bool IsAgree();

private:
	short			m_proposerCount;///proposer数量
	short			m_acceptorCount;//acceptor数量
	PROPOSAL		m_value;//预备提议
	bool			m_proposeFinished;//完成拉票，准备开始二阶段
	bool			m_isAgree;//m_value被批准
	unsigned int	m_maxAcceptedSerialNum;//已被接受的提议中流水号最大的
	time_t			m_start;//阶段开始时间，阶段一，阶段二共用
	short			m_okCount;//投票数量，阶段一，阶段二共用
	short			m_refuseCount;//拒绝数量，阶段一，阶段二共用
};

}

#endif //PROPOSER_H