// Thread.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////
#include "Thread.h"
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif

namespace mdk
{

Thread::Thread()
{
	m_bRun = false;
#ifdef WIN32
	m_hHandle = NULL;
#else
	m_bStop = true;
	int nError = 0;
	pthread_mutexattr_t mutexattr;
	if ( 0 != (nError = pthread_mutexattr_init( &mutexattr )) ) return ;
	if ( 0 != (nError = pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_TIMED_NP )) ) return ;
	if ( 0 != (nError = pthread_mutex_init( &m_exitMutex,  &mutexattr )) ) return;
	if ( 0 != (nError = pthread_mutexattr_destroy( &mutexattr )) ) return ;
	pthread_cond_init( &m_exit, NULL );
#endif
}

Thread::~Thread()
{
	Stop();
#ifndef WIN32
	pthread_cond_destroy( &m_exit );
	pthread_mutex_destroy( &m_exitMutex );
#endif	
}

//启动线程，使用全局函数，pParam传递给线程函数的参数
bool Thread::Run( FuntionPointer fun, void *pParam )
{
	if ( m_bRun ) return false;
	m_task.Accept(fun, pParam);
	m_bRun = Run(NULL);
	
	return m_bRun;
}

//启动线程，使用类成员函数，pParam传递给线程函数的参数
bool Thread::Run( MethodPointer method, void *pObj, void *pParam )
{
	if ( m_bRun ) return false;
	m_task.Accept(method, pObj, pParam);
	m_bRun = Run(NULL);

	return m_bRun;
}

//启动
bool Thread::Run( void *pParam )
{
#ifdef WIN32
	m_hHandle = CreateThread( 
		NULL, 0, 
		(LPTHREAD_START_ROUTINE)TMain,
		(LPVOID)this, 0, &m_nID );
	if ( NULL == m_hHandle ) return false;
#else
	m_bStop = false;
	// 分离状态的线程，结束时不需要pthread_join
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create( &m_nID, &attr, TMain, this );
	pthread_attr_destroy (&attr);
#endif

	return true;
}

void* Thread::TMain(void* param)
{
	Thread *pThis = (Thread*)param;
	
	return pThis->Main();
}

void* Thread::Main()
{
	void *ret = NULL;
	ret = m_task.Execute();
#ifndef WIN32
	m_bStop = true;
	pthread_cond_broadcast(&m_exit);
#endif
	m_bRun = false;

	return ret;
}

//停止
void Thread::Stop( long lMillSecond )
{
#ifdef WIN32
	if ( NULL == m_hHandle ) return;
	if ( WAIT_OBJECT_0 != WaitForSingleObject( m_hHandle, lMillSecond ) )
	{
		TerminateThread( m_hHandle, 0 );
	}
	CloseHandle(m_hHandle);
	m_hHandle = NULL;
#else
	if ( m_bStop ) 
	{
		m_bRun = false;
		return;
	}
	if ( 0 > lMillSecond ) lMillSecond = 3;
	int nSecond = lMillSecond / 1000;
	int nNSecond = (lMillSecond - nSecond * 1000) * 1000;
	timespec timeout;
	timeout.tv_sec=time(NULL) + nSecond;         
	timeout.tv_nsec=nNSecond;
	pthread_mutex_lock( &m_exitMutex );
	if ( 0 != pthread_cond_timedwait(&m_exit, &m_exitMutex, &timeout) )
	{
		pthread_kill(m_nID, 1);
	}
	pthread_mutex_unlock( &m_exitMutex );
#endif
	m_bRun = false;

	return;	
}

//等待线程停止
void Thread::WaitStop()
{
#ifdef WIN32
	WaitForSingleObject( m_hHandle, INFINITE );
#else
	pthread_mutex_lock( &m_exitMutex );
	pthread_cond_wait(&m_exit, &m_exitMutex);
	pthread_mutex_unlock( &m_exitMutex );
#endif
}

//取得ID
ThreadID Thread::GetID()
{
	return this->m_nID;
}

}//namespace mdk
