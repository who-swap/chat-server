# Multi-Client Chat Server

> **A chat server written in C because apparently suffering is part of learning networking.**

I wanted to understand how chat servers actually work instead of throwing some library at it and pretending I knew networking.

So I built one.

From scratch.

In C.

Yeah... I know. 

## 

A **multi-client TCP chat server** that allows multiple clients to connect to the same server and send messages to each other.

Each client gets a name, and messages are broadcast like:

```text
[Client 1] yo what are you doing
[Client 2] building a chat server without libraries
[Client 3] GOAT is here
```

Meanwhile the server is sitting there like:

```text
Client Connected
Client Connected
Client Connected
```

## Learned a lot about sockets

This project was basically my networking boss fight.

I got to work with:

* TCP sockets
* `socket()`
* `bind()`
* `listen()`
* `accept()`
* `connect()`
* `send()` / `recv()`
* File descriptors
* `fd_set`
* `select()`
* Multiple clients on a single process
* TCP message framing
* Handling partial `recv()` calls
* Newline-based message boundaries
* Broadcasting messages
* Client disconnects
* `SO_REUSEADDR`
* `errno` and debugging socket errors

And most importantly:

> **TCP is a stream, not a message delivery service.**

That one sentence caused approximately 47% of my suffering.

## Sockets and their loved file descriptors

The server keeps track of all connected clients using file descriptors.

`select()` watches them and tells the server:

> "yo, this socket has something to read"

The server then calls `recv()`, processes the message, and broadcasts it to the other connected clients.

Very roughly:

```text
              ┌─────────────┐
              │    SERVER   │
              │             │
              │   select()  │
              └──────┬──────┘
                     │
          ┌──────────┼──────────┐
          │          │          │
          ▼          ▼          ▼
       Client 1   Client 2   Client 3
       [Swapnil]   [Yadav]    [???]
```

One server.

Multiple clients.

One `select()`.

Infinite opportunities for bugs.

## Build

Compile the server and client with your favourite C compiler:

```bash
gcc server.c -o server
gcc client.c -o client
```

Then start the server:

```bash
./server
```

Open multiple terminals and run the clients:

```bash
./client
```

Connect them to the server and start talking absolute nonsense.

##  It actually works

## Current state

This is a learning project, so don't expect enterprise-grade infrastructure here.

It works.

It handles multiple clients.

It taught me a shitload about networking.

That's the point.

### TODO

* [ ] Better error handling
* [ ] Cleaner message framing
* [ ] Proper client commands
* [ ] Private messages
* [ ] Rooms/channels
* [ ] Better disconnect handling
* [ ] Maybe move from `select()` to `epoll()`
* [ ] Probably discover 900 more things I did wrong

## Why I made this

I wanted to understand what actually happens underneath things like:

```text
"send a message"
```

instead of just calling some high-level API and moving on.

So this project is basically me going down the networking rabbit hole one segmentation fault at a time.

---

### Final status

**Did I build a production-ready Discord replacement?**

No.

**Did I get multiple clients talking through a C TCP server?**

**HELL YEAH.**

