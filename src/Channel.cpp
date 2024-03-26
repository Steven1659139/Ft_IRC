#include "../includes/Channel.hpp"

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

void Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

std::string Channel::getTopic() const
{
	return (topic);
}