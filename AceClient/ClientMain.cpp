#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <conio.h>
#include <iostream>

#include "Login.h"
#include "Status.h"
#include "config.h"
#include "Util.h"
#include "Header.h"
#include "PacketHandler.h"
#include "PacketListener.h"

class Client : public PacketListener
{
public:
	void Client::onStatus(Status& status)
	{
		std::cout << "[Client::onStatus] status=[code: " << status.code() << "]" << std::endl;
	}

	void Client::onLogin(Login& login)
	{
		std::cout << "[Client::onLogin] login=[pid: " << login.pid() << ", name: " << login.name() << "]" << std::endl;
	}
};

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

	Login login(11, "uranium");

	PacketHandler::sendLogin(peer, login);

	Client client;
	PacketHandler::processPacket(peer, client);

	peer.close();
}

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	//test();

	runClient();

	_getch();

	return 0;
}