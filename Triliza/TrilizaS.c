#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <string.h>

#define BUFFSIZE		256
#define INPUT_PROMPT		"Input   > "
#define RECEIVED_PROMPT		"Received> "
#define MAX_COUNT       3

int recvln(connection, char *, int);
int readln(char *, int);
int CheckWinnerOrDraw(char[3][3],int); //returns: 1 c1wins   2 c2wins   3 draw   4 nothing



int main(int argc, char *argv[]){
	connection	conn1;//client 1
	connection	conn2;//client 2
	int		len;
	char		buff[BUFFSIZE];
    char tmpChar[] = "Please wait for Client2..." ;
	char Board[3][3];
	int turncounter=0; //if turncounter == 9 && noone won    we have draw
	int i,j;
	int flag;
	
	
	int k=-1;
	char * str;
	char cpy[10];
	char drawableBoard[10];
	char TempdrawableBoard[11];//for sending:Please wait for the other player...
	char FinaldrawableBoard[12];//for sending the board when game ends
	int AsciiToInteger1;
	int AsciiToInteger2;
	int Counter1;
	int Counter2;



	if (argc != 2 || (atoi(argv[1]) != 20000) ) {          //argv[0] = triliza   argv[1] = appnum
		(void) fprintf(stderr, "usage: %s <appnum(must be 20000)>\n", argv[0]); 	// in command line we must write: TrilizaS 20000
		exit(1);
	}
	 system("cls");
	 printf("\n     ----------     \n\nWelcome to Triliza Server\n\n     ----------     \n\n");

	(void) printf("Triliza Server Waiting For Connection.\n");

	//Wait for connection in appnum:20000        Casting: argv[1]->int->appnum(struct)
	conn1 = await_contact((appnum) atoi(argv[1]));  //atoi: Convert string to integer
	if (conn1 < 0)
		exit(1);
	(void) printf("Triliza Connection Established with Client 1.\n");

    //Mynhma Anamonhs gia Client 2
	len = sizeof(tmpChar);
	tmpChar[len - 1] = '\n';
	(void) send(conn1, tmpChar, len, 0);

	conn2 = await_contact((appnum) atoi(argv[1])); 
	if (conn2< 0)
		exit(1);
	(void) printf("Triliza Connection Established with Client 2.\n");

	
    (void) printf("\t1 \t2 \t3\n");  //print columms
	for (i=0; i < 3; i ++)
	{
		(void)printf("%d\t",(i+1));  //print rows
		for (j=0; j < 3; j++)
		{
			Board[i][j] =(char) '_';
			k++;
			drawableBoard[k] = Board[i][j];
			printf("%c \t",  (Board[i][j]) );
		}
	    printf("\n");
	}
	
	//***** While Loop *****//
		while(1) {
			
	      
			Counter1=0;
			do    //***** RE-MAKE drawableBoard *****//
			{
				k=-1;                  
				for (i=0; i < 3; i ++){
					for (j=0; j < 3; j ++){
						k++;
						drawableBoard[k] = Board[i][j];
					}
				}
				k++;	drawableBoard[k]='\n';	

				//***** INFORM Client2 about the board while Client1 plays *****//
				
				for(i=0;i<9;i++)
					TempdrawableBoard[i]=drawableBoard[i];

				TempdrawableBoard[9]='A';
				TempdrawableBoard[10]='\n';
				len = sizeof(TempdrawableBoard);
				(void) send(conn2, TempdrawableBoard, len, 0);//len == 10 + 1 =11


				Counter1++;
				if(Counter1 > MAX_COUNT){
					len = sizeof("SORRY KAHKES\n");    //o client1 kahke
					(void) send(conn1, "SORRY KAHKES\n", len, 0);

					len = sizeof("You Won bacause of too many failed tries\n"); //o client2 nikhse
					(void) send(conn2, "You Won bacause of too many failed tries\n", len, 0);//len == 41
					goto End ;
				}
				//Counter1 is 1 or 2 or 3
				len = sizeof(drawableBoard);
				(void) send(conn1, drawableBoard, len, 0);


				if ((len = recvln(conn1, buff, BUFFSIZE)) < 1)
					break;

			//client1 sent us "OUT_OF_TIME" so we quit and inform client2//
				if(len == 12){
					len = sizeof("The other player is Out Of His Time, Game Ends...\n"); 
					(void) send(conn2, "The other player is Out Of His Time, Game Ends...\n", len, 0);//len=50
					goto End ;
				}


			//ASCII CONVERSION			
				AsciiToInteger1 = buff[0] - '0'; //from char(that is actually in ascii)==>ASCII  ==>  int
				AsciiToInteger2 = buff[1] - '0'; 

			
			}while( (len!=3) || ( (AsciiToInteger1<1 && AsciiToInteger1>3)&&(AsciiToInteger2<1 && AsciiToInteger2>3) ) ||
					            (Board[AsciiToInteger1-1][AsciiToInteger2-1]!='_')  );

	   //prepei1(len == 3)  //prepei2 oi times na einai swstes arithmhtika //prepei3 place in board MUST NOT be taken


		//so far we have valid data
		Board[AsciiToInteger1-1][AsciiToInteger2-1]= 'X'; //client1 move
		turncounter++;
		
		flag = CheckWinnerOrDraw(Board, turncounter);

		if(flag== 1 || flag==2 || flag==3){  //PRINTING THE BOARD BEFORE SHUTTING THE GAME
			printf("Game ends...");
			//***** INFORM Client1 and Client2 about the game before shutting down *****//	
				k=-1;                  
				for (i=0; i < 3; i ++){
					for (j=0; j < 3; j ++){
						k++;
						FinaldrawableBoard[k] = Board[i][j];
					}
				}
				k++;
				FinaldrawableBoard[9]='A';
				k++;
				FinaldrawableBoard[10]='B';
				k++;
				FinaldrawableBoard[11]='\n';
				len = sizeof(FinaldrawableBoard);
				(void) send(conn1, FinaldrawableBoard, len, 0);//len == 12
				(void) send(conn2, FinaldrawableBoard, len, 0);//len == 12
		}
		if(flag==1){
			len = sizeof("You won, Congatulations!!!!\n");		
			(void) send(conn1, "You won, Congatulations!!!\n", len, 0);
			len = sizeof("You lost. You are a LOOSER!!!\n");		
			(void) send(conn2, "You lost. You are a LOOSER!!!\n", len, 0);
			goto End ;
		}
		else if(flag==2){
			len = sizeof("You lost. You are a LOOSER!!!\n");		
			(void) send(conn1, "You lost. You are a LOOSER!!!\n", len, 0);
			len = sizeof("You won, Congatulations!!!\n");		
			(void) send(conn2, "You won, Congatulations!!!\n", len, 0);
			goto End ;
		}
		else if(flag==3){
			len = sizeof("Draw, Noone Wins!!!\n");		
			(void) send(conn1, "Draw, Noone Wins!!!\n", len, 0);//len == 20
			(void) send(conn2, "Draw, Noone Wins!!!\n", len, 0);
			goto End ;
		}



		Counter2=0;
			do    //***** RE-MAKE drawableBoard *****//
			{
				k=-1;                  
				for (i=0; i < 3; i ++){
					for (j=0; j < 3; j ++){
						k++;
						drawableBoard[k] = Board[i][j];
					}
				}
				k++;	drawableBoard[k]='\n';	

				//***** INFORM Client1 about the board while Client2 plays *****//
		
				for(i=0;i<9;i++)
					TempdrawableBoard[i]=drawableBoard[i];

				TempdrawableBoard[9]='A';
				TempdrawableBoard[10]='\n';
				len = sizeof(TempdrawableBoard);
				(void) send(conn1, TempdrawableBoard, len, 0);//len == 10 + 1 =11


				Counter2++;
				if(Counter2 > MAX_COUNT){
					len = sizeof("SORRY KAHKES\n");    //o client2 kahke
					(void) send(conn2, "SORRY KAHKES\n", len, 0);

					len = sizeof("You Won bacause of too many failed tries\n"); //o client1 nikhse
					(void) send(conn1, "You Won bacause of too many failed tries\n", len, 0);//len == 41
					goto End ;
				}
				//Counter2 is 1 or 2 or 3
				len = sizeof(drawableBoard);
				(void) send(conn2, drawableBoard, len, 0);


				if ((len = recvln(conn2, buff, BUFFSIZE)) < 1)
					break;

			//client2 sent us "OUT_OF_TIME\n" so we quit and inform client1//
				if(len == 12){
					len = sizeof("The other player is Out Of His Time, Game Ends...\n"); 
					(void) send(conn1, "The other player is Out Of His Time, Game Ends...\n", len, 0);//len=50
					goto End ;
				}


			//ASCII CONVERSION			
				AsciiToInteger1 = buff[0] - '0'; //from char(that is actually in ascii)==>ASCII  ==>  int
				AsciiToInteger2 = buff[1] - '0'; 

			
			}while( (len!=3) || ( (AsciiToInteger1<1 && AsciiToInteger1>3)&&(AsciiToInteger2<1 && AsciiToInteger2>3) ) ||
					            (Board[AsciiToInteger1-1][AsciiToInteger2-1]!='_')  );


			Board[AsciiToInteger1-1][AsciiToInteger2-1]= 'O'; //client1 move
			turncounter++;
		
		flag = CheckWinnerOrDraw(Board, turncounter);

			if(flag== 1 || flag==2 || flag==3){  //PRINTING THE BOARD BEFORE SHUTTING THE GAME
			printf("Game ends...");
			//***** INFORM Client1 and Client2 about the game before shutting down *****//	
					k=-1;                  
				for (i=0; i < 3; i ++){
					for (j=0; j < 3; j ++){
						k++;
						FinaldrawableBoard[k] = Board[i][j];
					}
				}
				k++;
				FinaldrawableBoard[9]='A';
				k++;
				FinaldrawableBoard[10]='B';
				k++;
				FinaldrawableBoard[11]='\n';
				len = sizeof(FinaldrawableBoard);
				(void) send(conn1, FinaldrawableBoard, len, 0);//len == 12
				(void) send(conn2, FinaldrawableBoard, len, 0);//len == 12
		}
		if(flag==1){
			len = sizeof("You won, Congatulations!!!\n");		
			(void) send(conn1, "You won, Congatulations!!!\n", len, 0);
			len = sizeof("You lost. You are a LOOSER!!!\n");		
			(void) send(conn2, "You lost. You are a LOOSER!!!\n", len, 0);
			goto End ;
		}
		else if(flag==2){
			len = sizeof("You lost. You are a LOOSER!!!\n");		
			(void) send(conn1, "You lost. You are a LOOSER!!!\n", len, 0);
			len = sizeof("You won, Congatulations!!!\n");		
			(void) send(conn2, "You won, Congatulations!!!\n", len, 0);
			goto End ;
		}
		else if(flag==3){
			len = sizeof("Draw, Noone Wins!!!\n");		
			(void) send(conn1, "Draw, Noone Wins!!!\n", len, 0);
			(void) send(conn2, "Draw, Noone Wins!!!\n", len, 0);
			goto End ;
		}
			

	}  


End:
	// Triliza game starts here
	//create triliza board(X,O)
	//check triliza board for wins
	//send triliza board
	//(WAITING.............)
	//if client sends us:OUT OF TIME inform other client and terminate
	//else we have the move: elegxos sfalamatos TRUE: stelnoume ston idio client mynhma lathous kai perimenoyme ksana apanthsh
	//                                          FASLE: sent board to both players and client2 plays

		/* iteration ends when EOF found on stdin or triliza connection */

	(void) send_eof(conn1);
	(void) send_eof(conn2);
	(void) printf("\nTriliza Connection Closed.\n\n");
	return 0;
	system("pause");

	}

