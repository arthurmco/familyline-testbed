#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <common/logger.hpp>
#include <common/net/server_finder.hpp>
#include <optional>
#include <string_view>
#include <thread>

#include <nlohmann/json.hpp>

using namespace familyline::net;
using json = nlohmann::json;

in_addr getLocalIP()
{
    struct in_addr local_addr;
    local_addr.s_addr = inet_addr("192.168.1.112");

    return local_addr;
}

ServerFinder::ServerFinder()
{
    auto& log = LoggerService::getLogger();

    socket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_ < 0) {
        throw net_exception(fmt::format(
            "Could not create socket for server finder: {} ({})", strerror(errno), errno));
    }

    mcast_addr_            = {};
    mcast_addr_.sin_family = AF_INET;
    mcast_addr_.sin_port   = htons(multicast_port_);
    inet_pton(AF_INET, multicast_ip_str_, &mcast_addr_.sin_addr.s_addr);

    auto local_addr = getLocalIP();
    if (setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_IF, (char*)&local_addr, sizeof(local_addr)) <
        0) {
        throw net_exception(fmt::format(
            "Could not set multicast interface for server finder: {} ({})", strerror(errno),
            errno));
    }
}

ServerFinder::~ServerFinder()
{
    if (discovering_) this->stopDiscover();
    close(socket_);
}

std::vector<std::string> splitLines(std::string_view s, char sep = '\n') {
    auto strptr = s;
    std::vector<std::string> r;

    auto lastidx = 0;
    auto idx = 0;

    do {
        lastidx = idx;
        idx = s.find_first_of(sep, lastidx);
        strptr = s.substr(lastidx, idx-lastidx);

        std::string s{strptr};

        if (auto cr = s.find_first_of('\r'); cr != std::string::npos)
            s.erase(cr);
        
        r.push_back(s);

        if (idx != std::string_view::npos)
            idx += 1;
        
    } while (idx != std::string_view::npos);

    return r;
}

/**
 * Parse the HTTP-based information of the discovery response
 *
 * Return the server information, or nullopt if the server information is
 * invalid
 */
std::optional<ServerInfo> parseDiscoverInformation(std::string_view data){

    auto lines = splitLines(data);

    bool http_message = false;
    bool content_starts = false;
    
    std::optional<ServerInfo> si = {};

    std::string addr;
    unsigned int port = 8100;

    std::string json_data = {};
    std::string version;
    
    for (auto& l : lines) {
        if (l.starts_with("HTTP") && l.ends_with("200 OK")) {
            http_message = true;
            content_starts = false;
            si = {};
        }

        if (l.starts_with("Location")) {
            if (auto midx = l.find_first_of(' '); midx != std::string::npos) {
                auto lval = l.substr(midx+1);

                // Location has the format
                //    http://<url>
                //    url can be an IP or a hostname, followed by an optional port value after a colon

                if (auto httpos = lval.find("http"); httpos != std::string::npos) {
                    auto residx = httpos + 7;
                    auto portidx = lval.find_first_of(':', residx);

                    if (portidx != std::string::npos) {
                        addr = lval.substr(residx, portidx-residx);
                        
                        port = std::atoi(lval.substr(portidx+1).c_str());
                    } else {
                        addr = lval.substr(residx+1);                        
                    }
                    
                }
                
            }            
        }

        if (l == "" && http_message && !content_starts) {
            content_starts = true;
            continue;
        }

        if (l.starts_with("Server")) {
            if (auto midx = l.find_first_of(' '); midx != std::string::npos) {
                auto lval = l.substr(midx+1);

                if (lval.starts_with("familyline-server"))  {
                    int versionval = strlen("familyline-server") + 1;
                    
                    version = lval.substr(versionval);
                } else {
                    return std::nullopt;
                }
            }
        }

        if (content_starts) {
            json_data = l;
            content_starts = false;
            http_message = false;
        }
        
    }

    if (json_data.size() == 0) {
        return std::nullopt;
    }

    auto body = json::parse(json_data);

    ServerInfo sinfo = {};
    sinfo.ip_addr = addr;
    sinfo.port = port;
    sinfo.version = version;
    sinfo.name = body["name"];
    sinfo.description = "";
    sinfo.player_count = body["num_clients"];
    sinfo.player_max = body["max_clients"];
    
    return std::make_optional(sinfo);
    
};

/// Start the server discovery process.
///
/// You will need to register a callback; the server finder will call
/// it for each server it discovers
///
/// The discovery will happen in a separate thread, so watch out.
void ServerFinder::startDiscover(discovery_cb callback)
{
    discovering_  = true;
    thr_discover_ = std::thread(
        [](SOCKET& s, struct sockaddr_in multicastaddr, bool& operating, discovery_cb cb) {
            auto& log = LoggerService::getLogger();
            log->write("server-finder", LogType::Info, "starting discover");

            std::unordered_map<std::string, ServerInfo> servers;
            
            std::string message =
                "M-SEARCH * \r\n"
                "MAN: \"ssdp:discover\"\r\n"
                "ST: game_server:familyline1\r\n"
                "\r\n";

            if (sendto(
                    s, message.data(), message.size(), 0, (struct sockaddr*)&multicastaddr,
                    sizeof(multicastaddr)) < 0) {
                log->write(
                    "server-finder", LogType::Error,
                    "sendto() failed when sending discovery message: %d (%s)", errno,
                    strerror(errno));
                return;
            }

            usleep(2000);

            // Use a non-blocking receive so it does not block the rest of the game.
            //
            // We need to be able to detect that the discovery has been canceled.
            while (operating) {
                char buf[4096] = {};
                if (recvfrom(s, buf, 4095, MSG_DONTWAIT, nullptr, 0) < 0) {
                    switch (errno) {
                        case EWOULDBLOCK: break;
                        default:
                            log->write(
                                "server-finder", LogType::Error,
                                "recvfrom() failed when receiving response of discovery message: "
                                "%d (%s)",
                                errno, strerror(errno));
                            return;
                    }

                    continue;
                }

                log->write("server-finder", LogType::Info, "received something");

                auto data = parseDiscoverInformation(std::string_view{buf, 4096});
                if (data && !servers.contains(data->ip_addr)) {
                    cb(*data);
                    servers[data->ip_addr] = *data;
                }
            }

            puts(":(");
        },
        std::ref(socket_), mcast_addr_, std::ref(discovering_), callback);
}

/// Stop the server discovery process
void ServerFinder::stopDiscover()
{
    discovering_ = false;
    auto& log    = LoggerService::getLogger();
    thr_discover_.join();
    log->write("server-finder", LogType::Info, "stopping discover");
}
