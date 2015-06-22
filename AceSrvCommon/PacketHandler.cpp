#include "PacketHandler.h"
#include "acemsgr.pb.h"

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

	Util::tlog("[PacketHandler::processPacket] BEFORE int n = peer().recvv(io_vec);\n");

	int n = peer().recvv(io_vec);

	Util::tlog("[PacketHandler::processPacket] AFTER int n = peer().recvv(io_vec); n=%d\n", n);

	ACE_DEBUG((LM_DEBUG, "%m\n"));
	Util::dumpMessage(io_vec, 1, true);

	if (n > 0)
	{
		ACE_InputCDR icdr(io_vec->iov_base, n, ACE_CDR::BYTE_ORDER_BIG_ENDIAN);

		Header header;
		icdr >> header;

		switch (header.command())
		{
		case Header::Command::LOGIN:
			{
				/*Login login;
				icdr >> login;
				listener.onLogin(login);*/

				acemsgr::Login login;
				if (login.ParseFromArray(io_vec->iov_base + Header::HEADER_SIZE, header.psize()))
				{
					// TODO onLogin
					Util::log("\n\n\n");
					Util::tlog("[PacketHandler::processPacket] Login = [id: %d, name='%s'", login.id(), login.name().c_str());
					if (login.has_email()) {
						Util::log(", E-mail address='%s'", login.email().c_str());
					}
					Util::log("]\n\n\n");
				}
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

/*		*/
	}

	delete[] io_vec->iov_base;
	delete io_vec;

	return n;
}

int PacketHandler::close()
{
	return m_peer.close();
}
