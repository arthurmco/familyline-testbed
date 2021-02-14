#include <fmt/format.h>

#include <common/logger.hpp>
#include <common/net/server.hpp>
#include <nlohmann/detail/exceptions.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string_view>

using namespace familyline::net;
using json = nlohmann::json;

#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

using namespace curlpp::options;

std::stringstream CServer::buildRequest(
    curlpp::Easy& req, std::string endpoint, std::string method, bool jsonbody, std::string data)
{
    std::stringstream outstr;

    // Set the URL and the timeout value.
    req.setOpt<Url>(fmt::format("http://{}/{}", address_, endpoint));
    req.setOpt<Timeout>(timeout_secs_);
    req.setOpt<WriteStream>(&outstr);

    if (method == "POST") {
        req.setOpt<Post>(true);
    }

    if (jsonbody) {
        std::list<std::string> header;
        auto datasize = data.size();

        header.push_back("Accept: application/json");
        header.push_back("Content-Type: application/json");
        req.setOpt<HttpHeader>(header);
        req.setOpt<PostFields>(data);
        req.setOpt<PostFieldSize>(data.size());
        req.setOpt<Verbose>(true);
    }

    return outstr;
}

uint64_t CServer::getUserID() const { return userID_; }
bool CServer::isLogged() const { return client_token_ != ""; }

ServerResult CServer::checkErrors(unsigned httpcode, std::stringstream& body)
{
    body.seekg(0);

    auto& log     = LoggerService::getLogger();

    if (httpcode < 200 || httpcode > 299) {
        switch (httpcode) {
            case 400: {
                json response = json::parse(body);
                std::string message = response["message"];
                log->write("cli-server", LogType::Error, "login failure: %s", message.c_str());
                return ServerResult::LoginFailure;
            }
            default:
                log->write(
                    "cli-server", LogType::Error, "login returned weird HTTP code: %d", httpcode);
                return ServerResult::ConnectionError;
        }
    }

    body.seekg(0);
    return ServerResult::OK;
}

ServerResult CServer::login(std::string address, std::string username)
{
    address_  = address;
    auto& log = LoggerService::getLogger();

    log->write("cli-server", LogType::Info, "logging to %s", address.c_str());
    unsigned httpcode = 0;

    try {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        json j;
        j["name"] = username;
        auto sstr = this->buildRequest(myRequest, "login", "POST", true, j.dump());

        // Send request and get a result.
        // By default the result goes to standard output.
        myRequest.perform();

        auto httpcode = curlpp::infos::ResponseCode::get(myRequest);

        if (auto e = this->checkErrors(httpcode, sstr); e != ServerResult::OK) {
            return e;
        }

        json response = json::parse(sstr);
        client_token_ = response["token"];
        userID_       = response["user_id"];

        if (response["name"] != username || client_token_ == "" || userID_ == 0) {
            return ServerResult::ServerError;
        }

        log->write(
            "cli-server", LogType::Info,
            "logged succesfully: username=%s, userid=%ull, token=<XXX>", username.c_str(), userID_);

    } catch (curlpp::RuntimeError& e) {
        std::string_view exc{e.what()};
        if (exc.find("Connection timed out") != std::string::npos) {
            return ServerResult::ConnectionTimeout;
        }

        std::cerr << "Runtime: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "Logic: " << e.what() << std::endl;
    } catch (nlohmann::detail::exception& e) {
        log->write(
            "cli-server", LogType::Error,
            "server returned status %d and we could not parse the incoming JSON (%s),", httpcode,
            e.what());
        return ServerResult::ServerError;
    }

    return ServerResult::OK;
}

json createTokenMessage(std::string token)
{
    json j;
    j["token"] = token;
    return j;
}

/**
 * Logout from the server
 *
 * This function should not fail, but, like the real life, where some retarded
 * people might not let you leave a certain place because 'they want to stay with you',
 * the server might act retarded.
 * Or the server might crash.
 *
 * You must be prepared for that.
 */
ServerResult CServer::logout()
{
    auto& log = LoggerService::getLogger();
    
    if (!isLogged()) {
        log->write("cli-server", LogType::Warning, "logging out but already logged off");
        return ServerResult::AlreadyLoggedOff;
    }

    log->write("cli-server", LogType::Info, "logging out from %s", address_.c_str());
    unsigned httpcode = 0;

    try {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        json j    = createTokenMessage(this->client_token_);
        auto sstr = this->buildRequest(myRequest, "logout", "POST", true, j.dump());

        // Send request and get a result.
        // By default the result goes to standard output.
        myRequest.perform();

        auto httpcode = curlpp::infos::ResponseCode::get(myRequest);

        if (auto e = this->checkErrors(httpcode, sstr); e != ServerResult::OK) {
            return e;
        }

        json response = json::parse(sstr);
        auto serverid = response["id"];

        if (serverid != userID_) {
            log->write(
                "cli-server", LogType::Error,
                "for some reason, IDs between client and server differ (cli %llx != ser %llx)",
                userID_, (uint64_t)serverid);
            return ServerResult::ServerError;
        }

        log->write("cli-server", LogType::Info, "logged out succesfully");

    } catch (curlpp::RuntimeError& e) {
        std::string_view exc{e.what()};
        if (exc.find("Connection timed out") != std::string::npos) {
            return ServerResult::ConnectionTimeout;
        }

        std::cerr << "Runtime: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "Logic: " << e.what() << std::endl;
    } catch (nlohmann::detail::exception& e) {
        log->write(
            "cli-server", LogType::Error,
            "server returned status %d and we could not parse the incoming JSON (%s),", httpcode,
            e.what());
        return ServerResult::ServerError;
    }

    return ServerResult::OK;
}
