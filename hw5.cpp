#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

int main(int argc, char *argv[]){
	//argument check
	if(argc != 2){
		cout << "need exactly 1 argument" << endl;
		return 0;
	}

	//bio stuff
	SSL_library_init();
	SSL_load_error_strings();

	BIO *bio = NULL;
	SSL_CTX *ctx;
	SSL *ssl;

	//parsing input
	string address = argv[1];
	char* hosttemp;
	char* pathtemp;
	string path;
	string http_type;

	char * token;
	token = strtok((char*)address.c_str(), "/");
	int count = 0;
	while(token != NULL){
		if(count == 0){
			http_type = string(token);
		}
		if(count == 1){
			hosttemp = token;
		}
		else if(count > 1){
			pathtemp = token;
			string temp(pathtemp);
			path+=temp + "/";
		}
		token = strtok(NULL, "/");
		count++;
	}

	if(count == 3){}
	else{
		path[path.size()-1] = '\0';
	}

	http_type = http_type.substr(0, http_type.size()-1);

	string host(hosttemp);
	string port;

	string temp;
	string temp2;
	bool found = 0;
	for(int i = 0; i < host.size(); i++){
		if(found){
			temp2+=host[i];
		}
		else{
			temp+=host[i];
		}
		if(host[i] ==':'){
			found = 1;
		}
	}

	if(found == 1) temp[temp.size()-1] = 0;
	if(found == 0) {
		if(http_type == "http"){
			temp2 = "80";
		}
		else {
			temp2 = "443";
		}
	}
	
	host = temp;
	port = temp2;

	//cout << port << endl;

	//if it's openssl
	if(http_type == "https"){
		ctx = SSL_CTX_new(TLSv1_client_method());
		bio = BIO_new_ssl_connect(ctx);
		BIO_get_ssl(bio, &ssl);
		SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

		BIO_set_conn_hostname(bio, host.c_str());
		BIO_set_conn_port(bio, port.c_str());
		if(BIO_do_connect(bio) <= 0){
			perror("cannot connect");
			BIO_free_all(bio);
			SSL_CTX_free(ctx);
			return 0;
		}

		string sendmsg = "GET /" + path + " HTTP/1.1\r\n";
		BIO_write(bio, sendmsg.c_str(), strlen(sendmsg.c_str()));
		sendmsg = "Host: " + host +":" + port + "\r\n";
		BIO_write(bio, sendmsg.c_str(), strlen(sendmsg.c_str()));
		sendmsg = "Connection: close \r\n";
		BIO_write(bio, sendmsg.c_str(), strlen(sendmsg.c_str()));
		sendmsg = "User-Agent: matsus2-netprog-hw5/1.0\r\n\r\n";
		BIO_write(bio, sendmsg.c_str(), strlen(sendmsg.c_str()));

		int p;
		char r[1024];

		while(true){
			p = BIO_read(bio, r, 1023);
			if(p <= 0) break;
			r[p] = 0;
			printf("%s", r);
		}
		

		BIO_free_all(bio);
		SSL_CTX_free(ctx);
	}
	else{
		int fd;
		struct addrinfo hints;
		memset(&hints,0,sizeof(hints));
		hints.ai_family= AF_UNSPEC;
		hints.ai_socktype=SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags= (AI_ADDRCONFIG | AI_ALL);
		struct addrinfo* res= 0;

		int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
		if(err!=0){
			perror("cannot get addr");
			return 0;
		}

		//creating socket
		if((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0){
			perror("cannot create socket");
			return 0;
		}

		if(connect(fd, res->ai_addr, res->ai_addrlen) < 0){
			perror("cannot connect");
		}

		freeaddrinfo(res);


		string sendmsg = "GET /" + path + " HTTP/1.1\r\n";
		cout << sendmsg << endl;
		if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) < 0){
			perror("failed to send");
		}


		sendmsg = "Host: " + host +":" + port + "\r\n";
		if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) < 0){
			perror("failed to send");
		}

		sendmsg = "User-Agent: matsus2-netprog-hw5/1.0\r\n\r\n";
		if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) < 0){
			perror("failed to send");
		}

		/*
		char requesthead[100000];
		bzero(requesthead, 100000);

		recv(fd, requesthead, 100000, 0);
		printf("\n%s", requesthead);

		
		char requestoutput[100000];
		bzero(requestoutput, 100000);

		recv(fd, requestoutput, 100000, 0);
		printf("\n%s", requestoutput);
		*/

		int size_recv = 0;
		char chunk[1024];
		while(1){
			memset(chunk, 0, 1024);
			if((size_recv = recv(fd, chunk, 1024, 0)) < 0){
				break;
			}
			else{
				printf("%s", chunk);
			}
		}



		//string parsestring(requesthead);

		/*
		int find = parsestring.find("Content-Length: ");
		if(find==string::npos){
			perror("Content-Length not given");
			fputs(requesthead, stderr);
			return 0;
		}
		find+= 16;

		string size = parsestring.substr(find, parsestring.find("\r", find)-find);

		int sizeint = atoi(size.c_str());



		char request[sizeint];
		bzero(request, sizeint);

		int read = recv(fd, request, sizeint-1, MSG_WAITALL);

		//printf("%s", request);
		cout.write(reinterpret_cast<char*>(request), sizeof(request));
		*/
		close(fd);
	}
	return 0;
}