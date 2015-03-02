#pragma once

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Synch.h>

#include <iostream>
#include <time.h>

#define DLOG false

class Util
{
public:

	Util() {}
	virtual ~Util() {}

	static void dumpMessage(const iovec * io_vec, const size_t size, bool incoming);
	static void log(const char *pszFormat, ...);
	static void debug(const char *pszFormat, ...);

private:
	static ACE_Thread_Mutex mutex;
};

