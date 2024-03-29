#include "../includes/Server.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password){}

Server::~Server() {
    for(std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

    if (serverSocket != -1) {
        close(serverSocket);
    }
}

/*
    Créé le socket du serveur,

    int socket(int domain, int type, int protocol);

    Option de socket():
    AF_INET(domain): IPv4
    SOCK_STREAM(type): communication orienté connexion
    0(protocol): TCP par défaut
*/
void Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Erreur lors de la création du socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}
/*
    Configure le socket

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

    Option de setsockopt:

    SOL_SOCKET(level): Applique le niveau de la configuration, ici la config seulement sur ce socket
    SO_REUSEADDR(optname): permet de reutilisé l'adresse si on doit redémarrer le serveur rapidement, on évite que bind() nous renvoie une erreur parce que le port est encore en TIME_WAIT du précédent cycle
*/
void Server::configureSocket() {
    int yes = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        std::cerr << "Erreur de configuration du socket (SO_REUSEADDR)" << std::endl;
        exit(EXIT_FAILURE);
    }
}
/*
    Lie le socket serveur à une adresse IP et un port spécifique

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    Fun fact: htons() = Host TO Network Short

*/
void Server::bindSocket() {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces réseau
    serverAddr.sin_port = htons(port); // Convertit le numéro de port en format réseau (big-endian)
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur lors de la liaison du socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/*
    Marque le socket serveur comme une socket passif prêt à recevoir des connexions 

    int listen(int sockfd, int backlog);

*/
void Server::listenSocket() {

    if (listen(serverSocket, 5) < 0)
    {
        std::cerr << "Erreur lors de la mise en écoute du socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Serveur prêt et en écoute sur le port " << port << std::endl;
}

void Server::setupServerSocket()
{
    createSocket();
    configureSocket();
    bindSocket();
    listenSocket();
};

void Server::acceptNewConnection() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << "Erreur lors de l'acceptation d'une nouvelle connexion." << std::endl;
        return;
    }
    // Créer un nouvel objet Client et l'ajouter à la map
    Client* newClient = new Client(clientSocket, "defaultNickname", "defaultUsername");
    clients.insert(std::make_pair(clientSocket, newClient)); // J'ai changé la façon d'assigner pour qu'elle soit plus standard avec l'utilisation d'une map;

    std::cout << "Nouvelle connexion acceptée. ClientSocket: " << clientSocket << std::endl;
}

void Server::closeClientConnection(int clientSocket) {
    
    std::map<int, Client*>::iterator it = clients.find(clientSocket);
    
    if (it != clients.end()) {
        close(it->first);
        delete it->second;
        clients.erase(it);
        std::cout << "Connexion avec le client sur la socket " << clientSocket << " fermée." << std::endl;
    } else {
        std::cerr << "Tentative de fermer une connexion inexistante: socket " << clientSocket << std::endl;
    }
}

bool Server::handleClientData(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead < 0) {
        std::cerr << "Erreur lors de la lecture des données du client." << std::endl;
        return false;
    } else if (bytesRead == 0) {
        std::cout << "Client déconnecté." << std::endl;
        closeClientConnection(clientSocket);
        return false;
    } else {
        std::cout << "Message reçu: " << buffer << std::endl;
        return true;
    }
}

void Server::run() {
    fd_set readfds;
    int max_sd;

    setupServerSocket();
    std::cout << "Serveur en écoute sur le port " << port << std::endl;

    while (true) {
        initializeFDSet(readfds, max_sd);

        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0 && errno != EINTR) {
            std::cerr << "Erreur de select." << std::endl;
            continue;
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            acceptNewConnection();
        }
        processClientActivity(readfds);
    }
}



void Server::initializeFDSet(fd_set& readfds, int& max_sd) {
    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);
    max_sd = serverSocket;

    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        int sd = it->first;
        if (sd > 0) FD_SET(sd, &readfds);
        if (sd > max_sd) max_sd = sd;
    }
}

void Server::processClientActivity(fd_set& readfds) {
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();) {
        int sd = it->first;
        if (FD_ISSET(sd, &readfds)) {
            if (!handleClientData(sd)) {
                closeClientConnection(sd);
                clients.erase(it++);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}
