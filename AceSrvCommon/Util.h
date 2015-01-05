#pragma once

#include <ace/ACE.h>

#include <iostream>

class Util
{
public:

	Util() {}
	virtual ~Util() {}

	static void dumpMessage(const iovec * io_vec, const size_t size, bool incoming);
};

