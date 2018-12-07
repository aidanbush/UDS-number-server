Number server
=============

# Note:

"I’m a super-keener and implemented more than I was supposed to"
And as such actually implemented error handling for empty or full servers.
\# StealingQuotesFromCam
\# TwoHashtags

## Protocol

The protocol uses UDS stream sockets.

The protocol allows for 2 types of operations, storing and retrieving of numbers.
All numbers are 64 bit signed numbers, with the endianness determined by the client whom sent the number.

### Packets

#### Store packet: "STOR[num]"

Used to send a number to the server to store.
Responded to with an OK, or an ERR packet if there is no space available.

#### Retrieve packet: "RTRV"

A request to retrieve a number from the server

#### Ok packet: "OK"

Sent in response to a store request when the number is properly stored.

#### Number response packet: "[num]"

Sent in response to a retrieve request.
Contains a single 64 bit singed integer.

#### Error packet: "ERR"

Error packet sent if the buffer is empty and a RTRV request is sent or the buffer is full and a STOR request is made.

### Empty or Full Servers

To account for empty or full servers, and error packet is sent, this lets the client know that the request it made was not completed.

## Building and Testing

Requires GNU make, or a compatible equivalent.
* To build run `make all`
* To test run `make test`

num_server
==========

num_server is a simple UDS server that stores numbers in a ring buffer.

Usage: `num_server`

The buffer is defined by the `BUF_LEN` macro in the `buf.h` file, it has a default value of 64, but can be redefined.

The server uses the socket, "socket", which is created in the directory it is ran in.

### Error handling

The server will print error messages to STDERR when encountered.
Most errors are caused by bad packets, or the failure to send packets.

num_client
==========

num_client is a simple UDS client that can send store and receive requests to a server.

Usage: `num_client [operation] [options] socket`
The client must be given an operation to preform, these are either a store or retrieve.
In a store request a `-s` flag is used followed by the 64 bit number to be sent.
In a retrieve request a `-r` flag is used.
The final option is verbose, and is enabled by the `-v` flag, this enables printing of the number stored or retrieved.

Running `num_client -h` will produce a usage message.

### Error handling

The client will print error messages to STDERR when encountered.
Most errors are caused by bad packets, or the failure to send packets.
