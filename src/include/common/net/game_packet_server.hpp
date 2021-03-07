#pragma once

/**
 * Represents the game packet server.
 * This is the non-HTTP server, the server where the client send all of the game
 * data.
 *
 * Usually, it is the same server as the game server, just in a different port,
 * but it does not need to be. They only need to have access to the same data.
 *
 * Copyright (C) 2021 Arthur Mendes
 */

#include <string>
#include <cstdint>
#include <vector>
#include <optional>
#include <common/net/net_common.hpp>

#include <queue>

namespace familyline::net
{
    struct Packet {int size;};

class GamePacketServer
{
public:
    GamePacketServer(std::string address, int port)
        : address_(address), port_(port)
        {}

    /**
     * Connect to the server
     *
     * Returns true on success, false on error
     */
    bool connect();

    /**
     * Send pending messages to the server and receive messages from the server
     */
    void update();
    
private:
    std::string address_;
    int port_;

    SOCKET socket_;
    
    std::vector<uint8_t> createMessage(const Packet& p);
    std::optional<Packet> decodeMessage(std::vector<uint8_t>);

    std::queue<Packet> send_queue_;
    std::queue<Packet> receive_queue_;
    
};

}  // namespace familyline::net
