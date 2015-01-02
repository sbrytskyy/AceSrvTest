#include "Header.h"

int operator<< (ACE_OutputCDR &cdr, const Header &header)
{
	cdr << ACE_CDR::Long(header.command());

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Header &header)
{
	ACE_CDR::Long command;
	cdr >> command;
	header.command(command);

	return cdr.good_bit();
}


