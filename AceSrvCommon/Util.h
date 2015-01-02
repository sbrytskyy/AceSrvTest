#pragma once

#include <ace/ACE.h>

#include <iostream>

class Util
{
public:

	Util() {}
	virtual ~Util() {}

	static void dumpMessage(iovec * io_vec, bool incoming);
};

