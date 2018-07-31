#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<signal.h>
#include<assert.h>
#include<unistd.h>
#include<fcntl.h>
#include<ctype.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/resource.h>
#include<netinet/in.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<my_global.h>
#include<mysql/mysql.h>

//Function to split string separated by a specified delimeter
char** Split(const char *str, char delimiter){
	int len, i, j;
	char* buf;//Carries a character in a specified index for comparison with the delimeter
	char** ret;//Will carry the split job

	len = strlen(str);
	buf = malloc(len + 1);
	memcpy(buf, str, len + 1);

	j = 1;
	for (i = 0; i < len; ++i){
		if (buf[i] == delimiter){
			while (buf[i + 1] == delimiter) i++;
			j++;
		}
	}

	ret = malloc(sizeof(char*) * (j + 1));
	ret[j] = NULL;

	ret[0] = buf;
	j = 1;
	for (i = 0; i < len; ++i){
		if (buf[i] == delimiter){
			buf[i] = '\0';
			while (buf[i + 1] == delimiter) i++;
			ret[j++] = &buf[i + 1];
		}
	}
	return ret;
}

//Function to update the MySQL database tables
int tables(char* q){//q is a variable carrying a formatted MySQL querry from the main function
	MYSQL *con;
	con = mysql_init(NULL);

	//Make sure to insert your root user password in the mysql_real_connect query
	if(!mysql_real_connect(con,"localhost","root","mwanje256","StringTaskDB",3306,NULL,0)){
		printf("Error establishing connection to database server. Be sure to run server as root user");
		exit(1);
	}

	if(mysql_query(con,q)){
    printf("ERROR:Failed to update database\n");
  }

	int JobID = mysql_insert_id(con);//JobID is the ID of last inserted job

	mysql_close(con);
	return JobID;
}

