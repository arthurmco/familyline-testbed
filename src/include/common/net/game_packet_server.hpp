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

#include <common/net/net_common.hpp>
#include <cstdint>
#include <optional>
#include <queue>
#include <string>
#include <vector>
#include <chrono>

#include <network_generated.h>

namespace familyline::net {

/**
 * Really a copy of the NetPacket struct, so we can copy them
 */
    struct Packet {
        unsigned long int tick;
        unsigned long int source_client;
        unsigned long int dest_client;
        std::chrono::seconds timestamp;
        unsigned long int id;
    };
    

    
class GamePacketServer
{
public:
    GamePacketServer(std::string address, int port, long long id)
        : address_(address), port_(port), id_(id) {}

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
    long long id_;
    
    SOCKET socket_;

    std::vector<uint8_t> createMessage(
        flatbuffers::Offset<NetPacket> p, flatbuffers::FlatBufferBuilder& fbb);
    std::optional<Packet> decodeMessage(std::vector<uint8_t>);

    std::queue<std::tuple<flatbuffers::Offset<NetPacket>, flatbuffers::FlatBufferBuilder>>
        send_queue_;
    std::queue<Packet> receive_queue_;
};

}  // namespace familyline::net
