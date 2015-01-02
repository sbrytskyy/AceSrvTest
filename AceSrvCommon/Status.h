#pragma once

#include "ace/CDR_Stream.h"

class Status
{
public:
	Status() : m_lCode(-1) {}
	Status(long code) : m_lCode(code) {}
	virtual ~Status() {}

	const long code(void) const { return m_lCode; }

private:
	void code(long code) { m_lCode = code; }

	friend int operator>> (ACE_InputCDR &cdr, Status &status);

	long m_lCode;
};

// iostream operators for Status.
int operator>> (ACE_InputCDR &cdr, Status &status);
int operator<< (ACE_OutputCDR &cdr, const Status &status);
