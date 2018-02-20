#include "ChatLogger.hpp"
#include "ChatManager.hpp"
#include <algorithm>
#include <Log.hpp>

using namespace Tribalia::Server;

/**
 * Store message in the logger
 */
void ChatLogger::Push(ChatMessage* m)
{
    const char* dest;
    switch (m->destiny) {
    case ToClient:
	dest = "client"; break;
    case ToTeam:
	dest = "team"; break;
    case ToAll:
	dest = "all"; break;
    case ToInvalid:
	dest = "???"; break;
    }
    
    Log::GetLog()->InfoWrite("chat-logger", "[from %s to %s] %s",
			     (m->sender) ? m->sender->GetName() : "server",
			     dest, m->message);
    _messages.push_back(m);
}

/**
 * Get messages that have been received only after a certain timestamp
 */
std::list<ChatMessage*>&& ChatLogger::GetMessagesNewerThan(unsigned long long epoch) const
{
    std::list<ChatMessage*> ret;
    auto retend = std::copy_if(_messages.begin(), _messages.end(), ret.begin(),
			       [epoch](ChatMessage* m) {
				   return ((unsigned long long)m->time.count() <= epoch);
			       });
    ret.resize(std::distance(ret.begin(), retend));
    return std::move(ret);
}
