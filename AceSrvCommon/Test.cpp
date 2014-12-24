#include "ace/ACE.h"
#include "ace/Message_Block.h"

#include "User.h"

#include <iostream>
#include <conio.h>

int ACE_TMAIN(int, ACE_TCHAR *[])
{
	User user1(11, "uranium");
	ACE_OutputCDR ocdr;

	User::marshall(ocdr, user1);

	ACE_InputCDR icdr(ocdr);
	User user2;
	
	std::cout << "1. user pid: " << user2.getPid() << ", name: " << user2.getName() << std::endl;
	
	User::unmarshall(icdr, user2);

	std::cout << "2. user pid: " << user2.getPid() << ", name: " << user2.getName() << std::endl;

	_getch();

	return 0;
}

