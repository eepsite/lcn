/**

    @file      Core.cpp
    @brief     Base model for the client-server communication
    @details   Server
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
#include <chrono>
#include <thread>
#include "VersionInfo.hpp"
#include "NetworkInfo.hpp"
#include "MySQL.hpp"

/**

    @class   Log
    @brief   Interface for logging functions to be implemented for separate APIs
    @details MySQL has it's specific logging functions abstracted into it's own header

**/
class Log
{
public:
    Log() {}
    ~Log() {}
public:
    virtual void Info(const char* msg) const = 0;
    virtual void Warn(const char* msg) const = 0;
    virtual void Error(const char* msg) const = 0;
};

class WS2Log : public Log
{
public:
    WS2Log() {}
    ~WS2Log() {}
public:
    void Info(const char* msg) const override
    {
        std::cout << "[*][WS2]> " << msg << std::endl;
    }

    void Warn(const char* msg) const override
    {
        std::cout << "[**][WS2]> " << msg << std::endl;
    }

    void Error(const char* msg) const override
    {
        std::cout << "[***][WS2]> " << msg << std::endl;
    }
};

class SLog : public Log
{
public:
    SLog() {}
    ~SLog() {}
public:
    void Info(const char* msg) const override
    {
        std::cout << "[*][S]> " << msg << std::endl;
    }

    void Warn(const char* msg) const override
    {
        std::cout << "[**][S]> " << msg << std::endl;
    }

    void Error(const char* msg) const override
    {
        std::cout << "[***][S]> " << msg << std::endl;
    }
};

//!< Required version for WinSock2 as per developer's preference
VersionInfo versionInfo;
const int nReqVersion = versionInfo.GetVersion();

//!< Network information for client-server communication
//! Streaming service not yet implemented
NetworkInfo networkInfo;
const char* nAddress = networkInfo.GetAddress();
const int nCommunicationsPort = networkInfo.GetCommunicationsPort();
const int nRegistrationPort = networkInfo.GetRegistrationPort();
const int nStreamPort = networkInfo.GetStreamPort();

/**
    @brief Function prototypes as per standard
**/
void StartCommunication();
void StartRegistration();

/**
    @brief  Entry point for server; Arguments not used
    @param  argc - Amount of arguments passed via command line
    @param  argv - Array of arguments passed via command line
    @retval      - 
**/
int main(int argc, char* argv[])
{
    WSADATA wsaData;
    WS2Log* ws2Log = new WS2Log();

    if (WSAStartup(MAKEWORD(nReqVersion, 0), &wsaData) == NULL)
    {
        if (LOBYTE(wsaData.wVersion >= nReqVersion))
        {
            ws2Log->Info("Server started successfully. No errors to report.");

            std::thread registrationThread(StartRegistration);
            registrationThread.join();

            StartCommunication();
        }
        else
        {
            ws2Log->Error("Requested version is not supported.");
        }
    }
    else
    {
        ws2Log->Error("WinSock2 initial startup failed. Exiting.");
    }

    delete ws2Log;
    ExitProcess(EXIT_SUCCESS);
}

