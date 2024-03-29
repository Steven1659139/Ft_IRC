#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

Client::Client(int socket, const std::string& nickname, const std::string& username) : socket(socket), nickname(nickname), username(username) {}

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
    return ;
}

void Client::joinChannel(Channel* ch)
{
    size_t i = channels.count(ch->getName());
    if (i == 0)
        channels.insert(std::make_pair(ch->getName(), ch));
    else
        std::cerr << "You have already joined a channel named " << ch->getName() << "!" << std::endl;
}

void Client::leaveChannel(const std::string &channelName)
{
    size_t i = channels.count(channelName);
    if (i == 1)
        channels.erase(channelName);
    else
        std::cerr << "You have not joined any channel named " << channelName << "!" << std::endl;
}

bool Client::sendMessage(const std::string& message) const {
    // Ajout de "\r\n" à la fin du message pour respecter le protocole IRC
    std::string formattedMessage = message + "\r\n";

    ssize_t bytesSent = send(socket, formattedMessage.c_str(), formattedMessage.length(), 0);

    if (bytesSent < 0) {
        std::cerr << "Erreur lors de l'envoi du message." << std::endl;
        return false; 
    }

    return true; 
}