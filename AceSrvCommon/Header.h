#pragma once

#include "ace/CDR_Stream.h"

class Header
{
public:

	static const long HEADER_SIZE = 8;

	enum Command 
	{
		STATUS = 1,
		LOGIN = 2,
		MESSAGE = 3
	};

	Header() : m_lCommand(-1), m_lPSize(0) {}
	Header(Header& header) : m_lCommand(header.command()), m_lPSize(header.psize()) {}
	Header(long command, long psize) : m_lCommand(command), m_lPSize(psize) {}
	virtual ~Header() {}

	const long command(void) const { return m_lCommand; }
	const long psize(void) const { return m_lPSize; }

private:
	void command(long command) { m_lCommand = command; }
	void psize(long psize) { m_lPSize = psize; }

	friend int operator>> (ACE_InputCDR &cdr, Header &header);

	long m_lCommand;
	long m_lPSize;
};

// iostream operators for Header.
int operator>> (ACE_InputCDR &cdr, Header &header);
int operator<< (ACE_OutputCDR &cdr, const Header &header);
