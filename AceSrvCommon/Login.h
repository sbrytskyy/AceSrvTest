#pragma once

#include <ace/CDR_Stream.h>

#include <string>

class Login
{
public:
	Login() : m_lPid(-1) {}
	Login(long _lPid, std::string _name) : m_lPid(_lPid), m_Name(_name) {}
	virtual ~Login() {}

	const std::string& name(void) const { return m_Name; }
	const long pid(void) const { return m_lPid; }

	void pid(long _lPid) { m_lPid = _lPid; }
	void name(std::string _name) { m_Name = _name; }
	void name(char* _name) { m_Name.assign(_name); }

private:
	long m_lPid;
	std::string m_Name;
};

// iostream operators for Login.
int operator>> (ACE_InputCDR &cdr, Login &Login);
int operator<< (ACE_OutputCDR &cdr, const Login &Login);

