// server.cpp
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

std::vector<int> clients;  // List of client sockets

void handle_client(int client_socket) {
    char buffer[1024];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';

        // Broadcast the message to all other clients
        for (int client : clients) {
            if (client != client_socket) {
                send(client, buffer, bytes_received, 0);
            }
        }
    }

    // Remove client from list
    clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        // Accept incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Add new client to the list
        clients.push_back(new_socket);
        std::cout << "New client connected: " << new_socket << std::endl;

        // Create a thread to handle the new client
        std::thread(handle_client, new_socket).detach();
    }

    return 0;
}
