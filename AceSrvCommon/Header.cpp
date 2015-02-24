#include "Header.h"

int operator<< (ACE_OutputCDR &cdr, const Header &header)
{
	cdr << ACE_CDR::Long(header.command());
	cdr << ACE_CDR::Long(header.packetLength());

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Header &header)
{
	ACE_CDR::Long command;
	cdr >> command;
	header.command(command);

	ACE_CDR::Long packetLength;
	cdr >> packetLength;
	header.packetLength(packetLength);

	return cdr.good_bit();
}


