#include "Client.hpp"

#include <cstdio>
using namespace familyline::Server;

Client::Client(int sockfd, struct in_addr addr)
{
    _cmq = new NetMessageQueue(sockfd, addr);

    /* Set client name as its IP */
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (void*)&addr, ipstr, INET_ADDRSTRLEN);

    char cname[INET_ADDRSTRLEN + 6];
    sprintf(cname, "<<%s>>", ipstr);
    this->name = std::string{cname};

    this->closed = false;
}

void Client::Close() { this->closed = true; }

bool Client::IsClosed() const { return this->closed; }

bool Client::CheckHeaders() const { return this->_cmq->CheckHeaders(); }
void Client::SetCheckHeaders(bool val) { this->_cmq->SetCheckHeaders(val); }

ConnectionStatus Client::GetStatus() const { return this->cstatus; }

void Client::AdvanceStatus()
{
    if (this->cstatus <= CS_INGAME) this->cstatus = ConnectionStatus(int(this->cstatus) + 1);
}

const char* Client::getName() const { return this->name.c_str(); }
void Client::SetName(char* n) { this->name = std::string{n}; }

unsigned int Client::GetID() const { return strlen(this->name.c_str()); }

/* Gets the message queue of this client */
NetMessageQueue* Client::GetQueue() { return _cmq; }

Client::~Client()
{
    if (_cmq) {
        delete _cmq;
    }
}
