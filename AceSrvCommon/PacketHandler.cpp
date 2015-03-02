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

	//Util::log("[PacketHandler::processPacket] BEFORE int n = peer().recvv(io_vec);\n");

	int n = peer().recvv(io_vec);
	Util::debug("[PacketHandler::processPacket] peer().recvv(io_vec) = %d\n", n);
	if (n > 0)
	{
		//ACE_ERROR((LM_ERROR, ACE_TEXT("%N:%l: Failed to receive request. (errno = %i: %m)\n"), ACE_ERRNO_GET));
		//ACE_DEBUG((LM_DEBUG, "Client::receiving_thread:(%P|%t):%d:%m\n", errno));
		if (DLOG)
		{
			Util::dumpMessage(io_vec, 1, true);
		}

		ACE_InputCDR icdr(io_vec->iov_base, n);

		bool running = true;
		while (running)
		{
			size_t left = icdr.length();
			//Util::log("[PacketHandler::processPacket] left = %d\n", left);
			if (left <= ACE_CDR::MAX_ALIGNMENT) break;

			Header header;
			icdr >> header;

			switch (header.command())
			{
				case Header::Command::LOGIN:
				{
					Login login;
					icdr >> login;
					listener.onLogin(login);
					break;
				}
				case Header::Command::STATUS:
				{
					Status status;
					icdr >> status;
					listener.onStatus(status);
					break;
				}
				default:
				{
					size_t packetLength = header.packetLength();
					Util::log("[PacketHandler::processPacket] Unknown packet! Abort. Command=%d, packetLength=%d\n", header.command(), packetLength);
					running = false;
				}
			}
		}

		delete[] io_vec->iov_base;
		delete io_vec;
	}
	return n;
}

int PacketHandler::close()
{
	return m_peer.close();
}
