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

std::vector<std::string> CommandHandler::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


void CommandHandler::pass(Client& client, const std::vector<std::string>& args) {
    if (client.isAuth()) {
        Utils::ft_send(client.getSocket(), ERR_ALREADYREGISTERED(client.getNickname()));
        return;
    }

    if (args.empty()) {
        Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS"));
        return;
    }
    
    std::string providedPassword = args[0];
    if (providedPassword == server.getPass()) {
        client.goodPass = true;
    } else {
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
            Utils::ft_send(it->second->getSocket(), FORM_MSG(client.getNickname(), it->second->getNickname(), temp));
        }
        else
            Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), args[0]));
    }
}

void CommandHandler::join(Client &client, const std::vector<std::string>& args)
{
    if (!client.isAuth()) {
        Utils::ft_send(client.getSocket(), ERR_NOTREGISTERED(client.getNickname()));
        return;
    }

    if (args.empty())
    {
        Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
        return ;

    }
        if (!args[0].compare("#0"))
        {
            server.leaveAllChans(client);
            return ;
        }
        std::map<std::string, Channel *>::iterator it;
        size_t i = 0;
        std::string checker;
        std::string newchan;
        std::vector<std::string> vec = split(args[0], ',');
        std::vector<std::string> keyvec;
        if (args.size() > 1)
            keyvec = split(args[1], ',');
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
            server.sendMessageOnChan(FORM_JOIN(client.getNickname(), newchan), it, client);
            newchan.clear();
            i++;
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
    if (chan->isClientInChannel(&client))
    {
        Utils::ft_send(client.getSocket(), ERR_USERONCHANNEL(client.getNickname(), client.getNickname(), chan->getName()));
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
    std::string msg;
    if (args.size() > 1)
    {
        for (size_t j = 1; j < args.size(); ++j) {
                msg += args[j];
                if (j < args.size() - 1) 
                    msg += " ";
            }
    }
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
            if (!msg.empty())
                server.sendMessageOnChan(FORM_PARTMSG(client.getNickname(), it->second->getName(), msg), it, client);
            else
                server.sendMessageOnChan(FORM_PART(client.getNickname(), it->second->getName()), it, client);
            if (temp3.empty())
                temp3.append(it->second->getName());
            else
                temp3 += "," + it->second->getName();
        }
        i++;
    }
    if (!msg.empty())
        Utils::ft_send(client.getSocket(), FORM_PARTMSG(client.getNickname(), temp3, msg));
    else
        Utils::ft_send(client.getSocket(), FORM_PART(client.getNickname(), temp3));
}

