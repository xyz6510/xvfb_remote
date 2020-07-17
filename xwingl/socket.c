#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

int init_socket(struct sockaddr_in *sadr,int ip,int port)
{
	int sck=socket(AF_INET,SOCK_STREAM,0);
	if (sck<0) {
		printf("socket create error \n");
		exit(-1);
	}

	int r=0;
	int opt=1;
	r=setsockopt(sck,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
	if (r<0) {
		printf("no SO_REUSEPORT \n");
		exit(-1);
	}

	opt=1;
	r=setsockopt(sck,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt));
	if (r<0) {
		printf("no TCP_NODELAY \n");
		exit(-1);
	}

	memset(sadr,0,sizeof(struct sockaddr_in));
	sadr->sin_addr.s_addr=ip;
	sadr->sin_port=htons(port);
	sadr->sin_family=AF_INET;
	return sck;
}

void bind_socket(int sck,struct sockaddr_in *sadr)
{
	int r;
	r=bind(sck,(struct sockaddr *)sadr,sizeof(struct sockaddr_in));
	if (r<0) {
		printf("bind fail port: %i\n",ntohs(sadr->sin_port));
		exit(-1);
	}
}

int write_data(int socket,void *buf,int num,int *s)//on sucess returns number of bytes
{
	int r=0;
	int size=num;
	num=0;
	while (size>0) {
		r=write(socket,buf,size);
		if (r<0) break;
		num+=r;
		if (r==0) break;
		size-=r;
		buf+=r;
	}
	if (s) *s=num;
	return r;
}

int read_data(int socket,void *buf,int num,int *s)//on sucess returns number of bytes
{
	int r=0;
	int size=num;
	num=0;
	while (size>0) {
		r=read(socket,buf,size);
		if (r<0) break;
		num+=r;
		if (r==0) break;
		size-=r;
		buf+=r;
	}
	if (s) *s=num;
	return r;
}

void close_socket(int sck)
{
	shutdown(sck,SHUT_RDWR);
	close(sck);
}

int create_listen_socket(int ip,int port)
{
	struct sockaddr_in sadr;
	memset(&sadr,0,sizeof(struct sockaddr_in));
	int sck=init_socket(&sadr,ip,port);
	bind_socket(sck,&sadr);
	listen(sck,1);

	return sck;
}

int accept_connection(int sck,char *name,char *ip_addr,int port)
{
	struct sockaddr_in sadr;
	memset(&sadr,0,sizeof(struct sockaddr_in));
	printf("%s:%i %s wait connection\n",ip_addr,port,name);

	int adr_len=sizeof(struct sockaddr_in);
	int client=accept(sck,(struct sockaddr *)&sadr,(socklen_t*)&adr_len);
	printf("%s:%i %s connected\n",inet_ntoa(sadr.sin_addr),ntohs(sadr.sin_port),name);

	return client;
}

int connect_to(char *name,char *ip_addr,int ip,int port)
{
	struct sockaddr_in sadr;
	memset(&sadr,0,sizeof(struct sockaddr_in));

	int conn=init_socket(&sadr,ip,port);
	int err=connect(conn,(struct sockaddr *)&sadr,sizeof(struct sockaddr_in));
	if (err<0) {
		printf("%s:%i %s connection fail\n",ip_addr,port,name);
		exit(-1);
	}
	printf("%i:%s connected\n",conn,name);
	return conn;
}
