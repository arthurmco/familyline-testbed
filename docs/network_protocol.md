# Familyline gameplay network protocol

The Familyline protocol is used by connecting clients to the game server, allowing for 
some sort of good gameplay.

You might be able to use different games, if they're compatible with the same 
engine, if the server supports, although it's not recommended

## Ports

 - **TCP 12000**: Used by initial connection setup and transmitting player
   information and chat commands. Usually UTF-8 text
 
 - **UDP 12000**: Used by transmitting game commands and returning game object
   information. Always binary.
   
## Connection setup
   
This command sequency is used for setting up an initial connection. After this
command sequence, clients have to be set as valid, but not fully connected,
since they didn't receive some important data from the server.

_Note: All messages starts with `[TRIBALIA` and ends with `]` and a newline
(ascii 10). The newline is omitted for clarity_

 - CLIENT: `[TRIBALIA CONNECT]`
 - SERVER: `[TRIBALIA CONNECT <<client_ip>> ]`
 
 Here you need to inform the client IP. This might not be the real IP address,
 but the external address of its network.  
  TODO: Check how this affect gameplay through the internet
 
 - C: `[TRIBALIA CONNECT OK]`
 - S: `[TRIBALIA VERSION?]`
 - C: `[TRIBALIA VERSION 0.1]`
 
 Here the client replies with the version. The actual version is 0.1
 
 - C: `[TRIBALIA CAPS? ]`
 
 Here the client asks for the server capabilities. These two messages in
 sequence is done on purpose to break those shitty servers who don't split
 messages properly. (TCP is a stream protocol, these messages will certainly
 appear together)
 
 - S: `[TRIBALIA CAPS <<capabilities>>]`
 
 Here the server shows its capabilities. It can be, for example, another game
 within the same engine, or something nice like compression or encryption.  
 
 The capabilities are space-separated strings. Capabilities between parenthesis
 means that the capability is mandatory for the server to support it.
 [e.g `[TRIBALIA CAPS (encryption) compression hiper-big-maps]` means that the
 capability "encryption" is mandatory, but "compression" and "hiper-big-maps"
 are not.  
 
 Currently, no capability is officially supported yet.  
 
 The client _can_ abort the connection if it don't like some capability. It
 _must_ abort if it doesn't support some mandatory capability.
 
 - C: `[TRIBALIA CAPS <<capabilities>>]`
 
 Here the client show the supported capabilities.  
 
 The same capability rules for the server applies in the client.  
 
 The server _can_ abort the connection if it don't like some capability. It
 _must_ abort if the client doesn't support some mandatory capability.
 
 
 Now the client is valid. The server must show client status as "Connecting" in
 this phase. The client must show something like "Connecting to server" to the
 user.
 
## Requiring information

To be fully connected, the client needs to send its own player information and
require two informations from the server: the player list and the map list.

After these information were requested, the server might show the client as
"Connected" and the client might show a list of connected players and a list of
maps to choose or vote.

The next message after that **must** be the 'send player info'

### Send player info

 - S: `[TRIBALIA PLAYERINFO?]`
 
 The player information retrieval is triggered when the client asks the question
 above.
 
 - C: `[TRIBALIA PLAYERINFO <<name>> <<experience>> ]`
 
 The client sends its player name and experience.
 
 TODO: Add experience validation (in a master server?)


 - S: `[TRIBALIA PLAYERINFO <<id>>]`
 
 The server returns an ID to the player. This ID will be used in the UDP-based
 messages as a way to validate the client. It will be referred as the
 **player-id**
 
 
### Send other players to the client ###

It all starts with a question to the client;

 - C: `[TRIBALIA PLAYERS?]`
 
 - S: `[TRIBALIA PLAYERS <<qt_players>> | <<id[0]>> <<name[0]>> <<exp[0]>> | 
	 <<id[1]>> <<name[1]>> <<exp[1]>> | ... | <<id[n]>> <<name[n]>> <<exp[n]>> ]`
 
 id is the player ID, name is the player name and exp is the player experience
 
 From this moment, the player can receive client status update.
 
 This message includes the sending client own information too.
 
### Send map list to the client ###

 - C: `[TRIBALIA MAPS?]`
 
 - S: `[TRIBALIA MAPS <<qt_maps>> | <<map-tag[0]>> <<map-name[0]>> <<checksum[0]>> | 
	 <<map-tag[1]>> <<map-name[1]>> <<checksum[1]>> | ... | <<map-tag[n]>> <<map-name[n]>> <<checksum[n]>> ]`
	 
 map-tag is a unique string that identifies the map, map-name is the map name, as it would 
 appear in the game and checksum is a SHA-1 checksum of the whole map file.
 
 id is the player ID, name is the player name and exp is the player experience
 
#### Downloading maps ####

In some cases, the client will not have the map in the computer it is running

When this happens, the client needs to send the following message:

 - C: `[TRIBALIA MAP REQUIRE <<map-tag>> ?]`
 
 - S: `[TRIBALIA MAP REQUIRE <<map-tag>> <<map-size>>]`
 
 map-tag is the tag of the map you want to download, and map-size is the map size in bytes.
	 
 - C: `[TRIBALIA MAP REQUIRE <<map-tag>> START]`
  
 When the client send the message above, the download starts.
 
 The server will send to the client the map divided in 1kb blocks. Each of these blocks will 
 go in its own message. The message itself is like this:
 
 - S: `[TRIBALIA MAP REQUIRE <<map-tag>> <<block-num>> <<block-count> {{<<block-data>>}} ]`
 
 map-tag is the map tag, block_num is the number of the block who is being transferred, block count is the
 block count (the size in kilobytes rounded up) and the block data is the block binary data, circunded by
 two curly brackets.
 
 After all blocks get transferred, the conversation will continue like:
 
 - C: `[TRIBALIA MAP REQUIRE <<map-tag>> END <<map-checksum>>]`
 
 map-tag is the tag, map-checksum is the SHA-1 of receiving map.


## Game start.
 
All clients, when ready, should send a message called `[TRIBALIA GAME
READY]`. This might be symbolized as a "Ready" checkbox in the game selection
interface.
 
If the client isn't ready anymore, it should send `[TRIBALIA GAME NOTREADY]` to
the server. This might be symbolized as the user unchecking the "Ready" checkbox
mentioned above.

All clients should be marked as ready. When this happens, the server should send
`[TRIBALIA GAME STARTING]` and wait a few seconds before start the game. When
receiving the message, the client should alert the user that the game is
starting. The not ready message is still respected and will abort the countdown.

After the wait, the server should send the `[TRIBALIA GAME STARTED]` message to
all clients. Clients, then, should connect in the UDP port. The NOT
READY message will be ignored

## Client chatting

Some primitive form of chat is supported

TODO: Consider using XMPP as the chat protocol. Might add an official
capability.

C: `[TRIBALIA CHAT <<sender_id>> all|team|player:<<player_id>>
  <<message-string-in-bytes>> <<message-string>> ]`
  
  * sender_id: The client ID of the sender.
  
  * all, team, player: Only one can exist. If...
  
	  - all exists, it means that the message is for everyone in the game
   
	  - team exists, it means that the message is for everyone in the sender's team
   
	  - player:`<<player_id>>` exists, it means that is for the player with 
  ID equals to `<<player_id>>`
	 
  * message-string-in-bytes: The message size **in bytes**, not in chars!
  
  * message-strings: An UTF-8 formatted string message.
 
 
# UDP port

After the client notifies it is ready to the server (as told on [here](#game-start),
you should connect to the UDP port. The port has the same number of the TCP one.

The first step is sending a [START-CONNECTION](#START\_CONNECTION)
message, then do the conversation until the [START_SYNC](#START\_SYNC) message is sent, for
everyone. Then you can start the game loop

## How does it work?

	In Familyline, UDP messages sends commands from the client to the
server, or vice-versa, in binary format. 

The client waits for a certain amount of time (called a turn, 16ms by default) 
and then sends it to the server. The server waits for everyone to send their messages
in that turn and then waits for the next one.

> TODO: Make turn time customizable?

If one of the clients send a message with a turn that is less than the 
current one ... 
<span style="color:red">__(I don't know, maybe increase the turn time?)__</span>

If one of the clients send a message with a turn that is more than the
current one, than that client is disconnected.

<span style="color:red">**REALLY** needed? </span>
At the end of each turn, the server will inquiry the status of the
client by sending a query for the position of an object, and waiting
for a response. The client position needs to be equal of the server,
or the client will be disconnected with an "Object Position Mismatch"
error.

## Message Header

Every UDP message has the following header. Remember that all numbers
are in little endian format here.

| offset | bytes | name     | desc                                                   |
|-------:|------:|----------|:-------------------------------------------------------|
|     0h |     4 | magic    | The magic number of the message <br/>(0x4d4254 == TBM) |
|     4h |     4 | turn     | The turn number.                                       |
|     8h |     2 | cmdcount | The number of commands in this message.                |
|     Ah |     2 | length   | The size of the message, in bytes                      |
|     Ch |     4 | checksum | The message checksum                                   |
|    10h |   ... | commands | The commands                                           |
   
The magic number is for identificating the package as a Familyline
Message package (hence the TBM).

The turn number is the turn number for that package. If the gameplay
has not started (prior to the `START_SYNC` command), then the turn
is 0. Turn numbers start at 1.

The checksum is a 4-byte unsigned number (`u32` or `uint32_t`)
calculated by summing all the bytes of the message (aka convert the
whole message to an array of `uint8_t`s and sum them all), with the
checksum field filled with zeroes. If the sum overflows, then consider
only the least significant 4 bytes.

After the message header, we get to the command header.

## Command Header

Remember that the offset is relative to **the header**

| offset | bytes | name    | desc                        |
|-------:|------:|---------|:----------------------------|
|     0h |     2 | cmdtype | The command type            |
|     2h |     2 | cmdlen  | The command size (in bytes) |
|     4h |   ... | cmddata | The command data            |

We have a lot of commands:

### START\_CONNECTION ###

**Type:** Server->Client  
**Type ID:** 0x1  
Indicates the connection start. 

| offset | bytes | name     | desc                     |
|-------:|------:|----------|:-------------------------|
|     0h |     4 | cmdhdr   | The command header       |
|     4h |     2 | turntime | The time per turn, in ms\* |

\* The default turn period is 16ms (~60Hz)

After the client receives the message above, it needs to send the START\_CONECTION\_ACK

### START\_CONNECTION\_ACK ###

**Type:** Client->Server  
**Type ID:** 0x2  
Indicates that the client received the START\_CONNECTION message above
and is ready to start switching messages via UDP port

| offset | bytes | name     | desc                                          |
|-------:|------:|----------|:----------------------------------------------|
|     0h |     4 | cmdhdr   | The command header                            |
|     4h |     2 | turntime | The time per turn, in ms¹                     |
|     6h |     2 | rsvd0    | Reserved                                      |
|     8h |     4 | clientid | The client ID for this client                 |
|     Ch |     4 | maxping  | Maximum ICMP ping time from server to client² |
|        |       |          |                                               |

¹ As received from the server  
² This field isn't required (you can put '0' in it), but might help
the server.

Talking about ping, the following commands will determine the 'in game'
ping (the value you see in the 'Ping' menu)

### PING

**Type:** Client->Server  
**Type ID:** 0x3  
Indicates a ping message that the client sends to the server to
determine its ping

| offset | bytes | name          | desc                                        |
|-------:|------:|---------------|:--------------------------------------------|
|     0h |     4 | cmdhdr        | Command header                              |
|     4h |     4 | clientid      | Client id of the sender                     |
|     8h |     8 | sendtimestamp | UNIX timestamp of when you sent the message |

Using the `sendtimestamp` value, the server can determine the ping
value (just to `time() - sendtimestamp`)

### SEND_OBJECT
**Type:** Server->Client  
**Type ID:** 0x4  
Sends an object, from server to client.

| offset | bytes | name       | desc                                     |
|-------:|------:|------------|:-----------------------------------------|
|     0h |     4 | cmdhdr     | Command header                           |
|     4h |     4 | otypeid    | Type ID of the new object                |
|     8h |     4 | oobjid     | Object ID of the new object              |
|     Ch |     4 | onamestart | First 4 bytes of the object string name¹ |
|    10h |     4 | xPos       | A `float32` of the object X position     |
|    14h |     4 | yPos       | A `float32` of the object Y position     |
|    18h |     4 | zPos       | A `float32` of the object Z position     |

¹ Useful for conference (TODO: Send the _whole_ name?)

> TODO: Send all parameters.

### START_SYNC
**Type:** Client<->Server  
**Type ID:** 0x5  
The client send when it finished processing the initial objects from
the server. After the server receives this message from the last
client, it sends a `START_SYNC` message with the `clientid` zeroed,
and the game starts running.

| offset | bytes | name     | desc                    |
|-------:|------:|----------|:------------------------|
|     0h |     4 | cmdhdr   | Command header          |
|     4h |     4 | clientid | Client id of the sender |
|        |       |          |                         |


### PING_REQUEST ###

**Type:** Client->Server    
**Type ID:** 0x83    
Requires the last ping values.

| offset | bytes | name          | desc                                        |
|-------:|------:|---------------|:--------------------------------------------|
|     0h |     4 | cmdhdr        | Command header                              |
|     4h |     4 | clientid      | Client id of the sender                     |

### PING_RESPONSE ###

**Type:** Server->Client  
**Type ID:** 0x84  
Receives the last ping values

| offset | bytes | name          | desc                              |
|-------:|------:|---------------|:----------------------------------|
|     0h |     4 | cmdhdr        | Command header                    |
|     4h |     4 | clientid      | Client id of the receiver         |
|     8h |     4 | pingcount     | Count of the clients              |
|     Ch |     4 | pingclient[0] | Client ID of the first ping value |
|    10h |     4 | pingvalue[0]  | The ping of the first client      |
|    14h |     4 | pingclient[1] | Client ID of the first ping value |
|    18h |     4 | pingvalue[1]  | The ping of the first client      |
|    ... |    .. | ...           | ...                               |
|    -8h |     4 | pingclient[n] | Client ID of the 'n' ping value   |
|    -4h |     4 | pingvalue[n]  | Ping of the first client          |






