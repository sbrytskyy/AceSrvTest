#include <ace/Log_Msg.h>

#include <iostream>
#include <string>

#include "ChatServer.h"
#include "PacketHandler.h"
#include "Header.h"
#include "Login.h"
#include "Status.h"
#include "Util.h"

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
	int result = acceptor.accept(packetHandler().peer());
	if (result == EXIT_FAILURE)
	{
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.accept()"), EXIT_FAILURE);
	}

	packetHandler().peer().disable(ACE_NONBLOCK);

	return result;
}

int ChatServer::handle_data()
{

	long code = 0;
	Status status(code);

	PacketHandler::processPacket(packetHandler().peer(), *this);

	PacketHandler::sendStatus(packetHandler().peer(), status);

	return 0;
}

void ChatServer::onStatus(Status& status)
{
	std::cout << "[ChatServer::onStatus] status=[code: " << status.code() << "]" << std::endl;
}

void ChatServer::onLogin(Login& login)
{
	std::cout << "[ChatServer::onLogin] login=[pid: " << login.pid() << ", name: " << login.name() << "]" << std::endl;
}
