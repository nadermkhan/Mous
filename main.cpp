#define INITGUID
#include <winsock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Bthprops.lib")

void moveMouse(float dx, float dy) {
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    int newX = cursorPos.x + static_cast<int>(dx);
    int newY = cursorPos.y + static_cast<int>(dy);

    SetCursorPos(newX, newY);
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return 1;
    }

    // Create Bluetooth socket
    SOCKET serverSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return 1;
    }

    // Setup Bluetooth address
    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH;
    sa.btAddr = 0;
    sa.serviceClassId = GUID_NULL;
    sa.port = BT_PORT_ANY;

    if (bind(serverSocket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    int sa_len = sizeof(sa);
    getsockname(serverSocket, (SOCKADDR*)&sa, &sa_len);

    std::cout << "Listening on Bluetooth port: " << sa.port << std::endl;

    // Accept a client
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Accepted connection!" << std::endl;

    char buffer[1024] = {0};
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        std::string input(buffer);

        std::istringstream iss(input);
        std::string cmd;
        float dx, dy;

        while (iss >> cmd >> dx >> dy) {
            if (cmd == "MOVE") {
                moveMouse(dx, dy);
            }
        }
    }

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
