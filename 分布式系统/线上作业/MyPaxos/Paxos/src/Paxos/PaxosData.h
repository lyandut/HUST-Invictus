#ifndef PAXOS_DATA_H
#define PAXOS_DATA_H

namespace paxos
{
	//提议数据结构
	typedef struct PROPOSAL
	{
		unsigned int	serialNum;//流水号,1开始递增，保证全局唯一
		unsigned int	value;//提议内容
	}PROPOSAL;
}

#endif //PAXOS_DATA_H