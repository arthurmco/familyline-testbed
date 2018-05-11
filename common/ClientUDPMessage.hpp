/****
 * Familyline UDP message header and layout
 *
 * Copyright (C) 2018 Arthur M
 ****/

#ifndef CLIENTUDPMESSAGE_HPP
#define CLIENTUDPMESSAGE_HPP

#include <cstdint>

namespace Familyline::Server {

	/**
	 *  UDP message header layout, little endian
	 * A message is composed of multiple commands.
	 */

#pragma pack(push, 1)
	struct UDPMessageHeader {
		// The magic number (0x4d4254 = TBM\0)
		uint32_t magic;

		// The turn number
		uint32_t turn;

		// Number of commands in this message
		uint16_t cmdcount;

		// Size of this message, in bytes
		uint16_t message_size;

		// Message checksum
		// Sum all the bytes (as a uint8_t) and consider only the lower 4 bytes
		uint32_t checksum;

	};
#pragma pack(pop)

	/* The command header */
	struct UDPCommandHeader {
		uint16_t cmdtype;
		uint16_t cmdlen;
	};

	struct UDPMessage {
		UDPMessageHeader mhdr;
		UDPCommandHeader chdr[];
	};




}


#endif /* CLIENTUDPMESSAGE_HPP */

