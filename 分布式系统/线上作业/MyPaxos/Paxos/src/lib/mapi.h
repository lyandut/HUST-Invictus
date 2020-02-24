// mapi.h: interface for the mapi class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MDK_MAPI_H
#define MDK_MAPI_H

#include <string>
#include "FixLengthInt.h"

namespace mdk
{
	//断言
	void mdk_assert( bool isTrue );
	//睡眠
	void m_sleep( long lMillSecond );
	//地址保存到int64
	bool addrToI64(uint64 &addr64, const char* ip, int port);
	//int64解析出地址
	void i64ToAddr(char* ip, int &port, uint64 addr64);
	//压缩del包含的字符
	void TrimString( std::string &str, std::string del );
	//压缩右del包含的字符
	void TrimStringLeft( std::string &str, std::string del );
	//压缩左del包含的字符
	void TrimStringRight( std::string &str, std::string del );
	//压缩空白字符
	char* Trim( char *str );
	//压缩右空白字符
	char* TrimRight( char *str );
	//压缩左空白字符
	char* TrimLeft( char *str );
	//字节高低位顺序翻转
	int reversal(int i);
	//取得文件大小
	unsigned long GetFileSize(const char *filename);
	//获取CPU数量
	//maxCpu可能的最大cpu数，如果检测出cpu数量大于此值，则表示检测有误，将返回defaultCpuNumber
	//defaultCpuNumber默认cpu数量，当检测出错时，将认为cpu数量为此值
	unsigned int GetCUPNumber( int maxCpu, int defaultCpuNumber );
	uint64 CurThreadId();//当前线程id
	time_t mdk_Date();//返回0时0分0秒的当前日期
	bool GetExeDir( char *exeDir, int size );//取得可执行程序位置
	mdk::uint64 MillTime();//毫秒级当前时间
}

#endif // !defined MDK_MAPI_H 
