#pragma once

#include <ace/ACE.h>
#include <ace/Log_Msg.h>

#include <iostream>
#include <time.h>

class Util
{
public:

	Util() {}
	virtual ~Util() {}

	static void dumpMessage(const iovec * io_vec, const size_t size, bool incoming);
	static void tlog(const char *pszFormat, ...);
	static void log(const char *pszFormat, ...);
};

