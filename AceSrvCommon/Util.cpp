#include "Util.h"

void Util::dumpMessage(iovec * io_vec, bool incoming)
{
	if (incoming)
	{
		std::cout << "[RECEIVE] ";
	}
	else
	{
		std::cout << "[SEND] ";
	}
	std::cout << "io_vec->iov_len: " << io_vec->iov_len << ", io_vec->iov_base:[";
	ACE::write_n(ACE_STDOUT, io_vec->iov_base, io_vec->iov_len);
	std::cout << "]" << std::endl;
}



