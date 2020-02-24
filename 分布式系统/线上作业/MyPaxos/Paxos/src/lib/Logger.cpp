// Logger.cpp: implementation of the Logger class.
//
//////////////////////////////////////////////////////////////////////

#include "mapi.h"
#include "Logger.h"

#include <time.h>
#include <stdarg.h>
#include <cstring>

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include   <unistd.h>                     //chdir() 
#include   <sys/stat.h>                 //mkdir() 
#include   <sys/types.h>               //mkdir() 
#include   <dirent.h>					//closedir()
#endif

namespace mdk
{

	Logger::Logger()
	{
		m_isInit = false;
		m_index = 0;
		m_maxExistDay = 30;
		m_maxLogSize = 50;
		m_runLogDir = "";
		m_name = "";
		m_bRLogOpened = false;
		m_fpRunLog = NULL;

		m_bPrint = false;
		m_exeDir = new char[2048];
		GetExeDir(m_exeDir, 2048);
	}

	Logger::Logger(const char *name)
	{
		m_isInit = false;
		m_index = 0;
		m_maxExistDay = 30;
		m_maxLogSize = 50;
		m_runLogDir = "";
		m_name = "";
		SetLogName(name);
		m_bRLogOpened = false;
		m_fpRunLog = NULL;

		m_bPrint = false;
		if (NULL != m_exeDir)
		{
			delete[]m_exeDir;
		}
	}

	Logger::~Logger()
	{
		if (NULL != m_fpRunLog)
		{
			fclose(m_fpRunLog);
			m_fpRunLog = NULL;
		}
	}

	bool  Logger::SetLogName(const char *name)
	{
		if (m_isInit) return false;
		if (NULL == name)	m_name = "";
		else m_name = name;
		return CreateLogDir();
	}

	void Logger::SetMaxLogSize(int maxLogSize)
	{
		m_maxLogSize = maxLogSize;
	}

	bool Logger::CreateFreeDir(const char* dir)
	{
		if (-1 == access(dir, 0))
		{
#ifdef WIN32
			mkdir(dir);
#else
			umask(0);
			if (0 > mkdir(dir, 0777))
			{
				printf("create %s faild\n", dir);
				return false;
			}
#endif
		}
#ifndef WIN32
		umask(0);
		chmod(dir, S_IRWXU | S_IRWXG | S_IRWXO);
#endif

		return true;
	}

	bool Logger::CreateLogDir()
	{
		if (m_isInit) return false;
		m_runLogDir = m_exeDir;
		m_runLogDir += "/log";
		CreateFreeDir(m_runLogDir.c_str());
		if ("" != m_name) m_runLogDir += "/" + m_name;
		else m_runLogDir += "/run";
		CreateFreeDir(m_runLogDir.c_str());
		m_isInit = true;
		return true;
	}

	void Logger::RenameMaxLog()
	{
		time_t cutTime = time(NULL);
		tm *pCurTM = localtime(&cutTime);
		char log[256];

		std::string fromat = m_runLogDir + "/%Y-%m-%d.log";
		strftime(log, 256, fromat.c_str(), pCurTM);
		unsigned long lsize = GetFileSize(log);
		if (lsize >= (unsigned long)(1024 * 1024 * m_maxLogSize))
		{
			if (m_bRLogOpened)
			{
				fclose(m_fpRunLog);
				m_bRLogOpened = false;
			}
			m_index++;
			char maxLog[1024];
			sprintf(maxLog, "%s.%d", log, m_index);
			rename(log, maxLog);
		}
	}

	bool Logger::OpenRunLog()
	{
		time_t cutTime = time(NULL);
		tm *pCurTM = localtime(&cutTime);
		if (m_bRLogOpened)
		{
			char strTime[256];
			strftime(strTime, 30, "%Y-%m-%d", pCurTM);
			int nY, nM, nD;
			sscanf(strTime, "%d-%d-%d", &nY, &nM, &nD);
			if (m_nRunLogCurYear == nY &&
				m_nRunLogCurMonth == nM &&
				m_nRunLogCurDay == nD) return true;
			fclose(m_fpRunLog);
			m_fpRunLog = NULL;
			m_nRunLogCurYear = nY;
			m_nRunLogCurMonth = nM;
			m_nRunLogCurDay = nD;
			m_bRLogOpened = false;
			m_index = 0;
		}

		char strRunLog[256];
		std::string fromat = m_runLogDir + "/%Y-%m-%d.log";
		strftime(strRunLog, 256, fromat.c_str(), pCurTM);
		m_fpRunLog = fopen(strRunLog, "a");
		m_bRLogOpened = NULL != m_fpRunLog;

		return m_bRLogOpened;
	}

#ifdef WIN32
	time_t SystemTimeToTimet(SYSTEMTIME st)
	{
		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);
		LONGLONG nLL;
		ULARGE_INTEGER ui;
		ui.LowPart = ft.dwLowDateTime;
		ui.HighPart = ft.dwHighDateTime;
		nLL = ((mdk::uint64)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		time_t pt = (long)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
		return pt;
	}
#else
#include <dirent.h>  
#include <sys/stat.h>  

#endif


