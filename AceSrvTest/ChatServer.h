#pragma once

#include <ace/ACE.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/CDR_Stream.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Containers.h>
#include <ace/Handle_Set.h>

#include <vector>
#include <map>

#include "PacketListener.h"
#include "PacketHandler.h"
#include "config.h"

typedef ACE_Unbounded_Queue<char> BUFFER_TYPE;
typedef ACE_Hash_Map_Manager<ACE_HANDLE, BUFFER_TYPE *, ACE_SYNCH_RW_MUTEX> BUFFER_MAP;

class ChatServer : public PacketListener
{
public:
	ChatServer() {}
	virtual ~ChatServer();
	int run();
	int test();

	virtual void onStatus(Status& status);
	virtual void onLogin(Login& login);
	virtual void onLogin(acemsgr::Login& login);

	PacketHandler& packetHandler() { return m_packetHandler; }

protected:

	// Keep track of the acceptor socket handle and all the
	// connected stream socket handles.
	ACE_Handle_Set master_handle_set_;

	// Keep track of handles marked as active by <select>.
	ACE_Handle_Set active_handles_;

	virtual int wait_for_multiple_events();

	virtual int handle_connections();
	virtual int handle_data();

	// Accessor.
	ACE_SOCK_Acceptor& acceptor() { return m_acceptor; }

private:
	BUFFER_MAP buffers;

	int open();

	void extractMessageBlock(ACE_InputCDR& icdr);
	void sendResponse(long code);

	void printHandlesSet(ACE_Handle_Set& handles, bool master, char* procedure_name);

	ACE_INET_Addr server_addr;
	ACE_SOCK_Acceptor m_acceptor;
	PacketHandler m_packetHandler;
};

