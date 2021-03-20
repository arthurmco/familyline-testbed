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

#include <network_generated.h>

#include <chrono>
#include <common/logic/player_actions.hpp>
#include <common/net/net_client.hpp>
#include <common/net/net_common.hpp>
#include <cstdint>
#include <functional>
#include <future>
#include <optional>
#include <queue>
#include <string>
#include <tl/expected.hpp>
#include <variant>
#include <vector>
#include <mutex>

namespace familyline::net
{
/**
 * Has the same information of the NetPacket structure from flatbuffers, but in a more C++-sy way
 *
 * We needed to use this structure also because it was much easier to copy this structure. since the
 * NetPacket has no copy constructors, and moving it did not seem to work also
 */
struct Packet {
    unsigned long int tick;
    unsigned long int source_client;
    unsigned long int dest_client;
    std::chrono::seconds timestamp;
    unsigned long int id;

    struct NStartRequest {
        uint64_t client_id;
        std::string token;
    };

    struct NStartResponse {
        uint64_t client_ack;
        bool all_clients_ack;
    };

    struct NLoadingRequest {
        unsigned short percent;
    };

    struct NLoadingResponse {
        unsigned short percent;
    };

    struct NGameStartRequest {
        unsigned int val;
    };

    struct NGameStartResponse {
        unsigned int val;
    };

    struct InputRequest {
        uint64_t client_from;
        logic::PlayerInputType input;
    };

    struct InputResponse {
        uint64_t client_from;
        bool client_ack;
    };

    std::variant<
        NStartRequest, NStartResponse, NLoadingRequest, NLoadingResponse, NGameStartRequest,
        NGameStartResponse, std::monostate>
        message;
};

/**
 * The network client
 *
 * The network client is where you will send and receive packets (depending on its configuration)
 * You cannot use the GamePacketServer directly, so you will need to use this
 *
 * TODO: for the love of god, move this to its own file
 */
class NetworkClient
{
    using FnClientOut = std::function<void(Packet&&)>;
    using FnClientIn  = std::function<bool(Packet&)>;

private:
    // the client will call this function when it needs to send a packet
    FnClientOut fn_out_;

    // the client will call this function when it needs to receive a packet
    // you return true if you have a packet, false if you do not
    FnClientIn fn_in_;

    // the client ID
    uint64_t id_;

public:
    NetworkClient(uint64_t id, FnClientOut fn_out, FnClientIn fn_in)
        : id_(id), fn_out_(fn_out), fn_in_(fn_in)
    {
    }
};

class GamePacketServer
{
public:
    GamePacketServer(std::string address, int port, std::string token, long long id, std::vector<CClientInfo> clients)
        : address_(address), port_(port), token_(token), id_(id), clients_(clients)
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

    /**
     * Wait for all clients to be connected, or until a timeout
     *
     * This future will only be valid after all clients are connected.
     * If no clients are connected, it will return a NetResult, matching the ServerError we found.
     * Not all server errors will be returned, though
     */
    std::future<tl::expected<std::vector<NetworkClient>, NetResult>> waitForClientConnection(
        int timeout = 90);

    GamePacketServer(GamePacketServer&& c) {
        this->address_ = c.address_;
        this->port_ = c.port_;
        this->id_ = c.id_;
        this->token_ = c.token_;
        this->socket_ = c.socket_;
        this->clients_ = c.clients_;
        this->last_message_id_ = c.last_message_id_;
        this->send_queue_ = c.send_queue_;
        this->receive_queue_ = c.receive_queue_;
        
        c.socket_ = -1;
    }

    /// The move assignment operator and move constructor are needed, because, without them
    /// the copy assignment would be used in some places, and the default move constr would try
    /// to copy the mutex. This does not work for obvious reasons: it would be like copying
    /// a ticket.
    GamePacketServer& operator=(GamePacketServer&& c) {
        this->address_ = c.address_;
        this->port_ = c.port_;
        this->id_ = c.id_;
        this->token_ = c.token_;
        this->socket_ = c.socket_;
        this->clients_ = std::move(c.clients_);
        this->last_message_id_ = c.last_message_id_;
        this->send_queue_ = c.send_queue_;
        this->receive_queue_ = c.receive_queue_;
        
        c.socket_ = -1;
        return *this;
    }

private:
    std::string address_;
    int port_;
    std::string token_;
    long long id_;

    SOCKET socket_;
    bool connected_ = false;
    
    std::vector<CClientInfo> clients_;

    /**
     * The last message ID we sent
     *
     * We will increase the message ID as we send messages
     */
    int last_message_id_ = 0;

    /**
     * From a native packet, create a flatbuffers packet
     *
     * Returns the binary content of the packet
     */
    std::vector<uint8_t> createMessage(const Packet& p);

    /**
     * From a vector of bytes, build a packet.
     *
     * Returns a valid packet if the bytestream is a valid packet, else None if not
     */
    std::optional<Packet> decodeMessage(std::vector<uint8_t>);

    /**
     * Transform a native packet to a serialized packet
     */
    flatbuffers::Offset<::familyline::NetPacket> toSerializedPacket(
        const Packet& p, flatbuffers::FlatBufferBuilder& b);

    std::mutex send_mutex_;
    std::queue<Packet> send_queue_;
    std::mutex receive_mutex_;
    std::queue<Packet> receive_queue_;
};

}  // namespace familyline::net
