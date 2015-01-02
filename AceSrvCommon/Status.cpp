#include "Status.h"

int operator<< (ACE_OutputCDR &cdr, const Status &status)
{
	cdr << ACE_CDR::Long(status.code());

	return cdr.good_bit();
}

int operator>> (ACE_InputCDR &cdr, Status &status)
{
	ACE_CDR::Long code;
	cdr >> code;
	status.code(code);

	return cdr.good_bit();
}
