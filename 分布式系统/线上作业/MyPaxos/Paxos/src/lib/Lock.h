// Lock.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MDK_LOCK_H
#define MDK_LOCK_H

#ifdef WIN32
#else
#include <pthread.h>
#endif

namespace mdk
{

#ifdef WIN32
//为了不包含windows.h,定义一个绝对大于CRITICAL_SECTION的buffer
typedef struct CS_BUFFER
{
	char	buffer[64];
}CS_BUFFER;
#define OriginalMutex CS_BUFFER
#else
	typedef pthread_mutex_t OriginalMutex;//互斥锁
#endif
	
//自解锁封装，推荐使用自解锁进行锁操作，避免异常，忘记解锁等不完全的锁操作发生
class Mutex;
class AutoLock
{
public:
	AutoLock( Mutex *pLock );
	~AutoLock();
	void Unlock();
	
private:
	Mutex *m_pMutex;
	bool m_bLocked;
};

//锁基本操作封装
class Mutex
{
private:
	//互斥锁系统对象
	OriginalMutex m_mutex;
public:
	Mutex();
	~Mutex();
	//加锁
	void Lock();
	//解锁
	void Unlock();		
};

}//namespace mdk

#endif//MDK_LOCK_H
