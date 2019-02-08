================
Network Protocol
================

.. warning:: This is not implemented yet.


The game will have multiplayer support.

You will be able to play in a LAN network, or through the
Internet. The server will host game rooms, limited only by hardware.
Each room can host up to 8 players

The game will always use port 18818 by default. Handshake and initial
configuration messages will be sent via TCP, game state messages via
UDP.

Every message will start with the string ``!! FLINE`` and end with two
line feed characters (``\n``).

.. note:: The final line feeds will be omitted for clarity.


Server Discovery
================

To start server discovery, the client needs to send the following
message to the network broadcasting address::

  !! FLINE DISCOVER QUERY


The server need to answer, with the following format::

  !! FLINE DISCOVER ANSWER<LF>
  name: [SERVER_NAME]<LF>
  address: [SERVER_IP]<LF>
  version: [SERVER_GAME]-[SERVER_VERSION]

.. note:: | Parameter names always will be between brackets.
	  | The brackets, however, will not be included in the message

.. note:: <LF> means one line feed character.

The fields are:

 - ``SERVER_NAME`` is the server name, like the user see, something
   like "Nice Familyline Server".
 - ``SERVER_IP`` is the IP address of the server, like 198.51.100.18.
 - ``SERVER_GAME`` is the game for the server. Default is
   ``FAMILYLINE``, all caps like this, but you can change if you make
   a derived product.
 - ``SERVER_VERSION`` is the version of the server. For now, it is
   **1**

To connect, see the `Client Handshake`_ section


Client Handshake
================

To connect to a server, the client need to open a TCP connection and
start sending the following message to the server::

  !! FLINE CONNECTION INIT<LF>
  version: [CLIENT_VERSION]<LF>
  game: [CLIENT_GAME]

``CLIENT_VERSION`` is the compatible server version. For now, it is 1.

After the server check if the game if the version is supported, it
sends the following message::

  !! FLINE CONNECTION SERVER-ATTRIBUTES<LF>
  name: [SERVER_NAME]<LF>
  version: [SERVER_VERSION]<LF>
  capabilities: [SERVER_CAPS]

``SERVER_CAPS`` lists the server capabilities.

.. note:: TODO: List the capabilities

Then, the client will send a similar set of attributes::
  
  !! FLINE CONNECTION CLIENT-ATTRIBUTES<LF>
  name: [CLIENT_NAME]<LF>
  city: [CITY_NAME]<LF>
  version: [CLIENT_VERSION]<LF>
  capabilities: [CLIENT_CAPS]

The server will respond with::
  
  !! FLINE CONNECTION CLIENT-AUTH-OK<LF>
  name: [CLIENT_NAME]<LF>
  ip: [CLIENT_IP]<LF>
  id: [CLIENT_ID]<LF>

If the connection does not error, this means that we are connected!

.. note:: | From now on, we'll not show the <LF>s on line breaks.
          | They are implicit now

Pre-Game Queries
================

Before starting the game, we need to do some things

 - Query the list of players
 - Subscribe to the player add/remove events
 - Query the list of maps
 - Download any missing maps from the server
 - Send some text to other players

We'll see these things now.

Query Players
--------------

The client needs to send the following message::

  !! FLINE CLIENT QUERY-LIST

And the server responds with::

  !! FLINE CLIENT LIST
  count: [CLIENT_COUNT]
  [0]: <ID>, <NAME>, <CITY-NAME>
  [1]: <ID>, <NAME>, <CITY-NAME>
  ...
  [N]: <ID>, <NAME>, <CITY-NAME>

Just explaining: the message returns the cities in the format
``[index]: <ID>, <NAME>, <CITY-NAME>``. The index is the index of that
client in the client list, the ID is the ID of the client, and the
other fields are self-explanatory

Subscribe to the Player List
----------------------------

You might want to subscribe to the player list. This will send player
connection and disconnection to the client everytime these things
happen.

To do this, the client needs to send the following::

  !! FLINE CLIENT QUERY-SUBSCRIBE
  events: ...

| The ``events`` parameter accepts the following values:
| (Multiple values are separated by a comma)

| **connection**: Send connection events
| **disconnection**: Send disconnection events.

The server responds with::

  !! FLINE CLIENT SUBSCRIBE
  events: ...

``events`` are the events you chose.

When a player connects to the server, the message the server sends to
each client is:

 - For connection::

     !! FLINE CLIENT EVENT connection
     id: [CLIENT_ID]
     name: [CLIENT_NAME]
     ip: [CLIENT_IP]
     city: [CLIENT_CITY]

 - For disconnection::
     
     !! FLINE CLIENT EVENT disconnection
     id: [CLIENT_ID]


List of maps
------------

To get the list of maps, the client need to send the following message::
  
  !! FLINE CLIENT QUERY-MAPS

And the server responds with::

  !! FLINE CLIENT MAPS
  count: [MAP_COUNT]
  [0]: <FILE>, <NAME>, <MAP-MD5>
  [1]: <FILE>, <NAME>, <MAP-MD5>
  ...
  [n]: <FILE>, <NAME>, <MAP-MD5>

``FILE`` is the filename of the map, relative to the maps folder,
``name`` is the formal name for the map, like "Anatolia" or "Sweet
Mountains", ``MAP-MD5`` is the MD5SUM of the map file, to see if the
map is  equal to the one the client have

If it is not, you have to download.

Download the map
----------------

To download a map from the server, the client needs to send::

  !! FLINE CLIENT DOWNLOAD-MAP
  file: <MAP-FILE>
  md5: <MAP-MD5>
  size: <MAP-BLOCKS>

One block is equal to 10k of data.
  
The server will then send the following::

  !! FLINE CLIENT SEND-MAP
  file: <MAP-FILE>
  block: <CURRENT-BLOCK>/<MAP-BLOCKS>

  START-OF-BLOCK
  
  [Block data]

  END-OF-BLOCK
  
``CURRENT-BLOCK`` is the actual block. It starts at 1 and goes until
MAP-BLOCKS.

Remember that block data equals 10k. It is surronded by START-OF-BLOCK
and END-OF-BLOCK. The line breaks are intentional: the start of block
is followed by two line feeds, then data, then two line feeds, then
the end of block mark.

When all blocks get transmitted, you must check if the checksum
matches. If it doesn't, you error or reissue the transfer, depending
on what you want. Check Errors for the relevant error codes

Text sending
------------

TODO

Errors
=======

TODO
