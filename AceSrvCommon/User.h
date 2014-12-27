#pragma once

#include "ace/CDR_Stream.h"

#include <string>

class User
{
public:
	User() : lPid(-1) {};
	User(long _lPid, std::string _name) : lPid(_lPid), name(_name) {};
	virtual ~User() {};

	std::string& getName() { return name; }
	long getPid() const { return lPid; }

	void setPid(long _lPid) { lPid = _lPid; }
	void setName(std::string _name) { name = _name; }
	void setName(char* _name) { name.assign(_name); }

	static int writeExternal(ACE_OutputCDR& cdr, User& user);
	static int readExternal(ACE_InputCDR& cdr, User& user);

private:
	long lPid;
	std::string name;
};

