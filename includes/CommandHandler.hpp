#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
public:
    // Constructeur
    Client(int socketFd, const std::string& nickname, const std::string& username, const std::string& realName, const std::string& host);

    // Destructeur
    ~Client();

    // Getter pour le descripteur de fichier du socket client
    int getSocketFd() const;

    // Setters et getters pour les informations du client
    void setNickname(const std::string& nickname);
    std::string getNickname() const;

    void setUsername(const std::string& username);
    std::string getUsername() const;

    void setRealName(const std::string& realName);
    std::string getRealName() const;

    void setHost(const std::string& host);
    std::string getHost() const;

    // Méthode pour envoyer un message au client
    void sendMessage(const std::string& message) const;

    // Méthode pour recevoir un message du client
    std::string receiveMessage() const;

private:
    int socketFd;              // Descripteur de fichier pour le socket du client
    std::string nickname;      // Surnom du client utilisé dans les canaux
    std::string username;      // Nom d'utilisateur pour l'authentification
    std::string realName;      // Nom réel du client, pour des informations supplémentaires
    std::string host;          // Hôte/IP du client
};

#endif // CLIENT_HPP
