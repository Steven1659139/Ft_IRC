#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Channel; // Déclaration anticipée pour éviter une inclusion cyclique.

class Client {
public:
    // Constructeur
    Client(int socket, const std::string& nickname, const std::string& username);

    // Destructeur
    ~Client();

    // Obtenir l'identifiant du socket client
    // int getSocket() const;

    // Obtenir le pseudo du client
    // std::string getNickname() const;

    // Définir le pseudo du client
    // void setNickname(const std::string& nickname);

    // Obtenir le nom d'utilisateur du client
    // std::string getUsername() const;

    // Envoyer un message au client
    // void sendMessage(const std::string& message) const;

    // Ajouter un client à un canal
    // void joinChannel(Channel* channel);

    // Retirer un client d'un canal
    // void leaveChannel(const std::string& channelName);

    // Obtenir la liste des canaux auxquels le client est connecté
    // std::vector<Channel*> getChannels() const;

private:
    int socket; // Descripteur de fichier pour la connexion du client
    std::string nickname; // Pseudo du client
    std::string username; // Nom d'utilisateur du client
    std::vector<Channel*> channels; // Canaux auxquels le client est connecté
};

#endif // CLIENT_HPP
