#include<winsock2.h>
#include<iostream>
#include<string>
#include<io.h>
#include<thread>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
//-----常量声明
constexpr auto BUFFLEN = 2000;;
constexpr auto MAX_CONNS = 50;;
constexpr auto LITEBUF = 512;;
#define SERVER_PORT 6666

//处理文件类型
void GetFileType(char* file, char* fileType)
{
	memset(fileType, 0, 20);
	if (strstr(file, ".html") || 0 == strcmp(file, "/"))
	{
		strcpy(fileType, "text/html");
	}
	else if (strstr(file, ".bmp"))
	{
		strcpy(fileType, "application/x-bmp");
	}
	else if (strstr(file, ".gif"))
	{
		strcpy(fileType, "image/gif");
	}
	else if (strstr(file, ".png"))
	{
		strcpy(fileType, "image/png");
	}
	else if (strstr(file, ".jpg"))
	{
		strcpy(fileType, "image/jpeg");
	}
	else
	{
		strcpy(fileType, "text/plain");
	}
}


//处理访问请求
void HandleRequest(SOCKET NewConnection,char* file)
{
	if (strcmp(file, "./www/"))
	{
		strcpy(file, "./www/index.html");
	}
	//检测请求文件的类型
	char fileType[20];
	GetFileType(file, fileType);
	FILE* fp = fopen(file, "rb");
	if (fp == NULL)
	{
		//找不到相应文件
		char NotFund[] = "404 NOT FOUND \n\n";
		send(NewConnection, NotFund, strlen(NotFund), 0);
	}
	else
	{
		//在文件目录中寻找对应的资源
		int file_size = 0;
		char* content;
		char* response;
		fseek(fp, 0, SEEK_END);
		file_size=ftell(fp);
		fseek(fp, 0, SEEK_SET);
		content = (char*)malloc(file_size + 1);
		int i = fread(content, file_size, 1, fp);
		content[file_size] = 0;
		response = (char*)malloc(sizeof(content) + 100);
		
		//发送请求headers到客户端
		sprintf(response, "HTTP/1.1 200 OK\r\n");
		sprintf(response, "%sContent-Type: %s\r\n", response, fileType);
		sprintf(response, "%sContent-Length: %d\r\n", response, file_size);
		sprintf(response, "%sServer: Web Server\r\n\r\n", response);
		if (send(NewConnection, response, strlen(response), 0) == SOCKET_ERROR)
		{
			printf("Send headers failed!");
			closesocket(NewConnection);
		}
		else
		{
			// 显示headers
			cout << response;
		}
		//发送请求body到客户端
		sprintf(response, "%s", content);
		//send(clientSocket, response, strlen(response), NULL);

		if (send(NewConnection, content, file_size, 0) == SOCKET_ERROR)
		{
			printf("Send body failed!");
			closesocket(NewConnection);
		}
		else
		{
			// 显示body
			printf(content);
			printf("\r\n\r\n");
		}

		free(content);
	}

}
//-------多线程
void thread_task(int NewConnection)
{
	char recvBuff[2048];
	ZeroMemory(recvBuff, 2048);
	if (recv(NewConnection, recvBuff, sizeof(recvBuff), 0) == SOCKET_ERROR)
	{
		cout << "Recieve failed!\n";
		closesocket(NewConnection);
	}
	//显示请求
	cout << recvBuff;
	//相应浏览器
	char temp[LITEBUF];//请求方法
	char file[LITEBUF];//URL
	strcpy(file, "./www");
	if (sscanf(recvBuff, "%s %s", temp, file + 5) == 2)
	{
		//处理请求
		HandleRequest(NewConnection, file);
	}
	else cout << "Parameter error!\n";
	//关闭socket
	closesocket(NewConnection);
}

//------------主函数：
int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ListeningSock;
	SOCKET WorkingSock;
	int ClientAddrLen;
	sockaddr_in srvAddr, clientAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));//清零srvAddr
	int number = 0;

	//初始化 winsock
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
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

	if (bind(ListeningSock, (SOCKADDR*)&srvAddr, sizeof(srvAddr)) == SOCKET_ERROR)
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
		SOCKET WorkingSocket = accept(ListeningSock, (sockaddr*)&clientAddr, &client_addrLen);
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
			thread t(thread_task, WorkingSock);
			t.join();

		}
	}
	// 关闭监听Socket，然后退出应用程序  
	closesocket(ListeningSock);
	// 释放Windows Socket 的相关资源
	WSACleanup();
	return 0;
}
