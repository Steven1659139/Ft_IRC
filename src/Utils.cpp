#include "../includes/Utils.hpp"

int Utils::ft_send(int fd, const std::string buffer)
{
	int i = 1;
	int j = 0;
	std::string appender = "\r\n";
	std::string appenderone = "\r";
	std::string appendertwo = "\n";
	std::string::size_type itone = buffer.find_last_of(appenderone);
	std::string::size_type itwo = buffer.find_last_not_of(appendertwo);
	int flag = 0;
	if ((buffer.back() != '\n') || (itone == std::string::npos || itwo == std::string::npos))
		flag = 1;
	if (buffer.size() > 510)
	{
		j = buffer.size() - 510;
		while (j > 510)
		{
			i++;
			j = buffer.size() - (510 * i);
		}
		if (j > 0)
			i++;
		j = 0;
		while (j < i)
		{
			std::string temp = buffer.substr((j * 510), ((j + 1) * 510));
			send(fd, temp.c_str(), temp.size(), MSG_DONTWAIT);
			j++;
		}
		if (flag == 1)
			send(fd, appender.c_str(), appender.size(), MSG_DONTWAIT);
		return (buffer.size());
	}
	else
	{
		std::string temp = buffer;
		if (flag == 1)
			temp.append(appender);
		send(fd, temp.c_str(), temp.size(), MSG_DONTWAIT);
		temp.clear();
		return (buffer.size());
	}
	return (0);
}