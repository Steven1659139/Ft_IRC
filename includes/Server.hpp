
#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <ctime>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include "Client.hpp"
# include "Channel.hpp"
# include "CommandHandler.hpp"
# include <utility>
# include <algorithm>
# include <csignal>


// Formatted replies from the server to the client
# define FORM_JOIN(nickname, channel) ":" + nickname + "!localhost JOIN " + channel + "\r\n"
# define FORM_PART(nickname, channel) ":" + nickname + "!localhost PART " + channel + "\r\n"
# define FORM_PARTMSG(nickname, channel, message) ":" + nickname + "!localhost PART " + channel + " " + message + "\r\n"
# define FORM_MSG(nickname, target, message) ":" + nickname + "!localhost PRIVMSG " + target + " :" + message + "\r\n"
# define FORM_NICK(nickname, newnick) ":" + nickname + "!localhost NICK :" + newnick + "\r\n"
# define FORM_USER(nickname, newuser) ":" + nickname + "!localhost USER :" + newuser + "\r\n"
# define FORM_PASS(nickname, pass) ":" + nickname + "!localhost PASS :" + pass + "\r\n"
# define FORM_QUIT(nickname, reason) ":" + nickname + "!localhost ERROR :" + reason + "\r\n"
# define FORM_MODE(nickname, channel, modes) ":" + nickname + "!localhost MODE :" + channel + " " + modes + "\r\n"
# define FORM_INVITE(nickname, target, channel) ":" + nickname + "!localhost INVITE " + target + " " + channel + "\r\n"
# define FORM_TOPIC(nickname, channel, topic) ":" + nickname + "!localhost TOPIC " + channel + " :" + topic + "\r\n"
# define FORM_KICK(nickname, channel, target, msg) ":" + nickname + "!localhost KICK " + channel + " " + target + " :" + msg + "\r\n"
// RPLs
# define RPL_WELCOME(nickname)  ":localhost 001 " + nickname + " :Welcome " + nickname + " in this IRC server!\r\n"
# define RPL_TOPIC(nickname, channel, topic) ":localhost 331 " + nickname + " :" + channel + " :" + topic + "\r\n"
# define RPL_NOTOPIC(nickname, channel) ":localhost 331 " + nickname + " :" + channel + " :No topic is set.\r\n"
# define RPL_INVITING(nickname, target, channel) ":localhost 341 " + nickname + " :" + channel + " " + target + "\r\n"
# define RPL_CHANNELMODEIS(nickname, channel, modes) ":localhost 324 " + nickname + " :" + channel + " " + modes + "\r\n"
# define RPL_INVITING(nickname, target, channel) ":localhost 341 " + nickname + " :" + channel + " " + target + "\r\n" 
# define RPL_LOGGEDIN(nickname) ":localhost 900 " + nickname + " :You are now logged in as " + nickname + "\r\n"

