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
        std::cout << "ERREUR : Aucun mot de passe fourni" << std::endl;
        return;
    }

    std::string providedPassword = args[0];
    if (server.authenticateClient(providedPassword)) {
        client.setAuth(true);
        std::cout << "Client authentifié avec succès" << std::endl;
    } else {
        std::cout << "ERREUR : Mot de passe incorrect" << std::endl;

        client.setAuth(false);
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


        // J'ai modifié la boucle pour éviter les segfault si args est vide
        for (size_t i = 1; i < args.size(); ++i) {
            msg += args[i];
            if (i < args.size() - 1) 
                msg += " ";
        }

        // while (++i < args.size() - 1)
        //     msg = args[i] + " ";
        // msg.append(args[i]);
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


void CommandHandler::nick(Client& client, const std::vector<std::string>& args)
{
    std::string newNickname = args[0];

    client.setNickname(newNickname);
    std::cout << "Nickname mis à jour : " << newNickname << std::endl;


}



// Traiter une commande reçue d'un client
void CommandHandler::handleCommand(Client& client, const std::string& commandLine) {

    std::vector<std::string> args = splitCommandLine(commandLine);

    if (args.empty()) return;
    std::string command = args[0];
    args.erase(args.begin());

    // Dispatch vers la commande appropriée
    if (command == "PASS") 
        pass(client, args);
    if (client.isAuth())
    {
        if (command == "NICK") {
            nick(client, args);
        } else if (command == "USER") {
            // handleUserCommand(client, args);
        } else if (command == "JOIN") {
            join(client, args);
        } else if (command == "PART") {
            // handlePartCommand(client, args);
        } else if (command == "#PRIVMSG") {
            privMsg(client, args);
        } else if (command == "QUIT") {
            // handleQuitCommand(client, args);
        }
    }
}