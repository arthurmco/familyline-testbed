# Tribalia server admin protocol

This file documents the Tribalia server administration protocol. It is mainly used to communication between the main server application and its interface.

The communication happens in port TCP 12100 and is entirely textual (no binaries involved).

It will be encrypted when I discover how to do it nicely.

## Message types

 - Requisitions: Something that the interface asks the server.
 - Responses: The server answer to the interface request.
 
 They have the same rules as [the network protocol](network_protocol_contrib.md) ones: 
 starts with [TRIBALIA and ends with ] and a newline

## Connection

When the interface connects to the server, the server should send the following
message to it

`[TRIBALIA INTERFACE <<id>> <<flags>> ]`

`id` is the new ID the interface received from the server.

`flags` is the connection flags. The interface needs to support the sent flags, or else
it might not be able to understand the following messages.

The flag format has the following syntax:

`(AE)`

Each letter is a flag. Only the supported flags need to be sent, i.e, if the server supports
only the A flag, it needs to send only `(A)`

The supported flags are:
 - `E`: supports encryption
 - `A`: you need to authenticate first. To support authentication you need to support encryption,
   for security reasons
   
Currently the specification doesn't support or tell how you'll do encryption nor authentication.
> TODO: Fix that. Specify.

After finishing with the connection configuration (encryption, authentication...), the interface
needs to send

`[TRIBALIA INTERFACE OK ]`

And you are connected :]

## Requisitions

### Players

The **players** requisition gets the player connection status and basic information.

 - INTERFACE: `[TRIBALIA REQUEST PLAYERS ]`
 - SERVER: `[TRIBALIA RESPONSE PLAYERS <<player_count>> 
 <<player1_id>>  <<player1_name>>  <<player1_xp>> <<player1-status>>
 <<player1_ip>> <<player2_id>> ... <<playern_id>>  <<playern_name>> 
 <<playern_xp>> <<playern_status>> <<playern_ip>> ]`
 
 Player status can be "connecting", if the player didn't receive the PLAYERS 
 request from the server, or "connected" if it did receive.
 
### Chat

 - I: `[TRIBALIA REQUEST CHAT <<last-message-date>>]`
 
  * last-message-date is the starting unix timestamp of the message who 
  you want to see, the most older message you want to get.
 
 - S: `[TRIBALIA CHAT <<message-timestamp>> <<sender_id>>
  <<sender_name>>  all|team|player:<<player_id>>
  <<message-string-in-bytes>>  <<message-string>> ]`
  
  * message-timestamp: The UNIX 64-bit timestamp of the message
    receival in the server
  
  * sender_id: The client ID of the sender.
  
  * sender_name: The name of the player who sent the message
  
  * all, team, player: Only one can exist. If...
   - all exists, it means that the message is for everyone in the game
   - team exists, it means that the message is for everyone in the sender's team
   - player:`<<player_id>>` exists, it means that is for the player with ID equals
     to `<<player_id>>`
	 
  * message-string-in-bytes: The message size **in bytes**, not in chars!
  
  * message-strings: An UTF-8 formatted string message.

### Sending chat

To send chat, the interface need to send the following message:

 - I: `[TRIBALIA SEND-CHAT all|team|player:<<id>> <<message-size-in-bytes>> 
 <<message-string>> ]`
