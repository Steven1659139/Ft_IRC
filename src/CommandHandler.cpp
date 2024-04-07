#include "../includes/CommandHandler.hpp"
#include <sstream>
#include <iterator>

CommandHandler::CommandHandler(Server& server) : server(server) {}

std::vector<std::string> CommandHandler::splitCommandLine(const std::string& commandLine) {
    std::istringstream iss(commandLine);
    std::vector<std::string> tokens(
        (std::istream_iterator<std::string>(iss)),
        std::istream_iterator<std::string>());
    return tokens;
}

void CommandHandler::pass(Client& client, const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Erreur : Aucun mot de passe fourni." << std::endl;
        return;
    }

    std::string receivedPassword = args[0]; // Prend le premier argument comme mot de passe
    
    // Ici, vous vérifiez le mot de passe reçu.
    // Pour cet exemple, supposons que le mot de passe correct est "secret".
    if (receivedPassword == "secret") {
        client.setAuth(true);
        std::cout << "Client authentifié avec succès." << std::endl;
    } else {
        std::cerr << "Erreur : Mot de passe incorrect." << std::endl;
    }
}



// Traiter une commande reçue d'un client
void CommandHandler::handleCommand(Client& client, const std::string& commandLine) {

    std::vector<std::string> args = splitCommandLine(commandLine);

    if (args.empty()) return;
    std::string command = args[0];
    args.erase(args.begin());

    // Dispatch vers la commande appropriée
    if (command == "NICK") {
        // handleNickCommand(client, args);
    } else if (command == "USER") {
        // handleUserCommand(client, args);
    } else if (command == "JOIN") {
        // handleJoinCommand(client, args);
    } else if (command == "PART") {
        // handlePartCommand(client, args);
    } else if (command == "PRIVMSG") {
        // handlePrivMsgCommand(client, args);
    } else if (command == "QUIT") {
        // handleQuitCommand(client, args);
    } else if (command == "PASS") {
        pass(client, args);
    }
}