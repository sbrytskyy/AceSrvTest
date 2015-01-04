#pragma once

#include <ace/ACE.h>
#include <ace/SOCK_Stream.h>

#include <iostream>

#include "Login.h"

class Util
{
public:

	Util() {}
	virtual ~Util() {}

	static void dumpMessage(const iovec * io_vec, const size_t size, bool incoming);
	static void sendLogin(ACE_SOCK_Stream& peer, Login& login);
};