//ERRs
# define ERR_NEEDMOREPARAMS(nickname, command) ":localhost 461 " + nickname + " " + command + " :Not enough parameters\r\n"
# define ERR_ALREADYREGISTERED(nickname) ":localhost 462 " + nickname + " : Unauthorized command (already registered)\r\n"
# define ERR_UNKOWNCOMMAND(nickname, command) ":localhost 421 " + nickname + " :" + command + " :Unknown command.\r\n"
# define ERR_NONICKNAMEGIVEN(nickname) ":localhost 431 " + nickname + " :No nickname given\r\n"
# define ERR_NICKNAMEINUSE(nickname, newnick) ":localhost 433 " + nickname + " :" + newnick + " :Nickname is already in use\r\n"
# define ERR_ERRONEUSNICKNAME(nickname, newnick) ":localhost 432 " + nickname + " :" + newnick + " :Erroneous nickname.\r\n"
# define ERR_INVITEONLYCHAN(nickname, channel) ":localhost 473 " + nickname + " :" + channel + " :Cannot join channel (+i)\r\n"
# define ERR_CHANNELISFULL(nickname, channel) ":localhost 471 " + nickname + " :" + channel + " :Cannot join channel (+l)\r\n"
# define ERR_NOSUCHCHANNEL(nickname, channel) ":localhost 402 " + nickname + " :" + channel + " :No such channel\r\n"
# define ERR_BADCHANNELKEY(nickname, channel) ":localhost 475 " + nickname + " :" + channel + " :Cannot join channel (+k)\r\n"
# define ERR_TOOMANYCHANNELS(nickname, channel) ":localhost 405 " + nickname + " :" + channel + " :You have joined too many channels.\r\n"
# define ERR_NOTONCHANNEL(nickname, channel) ":localhost 442 " + nickname + " :" + channel + " :You're not on that channel.\r\n"
# define ERR_USERNOTINCHANNEL(nickname, target, channel) ":localhost 441 " + nickname + " :" + target + " " + channel + " :They aren't on that channel.\r\n"
# define ERR_CHANOPRIVSNEEDED(nickname, channel) ":localhost 482 " + nickname + " :" + channel + " :You're not a channel operator.\r\n"
# define ERR_UNKOWNMODE(nickname, char, channel) ":localhost 472 " + nickname + " :" + char + " :is unkown mode char to me for " + channel + ".\r\n"
# define ERR_KEYSET(nickname, channel) ":localhost 467 " + nickname + " :" + channel + " :Channel key already set\r\n"
# define ERR_NOSUCHNICK(nickname, channel) ":localhost 401 " + nickname + " :" + channel + " :No such nick/channel.\r\n"
# define ERR_USERONCHANNEL(nickname, target, channel) ":localhost 443 " + nickname + " :" + target + " " + channel + " :is already on channel.\r\n"
# define ERR_NOTEXTTOSEND(nickname) ":localhost 412 " + nickname + " :No text to send.\r\n"
# define ERR_NOTREGISTERED(nickname) ":localhost 451 " + nickname + " :You have not registered.\r\n"

# define ERR_PASSWDMISMATCH(nickname) ":localhost 464 " + nickname + " :Password incorrect.\r\n"


class Server {
public:
    // Le constructeur est explicite c'est surtout une question de sécurité pour eviter les conversions implicites non désirées
    explicit Server(int port, const std::string& password);
    
    ~Server();

    void run();
    std::string getPass();
    bool isClientHere(std::string name);
    std::map<int, Client *>::iterator findClient(std::string name);
    void createChannel(const std::string &name, const std::string &topic);
    std::map<std::string, Channel *>::iterator getChannel(const std::string& name);
    std::map<std::string, Channel *>::iterator getChannelEnd();
    void sendMessageOnChan(const std::string& message, std::map<std::string, Channel*>::iterator chanIter, Client &sender);
    bool authenticateClient(Client &client);
    void leaveAllChans(Client &client);
    void closeClientConnection(int clientSocket);
    bool isNicknameUsed(const std::string& nickname, int socket);
    bool isUsernameUsed(const std::string& username);
private:
    int port;                           // Port sur lequel le serveur écoute
    std::string password;               // Mot de passe requis pour la connexion
    int serverSocket;                   // Descripteur de fichier pour le socket serveur
    std::map<int, Client*> clients;     // map de pointeur sur client avec leur fd comme clé
    std::map<std::string, Channel*> channels; // map de pointeur sur channels avec leur nom comme clé
    int max_sd;
    timeval timeout;

    // On veux pas que ça puisse être copier, c'est le serveur c'est pour ça que c'est dans private et que ce seras pas implémenter
    Server(const Server&);              // Constructeur de copie
    Server& operator=(const Server&);   // Opérateur d'assignation
    Server();                           // Constructeur par défaut

    // Initialiser le socket serveur
    void setupServerSocket();
    void createSocket();
    void configureSocket();
    void bindSocket();
    void listenSocket();

    // Accepter une nouvelle connexion
    void acceptNewConnection();

    // Traiter les données reçues d'un client
    bool handleClientData(int clientSocket);

    // Vérifier si le mot de passe fourni est correct

    void processClientActivity(fd_set& readfds);
    void initializeFDSet(fd_set& readfds, int& max_sd);
    void updateMaxSD();


};

void setStopFlag(int);

#endif // SERVER_HPP