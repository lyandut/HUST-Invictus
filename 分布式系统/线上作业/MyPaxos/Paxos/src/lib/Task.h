#ifndef TOOL_C_TASK_H
#define TOOL_C_TASK_H

#include "Executor.h"

/**
	任务类
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

	mdk::Task t;
	//linux下必须&A::fun，windows下可使用A::fun
	//为了移植性，建议传递&A::fun给Bind()
	t.Accept( mdk::Executor::Bind(&A::fun), &a, (void*)param );
	t.Execute();

	t.Accept( g_fun, (void*)param );
	t.Execute();
*/

namespace mdk
{
class Task
{
public:
	Task(int i);
	Task();
	virtual ~Task();
	//接受任务
	//method为声明为void* fun(void*)的成员函数
	void Accept( MethodPointer method, void *pObj, void *pParam );
	//接受任务
	//fun为声明为void* fun(void*)的函数
	void Accept( FuntionPointer fun, void *pParam );
	//执行任务
	void* Execute();
	
private:
	void *m_pParam;
	MethodPointer m_method;
	void *m_pObj;
	FuntionPointer m_fun;
};

}//namespace mdk

#endif
