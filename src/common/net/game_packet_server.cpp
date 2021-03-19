#include <bits/stdint-uintn.h>
#include <fmt/format.h>
#include <sys/socket.h>

#include <common/logger.hpp>
#include <common/net/game_packet_server.hpp>
#include <common/net/server.hpp>
#include <optional>
#include "network_generated.h"


using namespace familyline::net;


Packet toNativePacket(const ::familyline::NetPacket* p) {
    std::chrono::seconds d{p->timestamp()};
    return Packet {
        p->tick(),
        p->source_client(),
        p->dest_client(),
        d,
        p->id()
    };
}

bool GamePacketServer::connect()
{
    auto& log = LoggerService::getLogger();
    
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket_ < 0) {
        throw net_exception(fmt::format(
            "Could not create socket for game packet server: {} ({})", strerror(errno), errno));
    }

    sockaddr_in addr = {};
    inet_pton(AF_INET, address_.c_str(), &addr.sin_addr.s_addr);
    addr.sin_port   = htons(port_);
    addr.sin_family = AF_INET;

    if (::connect(socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        log->write(
            "game-packet-server", LogType::Error,
            "Failed to connect to game packet server at %s:%d (%s)", address_.c_str(), port_,
            strerror(errno));

        return false;
    }

    flatbuffers::FlatBufferBuilder builder;
    auto sreq = CreateStartRequest(builder, id_, builder.CreateString("??????"));    
    auto np = CreateNetPacket(builder, 0, id_, 0, time(NULL), 1, Message_sreq, sreq.Union());

    send_queue_.push(std::make_tuple(std::move(np), std::move(builder)));

    return true;
}

/**
 * Send pending messages to their destinations and receive messages
 * from the clients
 */
void GamePacketServer::update()
{
    auto& log = LoggerService::getLogger();

    if (send_queue_.size() > 0) {
        auto& [msg, builder] = send_queue_.front();

        auto byte_data = this->createMessage(msg, builder);
        send_queue_.pop();

        auto r = send(socket_, byte_data.data(), byte_data.size(), 0);

        if (r < 0) {
            log->write(
                "game-packet-server", LogType::Error, "Error while sending a packet: %d (%s)",
                errno, strerror(errno));
        }

        if (r != byte_data.size()) {
            log->write(
                "game-packet-server", LogType::Error,
                "Data send was different than packet size (%d != %zu)", r, byte_data.size());
        }

        log->write("game-packet-server", LogType::Info, "data sent! (%zu)", byte_data.size());
    }

    uint8_t data[1024] = {};
    auto r             = recv(socket_, data, 1023, MSG_DONTWAIT);
    if (r == 0) {
        log->write("game-packet-server", LogType::Error, "Connection terminated");
        return;
    }

    if (r < 0) {
        if (errno == EWOULDBLOCK) {
            // no message to receive
            return;
        }

        log->write(
            "game-packet-server", LogType::Error, "Error while receiving a packet: %d (%s)", errno,
            strerror(errno));
    }

    std::vector<uint8_t> vdata(data, data + r);
    auto pkt = this->decodeMessage(vdata);

    if (!pkt) {
        log->write(
            "game-packet-server", LogType::Error,
            "received invalid package (len %zu, starts with %02x %02x %02x %02x",
            r,
            r>0 ? vdata[0]: 0, r>1 ? vdata[1] : 0,
            r>2 ? vdata[2]: 0, r>3 ? vdata[3] : 0);        
    } else {
        receive_queue_.push(*pkt);
    }
}

/**
 * A way to do multiple push_back()s in one line
 *
 * ```
 * push_multi(v, 1, 2, 3);
 * ```
 *
 * will work like you did
 *
 * ```
 * v.push_back(1);
 * v.push_back(2);
 * v.push_back(3);
 * ```
 */
void push_multi(std::vector<uint8_t>& v) {}

template <typename... Ts>
void push_multi(std::vector<uint8_t>& v, uint8_t val, Ts... args)
{
    v.push_back(val);
    push_multi(v, args...);
}

uint32_t calculateCRC(const std::vector<uint8_t>& data) { return 0x12345678; }

uint32_t getU32(const std::vector<uint8_t>& data, int offset)
{
    return data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) |
           (data[offset + 3] << 24);
}

bool validateCRC(const std::vector<uint8_t>& data, uint32_t expected)
{
    uint32_t checksum = data[8] | (data[9] << 8) | (data[10] << 16) | (data[11] << 24);
    return checksum == expected;
}

std::vector<uint8_t> GamePacketServer::createMessage(flatbuffers::Offset<NetPacket> p,
                                                     flatbuffers::FlatBufferBuilder& fbb)
{
    FinishNetPacketBuffer(fbb, p);
    
    std::string magic = "FAMI";
    uint32_t size = fbb.GetSize();
    std::vector<uint8_t> ret;
    std::vector<uint8_t> data(fbb.GetBufferPointer(), fbb.GetBufferPointer()+size);

    ret.insert(ret.begin(), magic.begin(), magic.begin() + 4);
    push_multi(ret, 0, 0, 0, 0);  // flags
    push_multi(ret, 0, 0, 0, 0);  // checksum (will be fixed later)
    push_multi(ret, size & 0xff, (size >> 8) & 0xff, (size >> 16) & 0xff, (size >> 24) & 0xff);

    std::copy(data.begin(), data.end(), std::back_inserter(ret));

    uint32_t checksum = calculateCRC(ret);
    ret[8]            = checksum & 0xff;
    ret[9]            = (checksum >> 8) & 0xff;
    ret[10]           = (checksum >> 16) & 0xff;
    ret[11]           = (checksum >> 24) & 0xff;

    return ret;
}


std::optional<Packet> GamePacketServer::decodeMessage(std::vector<uint8_t> data)
{
    if (data[0] != 'F' || data[1] != 'A' || data[2] != 'M' || data[3] != 'I') {
        return std::nullopt;
    }

    uint32_t flags = getU32(data, 4);
    if (flags != 0) {
        return std::nullopt;
    }

    uint32_t size = getU32(data, 12);

    auto pkt = GetNetPacket(data.data());
    return std::make_optional(toNativePacket(pkt));    
}