	void Logger::FindDelLog(char * path, int maxExistDay)
	{
		maxExistDay = 5;
		time_t curTime = mdk_Date();
		curTime -= 86400 * (maxExistDay - 1);

#ifdef WIN32
		char szFind[MAX_PATH];
		char szFile[MAX_PATH];
		WIN32_FIND_DATA FindFileData;
		time_t ftime;

		strcpy(szFind, path);
		strcat(szFind, "//*.*");
		HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind) return;

		do
		{
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (0 == strcmp(".", FindFileData.cFileName)
					|| 0 == strcmp("..", FindFileData.cFileName)) continue;
				strcpy(szFile, path);
				strcat(szFile, "//");
				strcat(szFile, FindFileData.cFileName);
				FindDelLog(szFile, maxExistDay);
			}
			else
			{
				SYSTEMTIME   st;
				FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &st);
				ftime = SystemTimeToTimet(st);
				if (ftime >= curTime) continue;
				std::string strRunLog = m_runLogDir + "//" + FindFileData.cFileName;
				remove(strRunLog.c_str());
			}
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
#else
		DIR              *pDir;
		struct dirent    *ent;
		char              childpath[512];
		pDir = opendir(path);
		memset(childpath, 0, sizeof(childpath));
		while (NULL != (ent = readdir(pDir)))
		{
			if (ent->d_type&DT_DIR)
			{
				if (0 == strcmp(".", ent->d_name) || 0 == strcmp("..", ent->d_name)) continue;
				sprintf(childpath, "%s/%s", path, ent->d_name);
				FindDelLog(childpath, maxExistDay);
			}
			else
			{
				std::string strRunLog = m_runLogDir + "//" + ent->d_name;
				struct stat buf;
				if (-1 == stat(strRunLog.c_str(), &buf)) continue;
				if (buf.st_ctime >= curTime) continue;
				remove(strRunLog.c_str());
			}
		}
		closedir(pDir);
#endif

	}

	void Logger::DelLog(int nDay)
	{
		FindDelLog((char*)m_runLogDir.c_str(), m_maxExistDay);
	}

	void Logger::SetPrintLog(bool bPrint)
	{
		m_bPrint = bPrint;
	}

	bool Logger::Info(const char *findKey, const char *format, ...)
	{
		AutoLock lock(&m_writeMutex);
		if (!m_isInit)
		{
			if (!SetLogName(NULL)) return m_isInit;
		}

		DelLog(m_maxExistDay);
		RenameMaxLog();

		if (!OpenRunLog()) return false;
		//取得时间
		time_t cutTime = time(NULL);
		tm *pCurTM = localtime(&cutTime);
		char strTime[32];
		strftime(strTime, 30, "%Y-%m-%d %H:%M:%S", pCurTM);
		//写入日志内容
		fprintf(m_fpRunLog, "%s Tid:%llu [%s] ", strTime, CurThreadId(), findKey);
		va_list ap;
		va_start(ap, format);
		vfprintf(m_fpRunLog, format, ap);
		va_end(ap);
#ifdef WIN32
		fprintf(m_fpRunLog, "\n");
#else
		fprintf(m_fpRunLog, "\r\n");
#endif
		fflush(m_fpRunLog);

		//打印日志内容
		if (m_bPrint)
		{
			printf("%s Tid:%llu [%s] ", strTime, CurThreadId(), findKey);
			va_list ap;
			va_start(ap, format);
			vprintf(format, ap);
			va_end(ap);
			printf("\n");
		}

		return true;
	}

	bool Logger::StreamInfo(const char *findKey, unsigned char *stream, int nLen, const char *format, ...)
	{
		AutoLock lock(&m_writeMutex);
		if (!m_isInit)
		{
			if (!SetLogName(NULL)) return m_isInit;
		}

		DelLog(m_maxExistDay);
		RenameMaxLog();

		if (!OpenRunLog()) return false;
		//取得时间
		time_t cutTime = time(NULL);
		tm *pCurTM = localtime(&cutTime);
		char strTime[32];
		strftime(strTime, 30, "%Y-%m-%d %H:%M:%S", pCurTM);
		//写入日志内容
		fprintf(m_fpRunLog, "%s Tid:%llu [%s] ", strTime, CurThreadId(), findKey);
		va_list ap;
		va_start(ap, format);
		vfprintf(m_fpRunLog, format, ap);
		va_end(ap);

		//打印日志内容
		if (m_bPrint)
		{
			printf("%s Tid:%llu [%s] ", strTime, CurThreadId(), findKey);
			va_list ap;
			va_start(ap, format);
			vprintf(format, ap);
			va_end(ap);
		}

		//写入流
		fprintf(m_fpRunLog, " stream:");
		if (m_bPrint) printf(" stream:");
		int i = 0;
		for (i = 0; i < nLen - 1; i++)
		{
			fprintf(m_fpRunLog, "%x,", stream[i]);
			if (m_bPrint) printf("%x,", stream[i]);
		}
#ifdef WIN32
		fprintf(m_fpRunLog, "%x\n", stream[i]);
#else
		fprintf(m_fpRunLog, "%x\r\n", stream[i]);
#endif
		if (m_bPrint) printf("%x\n", stream[i]);
		fflush(m_fpRunLog);


		return true;
	}

	void Logger::SetMaxExistDay(int maxExistDay)
	{
		m_maxExistDay = maxExistDay;
	}

}//namespace mdk
