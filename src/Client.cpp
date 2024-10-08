#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"

Client::Client(int socket, const std::string& nickname, const std::string& username) : socket(socket), nickname(nickname), username(username)
{
    auth = false;
    goodPass = false;
}

Client::~Client() {
    close(socket);
}

int Client::getSocket() const
{
    return (socket);
}

std::string Client::getUsername() const
{
    return(username);
}

std::string Client::getNickname() const
{
    return(nickname);
}

void Client::setNickname(const std::string &nickname)
{
    this->nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
    this->username = username;
}

void Client::setAuth(bool value) {
    auth = value;
}

bool Client::isAuth() const
{
    if (auth)
        return true;
    else
    {
        return false;
    }
}


void Client::joinChannel(Channel* ch)
{
    size_t i = channels.count(ch->getName());
    if (i == 0)
        channels.insert(std::make_pair(ch->getName(), ch));
    else
        Utils::ft_send(socket, ERR_USERONCHANNEL(nickname, nickname,ch->getName()));
}

void Client::appendToBuffer(std::string message)
{
    commandbuffer.append(message);
}

bool    Client::checkForEndChars() const
{
    std::string appenderone = "\r";
	std::string appendertwo = "\n";
    std::string::size_type itone = commandbuffer.find_last_of(appenderone);
	std::string::size_type itwo = commandbuffer.find_last_not_of(appendertwo);
    if (itone == std::string::npos || itwo == std::string::npos)
        return false;
    if (commandbuffer.back() != '\n')
        return false;
    if (commandbuffer.back() == '\n' && commandbuffer[commandbuffer.length() - 2] != '\r')
        return false;
    return true;
}

void Client::clearBuffer()
{
    commandbuffer.clear();
}

std::string Client::getCommandBuffer() const
{
    return (commandbuffer);
}

void Client::leaveChannel(const std::string &channelName)
{
    size_t i = channels.count(channelName);
    if (i == 1)
        channels.erase(channelName);
    else
        Utils::ft_send(socket, ERR_NOTONCHANNEL(nickname, channelName));
}

bool Client::sendMessage(const std::string& message) const {
    std::string formattedMessage = message + "\r\n";

    ssize_t bytesSent = send(socket, formattedMessage.c_str(), formattedMessage.length(), 0);

    if (bytesSent < 0) {
        std::cerr << "Erreur lors de l'envoi du message." << std::endl;
        return false; 
    }

    return true; 
}