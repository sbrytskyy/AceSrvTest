#include <ace/ACE.h>

#include "ChatServer.h"

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	Util::log("%s\n", "[Server] START");

	ChatServer chatServer;
	chatServer.run();

	return EXIT_SUCCESS;
}