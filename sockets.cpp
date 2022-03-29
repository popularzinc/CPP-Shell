#include <ws2tcpip.h>
#include <iostream>

//#pragma comment (lib, "Ws2_32.lib")

SOCKET Connect(std::string ip, int port, SOCKET* connection)
{
	WSADATA some_kind_of_data;
    WSAStartup(MAKEWORD(2, 2), &some_kind_of_data);

    HOSTENT *host = gethostbyname(ip.c_str());
    sockaddr_in connect_adress;
    connect_adress.sin_family = AF_INET;
    connect_adress.sin_port = htons(port);

    memcpy(&connect_adress.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(connect_adress.sin_addr.S_un.S_addr));

    SOCKET connection_socket = socket(AF_INET, SOCK_STREAM, 0);

    int r = connect(connection_socket, (sockaddr*)&connect_adress, sizeof(connect_adress));
    *connection = connection_socket;
    return r;
}

void Send(SOCKET Connection,std::string data)
{
	send(Connection, data.c_str(), data.length(), 0);
}

