# IRC Server

**IRC Server** est un projet développé pour implémenter un serveur IRC (Internet Relay Chat) en C++. Ce serveur gère les connexions clients, la création de canaux, et l'exécution de commandes IRC standard. Le projet met en œuvre des concepts avancés de programmation réseau, de gestion des processus, et de manipulation de sockets en C++.

## Fonctionnalités

- **Gestion des Connexions Réseau** :
  - Création et configuration de sockets pour la communication réseau.
  - Gestion des connexions multiples avec `select()`.
  - Support pour les connexions client via TCP/IP.

- **Gestion des Clients** :
  - Authentification des clients avec mot de passe.
  - Support pour plusieurs clients connectés simultanément.
  - Gestion des messages privés et des canaux.

- **Commandes IRC** :
  - **JOIN** : Rejoindre un canal.
  - **PART** : Quitter un canal.
  - **PRIVMSG** : Envoyer un message privé à un utilisateur ou à un canal.
  - **NICK** : Changer de pseudonyme.
  - **USER** : S'enregistrer avec un nom d'utilisateur.
  - **TOPIC** : Définir ou afficher le sujet d'un canal.
  - **MODE** : Gérer les modes des canaux (ex. clé, limite d'utilisateurs, etc.).
  - **KICK** : Expulser un utilisateur d'un canal.
  - **INVITE** : Inviter un utilisateur à rejoindre un canal.
  - **QUIT** : Quitter le serveur.

- **Gestion des Canaux** :
  - Création et suppression de canaux.
  - Gestion des opérateurs de canal.
  - Gestion des modes de canal (par exemple, canal privé, canal avec limite d'utilisateurs).

## Installation

Pour compiler et exécuter le serveur IRC, suivez les étapes ci-dessous :

1. Clonez ce dépôt sur votre machine locale :
    ```bash
    git clone https://github.com/Steven1659139/irc-server.git
    ```

2. Accédez au répertoire du projet :
    ```bash
    cd irc-server
    ```

3. Compilez le projet en utilisant `make` :
    ```bash
    make
    ```

## Utilisation

Pour lancer le serveur IRC, utilisez la commande suivante :

```bash
./irc-server <port> <password>
```

### Exemple

```bash
./irc-server 6667 mypassword
```

Ce qui démarre le serveur sur le port 6667 avec mypassword comme mot de passe.


## Fonctionnement Interne

1. **Création et configuration du socket** :
   - Le serveur crée un socket et le configure pour accepter les connexions TCP/IP.

2. **Gestion des connexions** :
   - Le serveur utilise `select()` pour surveiller les activités sur le socket principal (nouvelles connexions) et sur les sockets des clients connectés (messages entrants).

3. **Traitement des commandes** :
   - Les commandes IRC envoyées par les clients sont analysées et traitées par le `CommandHandler`, qui exécute les actions appropriées en fonction de la commande reçue.

4. **Gestion des canaux** :
   - Le serveur gère la création, la modification, et la suppression des canaux. Les utilisateurs peuvent rejoindre, quitter, et interagir avec les canaux selon les permissions définies.
