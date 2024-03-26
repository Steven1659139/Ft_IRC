#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

Client::Client(int socket, const std::string& nickname, const std::string& username) : socket(socket), nickname(nickname), username(username) {}

Client::~Client() {
    close(socket);
}

std::string Client::getUsername() const
{
    return(username);
}

void Client::setNickname(const std::string &nickname)
{
    this->nickname = nickname;
    return ;
}

void Client::joinChannel(Channel* ch)
{
    try
    {
        channels.at(ch->getName());
        std::cerr << "You have already joined " << ch->getName() << "!" << std::endl;
    }
    catch(std::exception &e)
    {
        channels.insert(std::make_pair(ch->getName(), ch));
    }
}

void Client::leaveChannel(const std::string &channelName)
{
    try
    {
        channels.at(channelName);
        channels.erase(channelName);
    }
    catch(std::exception &e)
    {
        std::cerr << "You have not joined any channels named " << channelName << "!" << std::endl;
    }
}