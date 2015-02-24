#pragma once

#include <ace/ACE.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/CDR_Stream.h>
#include "ace/Handle_Set.h"

#include "ServerPacketListener.h"
#include "PacketHandler.h"
#include "config.h"

class ChatServer
{
public:
	ChatServer() {}
	virtual ~ChatServer();
	int run();

protected:

	// Keep track of the acceptor socket handle and all the
	// connected stream socket handles.
	ACE_Handle_Set master_handle_set_;

	// Keep track of handles marked as active by <select>.
	ACE_Handle_Set active_handles_;

	virtual int wait_for_multiple_events() { return 0; }

	virtual int handle_connections();
	virtual int handle_data(ACE_SOCK_Stream * = 0);

	// Accessor.
	ACE_SOCK_Acceptor& acceptor() { return m_acceptor; }

private:
	int open();

	void extractMessageBlock(ACE_InputCDR& icdr);
	void sendResponse(long code);

	void printHandlesSet(ACE_Handle_Set& handles, bool master, char* procedure_name);

	ACE_INET_Addr server_addr;
	ACE_SOCK_Acceptor m_acceptor;

	struct Thread_Args {
		Thread_Args(ChatServer *lsp) : this_(lsp) {}

		ChatServer *this_;
		ACE_SOCK_Stream logging_peer_;
	};

	// Passed as a parameter to <ACE_Thread_Manager::spawn>.
	static ACE_THR_FUNC_RETURN run_svc(void *arg);

};

