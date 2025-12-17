# UDP Fortune Server

A lightweight, C-based UDP server that delivers random fortunes or quotes to connected clients. The server listens for incoming packets and replies with a text message selected from the included `fortunes` directory.

## Features

- **Protocol:** UDP (`SOCK_DGRAM`)
- **Port:** Default `4242`
- **Randomization:** Selects a random file from the `./fortunes` directory for every request.
- **Custom delimiters:** Supports `%` as a delimiter within fortune files.

## Prerequisites

- **GCC** (GNU Compiler Collection)
- **Netcat** (`nc`) for testing the client connection

## Installation & Compilation

1. **Clone the repository:**
   ```bash
   git clone <your-repo-url>
   cd <your-repo-name>
   ```
2. **Compile the program:**
   ```bash
   gcc -o usbserv usbserv.c
   ```
3. **Start the server**
   ```bash
   ./usbserv
   ```
4. Connect via netcat
   ```bash
   nc -u 127.0.0.1 4242
   ```
5. Press enter or type something and you recieve a fortune!

Troubleshooting

1. "open: No such file or directory": Ensure you are running the ./usbserv executable from the root of the repository so it can locate the ./fortunes folder.

2. "initialization failed": Check if port 4242 is already in use or if you have permission to bind to network ports.
