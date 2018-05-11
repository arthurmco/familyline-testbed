/***
 	Server message packet decoding and encoding

	Copyright (C) 2016 Arthur M
***/

#include <cstdint>

#ifndef MESSAGEPACKET_HPP
#define MESSAGEPACKET_HPP

namespace Familyline {
namespace Server {

/* The action values */
enum MessageAction {
	/* Object created
	 * Params: the position and all the object parameters you can get,
	 * Format is like this:
	 * <x><y><z><property1>\0<value1>\0<property2>\0<value2>\0...
	 * */
	ACTION_CREATED,		
	
	/* Object deleted
	 * Params: none
	 * */
	ACTION_DELETED,

	/* Position changed
	 * Params: the new position on terrain grid (x,y), in _game space_
	 * */
	ACTION_POS_CHANGED,

	/* Property changed.
	 * Params: the property name, zero terminated, and property value.
	 * This action can only be used with base types and structs */
	ACTION_PROPERTY_CHANGED,
};

/* The raw packet definition, the beginning of what will
 * be sent. After these data, only what changed will be sent
 * for the clients */
struct RawMessagePacket {
	uint32_t magic;	/* The packet magic number */
	uint32_t oid;	/* The object ID of referred object */
	uint32_t tid;	/* The type ID of referred object */
	int action;		/* What happened to this object */
};


class MessagePacket {
private:
	unsigned char* _raw_message;
	RawMessagePacket* _raw_msg_header;


public:
	MessagePacket();
	MessagePacket(unsigned char* data);


};



}
}


#endif
