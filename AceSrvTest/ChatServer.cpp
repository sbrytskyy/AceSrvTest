#include <ace/Log_Msg.h>
#include <ace/Truncate.h>

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

#define DLOG 1

ChatServer::~ChatServer()
{
	acceptor().close();
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
		if (handle_data() == EXIT_FAILURE)
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

int ChatServer::wait_for_multiple_events()
{
	if (DLOG)
	{
		printf("\n");
		Util::log("[ChatServer::wait_for_multiple_events] START\n");
	}
	if (DLOG)
	{
		Util::log("[ChatServer::wait_for_multiple_events] BEFORE active_handles_ = master_handle_set_;\n");
		printHandlesSet(active_handles_, false, "wait_for_multiple_events");
		printHandlesSet(master_handle_set_, true, "wait_for_multiple_events");
	}

	active_handles_ = master_handle_set_;
	int width = ACE_Utils::truncate_cast<int> ((intptr_t)active_handles_.max_set()) + 1;
	if (DLOG)
	{
		Util::log("[ChatServer::wait_for_multiple_events] width=%d\n", width);
	}
	if (DLOG)
	{
		Util::log("[ChatServer::wait_for_multiple_events] AFTER active_handles_ = master_handle_set_;\n");
		printHandlesSet(active_handles_, false, "wait_for_multiple_events");
		printHandlesSet(master_handle_set_, true, "wait_for_multiple_events");
	}

	int selected = select(width,
		active_handles_.fdset(),
		0,        // no write_fds
		0,        // no except_fds
		0);

	if (DLOG)
	{
		printf("\n\n");
		Util::log("[ChatServer::wait_for_multiple_events] SOCKET activity detected!!! Fired select()=%d\n", selected);
	}
	if (DLOG)
	{
		Util::log("[ChatServer::wait_for_multiple_events] AFTER select(width, active_handles_.fdset()....); \n");
		printHandlesSet(active_handles_, false, "wait_for_multiple_events");
	}

	if (selected == SOCKET_ERROR) // no timeout
		return SOCKET_ERROR;
	active_handles_.sync
		((ACE_HANDLE)((intptr_t)active_handles_.max_set() + 1));

	if (DLOG)
	{
		Util::log("[ChatServer::wait_for_multiple_events] AFTER active_handles_.sync(....); \n");
		printHandlesSet(active_handles_, false, "wait_for_multiple_events");
	}

	if (DLOG) Util::log("[ChatServer::wait_for_multiple_events] END\n");
	return 0;
}

int ChatServer::handle_connections()
{
	if (DLOG)
	{
		printf("\n");
		Util::log("[ChatServer::handle_connections] START\n");
	}

	ACE_HANDLE acceptorHandle = acceptor().get_handle();
	if (DLOG) Util::log("[ChatServer::handle_connections] acceptor().get_handle()=%d\n", acceptorHandle);

	if (DLOG)
	{
		Util::log("[ChatServer::handle_connections] BEFORE active_handles_.is_set(acceptorHandle);\n");
		printHandlesSet(active_handles_, false, "handle_connections");
	}

	if (active_handles_.is_set(acceptorHandle)) {
		if (DLOG) Util::log("[ChatServer::handle_connections] active_handles_.is_set(acceptorHandle)=%d\n", acceptorHandle);
		while (acceptor().accept(packetHandler().peer()) == 0)
		{
			ACE_HANDLE peerHandle = packetHandler().peer().get_handle();
			
			if (DLOG) Util::log("[ChatServer::handle_connections] master_handle_set_.set_bit() with packetHandler().peer().get_handle()=%d\n", peerHandle);
			if (DLOG)
			{
				Util::log("[ChatServer::handle_connections] BEFORE master_handle_set_.set_bit(peerHandle);\n");
				printHandlesSet(master_handle_set_, true, "handle_connections");
			}

			master_handle_set_.set_bit(peerHandle);
			
			if (DLOG)
			{
				Util::log("[ChatServer::handle_connections] AFTER master_handle_set_.set_bit(peerHandle);\n");
				printHandlesSet(master_handle_set_, true, "handle_connections");
			}

		}

		// Remove acceptor handle from further consideration.
		active_handles_.clr_bit(acceptorHandle);
		if (DLOG)
		{
			Util::log("[ChatServer::handle_connections] AFTER active_handles_.clr_bit(acceptorHandle);\n");
			printHandlesSet(active_handles_, false, "handle_connections");
		}
	}

	if (DLOG) Util::log("[ChatServer::handle_connections] END\n");
	return 0;
}

int ChatServer::handle_data()
{
	if (DLOG)
	{
		printf("\n");
		Util::log("[ChatServer::handle_data] START\n");
	}
	if (DLOG)
	{
		Util::log("[ChatServer::handle_data] BEFORE ACE_Handle_Set_Iterator peer_iterator(active_handles_);\n");
		printHandlesSet(active_handles_, false, "handle_data");
	}


	ACE_Handle_Set_Iterator peer_iterator(active_handles_);

	for (ACE_HANDLE handle; (handle = peer_iterator()) != ACE_INVALID_HANDLE;)
	{
		if (DLOG)
		{
			Util::log("[ChatServer::handle_data] BEFORE packetHandler().peer().set_handle(handle);\n");
			printHandlesSet(active_handles_, false, "handle_data");
			printHandlesSet(master_handle_set_, true, "handle_data");
		}

		packetHandler().peer().set_handle(handle);

		while (packetHandler().processPacket(*this));

		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		//if (!result) {
		// Handle connection shutdown or comm failure.
		master_handle_set_.clr_bit(handle);
		packetHandler().close();
		//}

		if (DLOG)
		{
			Util::log("[ChatServer::handle_data] AFTER master_handle_set_.clr_bit(handle);\n");
			printHandlesSet(active_handles_, false, "handle_data");
			printHandlesSet(master_handle_set_, true, "handle_data");
		}
	}
	if (DLOG) Util::log("[ChatServer::handle_data] END\n");
	return 0;
}

void ChatServer::onStatus(Status& status)
{
	//std::cout << "[ChatServer::onStatus] status=[code: " << status.code() << "]" << std::endl;
	Util::log("[ChatServer::onStatus] status=[code: %d]\n", status.code());

}

void ChatServer::onLogin(Login& login)
{
	//std::cout << "[ChatServer::onLogin] login=[pid: " << login.pid() << ", name: " << login.name() << "]" << std::endl;
	Util::log("[ChatServer::onLogin] login = [pid: %d, name : %s]\n", login.pid(), login.name().c_str());
	
	long code = 0;
	Status status(code);
	packetHandler().sendStatus(status);
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
		Util::log("; fdset is EMPTY\n");
	}
}

