#pragma once

#include <ace/ACE.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/CDR_Stream.h>

#include "PacketListener.h"
#include "PacketHandler.h"
#include "config.h"

class ChatServer : public PacketListener
{
public:
	ChatServer() {}
	virtual ~ChatServer();
	int run();

	virtual void onStatus(Status& status);
	virtual void onLogin(Login& login);

	PacketHandler& packetHandler() { return m_packetHandler; }

protected:
	virtual int wait_for_multiple_events() { return 0; }
	virtual int handle_connections();
	virtual int handle_data();

private:
	int open();

	void extractMessageBlock(ACE_InputCDR& icdr);
	void sendResponse(long code);

	ACE_INET_Addr server_addr;
	ACE_SOCK_Acceptor acceptor;
	PacketHandler m_packetHandler;
};

