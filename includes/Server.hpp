
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

// RPLs
# define RPL_WELCOME(nickname)  ":localhost 001 " + nickname + " :Welcome " + nickname + " in this IRC server!\r\n"
# define RPL_TOPIC(nickname, channel, topic) ":localhost 331 " + nickname = " :" + channel + ":" + topic + "\r\n"
# define RPL_NOTOPIC(nickname, channel) ":localhost 331 " + nickname = " :" + channel + " :No topic is set.\r\n"
# define RPL_INVITING(nickname, target, channel) ":localhost 341 " + nickname + " :" + channel + " " + target + "\r\n" 
//ERRs
# define ERR_NEEDMOREPARAMS(nickname, command) ":localhost 461 " + nickname + " " + command + " :Not enough parameters\r\n"
# define ERR_ALREADYREGISTERED(nickname) ":localhost 462 " + nickname + " : Unauthorized command (already registered)"
# define ERR_UNKOWNCOMMAND(nickname, command) ":localhost 421 " + nickname + " :" + command " :Unknown command.\r\n"
# define ERR_NONICKNAMEGIVEN(nickname) ":localhost 431 " + nickname + " :No nickname given\r\n"
# define ERR_NICKNAMEINUSE(nickname, newnick) ":localhost 433 " + nickname + " :" + newnick + " :Nickname is already in use"
# define ERR_ERRONEUSNICKNAME(nickname, newnick) ":localhost 432 " + nickname + " :" + newnick + " :Erroneous nickname.\r\n"
# define ERR_INVITEONLYCHAN(nickname, channel) ":localhost 473 " + nickname + " :" + channel " :Cannot join channel (+i)"
# define ERR_CHANNELISFULL(nickname, channel) ":localhost 471 " + nickname + " :" + channel " :Cannot join channel (+l)"
# define ERR_NOSUCHCHANNEL(nickname, channel) ":localhost 402 " + nickname + " :" + channel + " :No such channel"
# define ERR_BADCHANNELKEY(nickname, channel) ":localhost 475 " + nickname + " :" + channel " :Cannot join channel (+k)"
# define ERR_TOOMANYCHANNELS(nickname, channel) ":localhost 405 " + nickname + " :" + channel + " :You have joined too many channels.\r\n"
# define ERR_NOTONCHANNEL(nickname, channel) ":localhost 442 " + nickname + " :" + channel + " :You're not on that channel.\r\n"
# define ERR_USERNOTINCHANNEL(nickname, target, channel) ":localhost 441 " + nickname + " :" + target + " " + channel + " :They aren't on that channel.\r\n"
# define ERR_CHANOPRIVSNEEDED(nickname, channel) ":localhost 482 " + nickname + " :" + channel " :You're not a channel operator.\r\n"
# define ERR_UNKOWNMODE(nickname, char, channel) ":localhost 472 " + nickname + " :" + char + " :is unkown mode char to me for " + channel + ".\r\n"
# define ERR_KEYSET(nickname, channel) ":localhost 467 " + nickname + " :" + channel + " :Channel key already set"
# define ERR_NOSUCHNICK(nickname, channel) ":localhost 401 " + nickname + " :" + channel + " :No such nick/channel.\r\n"
# define ERR_USERONCHANNEL(nickname, target, channel) ":localhost 443 " + nickname + " :" + target + " " + channel + " :is already on channel.\r\n"
# define ERR_NOTEXTTOSEND(nickname) ":localhost 412 " + nickname + " :No text to send.\n\r"
# define ERR_NOTREGEISTERED(nickname) ":localhost 451 " + nickname + " :You have not registered.\r\n"
# define ERR_PASSWDMISMATCH(nickaname) ":localhost 464 " + nickname + " :Password incorrect.\r\n"


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
    void sendMessageOnChan(const std::string& message, std::map<std::string, Channel*>::iterator chanIter);
    bool authenticateClient(const std::string& receivedPassword);
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

    // Envoyer un message à tous les clients connectés (sauf l'expéditeur)
    // void broadcastMessage(const std::string& message, int senderFd);

    // Fermer proprement une connexion client
    void closeClientConnection(int clientSocket);

    // Vérifier si le mot de passe fourni est correct

    void processClientActivity(fd_set& readfds);
    void initializeFDSet(fd_set& readfds, int& max_sd);
    void updateMaxSD();


};

#endif // SERVER_HPP