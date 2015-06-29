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
	Util::tlog("[PacketHandler::processPacket] BEFORE reading socket;\n");

	char* header = new char[Header::HEADER_SIZE];

	long n = peer().recv(header, Header::HEADER_SIZE);

	Util::tlog("[PacketHandler::processPacket] AFTER reading socket; n=%d\n", n);

	//ACE_DEBUG((LM_DEBUG, "%m\n"));
	//Util::dumpMessage(io_vec, 1, true);

	if (n > 0)
	{
		ACE_InputCDR icdr(header, n, ACE_CDR::BYTE_ORDER_BIG_ENDIAN);

		Header header;
		icdr >> header;

		char* body = new char[header.psize()];
		long n = peer().recv(body, header.psize());

		switch (header.command())
		{
		case Header::Command::LOGIN:
			{
				/*Login login;
				icdr >> login;
				listener.onLogin(login);*/

				acemsgr::Login login;
				if (login.ParseFromArray(body, header.psize()))
				{
					// TODO onLogin
					Util::log("\n\n\n");
					Util::tlog("[PacketHandler::processPacket] Login = [id: %d, name='%s'", login.id(), login.name().c_str());
					if (login.has_email()) {
						Util::log(", E-mail address='%s'", login.email().c_str());
					}
					Util::log("]\n\n\n");

					listener.onLogin(login);
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

		delete[] body;

/*		*/
	}

	//delete[] io_vec->iov_base;
	delete[] header;
	//delete io_vec;

	return n;
}

int PacketHandler::close()
{
	return m_peer.close();
}
