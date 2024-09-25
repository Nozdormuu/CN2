
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


void HandleClient(SOCKET ClientSocket) {
   char recvbuf[DEFAULT_BUFLEN];
   int recvbuflen = DEFAULT_BUFLEN;
   int iResult;

   do {
      iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
      if (iResult > 0) {
         printf("Bytes received: %d\n", iResult);

        
         iResult = send(ClientSocket, recvbuf, iResult, 0);
         if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            return;
         }
         printf("Bytes sent: %d\n", iResult);
      }
      else if (iResult == 0)
         printf("Connection closing...\n");
      else {
         int lasterror = WSAGetLastError();
         if (WSAEWOULDBLOCK == lasterror) {
            printf("No data\n");
         }

  
         return;
      }
   } while (iResult > 0);
   closesocket(ClientSocket);
}

int main() {
   WSADATA wsaData;
   int iResult;

   SOCKET ListenSocket = INVALID_SOCKET;
   SOCKET ClientSocket = INVALID_SOCKET;

   struct addrinfo* result = NULL;
   struct addrinfo hints;

   int iSendResult;

   // Initializae Winsock
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != 0) {
      printf("WSAStartup failed with error: %d\n", iResult);
      return 1;
   }

   ZeroMemory(&hints, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_flags = AI_PASSIVE;

   // Resolve the server address and port
   iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
   if (iResult != 0) {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      return 1;
   }

   // Create a SOCKET for the server to listen for client connections.
   ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
   if (ListenSocket == INVALID_SOCKET) {
      printf("socket failed with error: %ld\n", WSAGetLastError());
      freeaddrinfo(result);
      WSACleanup();
      return 1;
   }

   iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
   if (iResult == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      freeaddrinfo(result);
      closesocket(ListenSocket);
      WSACleanup();
      return 1;
   }

   freeaddrinfo(result);

   iResult = listen(ListenSocket, SOMAXCONN);
   if (iResult == SOCKET_ERROR) {
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      WSACleanup();
      return 1;
   }

   while (true) {
      Sleep(5000); // временная задержка для демонстрации
      ClientSocket = accept(ListenSocket, NULL, NULL);
      if (ClientSocket == INVALID_SOCKET) {
         printf("accept failed with error: %d\n", WSAGetLastError());
         closesocket(ListenSocket);
         WSACleanup();
         return 1;
      }

      std::thread clientThread(HandleClient, ClientSocket); 
      clientThread.detach(); 
      

   }

   closesocket(ListenSocket);
   WSACleanup();

   return 0;
}

