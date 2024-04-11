#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"


Channel::Channel(const std::string& name, const std::string& topic) : name(name), topic(topic){}

std::string	Channel::getName() const
{
	return (name);
}

bool Channel::isClientInChannel(const Client *client) const
{
	std::set<Client*>::iterator it = std::find(clients.begin(), clients.end(), client);
	if (it == clients.end())
		return (false);
	return (true);
}

bool Channel::isClientAnOperator(const Client *client) const
{
	std::set<Client*>::iterator it = std::find(operators.begin(), operators.end(), client);
	if (it == operators.end())
		return (false);
	return (true);
}

bool Channel::addClient(Client *client)
{
	if (!isClientInChannel(client))
		clients.insert(client);
	else
		return (false);
	return (true);
}

bool Channel::removeClient(Client* client)
{
	if (isClientInChannel(client))
		clients.erase(client);
	else
		return (false);
	return (true);
}

bool Channel::addOperator(Client *client)
{
	if (isClientInChannel(client))
	{
		if (!isClientAnOperator(client))
		{
			operators.insert(client);
			return (true);
		}
		else
			return (false);
	}
	return (false);
}

bool Channel::removeOperator(Client *client)
{
	if (isClientInChannel(client))
	{
		if (isClientAnOperator(client))
		{
			operators.erase(client);
			return (true);
		}
		else
			return (false);
	}
	return (false);
}

void Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

std::string Channel::getTopic() const
{
	return (topic);
}

std::set<Client*> Channel::getClients() const
{
	return (clients);
}

/*void Channel::sendMessageOnChan(std::string message)
{
	int i = 0;
	Client *temp;
	std::set<Client*>::iterator it = clients.begin();
	while (it != clients.end())
	{
		temp = it;
		Utils::ft_send(it->getSocket(), message);
		it++;
	}
}*/