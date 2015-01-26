#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <conio.h>
#include <iostream>
#include <ctime>

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
		Util::log("[Client::onStatus] status=[code: %d]\n", status.code());
	}

	void Client::onLogin(Login& login)
	{
		Util::log("[Client::onLogin] login = [pid: %d, name : %s]\n", login.pid(), login.name());
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

void testSend(PacketHandler &packetHandler);

void runClient()
{
	const char* server_hostname = "localhost";

	PacketHandler packetHandler;

	ACE_SOCK_Connector connector;
	ACE_INET_Addr peer_addr;

	if (peer_addr.set(SERVER_PORT, server_hostname) == -1)
	{
		std::cerr << "Error initializing an ACE_INET_Addr from a port_number and the remote host_name.." << std::endl;
		return;
	}
	else if (connector.connect(packetHandler.peer(), peer_addr) == -1)
	{
		std::cerr << "Error connecting to a peer, producing a connected ACE_SOCK_Stream object if the connection succeeds." << std::endl;
		return;
	}

	for (int i=0; i<10; i++)
		testSend(packetHandler);

	packetHandler.close();
}

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	Util::log("%s\n", "[Client] START");

	//test();

	runClient();

	_getch();

	return 0;
}

static int counter = 0;

void testSend(PacketHandler &packetHandler)
{
	/*time_t rawtime;
	struct tm * timeinfo;

	//time ( &rawtime );
	timeinfo = localtime(&rawtime);*/
	//std::cout << "Current local time and date: " << time(NULL) << std::endl;

	Login login((long)time(NULL), "uranium");

	packetHandler.sendLogin(login);

	Client client;
	packetHandler.processPacket(client);
	//packetHandler.close();
}
