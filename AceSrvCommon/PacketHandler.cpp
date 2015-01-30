#include "PacketHandler.h"

void PacketHandler::sendLogin(Login& login)
{
	sendPacket(login, Header::Command::LOGIN);
}

void PacketHandler::sendStatus(Status& status)
{
	sendPacket(status, Header::Command::STATUS);
}

int PacketHandler::processPacket(PacketListener& listener)
{
	iovec *io_vec = new iovec();

	Util::log("[PacketHandler::processPacket] BEFORE int n = peer().recvv(io_vec);\n");

	int n = peer().recvv(io_vec);

	Util::log("[PacketHandler::processPacket] AFTER int n = peer().recvv(io_vec); n=%d\n", n);

	//ACE_DEBUG((LM_DEBUG, "%m\n"));
	//Util::dumpMessage(io_vec, 1, true);

	ACE_InputCDR icdr(io_vec->iov_base, n);

	Header header;
	icdr >> header;

	switch (header.command())
	{
	case Header::Command::LOGIN:
		{
			Login login;
			icdr >> login;
			listener.onLogin(login);
		}
		break;
	case Header::Command::STATUS:
		{
			Status status;
			icdr >> status;
			listener.onStatus(status);
		}
		break;
	}

	delete[] io_vec->iov_base;
	delete io_vec;

	return n;
}

int PacketHandler::close()
{
	return m_peer.close();
}