//Function to write to the Ready list text file
void Ready_list(int JobID,char* String,int priority){
	struct flock lock;// the file lock structure
  int fd;

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

	FILE *fptr;//Pointer to Ready_list file
	fptr = fopen("Ready_list.txt","a+");
	fd = open("Ready_list.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
	fcntl(fd, F_SETLKW, &lock);//Locking the Ready_list file while current fork write to it

	fprintf(fptr, "%d %s %d\n",JobID,String,priority);

	fclose(fptr);
	close(fd);//Closing the file lock
}

//Structure definition for array to carry the busylist
struct Busy_list{
	char ClientID[100];//Client IP address and Port number
	char JOB[100];//The Job itself
	int characters;//Number of characters in the job
	int No_jobs;//Number of jobs by from this partiular client
	int IsReplace;//Replace job or not?
	int Priority;//Priority of the job
}Array[1000];//Structure for carrying the busylist

//Function to evaluate the priorities
void PriorityEvaluator(){
	struct flock lock;
  int fd;

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  char line[256];//A line read from the Unsorted_busy_list file
	int size;
	FILE *Ufptr;//Pointer to Unsorted_busy_list file
	Ufptr = fopen("Unsorted_busy_list.txt","r+");//File that contains the waiting jobs before sorting in order of priorities

	for(size=0;fgets(line,sizeof(line),Ufptr);size++){

		char** Parts;//The resulting array after splitting the line to remove the tabs (\t)
		Parts = Split(line,'\t');
		strcpy(Array[size].ClientID,*(Parts+0));
		strcpy(Array[size].JOB,*(Parts+1));
		Array[size].characters = atoi(Parts[2]);
		Array[size].No_jobs = atoi(Parts[3]);
		Array[size].IsReplace = atoi(Parts[4]);
		Array[size].Priority = atoi(Parts[4]);
	}
	fd = open("Unsorted_busy_list.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);

	fcntl(fd, F_SETLKW, &lock);
 	remove("Unsorted_busy_list.txt");

	fclose(Ufptr);
	close(fd);

	//Getting client with the most number of jobs
	int max_jobs;
  max_jobs = Array[0].No_jobs;
  for (int i = 1; i < size; i++){
		if (max_jobs < Array[i].No_jobs)
			{	max_jobs = Array[i].No_jobs;	}
	}
	//Getting job with least number of characters
	int min_xters;
	min_xters = Array[0].characters;
  for (int i = 1; i < size; i++){
		if (min_xters > Array[i].characters)
			{	min_xters = Array[i].characters;	}
	}
	//Summing up the priorities
	for (int i = 0; i < size; i++){
		if(Array[i].characters == min_xters){
			Array[i].Priority += 1;
		}
		if(Array[i].No_jobs == max_jobs){
			Array[i].Priority += 2;
		}
	}

	//Rearranging the structure
	for(int i=0; i < size ;i++){
		for(int j=0; j < size ;j++){
			if(Array[i].Priority < Array[j].Priority){
				char swap1[100];
				strcpy(swap1,Array[i].ClientID);
				strcpy(Array[i].ClientID,Array[j].ClientID);
				strcpy(Array[j].ClientID,swap1);
				char swap2[100];
				strcpy(swap2,Array[i].JOB);
				strcpy(Array[i].JOB,Array[j].JOB);
				strcpy(Array[j].JOB,swap2);
				int a,b;
				a = Array[i].Priority;
				Array[i].Priority = Array[j].Priority;
				Array[j].Priority = a;
				b = Array[i].No_jobs;
				Array[i].No_jobs = Array[j].No_jobs;
				Array[j].No_jobs = b;
			}
		}
	}
	/*Printing the Busy list.We shall also print out the number of jobs sent by a
	particular client calculations of success and failure rate by the admin page*/
	for(int i=0; i < size ;i++){

		FILE *Bfptr;
		Bfptr = fopen("Busy_list.txt","a+");
		fprintf(Bfptr,"%s\t%s\t%d\t%d\n",Array[i].ClientID,Array[i].JOB,Array[i].Priority,Array[i].No_jobs);
		fclose(Bfptr);//closing the busy list file
	}
}

double EstimatedTime;//This is used for the time left for a pending job to get finished
//Function to get number of characters in astring
int NoOfCharacters(char* String){
	int xters = 0;
	for(int l=0;String[l] != '\0';l++){
		xters++;
		if(xters > 99){
			break;
		}
	}
	return xters;
}

struct task{
  char taskStringwd[80];
};

struct task Decrypter(struct task decrtask){
    float floatwgt;
    int th,end,val,nextchr=0,nsb4uppercase,lcasecharsb4,validNosAfterUpper,nsb4UpperCase,x,upperCasePos;
    char encriptedchar[2], finalstring[1200],wts[3];
    int j=0,k=0,i=0,m,xx,uu,xpos,t,s,len,u,nextbg,bgchr,chwleft,mm=0,w,bg=0,v=0,ii=0,pos=1,chgate=1, flag=0,flush,flush2, cur=0,chweight,wtleft=0,wtright=0;
    char srcharry[17] , descharry[17] ,wtstr[5], finalencrr[1003],destArry[80];
    char secondch,sha[2];
    char alph[]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
                 'w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S',
                 'T','U','V','W','X','Y','Z'};

    for(flush2=0;flush2<sizeof(descharry);flush2++){
      srcharry[flush2]=' ';
      descharry[flush2]=' ';
    }
	  for(i=0; i<sizeof(decrtask.taskStringwd); i++){
      if(decrtask.taskStringwd[i] != ' '&& bg==0){
        bg=i;
        break;
      }
    } //grab the reeal beginning of the string, ignoring first spaces.
    for(ii=bg; ii<sizeof(decrtask.taskStringwd); ii++){
      if(decrtask.taskStringwd[ii]=='\0'){
        break;
      }
      if(ii==bg){
			  if(decrtask.taskStringwd[bg]=='A'||decrtask.taskStringwd[bg]=='B'||decrtask.taskStringwd[bg]=='C'||decrtask.taskStringwd[bg]=='D'||decrtask.taskStringwd[bg]=='E'||
          decrtask.taskStringwd[bg]=='F'||decrtask.taskStringwd[bg]=='G'||decrtask.taskStringwd[bg]=='H'||decrtask.taskStringwd[bg]=='I'){
		      for(th=27; th<=35; th++){
            if(alph[th]==decrtask.taskStringwd[bg]){
              val=th;
            }
          }
				  sprintf(encriptedchar,"%c",alph[val-26]);
				  sprintf(&finalstring[nextchr],"%c",encriptedchar);
					nextchr=nextchr+1;
          bg = bg+1;
				}else{
          sha[0] = decrtask.taskStringwd[bg];
          bgchr = atoi(sha);
          secondch = decrtask.taskStringwd[bg+1];
          for(xx=1;xx<27;xx++){
            if(secondch==alph[xx]){
              xpos=xx;
            }
          }
          chweight = xpos+27;
          chwleft = (int)(chweight/10);
          if(bgchr != chwleft){
            chweight = xpos+1;
          }
          sprintf(&finalstring[nextchr],"%c",alph[chweight]);
          nextchr=nextchr+1;
          floatwgt = (chweight-0.1);
          nextbg = bg + ((int)(floatwgt/9))*3;
          bg = nextbg+1;
				 }
			}
    } //End for loop , ii...
    for(w=0;w<sizeof(finalstring);w++){
      if(finalstring[w] == '\0'){
        break;
      }else{
        sprintf(&decrtask.taskStringwd[w],"%c",finalstring[w]);
      }
    }
  return decrtask;
}

struct task Encrypter(struct task encrtask){
 int j=0,k=0,i=0,m,t,s,u,mm=0,w,bg=0,v=0,ii=0,pos=1,chgate=1, flag=0,flush,flush2, cur=0,chweight,wtleft=0,wtright=0;
 char srcharry[17] , descharry[17] ,wtstr[5], finalencrr[1003];
 char *dd;
 char alph[]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u',
              'v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q',
              'R','S','T','U','V','W','X','Y','Z'};

	char resultStr[1000];
  for(j=0;j<strlen(encrtask.taskStringwd);j++){
    if(encrtask.taskStringwd[j] == ' '||encrtask.taskStringwd[j] == ';'){
      break;
    }
    chgate=1;
    cur = 0;
    flag=0;
    for(flush2=0;flush2<sizeof(descharry);flush2++){
      srcharry[flush2]=' ';
      descharry[flush2]=' ';
    }
    for(k=0;k<sizeof(alph);k++){
      if(alph[k]==encrtask.taskStringwd[j]){
        pos=k;
      }
    }
    chweight=pos;
    while(flag == 0){
      if(chweight<10 && chweight > 0){
        ii=0;
        for(m=0; m<(sizeof(srcharry)-1); m++){
          if(m==cur){
            wtstr[0] = alph[chweight+26];
            descharry[cur] = wtstr[0];
            ii=1;
            flag=1;
          }/*end of if(i==cur)*/
          descharry[m+ii] = srcharry[m];
          if(srcharry[m] == ' '){
            continue;
          }
        }
        chweight =-9;
      }else{
        while(chweight > 9){
          ii=0;
          for(i=0; i < sizeof(srcharry);i++){
            if(i==cur){
              wtleft = (int)chweight/10;
              wtright = chweight%10;
              sprintf ( &wtstr[0], "%d", wtleft);
              if((chweight-1)<27){
                sprintf ( &wtstr[1], "%c", alph[chweight-1]);
              }else{
                sprintf ( &wtstr[1], "%c", alph[(chweight-1)-26]);
              }/*end of if((chweight-1)<27) {...} else {...} */
              sprintf ( &wtstr[2], "%d", wtright);
              descharry[cur]   =  wtstr[0];
              descharry[cur+1] =  wtstr[1];
              descharry[cur+2] =  wtstr[2];
              ii=3;
            }
            descharry[i+ii] = srcharry[i];
          }
          for(v=0;v<sizeof(descharry);v++){
            srcharry[v] = descharry[v];
          }
          cur+=2;
          chweight -= 9;
        }/*end of while loop, overwrighting tempcharyy[]*/
      }
      chgate++;
    }
    for(w=bg;w<sizeof(descharry);w++){
      if(descharry[w] == ' '){
        break;
      }else{
        sprintf(&resultStr[mm],"%c",descharry[w]);
      }
      mm++;
      if(mm==sizeof(resultStr)-2){
        break;
      }
    }
  }
	strcpy(encrtask.taskStringwd,resultStr);
  return encrtask;
}

