# Tribalia gameplay network protocol

The Tribalia protocol is used by connecting clients to the game server, allowing for 
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
 
 
 TODO: Add the other things
 
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
all clients. Clients, then, should connect in the UDP port.
 

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
 
 
 
 
