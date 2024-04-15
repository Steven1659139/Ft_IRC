#include "../includes/CommandHandler.hpp"
#include <sstream>
#include <iterator>
#include "../includes/Utils.hpp"

CommandHandler::CommandHandler(Server& server) : server(server) {initializeCommands();}

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

    if (providedPassword == server.getPass())
    {
        client.goodPass = true;
        server.authenticateClient(client);
    }
    else
    {
        client.sendMessage(ERR_PASSWDMISMATCH(client.getNickname()));
    }
}


void CommandHandler::privMsg(Client & client, const std::vector<std::string>& args)
{
    if(client.isAuth())
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
}

void CommandHandler::join(Client &client, const std::vector<std::string>& args)
{
    if (client.isAuth())
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
}


void CommandHandler::nick(Client& client, const std::vector<std::string>& args)
{
    std::string newNickname = args[0];

    client.setNickname(newNickname);
    std::cout << "Nickname mis à jour : " << newNickname << std::endl;
}

void CommandHandler::user(Client& client, const std::vector<std::string>& args)
{
    std::string newUsername = args[0];

    std::cout << "Username mis à jour : " << newUsername << std::endl;
    client.setUsername(newUsername);
}


// Traiter une commande reçue d'un client
void CommandHandler::handleCommand(Client& client, const std::string& commandLine) {
    std::vector<std::string> args = splitCommandLine(commandLine);
    if (args.empty()) return;

    std::string command = args[0];
    args.erase(args.begin());

    std::map<std::string, CommandFunc>::iterator it = commands.find(command);
    if (it != commands.end()) {
        CommandFunc func = it->second;
        (this->*func)(client, args);
    } else
    {
         client.sendMessage(ERR_UNKOWNCOMMAND(client.getNickname(), command));
    }
}

void CommandHandler::initializeCommands() {
    commands["NICK"] = &CommandHandler::nick;
    commands["USER"] = &CommandHandler::user;
    commands["JOIN"] = &CommandHandler::join;
    commands["#PRIVMSG"] = &CommandHandler::privMsg;
    commands["PASS"] = &CommandHandler::pass;
    // commands["PART"] = &CommandHandler::part;
    // commands["KICK"] = &CommandHandler::kick;
    // commands["QUIT"] = &CommandHandler::quit;
}

// void CommandHandler::handleCommand(Client& client, const std::string& commandLine) {

//     std::vector<std::string> args = splitCommandLine(commandLine);

//     if (args.empty()) return;
//     std::string command = args[0];
//     args.erase(args.begin());

//     std::cout << command << std::endl;

//     // Dispatch vers la commande appropriée
//     if (command == "PASS") 
//         pass(client, args);
//     if (command == "NICK")
//         nick(client, args);
//     if (command == "USER")
//         user(client, args);
//     if (client.isAuth())
//     {
//         if (command == "JOIN") {
//             join(client, args);
//         } else if (command == "PART") {
//             // handlePartCommand(client, args);
//         } else if (command == "#PRIVMSG") {
//             privMsg(client, args);
//         } else if (command == "QUIT") {
//             // handleQuitCommand(client, args);
//         }
//     }
// }