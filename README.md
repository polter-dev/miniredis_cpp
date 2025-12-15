# miniredis-cpp

A Redis-like in-memory key-value store written in C++ for learning systems programming.

> Goal: 
> by building a tiny version from scratch.

## Why this project?

My goal for this project is to expose myself to C++, being proficient in C, I want to utilize this towards a more object-oriented language. 
This was also created to gain a better understanding of how a real-world database server works under the hood

This project covers:
- **Network programming** – sockets, event loops, handling multiple clients.
- **Concurrency** - multi-threaded request handling with 'std::thread'
- **Systems-level C++** – working directly with POSIX APIs

This is a **learning project**, not production software.

## Features (planned to update as implemented)
- TCP server that accepts multiple concurrent clients
- Persistent connections (multiple commands per session)
- Supported Commands (atm)
  - `PING` - returns `PONG`
  - `ECHO <message>` - returns `<message>`
  - `QUIT` - closes the connection

## Tech stack

- C++
- POSIX sockets 
- `STD::thread` for concurrency
- macOS / Linux


## Building

```bash
g++ -o server server.cpp -pthread
g++ -o client client.cpp
```

# Usage
Start the server:
```bash
./server
```
In a new terminal window
```bash
./client
```
Then enter any command into the client terminal to get the result

`EXAMPLE OUTPUTS FOR INPUTS`
```
PING
PONG
ECHO hello world
hello world
QUIT
```

## Planned Features 
- `SET` and `GET` commands with hash tables
- key expiration
- nonblocking i/o with an event loop
