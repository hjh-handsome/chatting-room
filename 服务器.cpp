#include <stdio.h>
#include <winsock2.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
const int BUF_SIZE = 1024*100;
SOCKET sockSer;
SOCKADDR_IN addrSer;
string BUF = "ALL";
//string BUF1 = "NAME_ACC";
struct client {
	string name;
	SOCKET sockCli;
}cli[100];
int cnt = 0;
DWORD communication(LPVOID n) {
	int i = (int)n;
	while (1) {
		if (cli[i].sockCli == INVALID_SOCKET)break;
		char buff[BUF_SIZE];
		memset(buff, 0, BUF_SIZE);
		int temp = recv(cli[i].sockCli, buff, BUF_SIZE - 1, NULL);
		if (temp > 0) {
			string b = buff;
			if (b == BUF) {
				memset(buff, 0, BUF_SIZE);
				int temp1 = recv(cli[i].sockCli, buff, BUF_SIZE - 1, NULL);
				if (temp1 > 0) {
					cout << cli[i].name << "  >> 所有人： " << buff << endl;
					string BUF = cli[i].name + " 对所有人:" + buff;
					for (int j = 0; j < cnt; j++) {
						send(cli[j].sockCli, "2", strlen("2"), NULL);
						send(cli[j].sockCli, BUF.c_str(), strlen(BUF.c_str()), NULL);
					}
				}
			}
			else {
				string name = buff;
				int j;
				for (j = 0; j < cnt; j++) {
					if (cli[j].name == name) {
						memset(buff, 0, BUF_SIZE);
						int temp1 = recv(cli[i].sockCli, buff, BUF_SIZE - 1, NULL); 
						string op = buff;
						if (op == "1") {
							send(cli[j].sockCli, "1", strlen("1"), NULL);
							send(cli[j].sockCli, cli[i].name.c_str(), strlen(cli[i].name.c_str()), NULL);
							memset(buff, 0, BUF_SIZE);
							int temp1 = recv(cli[i].sockCli, buff, BUF_SIZE - 1, NULL);
							if (temp1 > 0) {
								string BUF = buff;
								send(cli[j].sockCli, BUF.c_str(), strlen(BUF.c_str()), NULL);
							}
						}
						else if(op == "2"){
							send(cli[j].sockCli, "2", strlen("2"), NULL);
							memset(buff, 0, BUF_SIZE);
							int temp1 = recv(cli[i].sockCli, buff, BUF_SIZE - 1, NULL);
							if (temp1 > 0) {
								//cout << cli[i].name << "  >> "<<name<<"： " << buff << endl;
								string BUF = cli[i].name + " 对你 : " + buff;
								send(cli[j].sockCli, BUF.c_str(), strlen(BUF.c_str()), NULL);
							}
						}
						break;
					}
				}
				if (j == cnt) {
					send(cli[i].sockCli, "0", strlen("0"), NULL);
				}
			}
		}
	}
	return 0;
}
int main() {
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		cout << "加载出错!" << endl;
		system("pause");
		return 0;
	}
	cout << "请求成功!" << endl;

	//创建socket
	sockSer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockSer == INVALID_SOCKET) {
		cout << "创建失败!" << endl;
		system("pause");
		return 0;
	}
	cout << "创建成功!" << endl;
	//初始化协议地址族
	addrSer.sin_addr.s_addr = inet_addr("222.194.53.32");
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(74751);

	//绑定socket
	int temp = bind(sockSer, (SOCKADDR*)&addrSer, sizeof(addrSer));
	if (temp == -1) {
		cout << "绑定失败!" << endl;
		system("pause");
		return 0;
	}
	cout << "绑定成功!" << endl;
	//监听
	temp=listen(sockSer, 5);
	if (temp == -1) {
		cout << "监听失败!" << endl;
		system("pause");
		return 0;
	}
	cout << "监听成功!" << endl;
	//等待客户端连接
	int i = 0;
	int naddr = sizeof(SOCKADDR_IN);
	while (i < 1024) {
		SOCKET sockCli = accept(sockSer, (SOCKADDR*)&cli[i].sockCli, &naddr);
		cli[i].sockCli = sockCli;
		if (cli[i].sockCli == SOCKET_ERROR) {
			cout << "客户端错误!" << endl;
			closesocket(sockSer);
			WSACleanup();
			return 0;
		}
		cnt++;
		char buff[BUF_SIZE];
		memset(buff, 0, BUF_SIZE); 
		int temp = recv(cli[i].sockCli, buff, BUF_SIZE - 1, NULL);
		string name = buff;
		int j;
		for (j = i - 1; j >= 0; j--) {
			if (cli[j].name == name) {
				send(cli[i].sockCli, "0", strlen("0"), NULL);
				break;
			}
		}
		if (j < 0) {
			cli[i].name = name;
			cout << "客户: " << cli[i].name << "  连接成功" << endl;
			//通信
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)communication, (LPVOID)i, NULL, NULL);
			i++;
		}
	}
	closesocket(sockSer);
	WSACleanup();
}