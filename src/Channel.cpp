#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"


Channel::Channel(const std::string& name, const std::string& topic) : name(name), topic(topic)
{
	modes.istherekey = false;
	modes.inviteonly = false;
	modes.key = "";
	modes.istherelimit = false;
	modes.limit = 0;
	modes.modtopic = false;
	setcoms["+k"] = &Channel::setKey;
	setcoms["-k"] = &Channel::remKey;
	setcoms["+t"] = &Channel::setModTopic;
	setcoms["-t"] = &Channel::remModTopic;
	setcoms["+i"] = &Channel::setInvite;
	setcoms["-i"] = &Channel::remInvite;
	setcoms["+l"] = &Channel::setLimit;
	setcoms["-l"] = &Channel::remLimit;
}

void Channel::setKey(Client& client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE: +k"));
		return ;
	}
	if (args.size() > 2)
	{
		Utils::ft_send(client.getSocket(), ERR_UNKOWNCOMMAND(client.getNickname(), "MODE :+k :too many params"));
		return ;
	}
	if (modes.istherekey == true)
	{
		Utils::ft_send(client.getSocket(), ERR_KEYSET(client.getNickname(), getName()));
		return ;
	}
	modes.istherekey = true;
	modes.key = args[1];
}

void Channel::remKey(Client& client, const std::vector<std::string>& args)
{
	if (args.size() > 2)
	{
		Utils::ft_send(client.getSocket(), ERR_UNKOWNCOMMAND(client.getNickname(), "MODE :+k :too many params"));
		return ;
	}
	modes.istherekey = false;
	modes.key.clear();
}

void Channel::setModTopic(Client& client, const std::vector<std::string>& args)
{
	(void)client;
	(void)args;
	modes.modtopic = true;
}

void Channel::remModTopic(Client& client, const std::vector<std::string>& args)
{
	(void)client;
	(void)args;
	modes.modtopic = false;
}

void Channel::setInvite(Client& client, const std::vector<std::string>& args)
{
	(void)client;
	(void)args;
	modes.inviteonly = true;
}

void Channel::remInvite(Client& client, const std::vector<std::string>& args)
{
	(void)client;
	(void)args;
	modes.inviteonly = false;
}

void Channel::setLimit(Client& client, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE: +k"));
		return ;
	}
	if (args.size() > 2)
	{
		Utils::ft_send(client.getSocket(), ERR_UNKOWNCOMMAND(client.getNickname(), "MODE :+k :too many params"));
		return ;
	}
	modes.istherelimit = true;
	modes.limit = std::stoi(args[1]);
}

void Channel::remLimit(Client& client, const std::vector<std::string>& args)
{
	if (args.size() > 2)
	{
		Utils::ft_send(client.getSocket(), ERR_UNKOWNCOMMAND(client.getNickname(), "MODE :+k :too many params"));
		return ;
	}
	modes.istherelimit = false;
	modes.limit = 0;
}

void Channel::setModes(Client &client, const std::vector<std::string>& args)
{
	if (setcoms.count(args[0]) < 1)
	{
		Utils::ft_send(client.getSocket(), ERR_UNKOWNMODE(client.getNickname(), args[0], getName()));
		return ;
	}
	SetFunc com = setcoms[args[0]];
	(this->*com)(client, args);
	Utils::ft_send(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), getName(), strModes()));
}

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

t_modes Channel::getModes() const
{
	return (modes);
}

std::string Channel::strModes() const
{
	std::string smodes;
	std::string temp;
	if (modes.istherekey == true)
	{
		smodes.append("+k");
		temp.append(modes.key);
	}
	if (modes.inviteonly == true)
		smodes.append("+i");
	if (modes.modtopic == true)
		smodes.append("+t");
	if (modes.istherelimit == true)
	{
		smodes.append("+l");
		temp.append(std::to_string(modes.limit));
	}
	if (!temp.empty())
		smodes += " " + temp;
	return (smodes);
}
