#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <conio.h>
#include <iostream>

#include "User.h"
#include "config.h"

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	const char* server_hostname = "localhost";

	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;
	ACE_INET_Addr peer_addr;

	if (peer_addr.set(SERVER_PORT, server_hostname) == -1)
	{
		return 1;
	}
	else if (connector.connect(peer, peer_addr) == -1) 
	{
		return 1;
	}

	ACE_OutputCDR ocdr;

	User user1(11, "uranium");
	User::marshall(ocdr, user1);

	iovec *io_vec = new iovec();
	io_vec->iov_base = ocdr.begin()->rd_ptr();
	io_vec->iov_len = ocdr.length();

	std::cout << "[SEND] io_vec->iov_len: " << io_vec->iov_len << ", io_vec->iov_base:[";
	ACE::write_n(ACE_STDOUT, io_vec->iov_base, io_vec->iov_len);
	std::cout << "]"<< std::endl;

	if (peer.sendv(io_vec, 1) == -1)
	{
		return 1;
	}

/*	for (ssize_t n; (n = peer.recv(buf, sizeof buf)) > 0; )
	{
		ACE::write_n(ACE_STDOUT, buf, n);
	}*/

	delete io_vec;

	peer.close();

	_getch();

	return 0;
}