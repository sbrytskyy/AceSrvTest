#pragma once

#include "ace/CDR_Stream.h"

class Header
{
public:

	enum Command {
		LOGIN = 0,
		MESSAGE = 1
	};

	Header() : m_lCommand(-1) {}
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
