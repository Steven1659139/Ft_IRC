#include <iostream>
#include <string>
#include "../includes/Server.hpp" // Assurez-vous que ce fichier existe dans votre dossier include/

int main(int argc, char** argv) {
    // Vérifier le nombre d'arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    // Conversion du port en int
    int port = std::atoi(argv[1]);
    if (port <= 0) {
        std::cerr << "Erreur : Le port doit être un nombre positif." << std::endl;
        return 1;
    }

    // Récupération du mot de passe
    std::string password(argv[2]);

    try {
        // Initialisation du serveur avec le port et le mot de passe
        Server ircServer(port, password);

        // Démarrage du serveur
        ircServer.run();
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'initialisation du serveur : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
