#include <stdio.h>
#include <winsock2.h>
#include <graphics.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <shlobj.h> // 可替换为 windows.h
#include <shellapi.h>
#pragma comment(lib,"ws2_32.lib")
const int BUF_SIZE = 1024 * 100;
using namespace std;
SOCKADDR_IN addrSer;
struct client {
    string name;
    SOCKET sockCli;
};
client cli;
void rev_show() {
    int i = 0;
    char buff[BUF_SIZE];
    while (1) {
        memset(buff, 0, BUF_SIZE);
        int temp = recv(cli.sockCli, buff, BUF_SIZE - 1, NULL);
        if (temp > 0) {
            string op = buff;
            if (op == "1") {
                memset(buff, 0, BUF_SIZE);
                recv(cli.sockCli, buff, BUF_SIZE - 1, NULL);
                string name = buff;
                cout << name << "正在向你发文件!!" << endl;
                memset(buff, 0, BUF_SIZE);
                recv(cli.sockCli, buff, BUF_SIZE - 1, NULL);
                ofstream ofile("OUTPUT.txt");
                ofile << buff;
                ofile.close();
                setbkmode(TRANSPARENT);
                setcolor(RED);
                settextstyle(16, 0, _T("Consolas"));
                outtextxy(5, (i + 3) * 20, "已接受到一份文件");
                i++;
                ShellExecute(NULL, "open", "OUTPUT.txt", NULL, NULL, SW_MINIMIZE);
            }
            else if (op == "2") {
                memset(buff, 0, BUF_SIZE);
                int temp1 = recv(cli.sockCli, buff, BUF_SIZE - 1, NULL);
                if (temp1 > 0) {
                    setbkmode(TRANSPARENT);
                    setcolor(BLACK);
                    settextstyle(16, 0, _T("Consolas"));
                    outtextxy(5, (i + 3) * 20, buff);
                    i++;
                }
            }
            else if (op == "0") {
                cout << "用户名已存在或聊天对象不存在!" << endl;
                system("pause");
                exit(0);
            }
        }
    }
}
string readFileIntoString(string filename) {
    fstream ifile(filename);
    if (!ifile.is_open())cout << "找不到文件!" << endl;
    else {
        ostringstream buf;
        char ch;
        while (buf && ifile.get(ch))
            buf.put(ch);
        ifile.close();
        return buf.str();
    }
}
int main() {
    initgraph(600, 600, SHOWCONSOLE);
    loadimage(NULL, "c:\\1.jpg");
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        cout << "加载出错!" << endl;
        system("pause");
        return 0;
    }
    cout << "请求成功!" << endl;

    //创建socket

    cli.sockCli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cli.sockCli == INVALID_SOCKET) {
        cout << "创建失败!" << endl;
        system("pause");
        return 0;
    }
    cout << "创建成功!" << endl;
    //初始化协议地址族
    addrSer.sin_addr.s_addr = inet_addr("222.194.53.32");
    addrSer.sin_family = AF_INET;
    addrSer.sin_port = htons(74751);

    //连接服务器

    int temp = connect(cli.sockCli, (SOCKADDR*)&addrSer, sizeof(addrSer));
    if (temp == -1) {
        cout << "连接失败" << endl;
        cout << WSAGetLastError();
        system("pause");
        return 0;
    }
    cout << "请输入用户名(用户名不含有空格，用户名不能为“ALL”以及“所有人”): ";
    string buff;
    getline(cin, buff);
    string name = buff;
    while (name == "ALL" || name == "所有人") {
        cout << "错误，重新输入:" << endl;
        getline(cin, buff);
        name = buff;
    }
    temp = send(cli.sockCli, buff.c_str(), strlen(buff.c_str()), NULL);
    cli.name = name;
    if (temp > 0)
        cout << "连接成功!" << endl;

    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rev_show, NULL, NULL, NULL);
    //通信
    while (1) {
        cout << "请输入你想要聊天的对象" << endl;
        cin >> buff;
        temp = send(cli.sockCli, buff.c_str(), strlen(buff.c_str()), NULL);
        int op;
        if (buff != "ALL") {
            cout << "如果想发文件请输入1，如果想发消息，请输入2" << endl;
            cin >> op;
            if (op == 1) {
                if (buff != "ALL") {
                    send(cli.sockCli, "1", strlen("1"), NULL);
                    cout << "请输入你想发送的文件名" << endl;
                    string buf;
                    cin >> buf;
                    string s = readFileIntoString(buf);
                    temp = send(cli.sockCli, s.c_str(), strlen(s.c_str()), NULL);
                    if (temp > 0)
                        cout << "发送成功!" << endl;
                }
                else cout << "不允许群发文件!" << endl;
            }
            else if (op == 2) {
                send(cli.sockCli, "2", strlen("2"), NULL);
                cout << "请输入你想要发送的内容" << endl;
                cin.ignore();
                getline(cin, buff);
                temp = send(cli.sockCli, buff.c_str(), strlen(buff.c_str()), NULL);
                if (temp > 0)
                    cout << "发送：" << buff << "  成功!" << endl;
            }
        }
        else {
            cout << "请输入你想要发送的内容" << endl;
            cin.ignore();
            getline(cin, buff);
            temp = send(cli.sockCli, buff.c_str(), strlen(buff.c_str()), NULL);
            if (temp > 0)
                cout << "发送：" << buff << "  成功!" << endl;
        }
    }
    closesocket(cli.sockCli);
    WSACleanup();
}

