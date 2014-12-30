#include "Status.h"


int Status::writeExternal(ACE_OutputCDR& cdr, Status& status)
{
	cdr << ACE_CDR::Long(status.code());

	return cdr.good_bit();
}

int Status::readExternal(ACE_InputCDR& cdr, Status& status)
{
	ACE_CDR::Long code;
	cdr >> code;
	status.code(code);

	return cdr.good_bit();
}
