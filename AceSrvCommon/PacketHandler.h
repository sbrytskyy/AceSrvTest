#pragma once

#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#include <iostream>

#include "PacketListener.h"
#include "Login.h"
#include "Status.h"

class PacketHandler
{
public:
	PacketHandler() {}
	virtual ~PacketHandler() {}

	static void sendLogin(ACE_SOCK_Stream& peer, Login& login);
	static void sendStatus(ACE_SOCK_Stream& peer, Status& status);

	static void processPacket(ACE_SOCK_Stream& peer, PacketListener& listener);
};

