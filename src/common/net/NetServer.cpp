#include <common/net/NetServer.hpp>

using namespace familyline::Net;

#ifdef _WIN32

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define usleep(x) Sleep(x / 1000)

#endif

/* Build the server socket */
Server::Server(const char* ipaddr, int port)
{
    // Create the socket
    _serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (_serversock < 0) {
        char* s_strerror = strerror(errno);
        char* err        = new char[256 + strlen(s_strerror)];
        sprintf(err, "error while creating socket: %s", s_strerror);

        throw ServerException(err);
        delete[] err;  // leak bitch
    }

    memset(&_serveraddr, 0, sizeof(struct sockaddr_in));
    _serveraddr.sin_family = AF_INET;
    _serveraddr.sin_port   = htons((short)port);
    inet_pton(AF_INET, ipaddr, &(_serveraddr.sin_addr));
    if (connect(_serversock, (struct sockaddr*)&_serveraddr, sizeof(struct sockaddr_in)) < 0) {
        char* s_strerror = strerror(errno);
        char* err        = new char[256 + strlen(s_strerror)];
        sprintf(err, "error while attempting connection: %s", s_strerror);

        throw ServerException(err);
        delete[] err;
    }

    cmq = new familyline::Server::NetMessageQueue(_serversock, _serveraddr.sin_addr);
}

/* Receive a message */
const char* Server::Receive(size_t maxlen)
{
    return nullptr;
}

/* Do the initial communications with the server
   Throws an exception if the server is not valid
*/
void Server::InitCommunications()
{
    
}

/* Receive messages and put them in the client message queue */
void Server::GetMessages()
{
  
}

// /**
//  * @brief Retrieves a player manager based on the player information
//  *
//  * Gets the player ID from the server. With this ID, it can know
//  * what messages come from the server and what ones come from the
//  * client.
//  * Then, with this ID, it constructs the player manager
//  *
//  * @param playername The local player name
//  */
familyline::logic::PlayerManager* Server::GetPlayerManager(const char* playername)
{
    return nullptr;
}

/**
 * Notify to the server that you're ready, or not
 * If 'v' is true, the ready status is set, else it's cleared
 */
void Server::SetReady(bool v)
{
    
}

/**
 * Check if the server will start the game.
 *
 * If this returns true, that means all clients are ready, if returns
 * false, some client isn't, and it won't start the game
 */
bool Server::IsGameStarting() const
{
   return false;
}

/* Destroy the connection */
Server::~Server()
{
    if (_serversock >= 0) {
        shutdown(_serversock, SHUT_RDWR);
        close(_serversock);
        _serversock = -1;
    }

    if (cmq) delete cmq;
}
