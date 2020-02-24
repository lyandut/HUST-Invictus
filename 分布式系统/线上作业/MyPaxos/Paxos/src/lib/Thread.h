// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////
/*
	线程类
	※fun不能是虚函数

	使用方法
	void* g_fun(void*)
	{
	return NULL;
	}
	class A
	{
		...
		void* RemoteCall fun(void*);
	}
	A a;
	
	mdk::Thread t;
	//linux下必须&A::fun，windows下可使用A::fun
	//为了移植性，建议传递&A::fun给Bind
	t.Run( mdk::Executor::Bind(&A::fun), &a, (void*)param );
	mdk::Thread t2;
	t2.Run( g_fun, (void*)param );
*/
#ifndef MDK_THREAD_H
#define MDK_THREAD_H

#include "FixLengthInt.h"
#include "Task.h"

#ifdef WIN32
//为了不include <windows.h>
typedef void *HANDLE;
typedef unsigned long       DWORD;
#else
#include <pthread.h>
#include <signal.h>
#endif

namespace mdk
{
	
#ifdef WIN32
	typedef DWORD ThreadID;
#else
	typedef pthread_t ThreadID;
#endif
	
//简单线程类
class Thread  
{
public:
	Thread();
	virtual ~Thread();

//////////////////////////////////////////////////////////////////////////
protected://属性
	ThreadID m_nID;	
	Task m_task;
	bool m_bRun;

	//线程退出控制属性
#ifdef WIN32
	HANDLE m_hHandle;
#else
	pthread_cond_t m_exit;
	pthread_mutex_t m_exitMutex;
	bool m_bStop;
#endif
	//End线程退出控制属性
	//////////////////////////////////////////////////////////////////////////
public://接口
	bool Run( MethodPointer method, void *pObj, void *pParam );//启动线程，使用类成员函数，pParam传递给线程函数的参数
	bool Run( FuntionPointer fun, void *pParam );//启动线程，使用全局函数，pParam传递给线程函数的参数
	/*
	 	停止
	 */
	void Stop(long lMillSecond = 0);
	/*
	 	等待线程停止
		注：linux下一旦返回，之后再调用WaitStop()与Stop();将不再等待，直接返回
		因为WaitStop();返回前会发送1次线程结束信号，通知其它可能存在等待的线程
	 */
	void WaitStop();
	ThreadID GetID();//取得线程ID

protected:
	bool Run( void *pParam );//启动线程，pParam传递给线程函数的参数
	static void* TMain(void*);
	void* Main();
};

}//namespace mdk

#endif // MDK_THREAD_H
