#include <ace/ACE.h>
#include "ace/Signal.h"
#include "ace/Thread_Manager.h"

#include "ChatServer.h"

namespace {
	extern "C" void sigterm_handler(int /* signum */) { /* No-op. */ }
}


int ACE_TMAIN(int, ACE_TCHAR *[])
{
	Util::log("%s\n", "[Server] START");

	// Register to receive the <SIGTERM> signal.
	ACE_Sig_Action sa((ACE_SignalHandler)sigterm_handler, SIGTERM);

	ChatServer chatServer;
	chatServer.run();

	// Cooperative thread cancellation and barrier synchronization.
	ACE_Thread_Manager::instance()->cancel_all();
	return ACE_Thread_Manager::instance()->wait();
}