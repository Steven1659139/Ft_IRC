#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <algorithm>
# include "Client.hpp" 

class Channel {
public:

    Channel(const std::string& name, const std::string& topic = "");

    // Ajoute un client au canal
    // bool addClient(Client* client);

    // Supprime un client du canal
    // bool removeClient(Client* client);

    // Envoie un message à tous les clients du canal
    // void broadcastMessage(const std::string& message, const Client* sender = nullptr);

    // Définit ou obtient le sujet du canal
    void setTopic(const std::string& topic);
    // std::string getTopic() const;

    // Vérifie si un client est déjà dans le canal
    // bool isClientInChannel(const Client* client) const;

    // Récupère le nom du canal
    std::string getName() const;

private:
    std::string const name; // Nom du canal
    std::string topic; // Sujet du canal
    std::set<Client*> clients; // Ensemble des clients dans ce canal
    Channel();
    Channel(const Channel &cp);
    Channel &operator=(const Channel &ref);

    // Permissions et autres configurations du canal pourraient être ajoutées ici
};

#endif // CHANNEL_HPP
