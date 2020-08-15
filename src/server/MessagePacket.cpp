#include "MessagePacket.hpp"

using namespace familyline::Server;

MessagePacket::MessagePacket() { _raw_msg_header = new RawMessagePacket; }

MessagePacket::MessagePacket(unsigned char* data)
{
    _raw_message    = data;
    _raw_msg_header = (RawMessagePacket*)data;
}
