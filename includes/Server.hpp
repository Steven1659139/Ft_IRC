
#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <ctime>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include "Client.hpp"
# include <utility>
# include <algorithm>

class Server {
public:
    // Le constructeur est explicite c'est surtout une question de sécurité pour eviter les conversions implicites non désirées
    explicit Server(int port, const std::string& password);
    
    ~Server();

    void run();

private:
    int port;                           // Port sur lequel le serveur écoute
    std::string password;               // Mot de passe requis pour la connexion
    int serverSocket;                   // Descripteur de fichier pour le socket serveur
    std::map<int, Client*> clients;     // map de pointeur sur client avec leur fd comme clé
    int max_sd;
    timeval timeout;

    // On veux pas que ça puisse être copier, c'est le serveur c'est pour ça que c'est dans private et que ce seras pas implémenter
    Server(const Server&);              // Constructeur de copie
    Server& operator=(const Server&);   // Opérateur d'assignation
    Server();                           // Constructeur par défaut

    // Initialiser le socket serveur
    void setupServerSocket();
    void createSocket();
    void configureSocket();
    void bindSocket();
    void listenSocket();

    // Accepter une nouvelle connexion
    void acceptNewConnection();

    // Traiter les données reçues d'un client
    bool handleClientData(int clientSocket);

    // Envoyer un message à tous les clients connectés (sauf l'expéditeur)
    // void broadcastMessage(const std::string& message, int senderFd);

    // Fermer proprement une connexion client
    void closeClientConnection(int clientSocket);

    // Vérifier si le mot de passe fourni est correct
    // bool authenticateClient(const std::string& receivedPassword);

    void processClientActivity(fd_set& readfds);
    void initializeFDSet(fd_set& readfds, int& max_sd);
    void updateMaxSD();
    
    //Vérifier si le message reçut contient une commande, et si oui, exécuter cette commande
    void checkForCommand(std::string message); 
};

#endif // SERVER_HPP