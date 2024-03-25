#include <iostream>
#include <string>
#include "../includes/Server.hpp"
#include <cstdlib>

int valid_arg(char **argv)
{
    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Erreur: Le port doit être un nombre entre 1 et 65535." << std::endl;
        return -1;
    }
    return (port);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    int port = valid_arg(argv);

    if (port > 0)
    {
        std::string password = argv[2];
        std::cout << "Démarrage du serveur sur le port " << port << " avec le mot de passe " << password << std::endl;
    }

    return 0;
}
