#pragma once

#include "ace/CDR_Stream.h"

class Status
{
public:
	Status() : m_lCode(-1) {};
	Status(long code) : m_lCode(code) {}
	virtual ~Status() {}

	const long code(void) { return m_lCode; }

	static int writeExternal(ACE_OutputCDR& cdr, Status& user);
	static int readExternal(ACE_InputCDR& cdr, Status& user);

private:
	void code(long code) { m_lCode = code; }

	long m_lCode;
};

