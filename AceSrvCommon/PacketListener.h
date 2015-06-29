#pragma once

#include "Status.h"
#include "Login.h"
#include "acemsgr.pb.h"

class PacketListener
{
public:
	PacketListener() {}
	virtual ~PacketListener() {}

	virtual void onStatus(Status& status) = 0;
	virtual void onLogin(Login& login) = 0;

	virtual void onLogin(acemsgr::Login& login) = 0;
};

