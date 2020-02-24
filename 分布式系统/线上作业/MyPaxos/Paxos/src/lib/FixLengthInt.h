//  Copyright [2012] <xiaoxie820125@sina.com>

#ifndef MDK_FIXLENGTHINT_H
#define MDK_FIXLENGTHINT_H

#ifndef WIN32
#include <sys/types.h>
#else
#pragma warning(disable:4996)
#endif


namespace mdk
{
	
	typedef char				int8;
	typedef unsigned char		uint8;
	typedef short				int16;
	typedef unsigned short		uint16;
	typedef int					int32;
	typedef unsigned int		uint32;
#ifdef WIN32
	typedef __int64				int64;
	typedef unsigned __int64	uint64;
#else
#include <sys/types.h>
	typedef int64_t				int64;
	typedef u_int64_t			uint64;
#endif

}//namespace mdk
	  
#endif  // MDK_FIXLENGTHINT_H
