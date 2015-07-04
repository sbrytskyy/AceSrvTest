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
	if (this->test() == EXIT_FAILURE)
		return EXIT_FAILURE;

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

	ACE_HANDLE acceptorHandle = acceptor().get_handle();
	master_handle_set_.set_bit(acceptorHandle);

	acceptor().enable(ACE_NONBLOCK);

	return result;
}

int ChatServer::wait_for_multiple_events()
{
	active_handles_ = master_handle_set_;
	int width = ACE_Utils::truncate_cast<int> ((intptr_t)active_handles_.max_set()) + 1;

	int selected = ACE::select(width,
		active_handles_,
		0);

	if (selected == SOCKET_ERROR) // no timeout
		return SOCKET_ERROR;

	active_handles_.sync
		((ACE_HANDLE)((intptr_t)active_handles_.max_set() + 1));

	return 0;
}

int ChatServer::handle_connections()
{
	ACE_HANDLE acceptorHandle = acceptor().get_handle();

	if (active_handles_.is_set(acceptorHandle)) {
		while (acceptor().accept(packetHandler().peer()) == 0)
		{
			ACE_HANDLE peerHandle = packetHandler().peer().get_handle();

			BUFFER_TYPE* buffer = new BUFFER_TYPE;
			buffers.bind(peerHandle, buffer);

			Util::tlog("[handle_connections] HANDLE=%d, data size=%d \n", peerHandle, buffer->size());

			master_handle_set_.set_bit(peerHandle);
		}

		// Remove acceptor handle from further consideration.
		active_handles_.clr_bit(acceptorHandle);
	}

	return 0;
}

int ChatServer::handle_data()
{
	ACE_Handle_Set_Iterator peer_iterator(active_handles_);

	for (ACE_HANDLE handle; (handle = peer_iterator()) != ACE_INVALID_HANDLE;)
	{
		packetHandler().peer().set_handle(handle);

		iovec *io_vec = new iovec();
		ssize_t bytesRead = packetHandler().peer().recvv(io_vec);

		Util::dumpMessage(io_vec, 1, true);

		BUFFER_TYPE *buffer;
		buffers.find(handle, buffer);
		Util::tlog("[handle_data] find buffer HANDLE=%d, data size=%d \n", handle, buffer->size());

		//buffer.insert(buffer.end(), io_vec->iov_base, io_vec->iov_base + bytesRead);
		//buffer->enqueue_tail('a');
		char* c = io_vec->iov_base;
		while (c < io_vec->iov_base + bytesRead)
		{
			buffer->enqueue_tail(*c);
			c++;
		}

		Util::tlog("[handle_data] after insert HANDLE=%d, data size=%d \n", handle, buffer->size());

		delete[] io_vec->iov_base;
		delete io_vec;

		if (bytesRead == 0)
		{
			master_handle_set_.clr_bit(handle);

			Util::tlog("[handle_data] CLEAR buffer HANDLE=%d \n", handle);

			buffer->reset();
			delete buffer;
			buffers.unbind(handle);

			packetHandler().close(); 
		}

		// TODO check. Call should be when closing connection to client
		//master_handle_set_.clr_bit(handle);
		//packetHandler().close(); 

/*		if (true)
		{
			while (packetHandler().processPacket(*this));
			master_handle_set_.clr_bit(handle);
			packetHandler().close();
		}
		else
		{
			int result = packetHandler().processPacket(*this);

			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			if (!result) 
			{
				// Handle connection shutdown or comm failure.
				master_handle_set_.clr_bit(handle);
				packetHandler().close();
			}

		}*/
	}

	return 0;
}

void ChatServer::onStatus(Status& status)
{
	//std::cout << "[ChatServer::onStatus] status=[code: " << status.code() << "]" << std::endl;
	Util::tlog("[ChatServer::onStatus] status=[code: %d]\n", status.code());

}

void ChatServer::onLogin(Login& login)
{
	//std::cout << "[ChatServer::onLogin] login=[pid: " << login.pid() << ", name: " << login.name() << "]" << std::endl;
	Util::tlog("[ChatServer::onLogin] login = [pid: %d, name : %s]\n", login.pid(), login.name().c_str());
	
	long code = 0;
	Status status(code);
	packetHandler().sendStatus(status);
}

void ChatServer::onLogin(acemsgr::Login& login)
{
	//std::cout << "[ChatServer::onLogin] login=[pid: " << login.pid() << ", name: " << login.name() << "]" << std::endl;
	Util::tlog("[ChatServer::onLogin] login = [pid: %d, name : %s]\n", login.id(), login.name().c_str());

	long code = 0;
	Status status(code);
	packetHandler().sendStatus(status);
}

void ChatServer::printHandlesSet(ACE_Handle_Set& handles, bool master, char* procedure_name)
{
	Util::tlog("%s ACE_Handle_Set %s; called from %s\n", "[ChatServer::printHandlesSet]", (master ? "MASTER" : "ACTIVE"), procedure_name);
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

int ChatServer::test()
{
	ACE_Message_Block mb;


	return EXIT_FAILURE;
}
