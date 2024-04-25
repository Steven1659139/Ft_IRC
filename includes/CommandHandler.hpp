#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp" 
#include "Server.hpp"
#include <string>
#include <vector>

class CommandHandler {
public:
    // Associer le CommandHandler à une instance de serveur
    CommandHandler(Server& server);
    typedef void (CommandHandler::*CommandFunc)(Client&, const std::vector<std::string>&);

    // Traiter une commande reçue d'un client
    void handleCommand(Client& client, const std::string& commandLine);
    void initializeCommands();
    std::vector<std::string> split(const std::string &s, char delimiter);

private:
    Server& server; // Référence au serveur pour accéder aux clients et aux canaux
    std::map<std::string, CommandFunc> commands;

    // Méthodes spécifiques à chaque commande
    void nick(Client& client, const std::vector<std::string>& args);
    void user(Client& client, const std::vector<std::string>& args);
    void pass(Client& client, const std::vector<std::string>& args);
    void privMsg(Client& client, const std::vector<std::string>& args);
    void join(Client& client, const std::vector<std::string>& args);
    void part(Client& client, const std::vector<std::string>& args);
    void quit(Client& client, const std::vector<std::string>& args);
    void mode(Client& client, const std::vector<std::string>& args);
    void invite(Client& client, const std::vector<std::string>& args);
    void topic(Client& client, const std::vector<std::string>& args);
    void kick(Client& client, const std::vector<std::string>& args);
    void pong(Client& client, const std::vector<std::string>& args);
    bool modeCheck(Channel *chan, std::string key, Client &client);

    // Utilitaire pour diviser la ligne de commande en mots
    std::vector<std::string> splitCommandLine(const std::string& commandLine);
};

#endif // COMMANDHANDLER_HPP