int CheckWinnerOrDraw(char Board[3][3],int turncounter){
		boolean c1wins=0;
		boolean c2wins=0;
		boolean draw=0;
		int i,j;
		//elegxos kata grammes
			for(i=0;i<3;i++){
				if(Board[i][0] ==Board[i][1] && Board[i][1] ==Board[i][2] && Board[i][2]!='_'){
					if(Board[i][2]=='X'){
						c1wins=1;
					}
					else{
						c2wins=1;
						}
				}
			}

			//elegxos kata sthles
			for(j=0;j<3;j++){
				if(Board[0][j] ==Board[1][j] && Board[1][j] ==Board[2][j] && Board[2][j]!='_')
					if(Board[2][j]=='X'){
						c1wins=1;
					}
					else{
						c2wins=1;
						}
			}

			if(Board[0][0] ==Board[1][1] && Board[1][1] ==Board[2][2] && Board[2][2]!='_'){
				if(Board[2][2]=='X'){
					c1wins=1;	
					}
				else{
					c2wins=1;
						}
			}
			
			if(Board[2][0] ==Board[1][1] && Board[1][1] ==Board[2][0] && Board[2][0]!='_'){
				if(Board[2][0]=='X'){
					c1wins=1;	
					}
				else{
					c2wins=1;
						}

			}
			if(turncounter>=9 && (!c1wins && !c2wins) ){//if turncounter == 9 && noone won    we have draw
				draw=1;
			}
			//Time to return something
			if(c1wins)     
				return 1;
			else if(c2wins) //  returns: 1 c1wins   2 c2wins   3 draw   4 nothing
				return 2;
			else if(draw)
				return 3;
			else
				return 4;

			
}


