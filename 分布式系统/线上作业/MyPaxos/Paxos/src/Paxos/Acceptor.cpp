#include "Acceptor.h"


namespace paxos
{


Acceptor::Acceptor(void)
{
	m_maxSerialNum = 0;
	m_lastAcceptValue.serialNum = 0;
	m_lastAcceptValue.value = 0;
}

Acceptor::~Acceptor(void)
{
}

bool Acceptor::Propose(unsigned int serialNum, PROPOSAL &lastAcceptValue)
{
	if ( 0 == serialNum ) return false;
	//提议不通过
	if ( m_maxSerialNum > serialNum ) return false;
	//接受提议
    //请完善下面逻辑

	/**********Begin**********/
	m_maxSerialNum = serialNum;
	lastAcceptValue = m_lastAcceptValue;
	/**********End**********/

	return true;
}

bool Acceptor::Accept(PROPOSAL &value)
{
	if ( 0 == value.serialNum ) return false;
	//Acceptor又重新答应了其他提议
   //请完善下面逻辑
	/**********Begin**********/
	if (m_maxSerialNum > value.serialNum) return false;
	/**********End**********/

    
	//批准提议通过
    //请完善下面逻辑
    /**********Begin**********/
	m_lastAcceptValue = value;
	/**********End**********/

	return true;
}

}
