#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <conio.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <random>


#include <windows.h>
#include <process.h>         // needed for _beginthread()


#include "Login.h"
#include "Status.h"
#include "config.h"
#include "Util.h"
#include "Header.h"
#include "PacketHandler.h"
#include "PacketListener.h"

const int MAX_PACKETS = 10;
const int THREADS_COUNT = 10;

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
	Util::log("[Client] Connected... Handle=%d\n", packetHandler.peer().get_handle());

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	srand(time(NULL));
	/* generate number between 1 and MAX_PACKETS: */
	int counter = rand() % MAX_PACKETS + 1;

	for (int i = 0; i < counter; i++)
		testSend(packetHandler);

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));

	packetHandler.close();
}

static int pidCounter = 0;

void testSend(PacketHandler &packetHandler)
{
	//Login login((long)time(NULL), "uranium");
	Login login(pidCounter++, "uranium");

	Util::log("[Client] Sending login with pid %d\n", login.pid());
	packetHandler.sendLogin(login);

	Client client;
	// TODO packetHandler.processPacket(client);
	//packetHandler.close();
}


void runInfLoop()
{
	for (;;)
	{
		for (int i = 0; i < 1; i++)
		{
			Util::log("%s\n", "[Client] press any key to start send session");
			_getch();
			runClient();

			/* generate number between 1 and 10: */
			//int counter = rand() % 10 + 1;

			//std::this_thread::sleep_for(std::chrono::milliseconds(counter * 1000));
		}

		Util::log("%s\n\n", "[Client] send session finished");
	}
}


unsigned __stdcall run(void* params)
{
	/* Wait one second between loops. */
	if (params != NULL)
	{
		// TODO do smth with parameters
	}

	runClient();
	return 0;
}

void runMultyThreadingTest()
{
	HANDLE handles[THREADS_COUNT];
	unsigned addresses[THREADS_COUNT];

	for (int i = 0; i < THREADS_COUNT; i++)
	{
		handles[i] = (HANDLE)_beginthreadex(NULL, 0, run, /*(void *)(&data[i])*/ nullptr, 0, &addresses[i]);

		Util::log("[runMultyThreadingTest] handle[%d] = %d\n", i, handles[i]);

		/* Wait one second between loops. */
		//Sleep( 1000L );
	}

	unsigned long waitResult = WaitForMultipleObjects(THREADS_COUNT, handles, true, INFINITE);
	if ((waitResult >= WAIT_OBJECT_0) && (waitResult <= (WAIT_OBJECT_0 + THREADS_COUNT - 1)))
	{
		Util::log("All threads ended, cleaning up for application exit... waitResult: %lu\n", waitResult);
	}
	else	// An error occurred
	{
		unsigned long status = GetLastError();
		Util::log("WaitForMultipleObjects failed: %lu, GetLastError(): %lu\n", waitResult);
	}

	for (int i = 0; i < THREADS_COUNT; i++)
	{
		CloseHandle(handles[i]);
	}

	Util::log("%s\n", "[Client] press any key to finish");
	_getch();
}

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	Util::log("%s\n", "[Client] START");

	/* initialize random seed: */
	srand(time(NULL));

	//test();

	//runInfLoop();
	runMultyThreadingTest();

	Util::log("%s\n", "[Client] FINISH");

	return 0;
}
