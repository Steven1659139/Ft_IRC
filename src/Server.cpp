#include "../includes/Server.hpp"

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


*/
void Server::bindSocket() {
    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces réseau
    serverAddr.sin_port = htons(port); // Convertit le numéro de port en format réseau (big-endian)
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur lors de la liaison du socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::setupServerSocket()
{
    createSocket();
    configureSocket();




};