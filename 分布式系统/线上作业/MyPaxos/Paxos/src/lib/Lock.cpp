// Lock.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////
#include "Lock.h"
#ifdef WIN32
#include <windows.h>
#endif

namespace mdk
{

//////////////////////////////////////////////////////////////////////////
//实现
AutoLock::AutoLock( Mutex *pLock )
:m_pMutex(pLock), m_bLocked(true)
{
	m_pMutex->Lock();
}
AutoLock::~AutoLock()
{
	Unlock();
}

void AutoLock::Unlock()
{
	if ( m_bLocked )
	{
		m_pMutex->Unlock();
		m_bLocked = false;
	}
}

Mutex::Mutex()
{
#ifdef WIN32
	InitializeCriticalSection( (CRITICAL_SECTION*)&m_mutex );
#else
	int kind = PTHREAD_MUTEX_RECURSIVE_NP;
	pthread_mutexattr_t mutexattr;//互斥量属性
	
	int nError = 0;
	if ( 0 != (nError = pthread_mutexattr_init( &mutexattr )) ) return;
	if ( 0 != (nError = pthread_mutexattr_settype( &mutexattr, kind )) ) return;
	if ( 0 != (nError = pthread_mutex_init( &m_mutex,  &mutexattr )) ) return;
	if ( 0 != (nError = pthread_mutexattr_destroy( &mutexattr )) ) return;
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	DeleteCriticalSection( (CRITICAL_SECTION*)&m_mutex );
#else
	pthread_mutex_destroy( &m_mutex );
#endif
}
 
void Mutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection( (CRITICAL_SECTION*)&m_mutex );
#else
	pthread_mutex_lock( &m_mutex );
#endif
}

void Mutex::Unlock()
{
#ifdef WIN32
	LeaveCriticalSection( (CRITICAL_SECTION*)&m_mutex );
#else
	pthread_mutex_unlock( &m_mutex );
#endif
}

}//namespace mdk
