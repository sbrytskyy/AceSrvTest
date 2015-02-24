#pragma once

#include <ace/SOCK_Stream.h>
//#include <ace/Log_Msg.h>

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
	PacketHandler(const ACE_SOCK_Stream &peer) : m_peer(peer) {}
	virtual ~PacketHandler() {}

	void sendLogin(Login& login);
	void sendStatus(Status& status);

	int processPacket(PacketListener& listener);

	ACE_SOCK_Stream& peer() { return m_peer; }
	int close();

private:
	template<class T> void sendPacket(T& packet, Header::Command command)
	{
		ACE_OutputCDR ocdr;
		ocdr << packet;

		size_t packetLength = ocdr.length();

		ACE_OutputCDR ocdrHeader;
		Header header(command, packetLength);
		ocdrHeader << header;

		iovec* io_vec = new iovec[2];

		io_vec[0].iov_base = ocdrHeader.begin()->rd_ptr();
		io_vec[0].iov_len = ocdrHeader.length();

		io_vec[1].iov_base = ocdr.begin()->rd_ptr();
		io_vec[1].iov_len = packetLength;

		//Util::dumpMessage(io_vec, 2, false);

		if (peer().sendv(io_vec, 2) == -1)
		{
			std::cerr << "Error sending " << command << std::endl;
		}
		//ACE_DEBUG((LM_DEBUG, "%m\n"));

		delete io_vec;
	}
	
	ACE_SOCK_Stream m_peer;
};

