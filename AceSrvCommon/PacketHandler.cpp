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
	Util::log("[PacketHandler::processPacket] peer().recvv(io_vec) = %d\n", n);
	if (n > 0)
	{
		//ACE_ERROR((LM_ERROR, ACE_TEXT("%N:%l: Failed to receive request. (errno = %i: %m)\n"), ACE_ERRNO_GET));
		//ACE_DEBUG((LM_DEBUG, "Client::receiving_thread:(%P|%t):%d:%m\n", errno));
		Util::dumpMessage(io_vec, 1, true);

		ACE_InputCDR icdr(io_vec->iov_base, n);

		while (true)
		{
			size_t left = icdr.length();
			//Util::log("[PacketHandler::processPacket] left = %d\n", left);
			if (left <= 1) break;

			Header header;
			icdr >> header;

			size_t packetLength = header.packetLength();

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
					ACE_CDR::Boolean skip = icdr.skip_bytes(packetLength);
					Util::log("[PacketHandler::processPacket] Unknown packet! Command=%d, packetLength=%d, skip=%d\n", header.command(), packetLength, skip);
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
