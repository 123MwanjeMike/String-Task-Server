#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(int argc, char *argv[]){
	if (argc < 3){//Checking that both the server IP address and binding port are provided
    printf("ERROR\nPlease enter a server IP and binding port\n");
    exit(0);
  }

	system("reset");
	printf("\n*** GROUP 13 ***\n");
	printf("String Task Client\n");
	printf("++++++++++++++++++++++\n\n");

	int the_socket,portno;

	the_socket = socket(AF_INET, SOCK_STREAM, 0);// creating a socket

	portno = atoi(argv[2]);

	// specifying the server address for the socket
	struct sockaddr_in server_address;//Structure array to hold the server address
	memset(&server_address,'\0',sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portno);//Binding port
	inet_pton(AF_INET, argv[1], &server_address.sin_addr);//IP address

	int connection_status = connect(the_socket, (struct sockaddr*)&server_address, sizeof(server_address));

	//Check for error with the connection;
	if (connection_status == -1){
		printf("There was an error setting up a connection to the remote socket\n\n");
		exit(0);
	}

	printf("[+]Client connection to server at %s:%d successful[+]\n",argv[1],portno);

	while (1){//keeping the client program running

		char job[1024];//Contains the job(s) to be sent to the server
		printf("\nEnter a task: ");
		gets(job);

		pid_t STATUSpid;
		if((STATUSpid = fork()) == 0){
			while(1){
				char status[8];//Array to carry all data entered by client before the results from server are gotten
				scanf("%s",status);

				send(the_socket, &status, sizeof(status), 0);

				int StatusReceiveTimes;//The number of times to receive status of pending jobs from the server
				recv(the_socket, &StatusReceiveTimes, sizeof(StatusReceiveTimes), 0);

				printf("Pending_job\tPriority\tTime_left(s)\n",StatusReceiveTimes);

				char StatusResult[1024];//Array to carry the status updates on pending jobs from the server
				for(int loop=0;loop < StatusReceiveTimes;loop++){

					//receive data from the server
					recv(the_socket, &StatusResult, sizeof(StatusResult), 0);

					printf("%s\n",StatusResult);
				}
			}
		}

		//sending job to the server
		send(the_socket, &job, sizeof(job), 0);

		int ReceiveTimes;//the number of times to receive from the server before closing the socket
		recv(the_socket, &ReceiveTimes, sizeof(ReceiveTimes), 0);

		char Result[1024];//Array that contains the string result from the server

		for(int loop=0;loop < ReceiveTimes;loop++){

			//receive data from the server
			recv(the_socket, &Result, sizeof(Result), 0);
			printf("RESULT: %s\n", Result);
		}
		bzero(job,sizeof(job));
		bzero(Result,sizeof(Result));
		kill(STATUSpid,SIGKILL);
	}
	close(the_socket);
	return 0;
}
