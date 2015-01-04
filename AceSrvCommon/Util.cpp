#include "Util.h"
#include "Header.h"

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
			std::cout << (int)io_vec->iov_base[i] << ":";
		}
		std::cout << std::dec;
		std::cout << "]" << std::endl;

		io_vec++;
		counter++;
	}
}

void Util::sendLogin(ACE_SOCK_Stream& peer, Login& login)
{
	ACE_OutputCDR ocdrHeader;
	Header header(Header::Command::LOGIN);
	ocdrHeader << header;

	ACE_OutputCDR ocdrLogin;
	ocdrLogin << login;

	iovec* io_vec = new iovec[2];

	io_vec[0].iov_base = ocdrHeader.begin()->rd_ptr();
	io_vec[0].iov_len = ocdrHeader.length();

	io_vec[1].iov_base = ocdrLogin.begin()->rd_ptr();
	io_vec[1].iov_len = ocdrLogin.length();

	Util::dumpMessage(io_vec, 2, false);

	if (peer.sendv(io_vec, 2) == -1)
	{
		std::cerr << "Error sending data." << std::endl;
	}

	delete io_vec;
}



