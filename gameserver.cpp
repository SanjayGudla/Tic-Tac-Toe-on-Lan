#include<bits/stdc++.h>
#include <iostream>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <chrono>
#include <ctime>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/ip_icmp.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
using namespace std;

char board[3][3];
int game_iter;
FILE* logfile;

void func_print(int x){
	  if(x == 0){fprintf(logfile,"## Draw \n\n");}
	  if(x == 1){fprintf(logfile,"## Player1 won!! \n\n");}
	  if(x == 2){fprintf(logfile,"## Player2 won!! \n\n");}
}
int  checkboard(){
  // Check Board for win /draw/None
	char a,b,c;
	for(int i=0;i<3;i++){
		a = board[i][0],c = board[i][1],b = board[i][2];
		if(a==b && b==c && c==a && a=='O'){
			return 1;
		}
		else if(a==b && b==c && c==a && a=='X'){
			return 2;
		}
	}
	for(int i=0;i<3;i++){
		a = board[0][i],c = board[1][i],b = board[2][i];
		if(a==b && b==c && c==a && a=='O'){
			return 1;
		}
		else if(a==b && b==c && c==a && a=='X'){
			return 2;
		}
	}
	 a = board[0][0],c = board[1][1],b = board[2][2];
		if(a==b && b==c && c==a && a=='O'){
			return 1;
		}
		else if(a==b && b==c && c==a && a=='X'){
			return 2;
		}
		a = board[0][2],c = board[1][1],b = board[2][0];
 		if(a==b && b==c && c==a && a=='O'){
 			return 1;
 		}
 		else if(a==b && b==c && c==a && a=='X'){
 			return 2;
 		}

		for(int i=0;i<3;i++){
				for(int j=0;j<3;j++){
					if(board[i][j]=='_'){
						return 3;
					}
				}
		}
		return 0;

}

void reset_matrix(){
	for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				board[i][j]='_';
			}
	}
}
void func_writeclient(int connfd,char state)
{
	char buff[MAX];
	bzero(buff, MAX);
  buff[0] = state;
  // and send that buffer to client
  write(connfd, buff, sizeof(buff));
	return;
}

int Move_Validity(int a, int b,int player){
	if(a>=3 || b<0 || a>=3 || b<0){
		fprintf(logfile,"## Incompatible Move by Palyer %d\n",player);
		return 0;
	}
	else{
		char c = board[a][b];
		if(c!='X'&&c!='O'){
			return 1;
		 }
		 else{
			 fprintf(logfile,"## Wrong Move by Player %d\n",player);
			 return 0;
		 }
	}
	return 0;
}

