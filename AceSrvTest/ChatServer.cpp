#include <ace/Log_Msg.h>
#include <ace/Truncate.h>
#include <ace/Auto_Ptr.h>
#include "ace/Thread_Manager.h"

#include <iostream>
#include <string>

#include <chrono>
#include <thread>

#include "ChatServer.h"
#include "PacketHandler.h"
#include "Header.h"
#include "Login.h"
#include "Status.h"
#include "Util.h"


ChatServer::~ChatServer()
{
	acceptor().close();
}

ACE_THR_FUNC_RETURN ChatServer::run_svc(void *arg)
{
	auto_ptr<Thread_Args> thread_args(static_cast<Thread_Args *> (arg));

	thread_args->this_->handle_data(&thread_args->m_chat_peer);
	thread_args->m_chat_peer.close();
	return 0;    // Return value is ignored
}


int ChatServer::run()
{
	if (this->open())
		return EXIT_FAILURE;

	for (;;)
	{
		if (wait_for_multiple_events() == EXIT_FAILURE)
			return EXIT_FAILURE;
		if (handle_connections() == EXIT_FAILURE)
			return EXIT_FAILURE;
	}

	ACE_NOTREACHED( return 0; )
}

int ChatServer::open()
{
	if (DLOG) printf("\n");
	if (server_addr.set(SERVER_PORT) == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p; server port: %n\n", "server_addr.set()", SERVER_PORT), EXIT_FAILURE);
	}
	int result = acceptor().open(server_addr);
	if (result == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.open()"), EXIT_FAILURE);
	}

	if (DLOG)
	{
		Util::log("[ChatServer::open] BEFORE master_handle_set_.set_bit(acceptorHandle);\n");
		printHandlesSet(master_handle_set_, true, "open");
	}

	ACE_HANDLE acceptorHandle = acceptor().get_handle();
	master_handle_set_.set_bit(acceptorHandle);

	if (DLOG) Util::log("[ChatServer::open] master_handle_set_.set_bit() with acceptor().get_handle()=%d\n", acceptorHandle);

	if (DLOG)
	{
		Util::log("[ChatServer::open] AFTER master_handle_set_.set_bit(acceptorHandle);\n");
		printHandlesSet(master_handle_set_, true, "open");
	}

	acceptor().enable(ACE_NONBLOCK);

	return result;
}

int ChatServer::handle_connections()
{
	if (DLOG)
	{
		printf("\n");
		Util::log("[ChatServer::handle_connections] START\n");
	}

	auto_ptr<Thread_Args> thread_args(new Thread_Args(this));

	if (acceptor().accept(thread_args->m_chat_peer) == -1)
		return -1;
	if (ACE_Thread_Manager::instance()->spawn(
		// Pointer to function entry point.
		ChatServer::run_svc,
		// <run_svc> parameter.
		static_cast<void *> (thread_args.get()),
		THR_DETACHED | THR_SCOPE_SYSTEM) == -1)
		return -1;
	thread_args.release();   // Spawned thread now owns memory

	if (DLOG) Util::log("[ChatServer::handle_connections] END\n");
	return 0;
}

int ChatServer::handle_data(ACE_SOCK_Stream *client)
{
	if (DLOG)
	{
		printf("\n");
		Util::log("[ChatServer::handle_data] START\n");
	}

	// Place the connection into blocking mode since this
	// thread isn't doing anything except handling this client.
	client->disable(ACE_NONBLOCK);

	PacketHandler handler = PacketHandler(*client);
	ServerPacketListener spl = ServerPacketListener(handler);
		
	// Keep handling chat messages until client closes connection
	// or this thread is asked to cancel itself.
	ACE_Thread_Manager *mgr = ACE_Thread_Manager::instance();
	ACE_thread_t me = ACE_Thread::self();
	while (!mgr->testcancel(me) &&
		handler.processPacket(spl) != 0)
		continue;
	handler.close();

	if (DLOG) Util::log("[ChatServer::handle_data] END\n");
	return 0;
}

void ChatServer::printHandlesSet(ACE_Handle_Set& handles, bool master, char* procedure_name)
{
	Util::log("%s ACE_Handle_Set %s; called from %s\n", "[ChatServer::printHandlesSet]", (master ? "MASTER" : "ACTIVE"), procedure_name);
	fd_set * fdset = handles.fdset();
	if (fdset)
	{
		u_int fd_count = fdset->fd_count;
		SOCKET* sa = fdset->fd_array;
		Util::log("\tfd_count=%d\n", fd_count);
		for (u_int i = 0; i < fd_count; i++)
		{
			SOCKET s = sa[i];
			Util::log("\t\tSOCKET=%d\n", s);
		}
	}
	else
	{
		Util::log("\tfdset is EMPTY\n");
	}
}

