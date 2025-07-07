#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ListenSocketAddr;
	memset(&ListenSocketAddr, 0, sizeof(ListenSocketAddr));
	ListenSocketAddr.sin_family = PF_INET;
	ListenSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ListenSocketAddr.sin_port = htons(32000);
	bind(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));
	
	listen(ListenSocket, 5);

	while (true)
	{
		//[][][][][]
		fd_set ReadSockets;
		FD_ZERO(&ReadSockets);
		FD_SET(ListenSocket, &ReadSockets);
		struct timeval Timeout;
		Timeout.tv_sec = 0;
		Timeout.tv_usec = 1000;
		//polling
		int ChangeSocketCount = select(0, &ReadSockets, 0, 0, &Timeout);
		if (ChangeSocketCount <= 0)
		{
			//다른 서버 작업
			std::cout << "서버 작업해야지" << std::endl;
			continue;
		}
		//네트워크 작업
		else
		{
			SOCKADDR_IN ClientSocketAddr;
			int ClientSocketAddrSize = sizeof(ClientSocketAddr);
			SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSocketAddr, &ClientSocketAddrSize);

			while (true)
			{
				char RecvBuffer[1024] = {};
				int RecvBytes = recv(ClientSocket, RecvBuffer, 1024, 0);
				if (RecvBytes <= 0)
				{
					std::cout << "Disconnect" << std::endl;
					break;
				}
				send(ClientSocket, RecvBuffer, 1024, 0);
			}

			closesocket(ClientSocket);
		}
	}
	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}