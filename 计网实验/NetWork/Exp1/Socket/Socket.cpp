// Server端
#pragma comment(lib,"WS2_32.lib")
#include <stdio.h>
#include <winsock2.h>
#include <list>
#include <algorithm>
#include <string.h>
#include <iostream>
#include<thread>
#include "Socket.h"
using namespace std;
//-----常量声明
constexpr auto BUFFLEN = 2000;;
constexpr auto MAX_CONNS = 5;;
#define SERVER_PORT 6666


//------------------全局变量声明
sockaddr_in srvAddr, clientAddr;

//-------多线程
void thread_task(SOCKET NewConnection)
{
	char recvBuff[2048];
	ZeroMemory(recvBuff, 2048);
	if (recv(NewConnection, recvBuff, sizeof(recvBuff), 0) == SOCKET_ERROR)
	{
		cout << "Recieve failed!\n";
		closesocket(NewConnection);
	}
}

//------------主函数：
int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ListeningSock;
	SOCKET WorkingSock;
	int ClientAddrLen;
	memset(&srvAddr, 0, sizeof(srvAddr));//清零srvAddr
	memset(&recvBuf, 0, sizeof(recvBuf));//清零数据接收区
	int number=0;

	//初始化 winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Winsock initial fialed！\n";
		WSACleanup();
		return -1;
	}
	cout << "Server's winsock initialized !\n";

	//创建 TCP socket
	ListeningSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ListeningSock == INVALID_SOCKET)
	{
		cout << "Socket create failed!\n";
		WSACleanup();
		return -1;
	}
	cout << "Server TCP socket create OK!\n";
	//初始化socket的地址
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	//绑定监听端口

	if (bind(ListeningSock, (SOCKADDR*)& srvAddr, sizeof(srvAddr)) == SOCKET_ERROR)
	{
		cout << "Server socket bind error!\n";
		closesocket(ListeningSock);
		WSACleanup();
		return -1;
	}
	cout << "Server socket bind OK!\n";

	//开始监听过程，等待客户的连接
	if (listen(ListeningSock, MAX_CONNS) == SOCKET_ERROR)
	{
		cout << "Server socket listen error!\n";
		closesocket(ListeningSock);
		WSACleanup();
		return -1;
	}
	cout << "Listening...";

	while (1)
	{
		int client_addrLen = sizeof(sockaddr_in);
		//创建真正用于连接的socket
		SOCKET WorkingSocket = accept(ListeningSock, (sockaddr*)& clientAddr, &client_addrLen);
		if (WorkingSocket == SOCKET_ERROR)
		{
			cout << "Server accept error!\n";
			closesocket(WorkingSock);
			break;
		}
		else
		{
			cout << "NO."
				<< number
				<< endl;
			
		}
	}
	// 关闭监听Socket，然后退出应用程序  
	closesocket(ListeningSock);
	// 释放Windows Socket 的相关资源
	WSACleanup();
	return 0;
}
