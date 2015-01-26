#include "Util.h"

#include <ace/OS_NS_sys_time.h>

void Util::dumpMessage(const iovec * io_vec, const size_t size, bool incoming)
{

	if (incoming)
	{
		//std::cout << "[RECEIVE]";
		Util::log("%s", "[RECEIVE]");
	}
	else
	{
		//std::cout << "[SEND]";
		std::cout << std::endl << std::endl;
		Util::log("%s", "[SEND]");
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
	std::cout << std::endl;
}


void Util::log(const char *pszFormat, ...)
{
	va_list ap;

	//long ms = (long)time(NULL);

	time_t rawtime;
	rawtime = time(NULL);
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);

	ACE_Time_Value detail_time = ACE_OS::gettimeofday();

	//printf("[%d] %d.%d.%d, %d:%d:%d.%lu ",
	printf("%d.%d.%d, %d:%d:%d.%lu ",
		//timeinfo.tm_wday, /* Mon - Sun */
		timeinfo.tm_mday,
		timeinfo.tm_mon + 1,
		timeinfo.tm_year + 1900,
		timeinfo.tm_hour,
		timeinfo.tm_min,
		timeinfo.tm_sec,
		detail_time.usec() / 1000);

	va_start(ap, pszFormat);

	vprintf(pszFormat, ap);
	//ACE_DEBUG((LM_DEBUG, pszFormat, ap));

	va_end(ap);

	
}
