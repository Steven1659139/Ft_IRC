
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include "Client.hpp" // Assurez-vous que ce fichier existe et est correctement structuré.

class Server {
public:
    // Constructeur explicite
    explicit Server(int port, const std::string& password);
    
    // Destructeur
    ~Server();



    // Méthode pour démarrer le serveur et entrer dans la boucle principale d'écoute
    void run();

private:
    int port;                           // Port sur lequel le serveur écoute
    std::string password;               // Mot de passe requis pour la connexion
    int serverSocket;                   // Descripteur de fichier pour le socket serveur
    std::map<int, Client> clients;      // Cartographie des clients connectés, avec le descripteur de fichier comme clé


    // On veux pas que ça puisse être copier, c'est le serveur c'est pour ça que c'est dans private et que ce seras pas implémenter
    Server(const Server&);              // Constructeur de copie
    Server& operator=(const Server&);   // Opérateur d'assignation

    // Initialiser le socket serveur
    // void setupServerSocket();

    // Accepter une nouvelle connexion
    // void acceptNewConnection();

    // Traiter les données reçues d'un client
    // void handleClientData(int clientSocket);

    // Envoyer un message à tous les clients connectés (sauf l'expéditeur)
    // void broadcastMessage(const std::string& message, int senderFd);

    // Fermer proprement une connexion client
    // void closeClientConnection(int clientSocket);

    // Vérifier si le mot de passe fourni est correct
    // bool authenticateClient(const std::string& receivedPassword);
};

#endif // SERVER_HPP