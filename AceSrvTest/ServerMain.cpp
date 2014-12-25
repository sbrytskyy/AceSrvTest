#include "ace/ACE.h"
#include <ace/Auto_Ptr.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Mem_Map.h>

#include "ace/Message_Block.h"

#include <iostream>
#include <string>

#include "User.h"
#include "config.h"

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	ACE_INET_Addr server_addr;
	ACE_SOCK_Acceptor acceptor;
	ACE_SOCK_Stream peer;

	if (server_addr.set(SERVER_PORT) == -1) return 1;
	if (acceptor.open(server_addr) == -1) return 1;

	for (;;)
	{
		if (acceptor.accept(peer) == -1) return 1;
		peer.disable(ACE_NONBLOCK);

		//char buf[1024];

		//ACE_Message_Block *messageBlock = new ACE_Message_Block(ACE_DEFAULT_CDR_BUFSIZE);

		iovec *io_vec = new iovec();
		int n = peer.recvv(io_vec);

		std::cout << "[RECEIVED] io_vec->iov_len: " << io_vec->iov_len << ", io_vec->iov_base:[";
		ACE::write_n(ACE_STDOUT, io_vec->iov_base, io_vec->iov_len);
		std::cout << "]" << std::endl;

		ACE_InputCDR icdr(io_vec->iov_base, io_vec->iov_len);

		User user2;
		User::unmarshall(icdr, user2);

		std::cout << "[RECEIVED] user=[pid: " << user2.getPid() << ", name: " << user2.getName() << "]" << std::endl;

		//ACE_Message_Block * mb = icdr.steal_contents();

		delete[] io_vec->iov_base;
		delete io_vec;

		peer.close();
	}

	return 0;
}