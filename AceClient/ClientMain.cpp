#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <conio.h>
#include <iostream>

#include "User.h"
#include "Status.h"
#include "config.h"
#include "Util.h"
#include "Header.h"

void test()
{
	Header header1(Header::MESSAGE);
	ACE_OutputCDR ocdr;

	ocdr << header1;

	ACE_InputCDR icdr(ocdr);
	Header header2;

	std::cout << "1. Header command: " << header2.command() << std::endl;

	icdr >> header2;

	std::cout << "2. Header command: " << header2.command() << std::endl;
}

void runClient()
{
	const char* server_hostname = "localhost";

	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;
	ACE_INET_Addr peer_addr;

	if (peer_addr.set(SERVER_PORT, server_hostname) == -1)
	{
		std::cerr << "Error initializing an ACE_INET_Addr from a port_number and the remote host_name.." << std::endl;
		return;
	}
	else if (connector.connect(peer, peer_addr) == -1)
	{
		std::cerr << "Error connecting to a peer, producing a connected ACE_SOCK_Stream object if the connection succeeds." << std::endl;
		return;
	}

	ACE_OutputCDR ocdr;

	User user(11, "uranium");
	ocdr << user;

	iovec *io_vec = new iovec();
	io_vec->iov_base = ocdr.begin()->rd_ptr();
	io_vec->iov_len = ocdr.length();

	Util::dumpMessage(io_vec, false);

	if (peer.sendv(io_vec, 1) == -1)
	{
		std::cerr << "Error sending data." << std::endl;
	}

	/*	for (ssize_t n; (n = peer.recv(buf, sizeof buf)) > 0; )
	{
	ACE::write_n(ACE_STDOUT, buf, n);
	}*/

	delete io_vec;

	io_vec = new iovec();
	int n = peer.recvv(io_vec);

	Util::dumpMessage(io_vec, true);

	ACE_InputCDR icdr(io_vec->iov_base, io_vec->iov_len);

	Status status;
	icdr >> status;

	std::cout << "[RECEIVED] status=[status: " << status.code() << "]" << std::endl;

	delete[] io_vec->iov_base;
	delete io_vec;

	peer.close();
}

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	//test();

	runClient();

	_getch();

	return 0;
}