void CommandHandler::quit(Client &client, const std::vector<std::string>& args)
{
    std::string newargs = args[0];
    if (newargs[0] == ':') {
        newargs.erase(0, 1);
    }

    if (!newargs.empty())
        Utils::ft_send(client.getSocket(), FORM_QUIT(client.getNickname(), newargs));
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
    if (!it->second->isClientInChannel(server.findClient(args[0])->second))
    {
        Utils::ft_send(client.getSocket(), ERR_USERONCHANNEL(client.getNickname(), server.findClient(args[0])->second->getNickname(), it->second->getName()));
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


void CommandHandler::nick(Client& client, const std::vector<std::string>& args) {


    if (args.empty() || (args.size() == 1 && args[0] == ":")) {
        Utils::ft_send(client.getSocket(), ERR_NONICKNAMEGIVEN(client.getNickname()));
        std::cout << "Erreur : Aucun nickname valide fourni." << std::endl;
        return;
    }

    std::string newNickname = args[0];
    if (newNickname[0] == ':') {
        newNickname.erase(0, 1);
    }

    if (newNickname.empty()) {
        Utils::ft_send(client.getSocket(), ERR_NONICKNAMEGIVEN(client.getNickname()));
        return;
    }
    if (client.getNickname() ==  newNickname){
        Utils::ft_send(client.getSocket(), ERR_NICKNAMEINUSE(client.getNickname(), newNickname));
        return;
    }

    client.setNickname(newNickname);
    if (server.isNicknameUsed(newNickname, client.getSocket())) {
        Utils::ft_send(client.getSocket(), ERR_NICKNAMEINUSE(client.getNickname(), newNickname));
    } else {
        Utils::ft_send(client.getSocket(), FORM_NICK(client.getNickname(), newNickname));
        std::cout << "Nickname mis à jour : " << newNickname << std::endl;
    }
}


void CommandHandler::user(Client& client, const std::vector<std::string>& args)
{

    if (!client.goodPass)
    {
        Utils::ft_send(client.getSocket(), ERR_PASSWDMISMATCH(client.getUsername()));
        return;
    }
        

    if (args.empty() || (args.size() == 1 && args[0] == ":")) {
        Utils::ft_send(client.getSocket(), ERR_NONICKNAMEGIVEN(client.getUsername()));
        std::cout << "Erreur : Aucun username valide fourni." << std::endl;
        return;
    }

    std::string newUsername = args[0];
    if (newUsername[0] == ':') {
        newUsername.erase(0, 1);
    }

    if (newUsername.empty()) {
        Utils::ft_send(client.getSocket(), ERR_NONICKNAMEGIVEN(client.getUsername()));
        return;
    }

    if (server.isUsernameUsed(newUsername)) {
        Utils::ft_send(client.getSocket(), ERR_NICKNAMEINUSE(client.getUsername(), newUsername));
    } else {
        client.setUsername(newUsername);
        Utils::ft_send(client.getSocket(), FORM_USER(client.getNickname(), newUsername));
        std::cout << "Username mis à jour : " << newUsername << std::endl;
    }
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

void CommandHandler::kick(Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
        return ;
    }
    std::vector<std::string> vec = split(args[0], ',');
    size_t i = 0;

    while (i < vec.size())
    {
       if (server.getChannel(vec[i]) == server.getChannelEnd())
        {
            Utils::ft_send(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), vec[i]));
            i++;
            return ;
        }
        if (!server.getChannel(vec[i])->second->isClientInChannel(&client))
        {
            Utils::ft_send(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), vec[i]));
            i++;
            return ;
        }
        if (!server.getChannel(vec[i])->second->isClientAnOperator(&client))
        {
            Utils::ft_send(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), vec[i]));
            i++;
            return ;
        }
        i++;
    }
    std::vector<std::string> vec2 = split(args[1], ',');

    i = 0;
    if (vec.size() > 1 && vec.size() != vec2.size())
    {
        Utils::ft_send(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
        return ;
    }
    if (vec.size() == 1)
    {
        while (i < vec2.size())
        {
            if (!server.isClientHere(vec2[i]))
            {
                Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), vec2[i]));
                i++;
                continue ;
            }
            if (!server.getChannel(vec[0])->second->isClientInChannel(server.findClient(vec2[i])->second))
            {
                Utils::ft_send(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), vec2[i], vec[0]));
                i++;
                continue ;
            }
            std::cout << "test\n";
            std::string msg;
            if (args.size() >= 3)
            {
                for (size_t j = 1; j < args.size(); ++j) {
                msg += args[j];
                if (j < args.size() - 1) 
                    msg += " ";
            }
            }
           
            if (msg.empty())
                msg = client.getNickname();
            server.getChannel(vec[0])->second->removeClient(server.findClient(vec2[i])->second);
            Utils::ft_send(server.findClient(vec2[i])->second->getSocket(), FORM_PARTMSG(server.findClient(vec2[i])->second->getNickname(), vec[0], msg));
            Utils::ft_send(client.getSocket(), FORM_KICK(client.getNickname(), vec[0], vec2[i], msg));
            server.sendMessageOnChan(FORM_KICK(client.getNickname(), vec[0], vec2[i], msg), server.getChannel(vec[0]), client);
            i++;
        }
    }
    if (vec.size() > 1)
    {
        while (i < vec.size())
        {
            if (!server.isClientHere(vec2[i]))
            {
                Utils::ft_send(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), vec2[i]));
                i++;
                continue ;
            }
            if (!server.getChannel(vec[i])->second->isClientInChannel(server.findClient(vec2[i])->second))
            {
                Utils::ft_send(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), vec2[i], vec[i]));
                i++;
                continue ;
            }
            std::string msg;
            if (args.size() >= 3)
            {
                for (size_t j = 1; j < args.size(); ++j) {
                msg += args[j];
                if (j < args.size() - 1) 
                    msg += " ";
            }
            }
            if (msg.empty())
                msg = client.getNickname();
            server.getChannel(vec[i])->second->removeClient(server.findClient(vec2[i])->second);
            Utils::ft_send(server.findClient(vec2[i])->second->getSocket(), FORM_PARTMSG(server.findClient(vec2[i])->second->getNickname(), vec[i], msg));
            Utils::ft_send(client.getSocket(), FORM_KICK(client.getNickname(), vec[i], vec2[i], msg));
            server.sendMessageOnChan(FORM_KICK(client.getNickname(), vec[i], vec2[i], msg), server.getChannel(vec[i]), client);
            i++;
        }
    }
}

// Traiter une commande reçue d'un client

void CommandHandler::handleCommand(Client& client, const std::string& commandLine) {
    std::vector<std::string> args = splitCommandLine(commandLine);
    if (args.empty()) {
        return;
    }

    std::string command = args[0];
    args.erase(args.begin());


    std::map<std::string, CommandFunc>::iterator it = commands.find(command);
    if (it != commands.end()) {
        CommandFunc func = it->second;
        (this->*func)(client, args); 
    } else {
        Utils::ft_send(client.getSocket() ,ERR_UNKOWNCOMMAND(client.getNickname(), command)); 
    }
    server.authenticateClient(client);
}

 void CommandHandler::pong(Client& client, const std::vector<std::string>& args)
 {
    (void)client;
    (void)args;
 }

void CommandHandler::initializeCommands() {
    commands["NICK"] = &CommandHandler::nick;
    commands["USER"] = &CommandHandler::user;
    commands["JOIN"] = &CommandHandler::join;
    commands["PRIVMSG"] = &CommandHandler::privMsg;
    commands["PASS"] = &CommandHandler::pass;
    commands["PART"] = &CommandHandler::part;
    commands["KICK"] = &CommandHandler::kick;
    commands["QUIT"] = &CommandHandler::quit;
    commands["MODE"] = &CommandHandler::mode;
    commands["INVITE"] = &CommandHandler::invite;
    commands["TOPIC"] = &CommandHandler::topic;
    commands["PONG"] = &CommandHandler::pong;
}
