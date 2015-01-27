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
	if (server_addr.set(SERVER_PORT) == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p; server port: %n\n", "server_addr.set()", SERVER_PORT), EXIT_FAILURE);
	}
	int result = acceptor().open(server_addr);
	if (result == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.open()"), EXIT_FAILURE);
	}
	master_handle_set_.set_bit(acceptor().get_handle());
	int m_num_set = master_handle_set_.num_set();
	//ACE_DEBUG((LM_DEBUG, "master_handle_set_.num_set()=%d\n", m_num_set));
	Util::log("master_handle_set_.num_set()=%d\n", m_num_set);
	acceptor().enable(ACE_NONBLOCK);

	return result;
}

int ChatServer::handle_connections()
{
	if (active_handles_.is_set(acceptor().get_handle())) {
		while (acceptor().accept(packetHandler().peer()) == 0)
			master_handle_set_.set_bit
			(packetHandler().peer().get_handle());

		// Remove acceptor handle from further consideration.
		active_handles_.clr_bit(acceptor().get_handle());
	}
	return 0;
}

int ChatServer::handle_data()
{
	ACE_Handle_Set_Iterator peer_iterator(active_handles_);

	for (ACE_HANDLE handle;
		(handle = peer_iterator()) != ACE_INVALID_HANDLE;
		) {
		packetHandler().peer().set_handle(handle);

		while (packetHandler().processPacket(*this));
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		//if (!result) {
			// Handle connection shutdown or comm failure.
			master_handle_set_.clr_bit(handle);
			packetHandler().close();
		//}
	}
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

int ChatServer::wait_for_multiple_events()
{
	active_handles_ = master_handle_set_;
	int width = ACE_Utils::truncate_cast<int> ((intptr_t)active_handles_.max_set()) + 1;
	if (select(width,
		active_handles_.fdset(),
		0,        // no write_fds
		0,        // no except_fds
		0) == -1) // no timeout
		return -1;
	active_handles_.sync
		((ACE_HANDLE)((intptr_t)active_handles_.max_set() + 1));
	return 0;
}
