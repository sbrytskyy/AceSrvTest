#include <ace/Log_Msg.h>

#include <iostream>
#include <string>

#include "ChatServer.h"
#include "User.h"
#include "Status.h"
#include "Util.h"

ChatServer::ChatServer()
{
}


ChatServer::~ChatServer()
{
	acceptor.close();
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
	int result = acceptor.open(server_addr);
	if (result == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.open()"), EXIT_FAILURE);
	}

	return result;
}

int ChatServer::handle_connections()
{
	int result = acceptor.accept(peer);
	if (result == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.accept()"), EXIT_FAILURE);
	}

	peer.disable(ACE_NONBLOCK);

	return result;
}

int ChatServer::handle_data()
{
	iovec *io_vec = new iovec();
	int n = peer.recvv(io_vec);

	Util::dumpMessage(io_vec, true);

	ACE_InputCDR icdr(io_vec->iov_base, io_vec->iov_len);

	User user;
	icdr >> user;

	std::cout << "[RECEIVED] user=[pid: " << user.pid() << ", name: " << user.name() << "]" << std::endl;

	delete[] io_vec->iov_base;
	delete io_vec;

	sendResponse(0);

	return 0;
}

void ChatServer::extractMessageBlock(ACE_InputCDR& icdr)
{
	//ACE_Message_Block *messageBlock = new ACE_Message_Block(ACE_DEFAULT_CDR_BUFSIZE);

	const ACE_Message_Block * mblk = icdr.start();

	//ACE::write_n(ACE_STDOUT, mblk->rd_ptr(), mblk->length());
	std::string message(mblk->rd_ptr(), mblk->length());
	std::cout << "mblk->length(): " << mblk->length() << ", message: " << message << std::endl;
}

void ChatServer::sendResponse(long code)
{
	ACE_OutputCDR ocdr;

	Status status(code);
	ocdr << status;

	iovec *io_vec = new iovec();
	io_vec->iov_base = ocdr.begin()->rd_ptr();
	io_vec->iov_len = ocdr.length();

	Util::dumpMessage(io_vec, false);

	if (peer.sendv(io_vec, 1) == -1)
	{
		std::cerr << "Error sending data." << std::endl;
	}

	delete io_vec;
}
