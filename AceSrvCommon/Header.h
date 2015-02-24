#pragma once

#include "ace/CDR_Stream.h"

class Header
{
public:

	enum Command 
	{
		STATUS = 1,
		LOGIN = 2,
		MESSAGE = 3
	};

	Header() : m_lCommand(-1) {}
	Header(Header& header, size_t packetLength) : m_lCommand(header.command()), m_packetLength(packetLength) {}
	Header(long command, size_t packetLength) : m_lCommand(command), m_packetLength(packetLength) {}
	virtual ~Header() {}

	const long command(void) const { return m_lCommand; }
	const long packetLength(void) const { return m_packetLength; }

private:
	void command(long command) { m_lCommand = command; }
	void packetLength(long packetLength) { m_packetLength = packetLength; }

	friend int operator>> (ACE_InputCDR &cdr, Header &header);

	long m_lCommand;
	size_t m_packetLength;
};

// iostream operators for Header.
int operator>> (ACE_InputCDR &cdr, Header &header);
int operator<< (ACE_OutputCDR &cdr, const Header &header);