int main(int argc, char *argv[]){

	if (argc < 2){//Incase no port number is provided
    fprintf(stderr,"ERROR:No port provided\n");
    exit(1);
  }

	int sockfd,ret;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("[*]Error setting up a connection.\n");
		exit(1);
	}

	system("reset");//Reseting the terminal window display
	//Starting the server
	printf("\n*** GROUP 13 ***\n");
	printf("The String Task Server\n");
	printf("==========================\n\n");
	printf("[*]Server socket created successfully[*]\n");

	int portno = atoi(argv[1]);//Server binding port specified by the user running the server program

	//Structure array for the server address
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portno);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
	if(ret<0){
		printf("[+]Error in binding to busy port %d.\n",portno);
		exit(1);
	}else{
		printf("[+]Server successfully binded to port %d \n",portno);
	}

	if(listen(sockfd,50)==0){
		printf("[+]Server listening...\n");
	}else{
		printf("[+]Listening error.\n");
	}

	pid_t childpid;//The child fork to handle a connected client
	while(1){
		//Structure array for the client address
		struct sockaddr_in ClientAddr;
		int	addr_size = sizeof(ClientAddr);

		int client_socket = accept(sockfd, (struct sockaddr*)&ClientAddr, (socklen_t*)&addr_size);
		if(client_socket<0){
			exit(1);
		}else{
			printf("\nClient %s:%d connected\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));
		}

		if((childpid = fork())==0){
			close(sockfd);

			while(1){

				char Job[1024];//Carries string received from the client
				recv(client_socket,Job,1024,0);

				time_t t = time(NULL);//Getting the time this jobs are received
				struct tm tm = *localtime(&t);

				printf("::::Client %s:%d made a request\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));

				int count = 1;//A variables that will contain the number of jobs received from the client

				char String[1024];//Carries the string that will be sent to the client
 				if(strcmp(Job,"exit")==0){//Checking if the job is an exit command
					strcpy(String,"Your exit from the String Task Server is complete\n\n\tCONNECTION TERMINATED\n");
					send(client_socket, &count, sizeof(count), 0);
					send(client_socket, &String, sizeof(String), 0);
					close(client_socket);
					printf("Client %s:%d disconnected\n\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));
					exit(0);
				}else{

					char Owner[20];//Stores the ClientID that will be used while reading jobs from the Busy_list
					sprintf(Owner,"%s:%d",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));

					//The fork for checking the status of client jobs
					pid_t STATUSpid;
					if((STATUSpid = fork()) == 0){
						while(1){
							char status[8];//Stores the string from the client when the fork working on the client's jobs is busy
							recv(client_socket,status,1024,0);
							printf("::::Client %s:%d a busy-time request\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));

							int	No_of_jobs_left = 1;//Contains the number of the clients jobs pending

							while((strcmp(status,"status")) != 0){//Checking if the received request is "Status"
								send(client_socket, &No_of_jobs_left, sizeof(No_of_jobs_left), 0);

								char WaitMessage[1024] = "Please wait...";//Incase the message received is not a status message
								send(client_socket, &WaitMessage, sizeof(WaitMessage), 0);
								recv(client_socket,status,1024,0);
								printf("::::Client %s:%d busy-time-request\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));
							}

							//Structure for pending jobs
							struct Pending{
								char JOB[60];//The pending job
								int Priority;//Priority of the pending job
								float Left_time;//Estimated time left before the pending job processing is finished
							}PendingJOB_[100];//Structure of arrays to carry the pending jobs

							//Status check code
							char sline[256];//Will carry the lines read from the Busy_list file
							FILE *Sfptr;//Pointer to Busy_list when checking for status of a job
							Sfptr = fopen("Busy_list.txt","r");

							char PendingJobClientID[20];//ClientID on the a peanding job in the Busy_list file

							for(int i=0; fgets(sline,sizeof(sline),Sfptr); i++){

								char** Parts;//Carries the different parts after splitting the line to remove tabs (\t)
								Parts = Split(sline,'\t');

								strcpy(PendingJobClientID,*(Parts+0));
								if(strcmp(PendingJobClientID,Owner) == 0){//Checking if the client owns the job on the read line

									No_of_jobs_left++;//Incrementing the number of pending jobs
									int	remainingjobs_positions = ++i;//Contains the position of the pending job in the Busy_list
									//Storing the Job, Priority, and estimated time before the job's processing is over in the structure of arrays
									strcpy(PendingJOB_[i].JOB,*(Parts+1));
									PendingJOB_[i].Priority = atoi(Parts[2]);
									PendingJOB_[i].Left_time = (EstimatedTime * remainingjobs_positions);//Span was a globally declared variable
								}
							}
							No_of_jobs_left--;//Decrementing by 1 because the initialisation started with 1
							send(client_socket, &No_of_jobs_left, sizeof(No_of_jobs_left), 0);//This will tell the client program how many pending jobs to receive from the server
							//Sending status of the pending jobs
							for (int i = 0; i < No_of_jobs_left; i++){
								char StatusResult[1024];//An array that contains the status of the pending job

								sprintf(StatusResult,"%s\t%d\t%f",PendingJOB_[i].JOB,PendingJOB_[i].Priority,PendingJOB_[i].Left_time);
								send(client_socket, &StatusResult, sizeof(StatusResult), 0);
								char Q[2048];//A string array that carries the MySQL querry for saving a copy of the status result in the database
								sprintf(Q,"INSERT INTO Status(ClientID,Job,Priority,Time_left) VALUES('%s','%s',%d,%f)",Owner,PendingJOB_[i].JOB,PendingJOB_[i].Priority,PendingJOB_[i].Left_time);
								tables(Q);//Passing the string array Q to the function that updates database tables
							}
						}
					}//End of status check

					//Ensuring received message is a legitimate job and not a DOS attack
					int DOS = 0;//This variable contains 0 if the job is a DOS
					for(int l=0;Job[l] != '\0';l++){
						if(Job[l] == ' '){
							if (Job[l+1] != ' '){
								DOS++;
							}
							break;
						}
					}	//End of check

					if(DOS == 0){//If the job is a DOS attack
						strcpy(String,"ALERT: YOU ENTERED AN ILLEGITIMATE JOB!!!\n\n\tCONNECTION TERMINATED\n");
						send(client_socket, &count, sizeof(count), 0);
						send(client_socket, &String, sizeof(String), 0);
						close(client_socket);
						printf("Client %s:%d disconnected (DOS)\n\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));
						kill(STATUSpid,SIGKILL);
						exit(0);
					}//Else, Job is a legitimate request

					//Getting the number of jobs received from the client
					for(int l=0;Job[l] != '\0';l++){
						if(Job[l] == ';'){
							count++;
						}
					}
					send(client_socket, &count, sizeof(count), 0);//This will tell the client program how many times to expect a result from the server

					//Structure array for locking of files to this particular fork
					struct flock lock;
					int fd,fdb;
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = 0;
					lock.l_len = 0;

					char** job = Split(Job,';');//"job" will carry the all jobs from the client

					for(int f = 0; f < count; f++){

						char** _SplitJob = Split(job[f],' ');//Separating the job command from the word to be manipulated

						//Checking if the command is one of replace
						char ReplaceOnot[20];
						int Replace_o_not;//variable to carry the priority of the job in reference to if it's a replace job
						strcpy(ReplaceOnot,*(_SplitJob));
						if(strcmp(ReplaceOnot,"replace")== 0){
							Replace_o_not = 3;
						}else{	Replace_o_not = 0;	}

						//Getting the number of characters in the job
						int xters = 0;
						strcpy(String,*(_SplitJob + 1));//Here, the String array will temporarily hold the the job to be processed
						xters = NoOfCharacters(String);//Getting number of characters in a job

						FILE *Ufptr;//Pointer to the Unsorted_busy_list
						Ufptr = fopen("Unsorted_busy_list.txt","a+");
						fd = open("Unsorted_busy_list.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
						fcntl(fd, F_SETLKW, &lock);

						fprintf(Ufptr, "%s:%d\t%s\t%d\t%d\t%d\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port),job[f],xters,count,Replace_o_not);

						close(fd);
						fclose(Ufptr);
					}

					PriorityEvaluator();//Function to sort out the priorities and print the pending jobs to the Busy_list

					//Loop to process the client's jobs
					for(int f=0; f < count; f++){

						char LostJob[20];//Variable to carry the ClientID on the waiting job in Busy_list

						char line[256];//Line read from the Busy_list file
						FILE *Bfptr;//Pointer to the Busy_list
						FILE *Cfptr;//Pointer to the Copy of the Busy_list
						Bfptr = fopen("Busy_list.txt","r+");

						//Checking if the top job in the Busy_list belongs to this client
						fgets(line,sizeof(line),Bfptr);
						char** Parts;
						Parts = Split(line,'\t');
						strcpy(LostJob,*(Parts+0));

						while(strcmp(LostJob,Owner) < 0){//Looping through untill the Client's job is the top on the list
							fgets(line,sizeof(line),Bfptr);
							Parts = Split(line,'\t');
							strcpy(LostJob,*(Parts+0));
						}

						//Removing the read line
						Cfptr = fopen("Copy.txt","w+");
						for(int i=0 ;fgets(line, sizeof(line), Bfptr) != NULL; i++){

							fd = open("Copy.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
							fcntl(fd, F_SETLKW, &lock);//Setting the lock on the Copy.txt file
							fprintf(Cfptr, "%s", line);
						}
						fdb = open("Busy_list.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
						fcntl(fdb, F_SETLKW, &lock);
						fclose(Cfptr);
						remove("Busy_list.txt");
						fclose(Bfptr);
						rename( "Copy.txt", "Busy_list.txt" );
						close(fd);
						close(fdb);

						//The job operation
						strcpy(job[f],*(Parts+1));//The job to be processed in the f(th) loop
						int priority = atoi(*(Parts+2));//Contains the priority of the job

						clock_t duration;
						duration = clock();//Will be used to get the duration of the job

						int xters = NoOfCharacters(String);

						char task[20];//This carries the type of command issued by the client
						char q[2048];/*This stores the MySQL formatted query that is sent to function manipulates the database tables*/
						char** SplitJob;//This contains the client command and string

						SplitJob = Split(job[f],' ');
						strcpy(task,*(SplitJob));
						strcpy(String,*(SplitJob + 1));//String contains the message to be sent to the user

						double span;//This variable contains the duration of processing a job

						if(xters<=50){

							int Doublejob  = strcmp(task,"double");
							int Reversejob = strcmp(task,"reverse");
							int Deletejob  = strcmp(task,"delete");
							int Replacejob = strcmp(task,"replace");
							int Encryptjob = strcmp(task,"encrypt");
							int Decryptjob = strcmp(task,"decrypt");

							if(Doublejob == 0){//If a double job

								char Double[256];
								strcpy(Double,String);
								strcat(String,Double);
							}else if(Reversejob == 0){//If a reverse job

								int r = strlen(String) - 1;
								int v = 0;
								char ch;
   							while (r > v){
      						ch = String[r];
     						  String[r] = String[v];
     						  String[v] = ch;
     						  r--; v++;
				    		}
							}else if(Deletejob == 0){//If a delete job

								char Str2Del[256];//Contains the string with characters to be deleted
								char DelTrend[256];/*Contains the string with all the indexes to be deleted along with commas e.g 1,6,3*/

								strcpy(Str2Del,*(SplitJob + 1));
								strcpy(DelTrend,*(SplitJob + 2));

								char** DelTrendP;//Contains the actual indexes of the characters to be deleted
								DelTrendP = Split(DelTrend,',');
								char *null = " ";//A white space will temporarilly be placed wherever an index is to be deleted

								//Replacing all characters to be deleted with a white space
								for(int k=0;DelTrendP[k] != '\0';k++){
									int p = atoi(DelTrendP[k]);
									--p;
									*(Str2Del + p) = *null;
								}
								//Removing the white spaces and appending the remaining characters together
								char* i = Str2Del;
 								char* j = Str2Del;
  							while(*j != 0){
				    			*i = *j++;
    							if(*i != ' ')
      							{	i++; }
  							}
  							*i = 0;
								strcpy(String,Str2Del);//Copying our result to String array
							}else if(Replacejob == 0){//If replace job

								char Str2Rep[256];//Contains the string with characters to be replaced
								char RepTrend[256];/*Contains the string with all the indexes to be replaced along with characters to replace them with e.g 1-o,6-d*/

								strcpy(Str2Rep,*(SplitJob + 1));
								strcpy(RepTrend,*(SplitJob + 2));

								char** RepTrendP;//Contains the actual indexes of jobs to be replaced
								RepTrendP = Split(RepTrend,',');

								for(int k=0;RepTrendP[k] != '\0';k++){

									char** tracer;//Contains the index and character to place in that index
									tracer = Split(RepTrendP[k],'-');

									int p = atoi(tracer[0]);//Contains the index of character to be replaced
									--p;

									char *rep = tracer[1];//Contains the character to place in the index at p
									*(Str2Rep + p) = *rep;//Replacing the index at p with the character in *rep
								}
								strcpy(String,Str2Rep);
							}else if(Encryptjob == 0){//If encrpytion job

								struct task StringR;
								strcpy(StringR.taskStringwd,String);
								StringR = Encrypter(StringR);

								strcpy(String,StringR.taskStringwd);
							}else if(Decryptjob == 0){//If decryption job

								struct task StringR;
								strcpy(StringR.taskStringwd,String);
								StringR = Decrypter(StringR);

								strcpy(String,StringR.taskStringwd);
							}else{//If none of the above
								char error[256] = {"ERROR:Please enter a recognised command"};//The error message sent to the client
								strcpy(String,error);
								strcpy(task,"ERROR");
							}

							duration = clock() - duration;
							double Duration = ((double)duration)/CLOCKS_PER_SEC;//Getting the duration of the job
							span = Duration;//Span contains the duration of the job

							sprintf(q,"INSERT INTO Logs(ClientIP,ClientPort,Submission_date,Submission_time,Job_type,Duration_sec) VALUES('%s',%d,'%d-%d-%d','%d:%d:%d','%s',%f)",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port),tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec,task,span);
							int JobID = tables(q);/*Calling the funtion that runs the MySQL query in q, JobID contains the ID of that particular Log in the database*/
							Ready_list(JobID,String,priority);//Calling the function that writes jobs to the Ready_list
						}else{

							//For jobs to be black listed
							duration = clock() - duration;
							double Duration = ((double)duration)/CLOCKS_PER_SEC;
							span = Duration;

							sprintf(q,"INSERT INTO Logs(ClientIP,ClientPort,Submission_date,Submission_time,Job_type,Duration_sec) VALUES('%s',%d,'%d-%d-%d','%d:%d:%d','%s',%f)",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port),tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec,task,span);
							int JobID = tables(q);

							sprintf(q,"INSERT INTO Black_list(JobID,Job_characters) VALUES(%d,%d)",JobID,xters);
							tables(q);
							strcpy(String,"|><|BLACKLISTED!!. Job contains more than 50 characters");
						}
						send(client_socket, &String, sizeof(String), 0);
						EstimatedTime = span;//Setting the estimated time left for other jobs to come basing on current processing rate
					}
					kill(STATUSpid,SIGKILL);//Killing the check status fork
					bzero(String,sizeof(String));
					bzero(Job,sizeof(Job));
					bzero(Owner,sizeof(Owner));
				}
			}
			printf("Client %s:%d disconnected\n\n",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port));
		}
		close(client_socket);
	}
	return 0;
}
