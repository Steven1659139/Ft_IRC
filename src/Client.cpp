#include "../includes/Client.hpp"

Client::Client(int socket, const std::string& nickname, const std::string& username) : socket(socket), nickname(nickname), username(username) {}

Client::~Client() {
    close(socket);
}