#pragma once

#include "ace/ACE.h"
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>

#include "config.h"

class ChatServer
{
public:
	ChatServer();
	virtual ~ChatServer();
	int run();

protected:
	virtual int wait_for_multiple_events() { return 0; }
	virtual int handle_connections();
	virtual int handle_data();

private:
	int open();

	ACE_INET_Addr server_addr;
	ACE_SOCK_Acceptor acceptor;
	ACE_SOCK_Stream peer;
};

