/*
introduction:
This is server program which accepts multiple client sockets and send specified file.
Also calculates MD5 hash for file and send along with file. 

compilation:
gcc serverSOCK_thread -o server -lpthread -lssl -lcrypto
*/


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<ctype.h>
#include<openssl/md5.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <arpa/inet.h>

#define FILENAME "new.txt"
#define BUFFER_SIZE 1024   

void error(const char *msg){
	perror(msg);
	
	exit(0);
}
void *server_handler (void *fd_pointer);

int main(int argc, char *argv[]){

	int sockfd, newsockfd,  portno, n, *new_sock;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	struct hostent *server;
	

	if(argc < 2){
		printf("\n !! Usage:: filename port number ! server terminated...");
		exit(1);
	}
	
	portno = atoi(argv[1]);
	// Socket Creation...
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		error("Error In Opening Socket..!!!");
	}

	
	
	//cleaning 
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//attributes	
	serv_addr.sin_family = AF_INET;
	//bcopy((char *)sever -> h_addr, (char *)&serv_addr.sin_addr.s_addr, server -> h_length);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	//bind
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("!!error in binding..");
	}
	//listen
	puts("bind success");
	listen(sockfd, 5);
		
	puts("Waiting for connections");
	clilen = sizeof(cli_addr);
	
	int th = 0;
	//accept
	while((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))){
		puts("Connection accepted");
		printf("\nnew client number : %d", ++th);
		pthread_t server_thread;
        	new_sock = malloc(1);
        	*new_sock = newsockfd;
		pthread_create(&server_thread,NULL,server_handler,(void*) new_sock);
	}

	if(newsockfd < 0){
		error(" \nError in Accepting connection.. ");
		error("error accept");
	}
	pthread_exit(NULL);
	close(newsockfd);
	return 0;


}

void *server_handler (void *fd_pointer){
	
	printf("Hello Server Handler \n");
	int newsockfd = *(int *)fd_pointer;
    //char client_message[2000];
    int read_size, write_size;
    char *message;
	
	FILE *f;
	int ch = 0;
	int words = 0, result, len = 0;
	size_t newlen;
	long buffsize;
	char ack_md5[BUFFER_SIZE];
	char num[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	
	//bzero((char *) &buffer, sizeof(buffer));
	memset(buffer, 0,  sizeof(buffer));
	memset(ack_md5, 0,  sizeof(ack_md5));
	

	f = fopen(FILENAME,"r");
	if(f == NULL){
		printf("file can't be opened..!1");
		error("file opening error");
	}
	else{
		if(fseek(f, 0L, SEEK_END) == 0){
			buffsize = ftell(f);
			printf("ftell : %d", buffsize);
			if(fseek(f, 0L, SEEK_SET) != 0){
					//error
			}
			newlen = fread(buffer, sizeof(char), buffsize, f);
		
		
		}
	}
	fclose(f);
	// sending file size... 
	write(newsockfd, &buffsize, sizeof(int));
	
	//sending file .....
	write(newsockfd, buffer, sizeof(buffer));
	//printf("\nbuffer: %s \n", buffer);
	
	

	//md5Hash(buffer);

	printf("file sent !!!\n");
	FILE *ff = popen("md5sum new.txt | cut -f 1 -d ' '", "r");
	if(ff != NULL){
		
		if(fgets(num, sizeof(num), ff))
			printf("%s",num);
		pclose(ff);
	}
	write(newsockfd, num, sizeof(num));
	
	printf("\n");
	system("md5sum new.txt | cut -f 1 -d ' '");
	printf("\n");
	
	free(fd_pointer);
     
    return 0;
	
}

