// mapi.cpp: implementation of the mapi class.
//
//////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "mapi.h"
#ifdef WIN32
#include <windows.h>
#include <sys/stat.h> 
#else
#include <unistd.h>
#include <sys/stat.h> 
#include <unistd.h>
#endif

#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

namespace mdk
{

void mdk_assert( bool isTrue )
{
	if ( isTrue ) return;
	char *p = NULL;
	*p = 1;
	exit(0);
}

void m_sleep( long lMillSecond )
{
#ifndef WIN32
	usleep( lMillSecond * 1000 );
#else
	Sleep( lMillSecond );
#endif
}
	
bool addrToI64(uint64 &addr64, const char* ip, int port)
{
	unsigned char addr[8];
	int nIP[4];
	sscanf(ip, "%d.%d.%d.%d", &nIP[0], &nIP[1], &nIP[2], &nIP[3]);
	addr[0] = nIP[0];
	addr[1] = nIP[1];
	addr[2] = nIP[2];
	addr[3] = nIP[3];
	char checkIP[25];
	sprintf(checkIP, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
	if ( 0 != strcmp(checkIP, ip) ) return false;
	memcpy( &addr[4], &port, 4);
	memcpy(&addr64, addr, 8);
	
	return true;
}

void i64ToAddr(char* ip, int &port, uint64 addr64)
{
	unsigned char addr[8];
	memcpy(addr, &addr64, 8);
	sprintf(ip, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
	memcpy(&port, &addr[4], 4);
}

void TrimString( string &str, string del )
{
	int nPos = 0;
	unsigned int i = 0;
	for ( ; i < del.size(); i++ )
	{
		while ( true )
		{
			nPos = str.find( del.c_str()[i], 0 );
			if ( -1 == nPos ) break;
			str.replace( str.begin() + nPos, str.begin() + nPos+1, "" );
		}
	}
}

void TrimStringLeft( string &str, string del )
{
	unsigned int i = 0;
	bool bTrim = false;
	for ( ; i < str.size(); i++ )
	{
		if ( string::npos == del.find(str.c_str()[i]) ) break;
		bTrim = true;
	}
	if ( !bTrim ) return;
	str.replace( str.begin(), str.begin() + i, "" );
}

void TrimStringRight( string &str, string del )
{
	int i = str.size() - 1;
	bool bTrim = false;
	for ( ; i >= 0; i-- )
	{
		if ( string::npos == del.find(str.c_str()[i]) ) break;
		bTrim = true;
	}
	if ( !bTrim ) return;
	str.replace( str.begin() + i + 1, str.end(), "" );
}

//压缩空白字符
char* Trim( char *str )
{
	if ( NULL == str || '\0' == str[0] ) return str;
	int i = 0;
	char *src = str;
	char strTemp[256];
	for ( i = 0; '\0' != *src ; src++ )
	{
		if ( ' ' == *src || '\t' == *src ) continue;
		strTemp[i++] = *src;
	}
	strTemp[i++] = '\0';
	strcpy( str, strTemp );
	return str;
}

//压缩空白字符
char* TrimRight( char *str )
{
	if ( NULL == str || '\0' == str[0] ) return str;
	int i = 0;
	char *src = str;
	for ( i = 0; '\0' != *src ; src++ )
	{
		if ( ' ' == *src || '\t' == *src ) 
		{
			i++;
			continue;
		}
		i = 0;
	}
	str[strlen(str) - i] = 0;
	return str;
}

//压缩空白字符
char* TrimLeft( char *str )
{
	if ( NULL == str || '\0' == str[0] ) return str;
	char *src = str;
	for ( ; '\0' != *src ; src++ )
	{
		if ( ' ' != *src && '\t' != *src ) break;
	}
	char strTemp[256];
	strcpy( strTemp, src );
	strcpy( str, strTemp );
	return str;
}

int reversal(int i)
{
	int out = 0;
	out = i << 24;
	out += i >> 24;
	out += ((i >> 8) << 24) >> 8;
	out += ((i >> 16) << 24) >> 16;
	return out;
}

unsigned long GetFileSize(const char *filename)  
{  
	unsigned long filesize = 0;      
	struct stat statbuff;  
	if(stat(filename, &statbuff) < 0) return filesize;  
	else  filesize = statbuff.st_size;  
	return filesize;  
}  

unsigned int GetCUPNumber(int maxCpu, int defaultCpuNumber)
{
#ifdef WIN32
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);
	int dwNumCpu = sysInfo.dwNumberOfProcessors;
	if ( dwNumCpu > maxCpu ) return defaultCpuNumber;
	return dwNumCpu;
#else
	unsigned int dwNumCpu = sysconf(_SC_NPROCESSORS_ONLN);
	if ( dwNumCpu > maxCpu ) return defaultCpuNumber;
	return dwNumCpu; 
#endif
}

#ifndef WIN32
#include <linux/unistd.h>
#include <sys/syscall.h>
/*
syscall(__NR_gettid)或者syscall(SYS_gettid)
SYS_gettid与__NR_gettid是相等的常量
在64bit=186，在32bit=224

命令行查看程序线程id方法
首先使用[pgrep 进程名]指令显示出主线程id
然后使用[ls /proc/主线程id/task/]显示所有子线程id
*/
#define gettid() syscall(__NR_gettid)
#endif

uint64 CurThreadId()
{
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return gettid();
#endif
}

//返回0时0分0秒的当前日期
time_t mdk_Date()
{
	time_t curTime = time(NULL);
	tm *pTm = localtime(&curTime);
	char hour[32];
	char mintue[32];
	char second[32];
	strftime( hour, 30, "%H", pTm );
	strftime( mintue, 30, "%M", pTm );
	strftime( second, 30, "%S", pTm );
	int sumSecond = atoi(hour) * 3600 + atoi(mintue) * 60 + atoi(second);
	curTime -= sumSecond;
	return curTime;
}

bool GetExeDir( char *exeDir, int size )
{
#ifdef WIN32
	TCHAR *pExepath = (TCHAR*)exeDir;
	GetModuleFileName(NULL, pExepath, size);
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( pExepath, drive, dir, fname, ext);
	_makepath( pExepath, drive, dir, NULL, NULL);
	return true;
#else
	int rval;
	char* last_slash;

	//读符号链接 /proc/self/exe 的目标
	rval = readlink ("/proc/self/exe", exeDir, size);
	if (rval == -1) //readlink调用失败
	{
		strcpy( exeDir, "./" );
		return false;
	}
	exeDir[rval] = '\0';
	last_slash = strrchr (exeDir, '/');
	if ( NULL == last_slash || exeDir == last_slash )//一些异常正在发生
	{
		strcpy( exeDir, "./" );
		return false;
	}
	size = last_slash - exeDir;
	exeDir[size] = '\0';
#endif
	return true;
}

mdk::uint64 MillTime()
{
#ifdef WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);

	FILETIME ft;
	SystemTimeToFileTime( &st, &ft );
	LONGLONG nLL;
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	nLL = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;
	mdk::int64 mt = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000);
	mt -= 3600 * 8 * 1000;//日历时间(time()返回的时间)
	return mt;
#else
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

}