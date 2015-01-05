#include <ace/ACE.h>

#include "ChatServer.h"

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	ChatServer chatServer;
	chatServer.run();

	return EXIT_SUCCESS;
}