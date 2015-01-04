#pragma once

#include "ace/CDR_Stream.h"

class Header
{
public:

	enum Command {
		STATUS = 1,
		LOGIN = 2,
		MESSAGE = 3
	};

	Header() : m_lCommand(-1) {}
	Header(Header& header) : m_lCommand(header.command()) {}
	Header(long command) : m_lCommand(command) {}
	virtual ~Header() {}

	const long command(void) const { return m_lCommand; }

private:
	void command(long command) { m_lCommand = command; }

	friend int operator>> (ACE_InputCDR &cdr, Header &header);

	long m_lCommand;
};

// iostream operators for Header.
int operator>> (ACE_InputCDR &cdr, Header &header);
int operator<< (ACE_OutputCDR &cdr, const Header &header);
