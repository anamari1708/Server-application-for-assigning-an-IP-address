#define MSG_SIZE 80
#define MAX_CLIENTS 254
#define error -1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int NadjiPrvuSlobodnuAdresu(char*);
int IzbrojiZnamenke(int);
void exitClient(int , fd_set* , char[], int* , char[]);


int main(int argc, char *argv[]) 
{
   int i=0,port,num_clients=0,server_sockfd, client_sockfd;
   struct sockaddr_in server_address;
   int addresslen = sizeof(struct sockaddr_in);
   int fd,sockfd, result,clientid,redni_broj_klijenta;
   char fd_array[MAX_CLIENTS];
   fd_set readfds, testfds, clientfds;
   char msg[MSG_SIZE + 1],kb_msg[MSG_SIZE + 10];     
   struct sockaddr_in address;
   char brojevi[255] = { 0 }; //0 i 255 su rezervirani, dakle 254 adrese + 1 za ''\0'
   char* konacna_adresa;
   char nadjeni_broj[4],primljena_adresa[20];
	 int rez = 0, velicina = 0, znamenka = 0,provjera_socketa,provjera_porta;
	
 	 if (argc != 3)
	{
		printf("Krivo pokrecete program! \nPokrenite npr: ./server 10.15.16.0/24 1234\n");
		return error;
	}

	printf("IP adresa %s \n", argv[1]);
	printf("Port je %s \n", argv[2]);
  
  port=atoi(argv[2]);
     
     printf("\n\t\t...Pokretanje servera... \n\t\t Za zavrsetak napisite: quit\n");
     fflush(stdout);

     /* Stvaranje socketa za server */
     provjera_socketa=server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if(provjera_socketa<0)
     {
       printf("Postoji problem sa kreiranjem socketa!\n");
       return error;
     }
     server_address.sin_family = AF_INET;
     server_address.sin_addr.s_addr = htonl(INADDR_ANY);
     server_address.sin_port = htons(port);
     provjera_porta=bind(server_sockfd, (struct sockaddr *)&server_address, addresslen);
     if(provjera_porta<0)
     {
       printf("Port mozda nije dostupan!\n");
       return error;
     }
  
     /* Stvaranje reda veze i inicijaliziranje skupa deskriptora datoteke */
     listen(server_sockfd, 1);
     FD_ZERO(&readfds);
      // FD_ZERO(&fdset) inicijalizira skup fdset deskriptora datoteke tako da ima nula bitova za sve deskriptore datoteka

     FD_SET(server_sockfd, &readfds);
     FD_SET(0, &readfds);  /* dodajemo keyboard u skup deskriptora datoteka*/
  
    memset(brojevi, '0', 254);

     /*  Sada se cekaju klijenti i requestovi */
     while (1) 
     {
        testfds = readfds;
        select(FD_SETSIZE, &testfds, NULL, NULL, NULL);

        /* Ako postoji aktivnost, pronaði kakva koristeci FD_ISSET */
        for (fd = 0; fd < FD_SETSIZE; fd++)  // FD_SETSIZE jer postoji maksimalan broj deskriptora datoteka o kojima objekt fd_set može sadržavati informacije
        {
           if (FD_ISSET(fd, &testfds)) 
           {
              
              if (fd == server_sockfd)  /* prihvacamo connection request */
              { 
                 client_sockfd = accept(server_sockfd, NULL, NULL);
                                             
                 if (num_clients < MAX_CLIENTS) 
                 {
                    FD_SET(client_sockfd, &readfds);
                    fd_array[num_clients]=client_sockfd;
                    /*Client ID*/
                    redni_broj_klijenta=num_clients+1;
                    printf("\n Oh, pridruzio nam se novi klijent, sada ima %d klijenta/klijenata!:) \n",redni_broj_klijenta);
                    num_clients++;

                    strcpy(primljena_adresa, argv[1]);
		                konacna_adresa = strtok(primljena_adresa, "/");
		                konacna_adresa[strlen(konacna_adresa) - 1] = 0;
                   
                   //netko se javio i treba mu dati prvu slobodnu adresu od 1 do 254
                    rez = NadjiPrvuSlobodnuAdresu(brojevi);
                    
                    if (rez == -1)
                    {
                      printf("Nema slobodne adrese! \n");
                    }
                       
                   else
                   {
                      brojevi[rez-1]='1'; //da postavimo da je ta adresa zauzeta
                      velicina = IzbrojiZnamenke(rez);
                      memset(nadjeni_broj, 0, 4);

                      for (int i = 0; i < velicina; i++)
                      {
                        znamenka = rez % 10;
                        nadjeni_broj[velicina - 1 - i] = znamenka + '0';
                        rez = rez / 10;
                      }

                      strcat(konacna_adresa, nadjeni_broj);

                      printf("Dobivena IP adresa glasi: %s\n", konacna_adresa);

                      write(client_sockfd, konacna_adresa, strlen(konacna_adresa));

                   }
                 }
                 else 
                 {
                    sprintf(msg, "Ispricavamo se, broj klijenata je ogranicen. Pokusajte ponovo kasnije.\n");
                    write(client_sockfd, msg, strlen(msg));
                    close(client_sockfd);
                 }
              }
             
             
              else if (fd == 0)  /* Process keyboard aktivnosti */
              {                   
                 fgets(kb_msg, MSG_SIZE + 1, stdin); //cita se s tipkovnice
                 if (strcmp(kb_msg, "quit\n")==0) 
                 {
                   for (i = 0; i < num_clients ; i++) 
                    {
                      
                       close(fd_array[i]);
                    }
                    close(server_sockfd);
                    return -1;
                 }
            
                
              }
             else if(fd) /*Process klijent specificne aktivnosti*/
           {  
             //server cita s otvorenog socketa
                 result = read(fd, msg, MSG_SIZE);
                 
                 if(result==-1) 
                   perror("read()");
               
                 else if(result>0)
                 {
                    sprintf(kb_msg," \nKlijent koji je zauzimao IP adresu sa zavrsetkom .%d nas napusta! ",fd-3);
                    msg[result]='\0';
                    
                    /*spojimo klijentov id sa njegovom porukom*/
                    strcat(kb_msg,msg+1);                                        
                    
                    }
                    /*isprintajmo dobijeno na server*/
                    printf("%s",kb_msg+1);
                    
                     /*ugasimo klijenta*/
                    if(msg[0] == 'Q')
                    {
                       exitClient(fd,&readfds, fd_array,&num_clients,brojevi);
                    }   
                 }                                   
              }                  
           } 
          
           }//if
        }//for
     //while


int NadjiPrvuSlobodnuAdresu(char* brojevi)
{
	for (int i = 0; i < strlen(brojevi); i++)
	{
		if (brojevi[i] == '0')
		{
			return (i + 1);
		}
	}

	return -1;
}

int IzbrojiZnamenke(int broj)
{
	int brojac = 0;
	while (broj > 0)
	{
		broj /= 10;
		brojac++;
	}

	return brojac;
}

void exitClient(int fd, fd_set *readfds, char fd_array[], int* num_clients, char brojevi[255])
{
    int i;  
    close(fd);
    FD_CLR(fd, readfds); //obriši klijenta koji odlazi 
  
    for (i = 0; i < (*num_clients) - 1; i++)
        if (fd_array[i] == fd)
            break; 
  
    brojevi[fd-4]='0'; //ponovno je slobodna ta adresa
  
    for (; i < (*num_clients) - 1; i++)
        (fd_array[i]) = (fd_array[i + 1]);
    (*num_clients)--;
}



