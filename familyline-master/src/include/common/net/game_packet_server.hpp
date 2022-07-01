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
#include <common/net/network_client.hpp>
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

#ifdef WIN32
#define send(s, buf, n, opt) send(s, (const char*)buf, n, opt)
#define recv(s, buf, n, opt) recv(s, (char*)buf, n, opt)
#endif


namespace familyline::net
{

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
     * Send a message that will tell the server that you are loading data
     */
    void sendLoadingMessage(int percent);

    /**
     * Send a message that will tell the server you are ready to start the game
     */
    void sendStartMessage();

    /**
     * Send an input message to the server (so it can tell other clients)
     */
    void sendInputMessage(logic::PlayerInputAction&);

    /**
     * The ID of our client
     */
    uint64_t id() { return id_; }
    
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
        this->client_receive_queue_ = std::move(c.client_receive_queue_);
        this->dispatch_client_messages_.exchange(c.dispatch_client_messages_);

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
        this->client_receive_queue_ = std::move(c.client_receive_queue_);
        this->dispatch_client_messages_.exchange(c.dispatch_client_messages_);

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
    std::atomic<bool> dispatch_client_messages_ = false;

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
     * From a vector of bytes, build a list of packets.
     *
     * Since TCP is a streaming protocol, two messages can be sent in one packet.
     * We must be prepared for that.
     *
     * Returns a vector with length > 0 if the packet is valid, otherwise return {}
     */
    std::vector<Packet> decodeMessage(std::vector<uint8_t>);

    /**
     * Transform a native packet to a serialized packet
     */
    flatbuffers::Offset<::familyline::NetPacket> toSerializedPacket(
        const Packet& p, flatbuffers::FlatBufferBuilder& b);


    Packet createPacket(uint64_t tick, uint64_t source, uint64_t dest,
                        uint64_t id, decltype(Packet::message) message);

    /**
     * Enqueue a packet
     */
    void enqueuePacket(Packet&& p);


    /**
     * Poll a packet for a specific client
     *
     * Returns true if we have a packet, false if we do not
     */
    bool pollPacketFor(uint64_t id, Packet& p);

    std::mutex send_mutex_;
    std::queue<Packet> send_queue_;
    std::mutex receive_mutex_;

    /**
     * A receive queue for the packets that came from the server directly
     */
    std::queue<Packet> receive_queue_;

    /**
     * An individual receive queue for each client
     *
     * We start using them after we detect all clients, after we return
     * a valid result for `waitForClientConnection()`.
     */
    std::unordered_map<uint64_t, decltype(receive_queue_)> client_receive_queue_;
};

}  // namespace familyline::net
