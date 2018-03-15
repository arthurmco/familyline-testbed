#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Tests the Tribalia server application
# Copyright (C) 2017 Arthur M

# Parameters: server-base-test SERVER_IP

def sock_str_send(sock, msg):
    return sock.sendall(bytes(msg, 'utf-8'))

def client_show_playerinfo(sock, name, xp):
    """ Send player info message. Return client ID, or -1 on error """
    pinfomsg = "[TRIBALIA PLAYERINFO "+name+" "+str(xp)+"]\n"
    sock_str_send(sock, pinfomsg)

    pexpect = "[TRIBALIA PLAYERINFO"

    try:
        serverresp = sock.recv(128).decode()
        if not serverresp.startswith(pexpect):
            return -1
    except socket.timeout:
        print('error: client_show_playerinfo: timeout')
        return -1
            

    lastbrkt = serverresp.index(']')
    val = 0
    try:
        val = int(serverresp[(len(pexpect)+1):(lastbrkt)])
    except ValueError:
        val = -1
    finally:
        print('id: %d' % val)
        return val

port=12000

import socket
import sys
import os
import time # time.sleep

if len(sys.argv) < 2:
    print('%s: tests Tribalia server app\n' % sys.argv[0])
    print('\n\tUsage: %s SERVER_IP\n' % sys.argv[0])
    sys.exit(1)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
addr = (sys.argv[1], port)

testname = "Test"
testxp = 2000
if len(sys.argv) > 2:
    testname = sys.argv[2]

if len(sys.argv) > 3:
    testxp = sys.argv[3]

print('Connecting...')
sock.connect(addr)

## Connect init
sock_str_send(sock, '[TRIBALIA CONNECT]\n')
time.sleep(1)
serverresp = sock.recv(64).decode()
if not serverresp.startswith('[TRIBALIA CONNECT'):
    print('\nUnexpected answer (1) %s', serverresp)
    sock.close()
    sys.exit(1)

# Get IP
lastbrkt = serverresp.index(']')
serverip = serverresp[17:lastbrkt].strip()
print('Server IP: %s' % serverip)
sock_str_send(sock, '[TRIBALIA CONNECT OK]\n')
time.sleep(1)
## Version
serverresp = sock.recv(64).decode()
if not serverresp == "[TRIBALIA VERSION?]\n":
    print('\nUnexpected answer (2)')
    sock.close()
    sys.exit(1)

sock_str_send(sock, '[TRIBALIA VERSION 0.1]\n')

## Capabilities
sock_str_send(sock, '[TRIBALIA CAPS?]\n')

serverresp = sock.recv(64).decode()
if not serverresp.startswith("[TRIBALIA CAPS"):
    print('\nUnexpected answer (3)')
    sock.close()
    sys.exit(1)

if len(serverresp.strip()) > 16:
    lastbrkt = serverresp.index(']')
    unkcap = serverresp[16:lastbrkt-1]
    print('warning: server unknown capabilities: %s" % unkcap')

sock_str_send(sock, '[TRIBALIA CAPS ]\n')

print('Connected to server!')

sock.settimeout(1)
quit = False
clientid = -1
player_sent = False

while not quit:
    try:
        if clientid < 0:
            servermsg = sock.recv(1024).decode()
    
            if servermsg == "[TRIBALIA PLAYERINFO?]\n":
                if clientid < 0:
                    clientid = client_show_playerinfo(sock, testname, testxp)
                    if clientid < 0:
                        print('error: failed to get player info')
                    else:
                        print('Client ID is %d' % clientid)
                        sock_str_send(sock, '[TRIBALIA CHAT %d all 5 Hello]\n' % clientid)
                               
    except socket.timeout as e:
        print("Socket timed out: %s" % e)
    except KeyboardInterrupt:
        quit = True

    if clientid > 0:
        if not player_sent:
            print('Connected. Requesting player info...')
            sock_str_send(sock, '[TRIBALIA PLAYERS?]\n')

            playerstr = sock.recv(1024).decode()
            playersarr = playerstr.split('|')
            print("Found %d players" % int(playersarr[0].split(' ')[2]))

            for player in playersarr[1:]:
                playerinfo = player.strip().split(' ')
                plid = int(playerinfo[0])

                if playerinfo[-1].strip() == ']':
                    plexp = int(playerinfo[-2])
                    plname = ' '.join(playerinfo[1:-2])
                else:
                    plexp = int(playerinfo[-1])
                    plname = ' '.join(playerinfo[1:-1])

                print(" -> Discovered player id %d, name %s, xp %d" % (plid, plname, plexp))

            player_sent = True

        chatmsg = ""
        try:
            chatmsg = input("chat> ")
        except KeyboardInterrupt:
            quit = True
        except EOFError:
            quit = True

        chatlen = len(chatmsg)

        if chatmsg.strip() == "/ready":
            sock_str_send(sock, "[TRIBALIA GAME READY]\n")
        elif chatlen > 2:
            sock_str_send(sock, "[TRIBALIA CHAT %d all %u %s]\n" % (clientid, chatlen, chatmsg))

print("Closing...")
sock.close()
exit(0)

