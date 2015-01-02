#include "User.h"

#include <ace/ACE_export.h>

// iostream operators for User.

int operator<< (ACE_OutputCDR &cdr, const User &user)
{
	cdr << ACE_CDR::Long(user.pid());
	
	cdr << ACE_CDR::Long(user.name().length());
	cdr.write_char_array(user.name().c_str(), user.name().length());

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, User &user)
{
	ACE_CDR::Long pid;
	cdr >> pid;
	user.pid(pid);

	ACE_CDR::Long buffer_len;
	cdr >> buffer_len;

	ACE_TCHAR* buffer = new ACE_TCHAR[buffer_len+1];

	cdr.read_char_array(buffer, buffer_len);
	buffer[buffer_len] = '\0';

	user.name(buffer);

	delete[] buffer;

	return cdr.good_bit();
}
