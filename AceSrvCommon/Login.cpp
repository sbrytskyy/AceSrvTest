#include "Login.h"

#include <ace/ACE_export.h>

// iostream operators for Login.

int operator<< (ACE_OutputCDR &cdr, const Login &Login)
{
	cdr << ACE_CDR::Long(Login.pid());
	
	int szLen = Login.name().length() + 1;
	cdr << ACE_CDR::Long(szLen);
	cdr.write_char_array(Login.name().c_str(), szLen);

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Login &Login)
{
	ACE_CDR::Long pid;
	cdr >> pid;
	Login.pid(pid);

	ACE_CDR::Long buffer_len;
	cdr >> buffer_len;

	ACE_TCHAR* buffer = new ACE_TCHAR[buffer_len];

	cdr.read_char_array(buffer, buffer_len);
	//buffer[buffer_len] = '\0';

	Login.name(buffer);

	delete[] buffer;

	return cdr.good_bit();
}
