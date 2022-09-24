/**

    @file      Client.cpp
    @brief     Base model for the client-server communication
    @details   Client
    @author    eepsite
    @date      23.09.2022
    @copyright © eepsite, 2022. All right reserved.

**/

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <string>
#include <future>
#include "VersionInfo.hpp"
#include "NetworkInfo.hpp"

class Log
{
public:
    Log() {}
    ~Log() {}
public:
    void Message(char* user, char* resultBuffer, int recvBytes)
    {
        std::cout << "[" << user << "]> " << std::string(resultBuffer, NULL, recvBytes) << std::endl;
    }
};

//!< Required version for WinSock2 as per developer's preference
VersionInfo versionInfo;
const int nReqVersion = versionInfo.GetVersion();

//!< Network information for client-server communication
NetworkInfo networkInfo;
const char* nAddress = networkInfo.GetAddress();
const int nCommunicationsPort = networkInfo.GetCommunicationsPort();
const int nRegistrationPort = networkInfo.GetRegistrationPort();

/**
    @brief Function prototypes as per standard
**/
void DisplayNameInitialization(std::promise<std::string> displayNamePromise);
void StartRegistration(std::promise<std::string> registrationPromise);
void StartCommunication(char* cs_displayName);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    int wsOk = WSAStartup(MAKEWORD(nReqVersion, 0), &wsaData);
    if (wsOk == NULL)
    {
        if (LOBYTE(wsaData.wVersion >= nReqVersion))
        {
            std::promise<std::string> registrationPromise;
            auto registrationFuture = registrationPromise.get_future();
            std::thread registrationThread(StartRegistration, std::move(registrationPromise));
            std::string displayName = registrationFuture.get();
            char* cs_displayName = const_cast<char*>(displayName.c_str());

            registrationThread.join();

            StartCommunication(cs_displayName);
        }
        else
        {
            std::cout << "Requested version is not supported." << std::endl;
        }
    }
    WSACleanup();
    ExitProcess(EXIT_SUCCESS);
}

void DisplayNameInitialization(std::promise<std::string> displayNamePromise)
{
    std::string displayName;
    std::cout << "[Enter a Display Name]> ";
    std::getline(std::cin, displayName);
    displayNamePromise.set_value(displayName);
}

void StartRegistration(std::promise<std::string> registrationPromise)
{
    std::promise<std::string> displayNamePromise;
    auto displayNameFuture = displayNamePromise.get_future();
    std::thread displayNameFetch(DisplayNameInitialization, std::move(displayNamePromise));
    std::string displayName = displayNameFuture.get();
    const char* cs_displayName = displayName.c_str();
    registrationPromise.set_value(cs_displayName);
    int sz_displayName = sizeof(cs_displayName);
    displayNameFetch.join();

    std::cout << "Authenticating..." << std::endl;

    SOCKET fdRegistration = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    switch (fdRegistration)
    {
    case INVALID_SOCKET:
        std::cout << "Failed to initalize local server socket." << std::endl;
        std::cout << WSAGetLastError() << std::endl;
        WSACleanup();
        break;
    default:
        sockaddr_in saClient;
        saClient.sin_family = AF_INET;
        saClient.sin_port = htons(nRegistrationPort);
        saClient.sin_addr.s_addr = inet_addr(nAddress);

        int szClient = sizeof(saClient);

        int connResult = connect(fdRegistration, reinterpret_cast<sockaddr*>(&saClient), szClient);
        switch (connResult)
        {
        case SOCKET_ERROR:
            std::cout << "Could not connect to local server on specified port." << std::endl;
            std::cout << WSAGetLastError() << std::endl;
            WSACleanup();
            break;
        default:
            break;
        }

        int registrationResponse = send(fdRegistration, cs_displayName, sz_displayName + 1, NULL);
        if (registrationResponse != SOCKET_ERROR)
        {
            std::cout << "Authenticated successfully!" << std::endl;
        }
        else
        {
            std::cout << "Failed to authenticate with local server." << std::endl;
            WSACleanup();
            ExitProcess(EXIT_FAILURE);
        }
    }
}
void StartCommunication(char* cs_displayName)
{
    SOCKET fdCommunication = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    switch (fdCommunication)
    {
    case INVALID_SOCKET:
        std::cout << "Failed to initialize local server socket." << std::endl;
        std::cout << WSAGetLastError() << std::endl;
        WSACleanup();
        break;
    default:
        sockaddr_in saClient;
        saClient.sin_family = AF_INET;
        saClient.sin_port = htons(nRegistrationPort);
        saClient.sin_addr.s_addr = inet_addr(nAddress);

        int szClient = sizeof(saClient);

        int connResult = connect(fdCommunication, reinterpret_cast<sockaddr*>(&saClient), szClient);
        switch (connResult)
        {
        case SOCKET_ERROR:
            std::cout << "Could not connect to local server on specified port." << std::endl;
            std::cout << WSAGetLastError() << std::endl;
            WSACleanup();
            break;
        default:
            break;
        }

        char responseBuffer[4096];
        std::string userInput;
        Log ClLog;

        do
        {
            std::cout << "> ";
            std::getline(std::cin, userInput);
            if (userInput.size() > 0)
            {
                int sendResult = send(fdCommunication, userInput.c_str(), userInput.size() + 1, NULL);
                if (sendResult != SOCKET_ERROR)
                {
                    memset(&responseBuffer, 0, sizeof(responseBuffer));
                    int recvBytes = recv(fdCommunication, responseBuffer, 4096, NULL);
                    if (recvBytes > 0)
                    {
                        ClLog.Message(cs_displayName, responseBuffer, recvBytes);
                    }
                }
            }
        } while (userInput.size() > 0);

        closesocket(fdCommunication);
    }
}