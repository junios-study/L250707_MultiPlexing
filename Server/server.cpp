#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define FD_SETSIZE      100

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

	fd_set ReadSockets;
	FD_ZERO(&ReadSockets);
	FD_SET(ListenSocket, &ReadSockets);

	while (true)
	{
		//[][][][][]
		fd_set ReadSocketCopys;
		//원본은 가지고 있고 복제본을 줌
		ReadSocketCopys = ReadSockets;

		struct timeval Timeout;
		Timeout.tv_sec = 0;
		Timeout.tv_usec = 1000;

		//polling
		int ChangeSocketCount = select(0, &ReadSocketCopys, NULL, NULL, &Timeout);
		if (ChangeSocketCount <= 0)
		{
			//다른 서버 작업
			//std::cout << "서버 작업해야지" << std::endl;
			continue;
		}
		//네트워크 작업
		else
		{
			for (int i = 0; i < (int)ReadSockets.fd_count; ++i)
			{
				if (FD_ISSET(ReadSockets.fd_array[i], &ReadSocketCopys))
				{
					if (ReadSockets.fd_array[i] == ListenSocket)
					{
						SOCKADDR_IN ClientSocketAddr;
						int ClientSocketAddrSize = sizeof(ClientSocketAddr);
						SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSocketAddr, &ClientSocketAddrSize);
						FD_SET(ClientSocket, &ReadSockets);
						std::cout << "Connect" << std::endl;
					}
					else
					{
						char RecvBuffer[1024] = {};
						int RecvBytes = recv(ReadSockets.fd_array[i], RecvBuffer, 1024, 0);
						if (RecvBytes <= 0)
						{
							closesocket(ReadSockets.fd_array[i]);

							FD_CLR(ReadSockets.fd_array[i], &ReadSockets);

							std::cout << "Disconnect" << std::endl;
						}
						else
						{
							for (int j = 0; j < (int)ReadSockets.fd_count; ++j)
							{
								if (ReadSockets.fd_array[j] != ListenSocket)
								{
									send(ReadSockets.fd_array[j], RecvBuffer, 1024, 0);
								}
							}
						}
					}
				}
			}
		}
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}