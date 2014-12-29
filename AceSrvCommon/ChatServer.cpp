#include "ChatServer.h"

#include <iostream>
#include <string>

#include "User.h"

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
		return -1;

	for (;;) {
		if (wait_for_multiple_events() == -1)
			return -1;
		if (handle_connections() == -1)
			return -1;
		if (handle_data() == -1)
			return -1;
	}

	ACE_NOTREACHED(return 0;)
}

int ChatServer::open()
{
	if (server_addr.set(SERVER_PORT) == -1) return 1;
	if (acceptor.open(server_addr) == -1) return 1;

	return 0;
}

int ChatServer::handle_connections()
{
	if (acceptor.accept(peer) == -1) return 1;
	peer.disable(ACE_NONBLOCK);

	return 0;
}

int ChatServer::handle_data()
{
	//char buf[1024];

	//ACE_Message_Block *messageBlock = new ACE_Message_Block(ACE_DEFAULT_CDR_BUFSIZE);

	iovec *io_vec = new iovec();
	int n = peer.recvv(io_vec);

	std::cout << "[RECEIVED] io_vec->iov_len: " << io_vec->iov_len << ", io_vec->iov_base:[";
	ACE::write_n(ACE_STDOUT, io_vec->iov_base, io_vec->iov_len);
	std::cout << "]" << std::endl;

	ACE_InputCDR icdr(io_vec->iov_base, io_vec->iov_len);

	const ACE_Message_Block * mblk = icdr.start();

	//ACE::write_n(ACE_STDOUT, mblk->rd_ptr(), mblk->length());
	std::string message(mblk->rd_ptr(), mblk->length());
	std::cout << "mblk->length(): " << mblk->length() << ", message: " << message << std::endl;

	User user2;
	User::readExternal(icdr, user2);

	std::cout << "[RECEIVED] user=[pid: " << user2.pid() << ", name: " << user2.name() << "]" << std::endl;

	delete[] io_vec->iov_base;
	delete io_vec;

	return 0;
}
