#include <ace/ACE.h>

#define BYTE_ORDER BIG_ENDIAN

#include "ChatServer.h"

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	Util::tlog("%s\n", "[Server] START");

	ChatServer chatServer;
	chatServer.run();

	//chatServer.test();

	return EXIT_SUCCESS;
}