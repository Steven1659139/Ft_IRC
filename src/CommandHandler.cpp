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
        Utils::ft_send(client.getSocket(), FORM_PASS(client.getNickname(), providedPassword));
    }
    else
    {
        Utils::ft_send(client.getSocket(), ERR_PASSWDMISMATCH(client.getNickname()));
    }
}


void CommandHandler::privMsg(Client & client, const std::vector<std::string>& args)
{
    if(client.isAuth())
    {
        size_t i = 0;
        std::string checker = args[0];
        if (checker[0] == '#')
        {
            std::map<std::string, Channel *>::iterator ito = server.getChannel(args[0]);
            if (ito == server.getChannelEnd())
            {
                Utils::ft_send(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), args[0]));
                return ;
            }
            if (!ito->second->isClientInChannel(&client))
            {
                Utils::ft_send(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), ito->second->getName()));
                return ;
            }
            std::string msg;
            // J'ai modifié la boucle pour éviter les segfault si args est vide
            for (size_t i = 1; i < args.size(); ++i) {
                msg += args[i];
                if (i < args.size() - 1) 
                    msg += " ";
            }
            server.sendMessageOnChan(FORM_MSG(client.getNickname(), ito->second->getName(), msg), ito, client);
            return ;
        }
        if (server.isClientHere(args[0]) == true)
        {
            std::map<int, Client *>::iterator it = server.findClient(args[0]);
            std::string temp;
            while (++i < args.size() - 1)
                temp += args[i] + " ";
            temp.append(args[i]);
            //temp.append(appender);
            Utils::ft_send(it->second->getSocket(), FORM_MSG(client.getNickname(), it->second->getNickname(), temp));
        }
        else
            Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), args[0]));
    }
}

void CommandHandler::join(Client &client, const std::vector<std::string>& args)
{
    if (client.isAuth())
    {
        if (!args[0].compare("#0"))
        {
            server.leaveAllChans(client);
            return ;
        }
        std::map<std::string, Channel *>::iterator it;
        size_t i = 0;
        std::string checker;
        std::string newchan;
        std::vector<std::string> vec;
        std::vector<std::string> keyvec;
        std::string temp;
        std::string temp2 = args[0];
        while (i < 1)
        {
            i = temp2.find_first_of(',');
            if (i == std::string::npos)
            {
                vec.push_back(temp2);
                break ;
            }
            temp = temp2.substr(0, i);
            temp2.erase(0, i + 1);
            vec.push_back(temp);
            i = 0;
        }
        i = 0;
        if (args.size() > 1)
        {
            temp2 = args[1];
            while (i < 1)
            {
                i = temp2.find_first_of(',');
                if (i == std::string::npos)
                {
                    keyvec.push_back(temp2);
                    break ;
                }
                temp = temp2.substr(0, i);
                temp2.erase(0, i + 1);
                keyvec.push_back(temp);
                i = 0;
            }
        }
        i = 0;
        bool canjoin;
        while (i < vec.size())
        {
            if (i >= keyvec.size())
                keyvec.push_back("");
            checker = vec[i];
            if (checker[0] != '#')
                newchan += "#" + vec[i];
            else
                newchan = vec[i];
            server.createChannel(newchan, "");
            it = server.getChannel(newchan);
            if (it->second->getClients().empty())
            {
                it->second->addClient(&client);
                it->second->addOperator(&client);
            }
            else
            {
                canjoin = modeCheck(it->second, keyvec[i], client);
                if (canjoin == false)
                {
                    i++;
                    newchan.clear();
                    continue;
                }
               it->second->addClient(&client); 
            }
            Utils::ft_send(client.getSocket(), FORM_JOIN(client.getNickname(), newchan));
            newchan.clear();
            i++;
        }
    }
}

bool CommandHandler::modeCheck(Channel *chan, std::string key, Client &client)
{
    t_modes vals = chan->getModes();
    if (vals.inviteonly == true)
    {
        Utils::ft_send(client.getSocket(), ERR_INVITEONLYCHAN(client.getNickname(), chan->getName()));
        return (false);
    }
    if (vals.istherelimit == true && chan->getClients().size() >= vals.limit)
    {
        Utils::ft_send(client.getSocket(), ERR_CHANNELISFULL(client.getNickname(), chan->getName()));
        return (false);
    }
    if (vals.istherekey == true && key != vals.key)
    {
        Utils::ft_send(client.getSocket(), ERR_BADCHANNELKEY(client.getNickname(), chan->getName()));
        return (false);
    }
    return (true);
}

