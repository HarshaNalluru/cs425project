#include <stdio.h>
#include <stdlib.h>      
#include <sys/socket.h>
#include <sys/types.h>    
#include <netinet/in.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ERROR -1
#define MAX_DATA 1024
#define PORT_NUMBER 9089
#define BUFFER 1024

int count_global = 0;
int main(int argc, char const *argv[])
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	int sock;
	int new;
	int sockaddr_len = sizeof(struct sockaddr_in);
	int data_len;
	int temp_len;
	char data[MAX_DATA];
	char penalty[MAX_DATA];
	char penalty_temp[MAX_DATA];
	char temp[MAX_DATA];
	char username[MAX_DATA];
	char password[MAX_DATA];
	char roomName[MAX_DATA];
	int rounds = 8;
	int flag_rounds = 0;
	int round = 0;
	int count = 0;
	pid_t PID;
	FILE * file_w;

	int number_of_players = 4;
	char send_penalties[number_of_players][BUFFER];
	int counting_players = 0;
	//GameStarts
	char input[BUFFER];
	char output[BUFFER];
	char last_word[BUFFER];
	char old_latest[BUFFER];


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
		perror("server socket: ");
		exit(-1);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT_NUMBER);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 8);

	if ((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR){
		perror("bind : ");
		exit(-1);
	}

	if ((listen(sock, 2)) == ERROR){
		perror("listen");
		exit(-1);
	}

	while(1){
		if ((new = accept(sock, (struct sockaddr *)&client, &sockaddr_len)) == ERROR){
			perror("accept");
			exit(-1);
		}
		if ( (PID = fork()) == -1 ) {
			/* Failed to fork: Give up */
			count = count+5;
			close(new);
			continue;
		} 
		else if ( PID > 0 ) {
			/* Parent process: */
			count++;
			close(new);
			continue;
		}
		printf("New client connected from port no. %d and IP %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr) );
		
		char const* const filename = "users.txt";
		FILE* file = fopen(filename, "r"); 
	    char line[1024];
	    char last_word[1024];
	    char user_status[1024];
	    data_len = 1;
	    data_len = recv(new, username, MAX_DATA, 0);
	    char * id;
	    char * pass;
	    int auth = 0;

	    while (fgets(line, sizeof(line), file)) {
	        id = strtok(line,"@");
	        pass = strtok(NULL,"@");
	        if (pass[strlen(pass)-1]=='\n'){
	        	pass[strlen(pass)-1] = '\0';
	        }
	        if (strcmp(username,id) == 0){

	        	char userok[MAX_DATA] = "#Server: Checking for the password...";
	        	send(new, userok, strlen(userok), 0);
	    		data_len = recv(new, password, MAX_DATA, 0);
	        	
	        	if (strcmp(password,pass)==0){
	        		auth = 1;
		        	//printf("count == %d\n", count);
		        	data_len = recv(new, roomName, MAX_DATA, 0);
		        	char * num_players_file = malloc(strlen(roomName)+strlen("_number_of_players.txt")+1);
					strcpy(num_players_file, roomName);
		    		strcat(num_players_file, "_number_of_players.txt");
		        	FILE* file = fopen(num_players_file, "r"); 
		        	if (file){
		        		while (fgets(line, sizeof(line), file)) {
							counting_players = atoi (line);
							if(counting_players==4){
								fclose(file);
								char failed[MAX_DATA] = "#Server: Room is full..! Choose a different room!!!";
					        	send(new, failed, strlen(failed), 0);

								printf("Client disconnected\n");
								close(new);
								return 0;
							}
							else{
								fclose(file);
								break;
							}
						}
		        	}
	        		char *success = malloc(strlen("#Server: Welcome to the room : ")+strlen(roomName)+1);
	        		strcpy(success, "#Server: Welcome to the room : ");
    				strcat(success, roomName);
		        	send(new, success, strlen(success), 0);
		        	
		        	break;
	        	}
	        	else{
		        	char failed[MAX_DATA] = "#Server: Authentication Failure!!!";
		        	send(new, failed, strlen(failed), 0);

					printf("Client disconnected\n");
					close(new);
	        	}
	        }
	    }

	    fclose(file);

	    if (auth!=1){
        	char failed[MAX_DATA] = "#Server: Authentication Failure!!!";
        	send(new, failed, strlen(failed), 0);
			
			printf("Client disconnected\n");
			close(new);
	    }
	    else{
			data_len = 1;

			//Input from the server prompt
			//FILE* file = fopen(input, "w"); 
			char * num_players_file = malloc(strlen(roomName)+strlen("_number_of_players.txt")+1);
			strcpy(num_players_file, roomName);
    		strcat(num_players_file, "_number_of_players.txt");
			FILE* file = fopen(num_players_file, "r"); 
			if (file){	
				while (fgets(line, sizeof(line), file)) {
					//send(new, line, strlen(line), 0);
					//printf("#number_of_players : %s\n", line);
					counting_players = atoi (line);
					if(counting_players==number_of_players){
						fclose(file);
						break;
					}
					else{
						fclose(file);
						FILE* file_w = fopen(num_players_file, "w"); 
						counting_players++;
						fprintf(file_w, "%d", counting_players);
						fclose(file_w);
						break;
					}
					//temp_len = recv(new, temp, MAX_DATA, 0);
					//printf("Client says : %s\n", temp);
					//printf("%s", line);
				}
			}
			else{
				FILE* file_w = fopen(num_players_file, "w"); 
				counting_players++;
				fprintf(file_w, "%d", counting_players);
				fclose(file_w);
			}
			count = counting_players - 1;
			data_len = recv(new, data, MAX_DATA, 0);
			data[data_len-1] = '\0';
			//printf("%s - %s\n",username,data);
			int flag = 0;
			while(1){
				//printf("somebody break me out -- %s\n", username );
				file = fopen(num_players_file, "r"); 
				if (file){	
					while (fgets(line, sizeof(line), file)) {
						//printf("#number_of_players : %s\n", line);
						counting_players = atoi (line);
						if(counting_players==4){
							flag = 1;
							break;
						}
					}
				}
				fclose(file);
				if (flag ==1){
					break;
				}
			}
			printf("I'm out - %s\n",username);

			char * players_online_file = malloc(strlen(roomName)+strlen("_players_online.txt")+1);
			strcpy(players_online_file, roomName);
    		strcat(players_online_file, "_players_online.txt");

			file = fopen(players_online_file, "a"); 
			fprintf(file, "%s\n", username);
			fclose(file);
			char GameStarts[MAX_DATA] = "### Game Starts ###";
			send(new, GameStarts, strlen(GameStarts), 0);
			data_len = recv(new, data, MAX_DATA, 0);
			data[data_len-1] = '\0';
			printf("%s - %s\n", data,username);

			char * word_now_file = malloc(strlen(roomName)+strlen("_word_now.txt")+1);
			strcpy(word_now_file, roomName);
			strcat(word_now_file, "_word_now.txt");

			char * words_all = malloc(strlen(roomName)+strlen("_words_all.txt")+1);
			strcpy(words_all, roomName);
			strcat(words_all, "_words_all.txt");

			char * game_progress_file = malloc(strlen(roomName)+strlen("_game_progress.txt")+1);
			strcpy(game_progress_file, roomName);
    		strcat(game_progress_file, "_game_progress.txt");

    		char * rounds_file = malloc(strlen(roomName)+strlen("_rounds_file.txt")+1);
			strcpy(rounds_file, roomName);
    		strcat(rounds_file, "_rounds_file.txt");
			//
			while(1){
				file = fopen(game_progress_file, "r"); 
				if (file){	
					

					//printf("entered if here-----\n");
					fclose(file);
					FILE* file_game_progress = fopen(game_progress_file, "r"); 
					fgets(user_status, sizeof(user_status), file_game_progress);
					fclose(file_game_progress);
					//printf("\n------------OK BRO--------------\n");

					//int x = number_of_players;
					//printf("%d\n",strlen(user_status));
					/*while(x > sizeof(user_status)/sizeof(char) ){
						file_game_progress = fopen(game_progress_file, "r"); 
						fgets(user_status, sizeof(user_status), file_game_progress);
						fclose(file_game_progress);
					}*/
					//printf("\n-----------user_status,user_status[count]  in if : %s,%c\n", user_status,user_status[count]);

					while(user_status[count%4] != '1'){
						
						file_game_progress = fopen(game_progress_file, "r"); 
						fgets(user_status, sizeof(user_status), file_game_progress);
						fclose(file_game_progress);
						/*if (strcmp(user_status,"0100")==0){
							printf("yeah----------in\n");
						}*/
						FILE* latest = fopen(word_now_file, "r");
						if(latest){
							fgets(last_word, sizeof(last_word), latest);
							fclose(latest);
							if (strcmp(old_latest,last_word)!=0){

								char last_word_cpy[BUFFER];
								strcpy(last_word_cpy,last_word);
								//strtok
								char * word_now = strtok(last_word_cpy,":");
								char * user = strtok(NULL, ":");
								char * penalty_temp2 = strtok(NULL, ":");
								//printf("lastword ------------ %s---%s\n",last_word,user );
								

								// FILE* users_file = fopen(players_online_file, "r"); 
								// int i = 0;
								// while (fgets(line, sizeof(line), users_file)) {
								// 	if (strcmp(line,user)){
								// 		strcpy(send_penalties[i%4],penalty_temp2);
								// 		strcat(send_penalties[i%4],"@");
								// 		strcat(send_penalties[i%4],user);
								// 		strcat(send_penalties[i%4],":");
								// 		int k = 3;
								// 		printf("----------------###################%d, %s----------------------\n",i,user);
								// 		while(k){
								// 			printf("%s\n",send_penalties[k] );
								// 			k--;
								// 		}
								// 		printf("----------------###################----------------------\n");
								// 	}
								// 	i++;
								// }
								// fclose(users_file);

								send(new, last_word, strlen(last_word), 0);
								//printf("sent : %s @%s\n",last_word,user );
								data_len = recv(new, temp, MAX_DATA, 0);
								temp[data_len-1] = '\0';
							}
							strcpy(old_latest,last_word);
							file = fopen(rounds_file, "r");
							if (file){
								//file = fopen(rounds_file, "r");
								fgets(line, sizeof(line), file);
								if (atoi(line) == rounds){
									flag_rounds = 1;
									break;
								}
								fclose(file);	
							}
						}
					}
					/*if (strcmp(user_status,"0100")){
						printf("yeah----------out\n");
					}
					printf("---------------####---------------\n");
					*/
					file_game_progress = fopen(game_progress_file, "r"); 
					fgets(user_status, sizeof(user_status), file_game_progress);
					fclose(file_game_progress);
					if(user_status[count%4] == '1'){

						//printf("#%s : ", username);

						//printf("It's %s's turn\n",username);
						
						//send 
						char Turn[MAX_DATA] = "It's your turn\n";
						send(new, Turn, strlen(Turn), 0);
						
						//printf("sent it jus now to %s\n", username);
						

						//time_t start = time(0);
						//recv
						data_len = recv(new, data, MAX_DATA, 0);

						data[data_len] = '\0';
						//printf("%s HELLO I AM ALIVE\n", data);


						int penalty_len = recv(new, penalty, MAX_DATA, 0);
						penalty[penalty_len] = '\0';
						//printf("%s\n", data);
						

						//store in word_now.txt
						FILE* file_word_now = fopen(word_now_file, "w");
						fprintf(file_word_now, "%s:%s:%s", data,username,penalty);
						fclose(file_word_now);

						FILE* file_words_all = fopen(words_all, "a");
						fprintf(file_words_all, "\n%s@%s:", username,penalty);
						fclose(file_words_all);


						file = fopen(rounds_file, "r"); 
						if (file){	
							int temp;
							while (fgets(line, sizeof(line), file)) {
								//printf("#number_of_players : %s\n", line);
								temp = atoi (line);
								temp++;
								fclose(file);
								file_w = fopen(rounds_file, "w"); 
								fprintf(file_w, "%d", temp);
								fclose(file_w);
								break;
							}
							if (temp == rounds){
								flag_rounds = 1;
							}
						}
						else{
							fclose(file);
							file_w = fopen(rounds_file, "w"); 
							fprintf(file_w, "%d", 1);
							fclose(file_w);
						}



						//add in words of user => append in words_{count}.txt

						char * individual_words_file = malloc(strlen(roomName)+strlen("words_")+strlen(username)+2);
						strcpy(individual_words_file, roomName);
			    		strcat(individual_words_file, "_words_");
	    				strcat(individual_words_file, username);
						FILE* file_word_user = fopen(individual_words_file, "a");
						fprintf(file_word_user, "%s:%s:%s\n", data,username,penalty);
						fclose(file_word_user);


						// strcpy(send_penalties[count%4],penalty);
						// strcat(send_penalties[count%4],"@");
						// strcat(send_penalties[count%4],username);
						// strcat(send_penalties[count%4],":");

						file = fopen(rounds_file, "r");
						if (file){
							//file = fopen(rounds_file, "r");
							fgets(line, sizeof(line), file);
							if (atoi(line) == rounds){
								flag_rounds = 1;
								break;
							}
							fclose(file);	
						}				

						//update game_progress.txt
						FILE* file_w = fopen(game_progress_file, "w"); 
						FILE* users_file = fopen(players_online_file, "r"); 
						int i = 0;
						while (fgets(line, sizeof(line), users_file)) {
							if (count%4 == number_of_players-1 && i == 0){
								fprintf(file_w, "%d", 1);
							}
							else if (count%4 == i-1){
								fprintf(file_w, "%d", 1);
							}
							else{
								fprintf(file_w, "%d", 0);
							}
							i++;
						}
						fclose(users_file);
						fclose(file_w);
					}
				}
				else if (count%4 == 0){
					printf("reached here-----\n");
					//fclose(file);
					FILE* file_w = fopen(game_progress_file, "w"); 
					int i = 0;
					int x = number_of_players;
					while (x--) {
						if (count%4 == i){
							fprintf(file_w, "%d", 1);
							printf("printed 1 by %s\n",username );
						}
						else{
							fprintf(file_w, "%d", 0);
							printf("printed 0 by %s\n",username );
						}
						i++;
					}
					//fclose(users_file);
					fclose(file_w);

					FILE* file_game_progress = fopen(game_progress_file, "r"); 
					fgets(user_status, sizeof(user_status), file_game_progress);
					fclose(file_game_progress);
					printf("\n-----------user_status : %s", user_status);
					if(user_status[count%4] == '1'){
						printf("It's %s's turn\n",username);
						
						//send 
						char Turn[MAX_DATA] = "It's your turn one\n";
						printf("%s\n",Turn );
						//Turn[]
						send(new, Turn, strlen(Turn), 0);
						
						//recv
						char word[MAX_DATA];
						data_len = recv(new, word, MAX_DATA, 0);
						word[data_len] = '\0';
						printf("%s\n", word);

						int penalty_len = recv(new, penalty, MAX_DATA, 0);
						penalty[penalty_len] = '\0';


						//store in word_now.txt
						FILE* file_word_now = fopen(word_now_file, "w");
						fprintf(file_word_now, "%s:%s:%s", word,username,penalty);
						fclose(file_word_now);


						FILE* file_words_all = fopen(words_all, "a");
						fprintf(file_words_all, "%s@%s:", username,penalty);
						fclose(file_words_all);


						file = fopen(rounds_file, "r"); 
						if (file){	
							int temp;
							while (fgets(line, sizeof(line), file)) {
								//printf("#number_of_players : %s\n", line);
								temp = atoi (line);
								temp++;
								fclose(file);
								file_w = fopen(rounds_file, "w"); 
								fprintf(file_w, "%d", temp);
								fclose(file_w);
								break;
							}
							if (temp == rounds){
								flag_rounds = 1;
							}
						}
						else{
							//fclose(file);
							file_w = fopen(rounds_file, "w"); 
							fprintf(file_w, "%d", 1);
							fclose(file_w);
						}
						//add in words of user => append in words_{count}.txt

						char * individual_words_file = malloc(strlen(roomName)+strlen("words_")+strlen(username)+2);
						strcpy(individual_words_file, roomName);
			    		strcat(individual_words_file, "_words_");
	    				strcat(individual_words_file, username);
						FILE* file_word_user = fopen(individual_words_file, "a");
						fprintf(file_word_user, "%s:%s:%s\n", word,username,penalty);
						fclose(file_word_user);
						
						// strcpy(send_penalties[count%4],penalty);
						// strcat(send_penalties[count%4],"@");
						// strcat(send_penalties[count%4],username);
						// strcat(send_penalties[count%4],":");
						//update game_progress.txt

						file = fopen(rounds_file, "r");
						if (file){
							//file = fopen(rounds_file, "r");
							fgets(line, sizeof(line), file);
							if (atoi(line) == rounds){
								flag_rounds = 1;
								break;
							}
							fclose(file);	
						}				
						file_w = fopen(game_progress_file, "w"); 
						FILE* users_file = fopen(players_online_file, "r"); 
						int i = 0;
						while (fgets(line, sizeof(line), users_file)) {
							if (count%4 == i-1){
								fprintf(file_w, "%d", 1);
								//printf("Next turn for%s by %s\n",line,username );
							}
							else{
								fprintf(file_w, "%d", 0);
							}
							i++;
						}
						fclose(users_file);
						fclose(file_w);
					}
				}
				//fclose(file);
				file = fopen(rounds_file, "r");
				if (file){
					//file = fopen(rounds_file, "r");
					fgets(line, sizeof(line), file);
					if (atoi(line) == rounds){
						flag_rounds = 1;
						break;
					}
					fclose(file);	
				}				
			}
			if (flag_rounds==1){
				int temp=number_of_players-1;
				char penalty_str[BUFFER*4];
				
				FILE* words_all_file = fopen(words_all, "r"); 
				int i = 0;
				while (fgets(line, sizeof(line), words_all_file)) {
					line[strlen(line)-1] = '\0';
					strcpy(send_penalties[i%4],line);
					i++;	
				}
				fclose(words_all_file);

				i = 0;
				strcpy(penalty_str,send_penalties[i]);
				i++;
				while(i<4){
					strcat(penalty_str,send_penalties[i]);
					i++;
				}

				printf("penalty str ----------------%s\n",penalty_str);
				char Turn[MAX_DATA] = "Scores";
				send(new, Turn, strlen(Turn), 0);
				send(new, penalty_str, strlen(penalty_str), 0);
			}

			//printf("%s is out-------------///////////////\n",username );

/*			data_len = recv(new, data, MAX_DATA, 0);
			data[data_len-1] = '\0';
			char const* const filename = data;

			file = fopen(filename, "r"); 
			if (file){	
				char download[MAX_DATA] = "#Server: Download Initiated...";
				printf("\n-------- count_global = %d\n",count_global);
				count_global = count;
				printf("\n-------- count = %d\n",count);
				printf("-------- count_global = %d\n",count_global);
				send(new, download , strlen(download), 0);
				
				data_len = recv(new, data, MAX_DATA, 0);
				data[data_len-1] = '\0';

				while (fgets(line, sizeof(line), file)) {
					send(new, line, strlen(line), 0);
					//printf("Sent Line : %s\n", line);
					temp_len = recv(new, temp, MAX_DATA, 0);
					//printf("Client says : %s\n", temp);
					//printf("%s", line);
				}
				fclose(file);
			}
			else{
				char download[MAX_DATA] = "#Server: File Not Found";
				send(new, download , strlen(download), 0);
			}*/

			printf("Client disconnected\n");
			close(new);
	    }
	}
	return 0;
}