/**
    @brief Transmission of data between the client and the server
**/
void StartCommunication()
{
    WS2Log* ws2Log = new WS2Log();
    SLog* sLog = new SLog();

    SOCKET fdCommunication = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    switch (fdCommunication)
    {
    case INVALID_SOCKET:
        ws2Log->Error("Failed to initialize communications socket for local server.");
        WSACleanup();
        break;
    default:
        sockaddr_in saCommunicationServer;
        saCommunicationServer.sin_family = AF_INET;
        saCommunicationServer.sin_port = htons(nCommunicationsPort);
        saCommunicationServer.sin_addr.s_addr = inet_addr(nAddress);

        int bindResult = bind(fdCommunication, reinterpret_cast<sockaddr*>(&saCommunicationServer), sizeof(saCommunicationServer));
        int listenResult = listen(fdCommunication, SOMAXCONN);

        sLog->Info("Starting communications service...");

        sockaddr_in saClient;
        int szClient;

        SOCKET hClient = accept(fdCommunication, reinterpret_cast<sockaddr*>(&saClient), &szClient);
        switch (hClient)
        {
        case SOCKET_ERROR:
            ws2Log->Error("Failed to accept remote connection.");
            WSACleanup();
            break;
        default:
            break;
        }

        char Host[NI_MAXHOST];
        char Service[NI_MAXSERV];

        memset(&Host, 0, sizeof(Host));
        memset(&Service, 0, sizeof(Service));

        // Attempt to resolve hostname
        if (getnameinfo(reinterpret_cast<sockaddr*>(&saClient), szClient, Host, NI_MAXHOST, Service, NI_MAXSERV, 0) == NULL)
        {
            std::cout << Host << " connected on port " << Service << std::endl;
        }
        else
        {
            inet_ntop(AF_INET, &saClient.sin_addr, Host, NI_MAXHOST);
            std::cout << Host << " connected on port " << ntohs(saClient.sin_port) << std::endl;
        }

        closesocket(fdCommunication);

        char messageBuffer[4096];

        while (true)
        {
            memset(&messageBuffer, 0, 4096);

            int recvBytes = recv(hClient, messageBuffer, 4096, 0);
            if (recvBytes == 0)
            {
                ws2Log->Warn("Read 0 bytes... Exiting.");
                WSACleanup();
                ExitProcess(EXIT_FAILURE);
            }

            std::cout << std::string(messageBuffer, NULL, recvBytes) << std::endl;

            if (send(hClient, messageBuffer, recvBytes, 0) == SOCKET_ERROR)
            {
                std::cout << Host << " has disconnected." << std::endl;
                WSACleanup();
                std::cin.get();
                break;
            }
        }

        closesocket(hClient);
    }

    delete ws2Log;
    delete sLog;
}

/**
    @brief MySQL database initialization via API
**/
void StartRegistration()
{
    WS2Log* ws2Log = new WS2Log();
    SLog* sLog = new SLog();
    MySQL* db = new MySQL("localhost", "root", "oracle", "db", 3306);

    SOCKET fdRegistration = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    switch (fdRegistration)
    {
    case INVALID_SOCKET:
        ws2Log->Error("Failed to initialize registration socket for local server.");
        WSACleanup();
        break;
    default:
        sockaddr_in saRegistrationServer;
        saRegistrationServer.sin_family = AF_INET;
        saRegistrationServer.sin_port = htons(nRegistrationPort);
        saRegistrationServer.sin_addr.s_addr = inet_addr(nAddress);

        int bindResult = bind(fdRegistration, reinterpret_cast<sockaddr*>(&saRegistrationServer), sizeof(saRegistrationServer));
        int listenResult = listen(fdRegistration, SOMAXCONN);

        sLog->Info("Starting registration service...");

        sockaddr_in saClient;
        int szClient = sizeof(saClient);

        SOCKET hClient = accept(fdRegistration, reinterpret_cast<sockaddr*>(&saClient), &szClient);
        switch (hClient)
        {
        case SOCKET_ERROR:
            ws2Log->Error("Failed to accept remote connection.");
            WSACleanup();
            break;
        default:
            break;
        }

        char credentialBuffer[4096];
        memset(&credentialBuffer, 0, 4096);

        int recvBytes = recv(hClient, credentialBuffer, 4096, 0);
        if (recvBytes == 0)
        {
            ws2Log->Warn("Read 0 bytes... Exiting.");
            WSACleanup();
            ExitProcess(EXIT_FAILURE);
        }

        db->CreateTable("accounts", "username");
        db->InsertKey("accounts", "username", credentialBuffer);
        db->QueryTable("accounts", 0);

        sLog->Info("Registration completed successfully. No errors to report.");

        closesocket(hClient);
        closesocket(fdRegistration);
    }

    delete ws2Log;
    delete sLog;
    delete db;
}