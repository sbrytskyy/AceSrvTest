#pragma once

#include "PacketListener.h"
#include "PacketHandler.h"

class ServerPacketListener :
	public PacketListener
{
public:
	ServerPacketListener(PacketHandler& handler) : m_handler(handler) {};
	virtual ~ServerPacketListener() {};

	virtual void onStatus(Status& status) override;

	virtual void onLogin(Login& login) override;

private:
	PacketHandler m_handler;
};

