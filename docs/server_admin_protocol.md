# Tribalia server admin protocol

This file documents the Tribalia server administration protocol. It is mainly used to communication between the main server application and its interface.

The communication happens in port TCP 12100 and is entirely textual (no binaries involved).

It will be encrypted when I discover how to do it nicely.

## Message types

 - Requisitions: Something that the interface asks the server.
 - Responses: The server answer to the interface request.
 
 They have the same rules as [the network protocol](network_protocol_contrib.md) ones: starts with [TRIBALIA and ends with ] and a newline
 
## Requisitions

### Players

The **players** requisition gets the player connection status and basic information.

 - INTERFACE: `[TRIBALIA REQUEST PLAYERS]`
 - SERVER: `[TRIBALIA RESPONSE PLAYERS <<player_count>> 
 <<player1_id>>  <<player1_name>>  <<player1_xp>> 
 <<player1_ip>> <<player2_id>> ... <<playern_id>> 
 <<playern_name>> <<playern_xp>> <<playern_ip>>`
 
### Chat

 - I: `[TRIBALIA REQUEST CHAT <<last-message-date>>]`
 
  * last-message-date is the starting unix timestamp of the message who 
  you want to see, the most older message you want to get.
 
 - S: `[TRIBALIA CHAT <<sender_id>> <<sender_name>> 
 all|team|player:<<player_id>> <<message-string-in-bytes>> 
 <<message-string>> ]`
  
  * sender_id: The client ID of the sender.
  
  * sender_name: The name of the player who sent the message
  
  * all, team, player: Only one can exist. If...
   - all exists, it means that the message is for everyone in the game
   - team exists, it means that the message is for everyone in the sender's team
   - player:`<<player_id>>` exists, it means that is for the player with ID equals
     to `<<player_id>>`
	 
  * message-string-in-bytes: The message size **in bytes**, not in chars!
  
  * message-strings: An UTF-8 formatted string message.
 

