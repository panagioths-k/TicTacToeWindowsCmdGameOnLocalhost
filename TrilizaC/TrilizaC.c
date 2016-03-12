/* TrilizaC.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <time.h>
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */

#if defined(LINUX) || defined(SOLARIS)
#include <sys/time.h>
#endif

#define BUFFSIZE		256
#define INPUT_PROMPT		"Input   > "
#define RECEIVED_PROMPT		"Received> "
#define TIME_PER_ROUND  15

int recvln(connection, char *, int);
int readln(char *, int);

FILE *fr;            /* declare the file pointer */

int main(int argc, char *argv[])
{
	computer	comp;
	connection	conn;
	char		buff[BUFFSIZE];
	int		len;
	int i,j,k;
	//Time//
char		*timestr;
double tstart;
#if defined(LINUX) || defined(SOLARIS)
	struct timeval	tv;
#elif defined(WIN32)
	time_t          tv;
#endif

	//diavasma kai argv[]
	 fr = fopen ("triliza.conf", "r");  /* open the file for reading */

	 if(fr == NULL)
		 printf("File Not Found!\n");
	

	 argv[1] = (char*) malloc (sizeof(char)*20); //Reading
	 fgets( argv[1], 20, fr);

	 argv[2] = (char*) malloc (sizeof(char)*20);
	 fgets( argv[2], 20, fr);

	 (argv[1])[strlen(argv[1])-1]= 0; //Last char of 1rst line is:'\n' and we cant give value to computer comp, so we give NULL(0)

	 fclose(fr);  /* close the file prior to exiting the routine */	
 /* convert the compname to binary form comp */
  

	comp = cname_to_comp(argv[1]);
	
	if (comp == -1){
		exit(1);
	}
	/* make a connection to the TrilizaS */

	conn = make_contact(comp, (appnum) atoi(argv[2]));//make contact with: argv[1](comp) , argv[2](appnum)
	if (conn < 0) 
		exit(1);
	 system("cls");
	(void) printf("Triliza Connection Established.\n");

	printf("\n     ----------     \n\nWelcome to Triliza Client\n\n     ----------     \n\n");

	while((len = recvln(conn, buff, BUFFSIZE)) > 0) {
	  
	

		if(len == 27){ //Here we have: "Please wait for Client2..."
			(void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
		}
		else if(len == 13){//Here we have: "SORRY KAHKES\n"
			
			(void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
			break;
		}
		else if(len == 28){//Here we have: "You won, Congatulations!!!!\n"
			
			(void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
			break;
		}
	   else if(len == 30){//Here we have:"You lost. You are a LOOSER!!!\n"
		(void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
			break;
	}
	   else if(len == 41){//Here we have:"You Won bacause of too many failed tries\n"
		   (void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
			break;
	   }
	   else if (len == 50){//Here we have:"The other player is Out Of His Time, Game Ends...\n"
		   (void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
			break;
	   }
	   else if(len == 20){//Here wqew have: "Draw, Noone Wins!!!\n"
		    (void) printf(RECEIVED_PROMPT);
			(void) fflush(stdout);
			(void) write(STDOUT_FILENO, buff, len);
			break;
	   }

	   else if(len == 11 ){//Here we have: drawableboard+"0"\n " Just print the drawableboard
		   k=0;
		   printf("\nPlease wait for the other player...\n");
			(void) printf("\n\t1 \t2 \t3\n");  //print columms
			for (i=0; i < 3; i ++)
			{
				(void)printf("%d\t",(i+1));  //print rows
				for (j=0; j < 3; j++)
				{
					printf("%c \t", buff[k++] );	
				}
				printf("\n");
			}
			printf("\n");


	   }
	   else if( len == 12) //PRINTING THE BOARD BEFORE SHUTTING THE GAME
	   {
		   k=0;
		   printf("GAME ENDED");
		   (void) printf("\n\t1 \t2 \t3\n");  //print columms
			for (i=0; i < 3; i ++)
			{
				(void)printf("%d\t",(i+1));  //print rows
				for (j=0; j < 3; j++)
				{
					printf("%c \t", buff[k++] );	
				}
				printf("\n");
			}
			printf("\n");

	   }
		else if(len == 10)////Here we JUST PLAY
		{
			//time1
#if defined(LINUX) || defined(SOLARIS)
			gettimeofday(&tv, NULL);
			timestr = ctime(&tv.tv_sec);
#elif defined(WIN32)
			time(&tv);
			timestr = ctime(&tv);
#endif
			tstart = tv;

			/* print Triliza Board using buff[10] */
			 printf("\nPlease make your move...\n");
			k=0;
			(void) printf("\n\t1 \t2 \t3\n");  //print columms
			for (i=0; i < 3; i ++)
			{
				(void)printf("%d\t",(i+1));  //print rows
				for (j=0; j < 3; j++)
				{
					printf("%c \t", buff[k++] );	
				}
				printf("\n");
			}
			printf("\n");

			printf("Give the coordinates of your move:(ex: 11, 13, 21, 33..)(Rows first)\n You got 15 SECS,3 Tries\n");
			(void) printf(INPUT_PROMPT);
			(void) fflush(stdout);
			if ((len = readln(buff, BUFFSIZE)) < 1)	
				break;
	
			buff[len - 1] = '\n';

		//time2
#if defined(LINUX) || defined(SOLARIS)
			gettimeofday(&tv, NULL);
			timestr = ctime(&tv.tv_sec);
#elif defined(WIN32)
			time(&tv);
			timestr = ctime(&tv);
#endif
			
			if(tv-tstart > TIME_PER_ROUND){
				printf("Your time is: %3.0f seconds. Game Ends...\n",(tv -tstart) );
				len = sizeof("OUT_OF_TIME\n"); //len =12
				(void) send(conn, "OUT_OF_TIME\n", len, 0);
				break;
			}
			else
				(void) send(conn, buff, len, 0);
		}
	}

	/* iteration ends when stdin or the connection indicates EOF */
	(void) printf("\nTriliza Connection Closed.\n");
	(void) send_eof(conn);
	exit(0);

}