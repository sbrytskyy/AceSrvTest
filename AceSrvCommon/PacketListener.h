#pragma once

#include "Status.h"
#include "Login.h"

class PacketListener
{
public:
	PacketListener() {}
	virtual ~PacketListener() {}

	virtual void onStatus(Status& status) = 0;
	virtual void onLogin(Login& login) = 0;
};

