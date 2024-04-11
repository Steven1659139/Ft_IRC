#include "../includes/CommandHandler.hpp"
#include <sstream>
#include <iterator>
#include "../includes/Utils.hpp"

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

void CommandHandler::privMsg(Client & client, const std::vector<std::string>& args)
{
    //std::string appender = "\r\n";
    size_t i = 0;
    std::string checker = args[0];
    if (checker[0] == '#')
    {
        std::map<std::string, Channel *>::iterator ito = server.getChannel(args[0]);
        if (ito == server.getChannelEnd())
            return ;
        std::string msg = ":" + client.getNickname() + "!localhost PRIVMSG " + args[0] + " :";
        while (++i < args.size() - 1)
            msg = args[i] + " ";
        msg.append(args[i]);
        server.sendMessageOnChan(msg, ito);
        return ;
    }
    if (server.isClientHere(args[0]) == true)
    {
        
        std::map<int, Client *>::iterator it = server.findClient(args[0]);
        std::string temp = ":" + client.getNickname() + "!localhost PRIVMSG " + it->second->getNickname() + " :";
        while (++i < args.size() - 1)
            temp += args[i] + " ";
        temp.append(args[i]);
        //temp.append(appender);
        Utils::ft_send(it->second->getSocket(), temp);
    }
}

void CommandHandler::join(Client &client, const std::vector<std::string>& args)
{
    std::map<std::string, Channel *>::iterator it;
    std::string checker = args[0];
    std::string newchan;
    if (checker[0] != '#')
        newchan += "#" + args[0];
    else
        newchan = args[0];
    server.createChannel(newchan, "defaultTopic");
    it = server.getChannel(newchan);
    it->second->addClient(&client);
    std::string temp = ":" + client.getNickname() + "!localhost JOIN " + newchan + "\r\n";
    Utils::ft_send(client.getSocket(), temp);
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
        join(client, args);
    } else if (command == "PART") {
        // handlePartCommand(client, args);
    } else if (command == "PRIVMSG") {
       privMsg(client, args);
    } else if (command == "QUIT") {
        // handleQuitCommand(client, args);
    } else if (command == "PASS") {
        pass(client, args);
    }
}