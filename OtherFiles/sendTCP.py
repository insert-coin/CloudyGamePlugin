# Simple code to send command over TCP to CloudyPanelPlugin
# Format is 4 digit command, 4 digit controller id.
# Currently these commands are supported:
# 0000 - Join game
# 0001 - Quit game
# Example 00000001 - join game with controller id 1

import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 55556
BUFFER_SIZE = 1024
MESSAGE = input("Enter msg to be sent: ")
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

# convert string to bytes to send
s.send(MESSAGE.encode())

data = s.recv(BUFFER_SIZE)
s.close()

# convert bytes to string before printing
data = data.decode("utf-8")
print("Received data: " + data)