// Function designed for chat between client and server.
int game(int connfd1,int connfd2)
{
	char buff[MAX];
  char buff1[MAX];
	int n;
	for (;;) {
		//checking state of game
		int x = checkboard();
    bzero(buff, MAX);
		buff[0]='0'+x;
    write(connfd1, buff, sizeof(buff));write(connfd2, buff, sizeof(buff));
    if(x!=3){
			func_print(x);
      return x;
    }
		bzero(buff, MAX);
		char a;
		char b;

		//get input from client 1
		while(1){
			bzero(buff, MAX);
			if(read(connfd1, buff, sizeof(buff))==0){
				char buff2[MAX];
				bzero(buff2, MAX);
				buff2[0]='e';
				fprintf(logfile,"Player1 Disconnected\n###############################################################################\n");
				write(connfd2,buff2,sizeof(buff2));
				exit(1);
			}
			else if(buff[0]=='y'){
				fprintf(logfile,"## Player1 TimedOut \n");
				write(connfd2,buff,sizeof(buff));
				return 4;
			}
			if(Move_Validity(buff[0]-'1',buff[2]-'1',1)){
				a = buff[0];
				b = buff[2];
				break;
			}
		}

    board[a-'1'][b-'1'] = 'O';
		fprintf(logfile,"## Move From Palyer1:- (%c,%c) \n",a,b);
		for(int j=0;j<3;j++){
			bzero(buff, MAX);
			int n =0;
			for(int i=0;i<3;i++){
				buff[n++] = board[j][i];
				if(i!=2){
					buff[n++]='|';
				}
			}
			   //Send updated state of board to both clients
			   write(connfd1, buff, sizeof(buff));
    	   write(connfd2, buff, sizeof(buff));
		}

		//check state of board again.
    x = checkboard();
		bzero(buff, MAX);
		buff[0]='0'+x;
    write(connfd1, buff, sizeof(buff));write(connfd2, buff, sizeof(buff));
    if(x!=3){
			func_print(x);
       return x;
    }
		bzero(buff, MAX);

    //get input from client2
		while(1){
			bzero(buff, MAX);
			if(read(connfd2, buff, sizeof(buff))==0){
				char buff2[MAX];
				bzero(buff2, MAX);
				buff2[0]='e';
				fprintf(logfile,"Player2 Disconnected\n###############################################################################\n");
				write(connfd1,buff2,sizeof(buff2));
				exit(1);
			}
			else if(buff[0]=='y'){
				fprintf(logfile,"## Player2 TimedOut \n");
				write(connfd1,buff,sizeof(buff));
				return 4;
			}
			if(Move_Validity(buff[0]-'1',buff[2]-'1',2)){
				a = buff[0];
				b = buff[2];
				break;
			}
		}
    a = buff[0];
		b = buff[2];
    board[a-'1'][b-'1'] = 'X';
		fprintf(logfile,"## Move From Palyer2:- (%c,%c) \n",a,b);
		for(int j=0;j<3;j++){
			bzero(buff, MAX);
			int n =0;
			for(int i=0;i<3;i++){
				buff[n++] = board[j][i];
				if(i!=2){
					buff[n++]='|';
				}
			}
			//send updated borad to both client 1 and client 2
			write(connfd1, buff, sizeof(buff));
			write(connfd2, buff, sizeof(buff));
		}

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
	return 0;
}


// Driver function
int main()
{
	for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				board[i][j]='_';
			}
	}
	game_iter = 0;
	logfile = fopen("logfile.txt","w");

	int sockfd, connfd1,connfd2;
	unsigned int len1,len2;
	struct sockaddr_in servaddr, cli1,cli2;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Game server started. Waiting for players.\n");
	len1 = sizeof(cli1);
  len2 = sizeof(cli2);

	//Establishing Connection with Client 1
	connfd1 = accept(sockfd, (SA*)&cli1, &len1);
	if (connfd1 < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
  //Acknowledging  state as 1 to client 1
  func_writeclient(connfd1,'1');

	//Establishing Connection with Client 2
  connfd2 = accept(sockfd, (SA*)&cli2, &len2);
	if (connfd2 < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	//Acknowledging  state as 1 to client 1
  func_writeclient(connfd2,'2');

	// Acknowledging Client1 that Client had connected to server.
  func_writeclient(connfd1,'1');

	//Running server until game or games between players end
  while(1){
		// Resetting borad to initial state
    reset_matrix();

		//Starting time for the game
		auto t_start = chrono::high_resolution_clock::now();
		game_iter+=1;
		fprintf(logfile,"\n###############################################################################\n");
		fprintf(logfile,"## Game Number :- %d \n",game_iter );

		//Play game until draw or any one wins
    int x = game(connfd1,connfd2);

		//get replies from client 1 and client 2 for replay.
    char buff[MAX],buff1[MAX];
    bzero(buff, MAX);
		if(read(connfd1, buff, sizeof(buff))==0){
			char buff2[MAX];
			bzero(buff2, MAX);
			buff2[0]='e';
			fprintf(logfile,"Player1 Disconnected\n###############################################################################\n");
			write(connfd2,buff2,sizeof(buff2));
			exit(1);
		}
    bzero(buff1, MAX);
		if(read(connfd2, buff1, sizeof(buff1))==0){
			char buff2[MAX];
			bzero(buff2, MAX);
			buff2[0]='e';
			fprintf(logfile,"Player2 Disconnected\n###############################################################################\n");
			write(connfd1,buff2,sizeof(buff2));
			exit(1);
		}
    char p = buff[0],q = buff1[0];
    char replay = '0';

		//If both agrees for replay then only replay else stop server.
		//send acknowledgement to both clients accordingly.
    if(p=='1' && q=='1'){replay = '1';}
    else{
			 auto t_end = std::chrono::high_resolution_clock::now();
	 		 double elapsed_time_ms = chrono::duration<double, std::milli>(t_end-t_start).count();
	 		 fprintf(logfile,"## Total time for which game elapsed :- %lf ms \n",elapsed_time_ms);
	 		 fprintf(logfile,"###############################################################################\n");
			 break;
	   }
      bzero(buff, MAX);
      buff[0]=replay;
      write(connfd1, buff, sizeof(buff));write(connfd2, buff, sizeof(buff));

			auto t_end = std::chrono::high_resolution_clock::now();
			double elapsed_time_ms = chrono::duration<double, std::milli>(t_end-t_start).count();
      fprintf(logfile,"## Total time for which game elapsed :- %lf ms \n",elapsed_time_ms);
			fprintf(logfile,"###############################################################################\n");
  }

	// After chatting close the socket
	close(sockfd);
}
