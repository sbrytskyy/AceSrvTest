#include "PacketHandler.h"

void PacketHandler::sendLogin(ACE_SOCK_Stream& peer, Login& login)
{
	sendPacket(peer, login, Header::Command::LOGIN);
}

void PacketHandler::sendStatus(ACE_SOCK_Stream& peer, Status& status)
{
	sendPacket(peer, status, Header::Command::STATUS);
}

void PacketHandler::processPacket(ACE_SOCK_Stream& peer, PacketListener& listener)
{
	iovec *io_vec = new iovec();

	int n = peer.recvv(io_vec);

	ACE_DEBUG((LM_DEBUG, "%m\n"));
	Util::dumpMessage(io_vec, 1, true);

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
}