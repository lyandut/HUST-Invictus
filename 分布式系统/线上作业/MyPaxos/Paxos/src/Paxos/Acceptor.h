#ifndef ACCTPTOR_H
#define ACCTPTOR_H

#include "PaxosData.h"
namespace paxos
{

//投票者
class Acceptor
{
public:
	Acceptor(void);
	virtual ~Acceptor(void);

	//同意投票
	bool Propose(unsigned int serialNum, PROPOSAL &lastAcceptValue);
	//接受提议
	bool Accept(PROPOSAL &value);

private:
	PROPOSAL		m_lastAcceptValue;//最后接受的提议
	unsigned int	m_maxSerialNum;//Propose提交的最大流水号
};

}

#endif //ACCTPTOR_H