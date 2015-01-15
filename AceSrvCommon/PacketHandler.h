#pragma once

#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#include <iostream>

#include "PacketListener.h"
#include "Login.h"
#include "Header.h"
#include "Status.h"
#include "Util.h"


class PacketHandler
{
public:
	PacketHandler() {}
	virtual ~PacketHandler() {}

	static void sendLogin(ACE_SOCK_Stream& peer, Login& login);
	static void sendStatus(ACE_SOCK_Stream& peer, Status& status);

	static void processPacket(ACE_SOCK_Stream& peer, PacketListener& listener);

	ACE_SOCK_Stream& peer() { return m_peer; }

private:
	template<class T> static void sendPacket(ACE_SOCK_Stream& peer, T& packet, Header::Command command)
	{
		ACE_OutputCDR ocdrHeader;
		Header header(command);
		ocdrHeader << header;

		ACE_OutputCDR ocdr;
		ocdr << packet;

		iovec* io_vec = new iovec[2];

		io_vec[0].iov_base = ocdrHeader.begin()->rd_ptr();
		io_vec[0].iov_len = ocdrHeader.length();

		io_vec[1].iov_base = ocdr.begin()->rd_ptr();
		io_vec[1].iov_len = ocdr.length();

		Util::dumpMessage(io_vec, 2, false);

		if (peer.sendv(io_vec, 2) == -1)
		{
			std::cerr << "Error sending Status." << std::endl;
		}

		delete io_vec;
	}

	ACE_SOCK_Stream m_peer;
};

