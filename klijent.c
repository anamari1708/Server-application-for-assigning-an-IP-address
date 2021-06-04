#define MSG_SIZE 80
#define MAX_CLIENTS 254
#define error -1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<time.h>

int main(int argc, char *argv[])
{
	 int port,clientid,sockfd,result,random_sek,fd;
   int num_clients = 0;
   int server_sockfd, client_sockfd;
   char fd_array[MAX_CLIENTS];
   fd_set readfds, testfds, clientfds;
   char msg[MSG_SIZE + 1],kb_msg[MSG_SIZE + 10];  
   struct hostent *hostinfo;
   struct sockaddr_in address;
   char alias[MSG_SIZE];
   time_t start, end;
   double proteklo_vrijeme;
   srand(time(NULL));
   
    if (argc != 2)
    {
      printf("Pokretanje: ./<ime programa> <port>\n");
      return error;
    }
     random_sek=rand()%(10-5)+5;
     port=atoi(argv[1]);
  
     printf("\n*** Klijent program se pokrece i ugasit ce se za %d sekundi!\n",random_sek);
     fflush(stdout);
     
     /* Kreirajmo socket za klijenta */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
     if(sockfd<0)
     {
       printf("Postoji problem sa kreiranjem socketa!\n");
       return error;
     }

     address.sin_family = AF_INET;
     address.sin_port = htons(port);
    

     /* Spojimo socket sa serverovim socketom */
     if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
     {
       printf("Klijent nije spojen! Vjerojatno je greska u portu! \n");
       return error;
     }
     
     fflush(stdout);
     
     FD_ZERO(&clientfds);
     FD_SET(sockfd,&clientfds);
     FD_SET(0,&clientfds);//stdin

        
     /*  Sada cekamo poruke sa servera */
     while (1) 
     {
       testfds=clientfds;
       select(FD_SETSIZE,&testfds,NULL,NULL,NULL);
       
       for(fd=0;fd<FD_SETSIZE;fd++)
       
       {
          if(FD_ISSET(fd,&testfds))
          
          {
             if(fd==sockfd)  /*klijent obavlja read*/
             {  

                //read data from open socket
                 result = read(sockfd, msg, MSG_SIZE);
                 msg[result] = '\0';  
                 printf("IP adresa ovog klijenta je  %s\n", msg );
                 time(&start);
               
                 do {
                      time(&end);
 
                      proteklo_vrijeme = difftime(end, start);
                   } while(proteklo_vrijeme < random_sek); 
               
                    sprintf(msg, "QClient is shutting down.\n");
                    write(sockfd, msg, strlen(msg));
                    close(sockfd); 
                    exit(0); //zavrsi program
                    printf("Quit je za gasenje\n");
             }

            
             else if(fd == 0) /*Process keyboard aktivnosti*/
             { 
               fgets(kb_msg, MSG_SIZE+1, stdin);
                //printf("%s\n",kb_msg);
                if (strcmp(kb_msg, "quit\n")==0) 
                {
                    sprintf(msg, "QClient is shutting down.\n");
                    write(sockfd, msg, strlen(msg));
                    close(sockfd); 
                    exit(0); //zavrsi program
                }

             }          
          }
       }      
     }
   }
