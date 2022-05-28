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

int game_iter = 0;
int first_iter = 0;
//char tempfun(char state,int sockfd);
char board[3][3];

void reset_matrix(){
	for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				board[i][j]='_';
			}
	}
}
char func_readserver(int sockfd)
{
	char buff[MAX];
  bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	if(buff[0]=='e'){
		printf("Sorry Your Opponent has left \n");
		exit(1);
	}
	else if(buff[0]=='y'){
		printf("Sorry Your opponent timedout \n");
	}
	return buff[0];
}

int readfromserver(int sockfd){
  char buff[MAX];
  for(int i=0;i<3;i++){
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
		if(buff[0]=='e'){
			printf("Sorry Your Opponent has left \n");
			exit(1);
		}
		else if(buff[0]=='y'){
			printf("Sorry Your opponent timedout \n");
			return 4;
		}
    board[i][0]=buff[0];
    board[i][1]=buff[2];
    board[i][2]=buff[4];
    printf("%s\n",buff);
  }
  printf("\n");
	return 1;
}

int writetoserver(int sockfd){
  char buff[MAX];
	char buff1[MAX];
	int n;
	fflush(stdin);
		bzero(buff, sizeof(buff));
		if(game_iter!=0&&first_iter==0){
			char temp1,temp2;
			scanf("%c",&temp1);
			scanf("%c",&temp1);
		}
    while(1){
  		printf("Enter (ROW, COL) for placing your mark: ");
  		n = 0;
		  bzero(buff1, sizeof(buff1));
			auto starttime = std::chrono::high_resolution_clock::now();
  		while ((buff[n++] = getchar()) != '\n')
  			;
      auto endtime = std::chrono::high_resolution_clock::now();
			double time_taken = std::chrono::duration<double, std::milli>(endtime-starttime).count();
			if(time_taken > 15000){
           buff1[0] = 'y';
					 write(sockfd,buff1,sizeof(buff1));
					 printf("Sorry You have exceeded the time limit \n" );
					 return 0;
			}
			write(sockfd,buff,sizeof(buff));
			if(buff[0]-'1'>=3 || buff[0]-'1'<0 || buff[2]-'1'>=3 || buff[2]-'1'<0){
				printf("Please Enter different location as the given is not compatable\n");
			}
			else{
				char c = board[buff[0]-'1'][buff[2]-'1'];
	      if(c!='X'&&c!='O'){
	        break;
	       }
	       else{
	         printf("Please Enter different location as the given is filled already\n");
	       }
			}
    }
		return 1;
}

int handle(char x,int sockfd,int state){
	//take input for replay and send that to server
  if(x == '0'){printf("Draw \n\n");}
  if(x == '1'){printf("Player1 won!! \n\n");}
  if(x == '2'){printf("Player2 won!! \n\n");}
  char decide = '0';
  if(x!='3'){
    char c;
    printf("Please Enter 1 for replay else 0 :- ");
    char buff[MAX];
    bzero(buff, sizeof(buff));
		int n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		
    write(sockfd, buff , sizeof(buff));
    decide = func_readserver(sockfd);
    if(decide == '1'){
      return 1;
    }
  }
  return 0;
}

char game(char state,int sockfd){
  char c;
	//Based on state recieved play.
	//player1 moves are :-
	// 1) send input as row,col
	// 2) get upadated matrix due to change by Client1
	// 3) get upadated matrix due to change by Client2
	if(state == '1'){
		  if(game_iter==0){
				printf("Starting the game …\n");
			}
			else{
				printf("Starting the New game …\n");
				printf("Your ID is again 1, symbol is 'O' …\n");
			}
			printf(" _|_|_\n");
			printf(" _|_|_\n");
			printf(" _|_|_\n");
			for(;;){
				c = func_readserver(sockfd);
        if(c!='3'){return c;}
				if(writetoserver(sockfd)==0){
					return '4';
				}
				first_iter++;
				if(readfromserver(sockfd)==4){return '4';}
        c = func_readserver(sockfd);
        if(c!='3'){return c;}
				if(readfromserver(sockfd)==4){return '4';}
			}
		}
		//player2 moves are :-

		// 1) get upadated matrix due to change by Client1
		// 2) send input as row,col
		// 3) get upadated matrix due to change by Client2
		else{
			if(game_iter==0){
				printf("Starting the game …\n");
			}
			else{
				printf("\n Starting the New game …\n");
				printf("Your ID is again 2, symbol is 'X' …\n");
			}
			printf(" _|_|_\n");
			printf(" _|_|_\n");
			printf(" _|_|_\n");
			for(;;){
				c = func_readserver(sockfd);
        if(c!='3'){return c;}
				if(readfromserver(sockfd)==4){return '4';}
				c = func_readserver(sockfd);
        if(c!='3'){return c;}
				if(writetoserver(sockfd)==0){
					return '4';
				}
				first_iter++;
				if(readfromserver(sockfd)==4){return '4';}
			 }
		}
    return 'a';
}

int main()
{
  for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				board[i][j]='_';
			}
	}
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}


	// function for chat
	char state = func_readserver(sockfd);

	if(state == '1'){
		printf("Connected to the game server. Your player ID is 1. Waiting for a partner to join . . .\n");
		char state1 = func_readserver(sockfd);
		//printf("%c \n",state1);
		printf("Your partner's ID is 2. Your symbol is ‘O’.\n");
	}

	else{
		printf("Connected to the game server. Your player ID is 2. Your partner’s ID is 1. Your symbol is ‘X’. . .\n");
	}
  int it = 0;
  while(1){
    reset_matrix();
		//Play game until draw or one of them wins.
    char x = game(state,sockfd);
		//input YES or No for replay.
    int decide = handle(x,sockfd,state);
		game_iter++;
    if(decide ==0){
      break;
    }
  }

  	close(sockfd);
  }
