#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <unistd.h>

class Channel;

class Client {
public:
    // Constructeur explicite
    explicit Client(int socket, const std::string& nickname, const std::string& username);

    ~Client();


    // Méthodes pour la gestion des clients et des canaux (décommentez ou implémentez selon les besoins)
    // int getSocket() const;
    // std::string getNickname() const;
    // void setNickname(const std::string& nickname);
    // std::string getUsername() const;
    // void sendMessage(const std::string& message) const;
    // void joinChannel(Channel* channel);
    // void leaveChannel(const std::string& channelName);
    // std::vector<Channel*> getChannels() const;

private:
    int socket; // Descripteur de fichier pour la connexion du client
    std::string nickname; // Pseudo du client
    std::string username; // Nom d'utilisateur du client
    std::vector<Channel*> channels; // Canaux auxquels le client est connecté

    Client(const Client& other);
    Client& operator=(const Client& other);
};

#endif // CLIENT_HPP
