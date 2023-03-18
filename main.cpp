#include <winsock2.h>
#include <iostream>
#include <conio.h>//for _kbhit()
#include <ws2tcpip.h>
#include <cstring>
using namespace std;
int main(/*int argc,char** argv*/)
{
    WSAData wsadata;
    if(WSAStartup(MAKEWORD(2,0),&wsadata)){
        cout<<"wsa startup failed"<<endl;
        return -1;
    }

    /*if(argc < 3){
        cout<<"bad command"<<endl;
        cout<<"use program-name hostname port"<<endl;
        return -1;
    }*/
    input:
    string hostname,port;
    cout<<"Enter hostname : ";
	cin>>hostname;
	/*char is_IPv4 = 'y';
	for(int i=0;i<(int)hostname.length();++i){
		if(hostname[i]==':'){
			is_IPv4 = 'n';
			break;
		}
	}*/
    if(hostname == ""){
        WSACleanup();
        return 0;
    }
    cout<<"Enter port : ";
    cin>>port;
	//bool is_http = (port=="80")?true:false;
    /*
    The hostname could be a domain name like example.com
    or an IP address such as 192.168.17.23 or ::1. The port
    can be a number, such as 80, or a protocol, such as http.
    */
    addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
	/*if(is_IPv4=='n'){
		hints.ai_family = AF_INET6;
	}else{*/
	//#define AI_V4MAPPED                 0x00000800
	//hints.ai_flags = AI_V4MAPPED;
	hints.ai_family = AF_INET;
	//}
	addrinfo* peer_addr;
	if(getaddrinfo(hostname.c_str(),port.c_str(),&hints,&peer_addr)){
        cout<<"getaddrinfo() failed"<<endl;
        return -1;
    }
	/*cout<<"Remote address is : "<<endl;
    char address_buff[100],service_buff[50];//service = port num
    getnameinfo(peer_addr->ai_addr,peer_addr->ai_addrlen,address_buff,sizeof(address_buff),
                service_buff,sizeof(service_buff),NI_NUMERICHOST);
	cout<<address_buff<<endl;*/
    //NI_NUMERICHOST flag, which specifies that we want to see the hostname as an IP address.

    cout<<"creating socket"<<endl;
    SOCKET s = socket(peer_addr->ai_family,peer_addr->ai_socktype,peer_addr->ai_protocol);
    if(s == INVALID_SOCKET){
        cout<<"socket() failed"<<endl;
        return -1;
	}

    //connecting...
    if(connect(s,peer_addr->ai_addr,peer_addr->ai_addrlen)){
		cout<<"connect() failed : "<<WSAGetLastError()<<endl;
        return -1;
	}
	cout<<"connected"<<endl;
    freeaddrinfo(peer_addr);

    //int bytes = send(s,"1",1,MSG_OOB);
    //int bytes = send(s,"helllllo",strlen("helllllo"),0);
    //if(bytes<=0){
    //	cout<<"error in send()"<<endl;
    //	return 1;
    //}//else{
    //	cout<<"sent OOB data"<<endl;
    //}
    while(1){
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(s,&reads);
        //The Windows select()  function only works on sockets.
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        if(select(s+1,&reads,0,0,&timeout) < 0){//block until somethings be ready to read from socket/sockets
            cout<<"select() failed"<<endl;
            break;
        }

        //checking for TCP socket new data...
        if(FD_ISSET(s,&reads)){
            char buff[4096];
            int bytes_received = recv(s,buff,4096,0);
            if(bytes_received < 1){
                cout<<"connection_closed by server"<<endl;
                break;
            }
            //cout<<"bytes received : "<<bytes_received<<endl;
            buff[bytes_received]=0;
            cout<<buff<<endl;
        }

        //checking for user new data...
        if(_kbhit()){
            char to_send[2048];
            int bytes_sent;
            cin.sync();
			cin.getline(to_send,2048,'\n');//Enter
            if(to_send[0] != '\0'){
                int strlength = strlen(to_send);
				bytes_sent = send(s,to_send,strlength,0);
				//if(is_http){
				//	send(s,"\r\n\r\n",strlen("\r\n\r\n"),0);
				//}
                cout<<"sent bytes : "<<bytes_sent<<endl;
            }
        }
    }
    goto input;
    return 0;
}
