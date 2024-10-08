#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <map>
# include <iostream>
# include <unistd.h>
# include <algorithm>
# include <exception>
# include <utility>
# include <sys/socket.h>

class Channel;
class Server;

class Client {
public:
    // Constructeur explicite
    explicit Client(int socket, const std::string& nickname, const std::string& username);

    ~Client();


    void setAuth(bool value);
    bool isAuth() const;
    bool goodPass;

    // Méthodes pour la gestion des clients et des canaux (décommentez ou implémentez selon les besoins)
    int getSocket() const;
    std::string getNickname() const;
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    std::string getUsername() const;
    bool checkForEndChars() const;
    bool sendMessage(const std::string& message) const;
    std::string getCommandBuffer() const;
    void joinChannel(Channel* ch);
    void appendToBuffer(std::string message);
    void clearBuffer();
    void leaveChannel(const std::string& channelName);
    void recieveMessage(int chansocket, std::string const &msg); // début de la logique pour les messages?
private:
    int socket; // Descripteur de fichier pour la connexion du client
    std::string nickname; // Pseudo du client
    std::string username; // Nom d'utilisateur du client
    std::string commandbuffer;
    std::map<std::string ,Channel*> channels; // Canaux auxquels le client est connecté <- Je le change pour une map pour pouvoir limiter le nombre d'opération
    bool auth;

    Client(const Client& other);
    Client& operator=(const Client& other);
    Client();
};

#endif // CLIENT_HPP
