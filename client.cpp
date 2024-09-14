// client.cpp
#include <iostream>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

void receive_messages(int client_socket) {
    char buffer[1024];
    int bytes_received;
    
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }
}

int main() {
    int client_socket;
    struct sockaddr_in address;

    // Creating socket file descriptor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    // Start a thread to receive messages from the server
    std::thread(receive_messages, client_socket).detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        send(client_socket, message.c_str(), message.size(), 0);
    }

    close(client_socket);
    return 0;
}