void CommandHandler::part(Client& client, const std::vector<std::string>& args)
{
    std::vector<std::string> vec;
    std::string temp;
    std::string temp2 = args[0];
    size_t i = 0;
    while (i < 1)
    {
        i = temp2.find_first_of(',');
        if (i == std::string::npos)
        {
            vec.push_back(temp2);
            break ;
        }
        temp = temp2.substr(0, i);
        temp2.erase(0, i + 1);
        vec.push_back(temp);
        i = 0;
    }
    i = 0;
    std::map<std::string, Channel *>::iterator it;
    std::string temp3;
    while (i < vec.size())
    {
        it = server.getChannel(vec[i]);
        if (it == server.getChannelEnd())
            Utils::ft_send(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), vec[i]));
        else if (it != server.getChannelEnd() && !it->second->isClientInChannel(&client))
            Utils::ft_send(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), vec[i]));
        else
        {
            it->second->removeClient(&client);
            if (temp3.empty())
                temp3.append(it->second->getName());
            else
                temp3 += "," + it->second->getName();
        }
        i++;
    }
    if (args.size() > 1)
    {
        std::string msg;
        for (size_t j = 1; j < args.size(); ++j) {
                msg += args[j];
                if (j < args.size() - 1) 
                    msg += " ";
            }
        Utils::ft_send(client.getSocket(), FORM_PARTMSG(client.getNickname(), temp3, msg));
    }
    else
        Utils::ft_send(client.getSocket(), FORM_PART(client.getNickname(), temp3));
}

void CommandHandler::quit(Client &client, const std::vector<std::string>& args)
{
    if (args.size() > 0)
        Utils::ft_send(client.getSocket(), FORM_QUIT(client.getNickname(), args[0]));
    else
        Utils::ft_send(client.getSocket(), FORM_QUIT(client.getNickname(), "Leaving..."));
    server.closeClientConnection(client.getSocket());
}

void CommandHandler::mode(Client &client, const std::vector<std::string>& args)
{
    std::map<std::string, Channel *>::iterator it = server.getChannel(args[0]);
    if (it == server.getChannelEnd())
    {
        Utils::ft_send(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), args[0]));
        return ;
    }
    std::vector<std::string> newargs = args;
    newargs.erase(newargs.begin());
    if (!it->second->isClientInChannel(&client))
    {
        Utils::ft_send(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), it->second->getName()));
        return ;
    }
    if (!it->second->isClientAnOperator(&client))
    {
        Utils::ft_send(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), it->second->getName()));
        return ;
    }
    if (args[0] == "+o")
    {
        if (args.size() < 2)
        {
            Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
            return ;
        }
        if (args.size() > 2)
        {
            Utils::ft_send(client.getSocket(), ERR_UNKOWNCOMMAND(client.getNickname(), "MODE: +o: too many params"));
            return ;
        }
        if (!server.isClientHere(args[1]))
        {
            Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), args[1]));
            return ;
        }
        std::map<int, Client*>::iterator it2 = server.findClient(args[1]);
        if (!it->second->isClientInChannel(it2->second))
        {
            Utils::ft_send(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), args[1], it->second->getName()));
            return ;
        }
        it->second->addOperator(it2->second);
        return ;
    }
     if (args[0] == "-o")
    {
        if (args.size() < 2)
        {
            Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
            return ;
        }
        if (args.size() > 2)
        {
            Utils::ft_send(client.getSocket(), ERR_UNKOWNCOMMAND(client.getNickname(), "MODE: +o: too many params"));
            return ;
        }
        if (!server.isClientHere(args[1]))
        {
            Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), args[1]));
            return ;
        }
        std::map<int, Client*>::iterator it2 = server.findClient(args[1]);
        if (!it->second->isClientInChannel(it2->second))
        {
            Utils::ft_send(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), args[1], it->second->getName()));
            return ;
        }
        if (!it->second->isClientAnOperator(it2->second))
        {
            Utils::ft_send(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), it2->second->getNickname(), it->second->getName()));
            return ;
        }
        it->second->removeOperator(it2->second);
        return ;
    }
    it->second->setModes(client, newargs);
    //Utils::ft_send(client.getSocket(), FORM_MODE(client.getNickname(), it->second->getName(), it->second->strModes()));
    server.sendMessageOnChan(FORM_MODE(client.getNickname(), it->second->getName(), it->second->strModes()), it, client);
}

