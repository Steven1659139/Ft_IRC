// #ifndef COMMANDHANDLER_HPP
// #define COMMANDHANDLER_HPP

// #include "Client.hpp" // Assurez-vous que ce fichier existe et est correctement structuré.
// #include "Server.hpp" // Assurez-vous que ce fichier existe et est correctement structuré.
// #include <string>
// #include <vector>

// class CommandHandler {
// public:
//     // Associer le CommandHandler à une instance de serveur
//     CommandHandler(Server& server);

//     // Traiter une commande reçue d'un client
//     void handleCommand(Client& client, const std::string& commandLine);

// private:
//     Server& server; // Référence au serveur pour accéder aux clients et aux canaux

//     // Méthodes spécifiques à chaque commande
//     void handleNickCommand(Client& client, const std::vector<std::string>& args);
//     void handleUserCommand(Client& client, const std::vector<std::string>& args);
//     void handleJoinCommand(Client& client, const std::vector<std::string>& args);
//     void handlePartCommand(Client& client, const std::vector<std::string>& args);
//     void handlePrivMsgCommand(Client& client, const std::vector<std::string>& args);
//     void handleQuitCommand(Client& client, const std::vector<std::string>& args);
//     // Ajoutez d'autres commandes selon les besoins...

//     // Utilitaire pour diviser la ligne de commande en mots
//     std::vector<std::string> splitCommandLine(const std::string& commandLine);
// };

// #endif // COMMANDHANDLER_HPP
