#include "PacketHandler.h"

#include "Header.h"
#include "Status.h"
#include "Util.h"
#include "PacketListener.h"

void PacketHandler::sendLogin(ACE_SOCK_Stream& peer, Login& login)
{
	ACE_OutputCDR ocdrHeader;
	Header header(Header::Command::LOGIN);
	ocdrHeader << header;

	ACE_OutputCDR ocdrLogin;
	ocdrLogin << login;

	iovec* io_vec = new iovec[2];

	io_vec[0].iov_base = ocdrHeader.begin()->rd_ptr();
	io_vec[0].iov_len = ocdrHeader.length();

	io_vec[1].iov_base = ocdrLogin.begin()->rd_ptr();
	io_vec[1].iov_len = ocdrLogin.length();

	Util::dumpMessage(io_vec, 2, false);

	if (peer.sendv(io_vec, 2) == -1)
	{
		std::cerr << "Error sending Login." << std::endl;
	}

	delete io_vec;
}

void PacketHandler::sendStatus(ACE_SOCK_Stream& peer, Status& status)
{
	ACE_OutputCDR ocdrHeader;
	Header header(Header::Command::STATUS);
	ocdrHeader << header;

	ACE_OutputCDR ocdrStatus;
	ocdrStatus << status;

	iovec* io_vec = new iovec[2];

	io_vec[0].iov_base = ocdrHeader.begin()->rd_ptr();
	io_vec[0].iov_len = ocdrHeader.length();

	io_vec[1].iov_base = ocdrStatus.begin()->rd_ptr();
	io_vec[1].iov_len = ocdrStatus.length();

	Util::dumpMessage(io_vec, 2, false);

	if (peer.sendv(io_vec, 2) == -1)
	{
		std::cerr << "Error sending Status." << std::endl;
	}

	delete io_vec;
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
	case Header::Command::STATUS:
		{
			Status status;
			icdr >> status;
			listener.onStatus(status);
		}
	}

	delete[] io_vec->iov_base;
	delete io_vec;
}