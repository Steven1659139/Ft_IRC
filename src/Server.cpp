#include "../includes/Server.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password){timeout.tv_sec = 5; timeout.tv_usec = 0;}
// Server::Server(int port, const std::string& password) : port(port), password(password), commandHandler(*this){timeout.tv_sec = 5; timeout.tv_usec = 0;}


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
    // Met le socket en mode non bloquant
    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Erreur lors de la récupération des flags du socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(serverSocket, F_SETFL, flags) == -1) {
        std::cerr << "Erreur lors de la mise du socket en mode non bloquant" << std::endl;
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
    Client* newClient = new Client(clientSocket, "defaultNickname", "defaultUsername", false);
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
    }
    // Pas besoin d'afficher d'erreur ici la fonction est conçue pour être idempotente.
}


bool Server::handleClientData(int clientSocket) {
    //char buffer[1024];
    //memset(buffer, 0, sizeof(buffer));
    char buffer;
    std::string msg;

    ssize_t bytesRead = recv(clientSocket, &buffer, 1, MSG_DONTWAIT);
    
    if (bytesRead < 0) {
        std::cerr << "Erreur lors de la lecture des données du client." << std::endl;
        return false; // Indique une erreur, sans fermer ici.
    } else if (bytesRead == 0) {
        std::cout << "Client déconnecté." << std::endl;
        return false; // Indique que le client est déconnecté, sans fermer ici.
    }else
    {
        msg += buffer;        
        while (bytesRead > 0)
        {
            bytesRead = recv(clientSocket, &buffer, 1, MSG_DONTWAIT);
            msg += buffer;
        }
        //ici je voudrais que que le commandHandler exécute les commande dans le buffer
        std::cout << "Message reçu: " << msg << std::endl;
        msg.clear();
        //send(clientSocket, buffer, sizeof(buffer), MSG_DONTWAIT);
        return true; // Données reçues et traitées correctement.
    }
}

/**
 * Boucle principale du serveur. 
 * - Configure le socket d'écoute.
 * - Entre dans une boucle infinie qui:
 *   - Initialise les FD sets pour surveiller le socket d'écoute et les sockets clients.
 *   - Attend une activité sur l'un de ces sockets via select(). 
 *     - En cas d'erreur avec select(), affiche un message d'erreur et continue la boucle.
 *     - Si une activité est détectée sur le socket d'écoute, accepte une nouvelle connexion.
 *   - Traite l'activité sur les sockets clients (lire les données entrantes, fermer les connexions, etc.).
 */
void Server::run() {
    fd_set readfds;
    int max_sd;

    setupServerSocket();
    std::cout << "Serveur en écoute sur le port " << port << std::endl;

    while (true) {
        initializeFDSet(readfds, max_sd);

        if (select(max_sd + 1, &readfds, NULL, NULL, &timeout) < 0 && errno != EINTR) {
            std::cerr << "Erreur de select." << std::endl;
            continue;
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            acceptNewConnection();
        }
        processClientActivity(readfds);
    }
}
/**
 * Prépare l'ensemble des descripteurs de fichier et le descripteur maximum pour `select()`.
 * - Réinitialise l'ensemble des descripteurs de fichier (`readfds`) pour la lecture.
 * - Ajoute le socket serveur à l'ensemble pour détecter les nouvelles connexions.
 * - Parcourt tous les clients connectés, les ajoute à l'ensemble pour surveiller leur activité,
 *   et met à jour `max_sd` avec le plus grand descripteur de fichier.
 */
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

/**
 * Traite les activités des clients: lit les données si disponibles, ferme les connexions si nécessaire.
 * - Vérifie chaque client pour des données entrantes.
 * - Utilise `handleClientData` pour lire/traiter les données.
 * - Ferme et supprime le client si `handleClientData` indique une déconnexion.
 */
void Server::processClientActivity(fd_set& readfds) {
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();) {
        int sd = it->first; 
        std::map<int, Client*>::iterator next_it = ++it;

        if (FD_ISSET(sd, &readfds) && !handleClientData(sd)) {
            closeClientConnection(sd); 
        }
        it = next_it; 
    }
}

bool Server::authenticateClient(const std::string& receivedPassword)
{
    if (receivedPassword == password)
        return true;
    else
        return false;
}