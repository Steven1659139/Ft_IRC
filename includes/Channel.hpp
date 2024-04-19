#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <algorithm>
# include <vector>
# include <map>
# include "Client.hpp" 

typedef struct s_mode
{ 
    bool istherekey;
    bool inviteonly;
    bool istherelimit;
    bool modtopic;
    size_t limit;
    std::string key;
}   t_modes;


class Channel {
public:

    Channel(const std::string& name, const std::string& topic);
    typedef void (Channel::*SetFunc)(Client&, const std::vector<std::string>&);
    // Ajoute un client au canal
    
    bool addClient(Client* client);

    // Supprime un client du canal
    bool removeClient(Client* client);

    // Ajoute et Enlève les droits d'opérateur
    bool addOperator(Client* client);
    bool removeOperator(Client* client);

    // Envoie un message à tous les clients du canal
    // void broadcastMessage(const std::string& message, const Client* sender = nullptr);

    // Définit ou obtient le sujet du canal
    void setTopic(const std::string& topic);
    std::string getTopic() const;

    // Vérifie si un client est déjà dans le canal
    bool isClientInChannel(const Client* client) const;

    // Vérifie si un client est déjà un opérateur
    bool isClientAnOperator(const Client* client) const;

    // Récupère le nom du canal
    std::string getName() const;

    // Récupère les clients du canal
    std::set<Client*> getClients() const;

    // Envoie un message à tous les clients sur le cannal
    //void sendMessageOnChan(std::string message);

    // Récupère les modes du channel
    t_modes getModes() const;
    std::string strModes() const;

    //Fonction pour changer les modes, qui va appeler des fonctions privées.
    void    setModes(Client &client, const std::vector<std::string> &args);


private:
    std::string const name; // Nom du canal
    std::string topic; // Sujet du canal
    std::set<Client*> clients; // Ensemble des clients dans ce canal
    std::set<Client*> operators; // Ensemble des clients qui sont des operators du canal
    Channel();
    Channel(const Channel &cp);
    Channel &operator=(const Channel &ref);
    // Permissions et autres configurations du canal pourraient être ajoutées ici
    t_modes modes; //struct contenant les modes du channel.
    std::map<std::string, SetFunc> setcoms;
    void setKey(Client& client, const std::vector<std::string>& args);
    void setInvite(Client& client, const std::vector<std::string>& args);
    void setLimit(Client& client, const std::vector<std::string>& args);
    void setModTopic(Client& client, const std::vector<std::string>& args);
    void remKey(Client& client, const std::vector<std::string>& args);
    void remInvite(Client& client, const std::vector<std::string>& args);
    void remLimit(Client& client, const std::vector<std::string>& args);
    void remModTopic(Client& client, const std::vector<std::string>& args);
};

#endif // CHANNEL_HPP
