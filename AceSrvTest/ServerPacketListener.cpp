#include "ServerPacketListener.h"
#include "Util.h"


void ServerPacketListener::onStatus(Status& status)
{
	//std::cout << "[ChatServer::onStatus] status=[code: " << status.code() << "]" << std::endl;
	Util::log("[ChatServer::onStatus] status=[code: %d]\n", status.code());

}

void ServerPacketListener::onLogin(Login& login)
{
	//std::cout << "[ChatServer::onLogin] login=[pid: " << login.pid() << ", name: " << login.name() << "]" << std::endl;
	Util::log("[ChatServer::onLogin] login = [pid: %d, name : %s]\n", login.pid(), login.name().c_str());

	long code = 0;
	Status status(code);
	//m_handler.sendStatus(status);
}
