#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include "Client.hpp" // Supposons que vous ayez une classe Client pour gérer les connexions clients.

class Server {
public:
    // Constructeur
    Server(int port, const std::string& password);
    
    // Destructeur
    ~Server();

    // Méthode pour démarrer le serveur et entrer dans la boucle principale d'écoute
    void run();

private:
    int port;                           // Port sur lequel le serveur écoute
    std::string password;               // Mot de passe requis pour la connexion
    int serverSocket;                   // Descripteur de fichier pour le socket serveur
    std::map<int, Client> clients;      // Cartographie des clients connectés, avec le descripteur de fichier comme clé

    // Initialiser le socket serveur
    void setupServerSocket();

    // Accepter une nouvelle connexion
    void acceptNewConnection();

    // Traiter les données reçues d'un client
    void handleClientData(int clientSocket);

    // Envoyer un message à tous les clients connectés (sauf l'expéditeur)
    void broadcastMessage(const std::string& message, int senderFd);

    // Fermer proprement une connexion client
    void closeClientConnection(int clientSocket);

    // Vérifier si le mot de passe fourni est correct
    bool authenticateClient(const std::string& receivedPassword);
};

#endif // SERVER_HPP
