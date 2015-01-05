#include "Util.h"

void Util::dumpMessage(const iovec * io_vec, const size_t size, bool incoming)
{
	if (incoming)
	{
		std::cout << "[RECEIVE]";
	}
	else
	{
		std::cout << "[SEND]";
	}
	if (size == -1)
	{
		std::cout << " empty packet." << std::endl;
		return;
	}
	std::cout << std::endl;
	size_t counter = 1;
	while (counter <= size)
	{
		std::cout << "\t" << counter << ": io_vec->iov_len: " << io_vec->iov_len << ", io_vec->iov_base:[";
		std::cout << std::hex;
		//ACE::write_n(ACE_STDOUT, io_vec->iov_base, io_vec->iov_len);
		for (unsigned int i = 0; i < io_vec->iov_len; i++)
		{
			std::cout << (int)io_vec->iov_base[i];

			if (i + 1 < io_vec->iov_len)
			{
				std::cout << ":";
			}
		}
		std::cout << std::dec;
		std::cout << "]" << std::endl;

		io_vec++;
		counter++;
	}
}

