/**
   Provides logging functionality for the chat

   Copyright (C) 2018 Arthur M
**/

#include <list>

#ifndef CHATLOGGER_HPP
#define CHATLOGGER_HPP

namespace Tribalia::Server {

struct ChatMessage;

class ChatLogger {
private:
    std::list<ChatMessage*> _messages;

public:
    /**
     * Store message in the logger
     */
    void Push(ChatMessage*);

    /**
     * Get messages that have been received only after a certain timestamp
     */
    std::list<ChatMessage*>&& GetMessagesNewerThan(unsigned long long epoch) const;
    
};



}  // Tribalia::Server



#endif /* CHATLOGGER_HPP */