void CommandHandler::invite(Client &client, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "INVITE"));
        return ;
    }
    if (!server.isClientHere(args[0]))
    {
        Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), args[0]));
        return ;
    }
    std::map<std::string, Channel *>::iterator it = server.getChannel(args[1]);
    if (it == server.getChannelEnd())
    {
        Utils::ft_send(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), args[0]));
        return ;
    }
    if (!it->second->isClientInChannel(&client))
    {
        Utils::ft_send(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), it->second->getName()));
        return ;
    }
    if (!it->second->isClientAnOperator(&client) && it->second->getModes().inviteonly == true)
    {
        Utils::ft_send(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), it->second->getName()));
        return ;
    }
    std::map<int, Client*>::iterator it2 = server.findClient(args[0]);
    Utils::ft_send(it2->second->getSocket(), FORM_INVITE(client.getNickname(), it2->second->getNickname(), it->second->getName()));
    Utils::ft_send(client.getSocket(), RPL_INVITING(client.getNickname(), it->second->getName(), it2->second->getNickname()));
    it->second->addClient(it2->second);
}

void CommandHandler::nick(Client& client, const std::vector<std::string>& args)
{
    std::string newNickname = args[0];

    Utils::ft_send(client.getSocket(), FORM_NICK(client.getNickname(), newNickname));
    client.setNickname(newNickname);
    std::cout << "Nickname mis à jour : " << newNickname << std::endl;
    Utils::ft_send(client.getSocket(), RPL_WELCOME(client.getNickname()));
}

void CommandHandler::user(Client& client, const std::vector<std::string>& args)
{
    std::string newUsername = args[0];

    std::cout << "Username mis à jour : " << newUsername << std::endl;
    client.setUsername(newUsername);
}

void CommandHandler::topic(Client& client, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC"));
        return ;
    }
    if (server.getChannel(args[0]) == server.getChannelEnd())
    {
        Utils::ft_send(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), args[0]));
        return ;
    }
    if (!server.getChannel(args[0])->second->isClientInChannel(&client))
    {
        Utils::ft_send(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), args[0]));
        return ;
    }
    if (args.size() == 1)
    {
        std::cout << "test\n";   
        if (server.getChannel(args[0])->second->getTopic().empty())
        {
            Utils::ft_send(client.getSocket(), RPL_NOTOPIC(client.getNickname(), args[0]));
            return ;
        }
        Utils::ft_send(client.getSocket(), RPL_TOPIC(client.getNickname(), args[0], server.getChannel(args[0])->second->getTopic()));
        return ;
    }
    if (args.size() > 1)
    {
        if (server.getChannel(args[0])->second->getModes().modtopic == true && !server.getChannel(args[0])->second->isClientAnOperator(&client))
        {
            Utils::ft_send(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), args[0]));
            return ;
        }
        std::string top;
        for(size_t i = 1; i < args.size(); ++i)
        {
            top += args[i];
            if (i < args.size() - 1)
                top += " ";
        }
        if (top.empty())
        {
            server.getChannel(args[0])->second->setTopic("");
            server.sendMessageOnChan(FORM_TOPIC(client.getNickname(), args[0], ""), server.getChannel(args[0]), client);
            Utils::ft_send(client.getSocket(), FORM_TOPIC(client.getNickname(), args[0], ""));
            return ;
        }
        server.getChannel(args[0])->second->setTopic(top);
        server.sendMessageOnChan(FORM_TOPIC(client.getNickname(), args[0], top), server.getChannel(args[0]), client);
        Utils::ft_send(client.getSocket(), FORM_TOPIC(client.getNickname(), args[0], top));
        return ;
    }
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
    commands["PRIVMSG"] = &CommandHandler::privMsg;
    commands["PASS"] = &CommandHandler::pass;
    commands["PART"] = &CommandHandler::part;
    // commands["KICK"] = &CommandHandler::kick;
    commands["QUIT"] = &CommandHandler::quit;
    commands["MODE"] = &CommandHandler::mode;
    commands["INVITE"] = &CommandHandler::invite;
    commands["TOPIC"] = &CommandHandler::topic;
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