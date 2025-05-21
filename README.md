# WinSock Implementations
## TCP
- Echos the message back to clients, and has a basic command to close the server.
  - OneServerOneClient serves clients one at a time and places the rest in a queue.
  - MultipleClients serves clients in a main loop consecutively.
## UDP
- Sends the mouse coordinates from the client to the server
  - The client serializes the POINT struct into the buffer
  - The server deserializes the POINT struct and prints it to the console
## Resources 
- General Knowledge for Sockets - [Beej's Guide to Network Programming](https://www.beej.us/guide/bgnet/html/split/intro.html)
- Hands on approach to a TCP Server setup with great explainations of individual components - [Programming Challenges - 29.1 - Winsock Server (C) - Michael Grieco](https://www.youtube.com/watch?v=QedzfsexcdU)
- Hands on UDP Server setup (Requires Port to C) - [Starter UDP Server And Client in C++](https://www.youtube.com/watch?v=uIanSvWou1M)
## Additional Reading
[Stack Overflow - Blocking vs Non-Blocking sockets](https://stackoverflow.com/questions/10654286/why-should-i-use-non-blocking-or-blocking-sockets)
## TCP Client-Server Interaction
![image](https://github.com/user-attachments/assets/bbf305e5-da67-41c2-8f85-b88c601c0a12)
![image](https://github.com/user-attachments/assets/d70ad21a-a826-4d9d-ae2b-2c8c4870a453)
## UDP Client-Server Interaction
![image](https://github.com/user-attachments/assets/6751890c-b609-4b7f-9512-8e6abdc08ab8)


