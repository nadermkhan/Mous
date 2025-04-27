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
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET serverSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH;
    sa.btAddr = 0;
    sa.serviceClassId = GUID_NULL;
    sa.port = BT_PORT_ANY;

    bind(serverSocket, (SOCKADDR*)&sa, sizeof(sa));
    listen(serverSocket, 1);

    int sa_len = sizeof(sa);
    getsockname(serverSocket, (SOCKADDR*)&sa, &sa_len);

    std::cout << "Listening on Bluetooth port: " << sa.port << std::endl;

    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    std::cout << "Accepted connection!" << std::endl;

    char buffer[1024] = {0};
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
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

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
