#pragma once

#include "ace/CDR_Stream.h"

#include <string>

class User
{
public:
	User() : m_lPid(-1) {};
	User(long _lPid, std::string _name) : m_lPid(_lPid), m_Name(_name) {};
	virtual ~User() {};

	const std::string& name(void) { return m_Name; }
	const long pid(void) const { return m_lPid; }

	void pid(long _lPid) { m_lPid = _lPid; }
	void name(std::string _name) { m_Name = _name; }
	void name(char* _name) { m_Name.assign(_name); }

	static int writeExternal(ACE_OutputCDR& cdr, User& user);
	static int readExternal(ACE_InputCDR& cdr, User& user);

private:
	long m_lPid;
	std::string m_Name;
};

