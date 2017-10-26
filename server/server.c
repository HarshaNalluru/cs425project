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
#define PORT_NUMBER 9054
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
	char temp[MAX_DATA];
	char username[MAX_DATA];
	char password[MAX_DATA];
	int count = 0;
	pid_t PID;

	int number_of_players = 4;
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
	        		char *success = malloc(strlen("#Server: Hello ")+strlen(username)+1);
	        		strcpy(success, "#Server: Hello ");
    				strcat(success, username);
		        	//printf("count == %d\n", count);
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
			FILE* file = fopen("number_of_players.txt", "r"); 
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
						FILE* file_w = fopen("number_of_players.txt", "w"); 
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
				FILE* file_w = fopen("number_of_players.txt", "w"); 
				counting_players++;
				fprintf(file_w, "%d", counting_players);
				fclose(file_w);
			}

			data_len = recv(new, data, MAX_DATA, 0);
			data[data_len-1] = '\0';
			//printf("%s - %s\n",username,data);
			int flag = 0;
			while(1){
				//printf("somebody break me out -- %s\n", username );
				file = fopen("number_of_players.txt", "r"); 
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
			file = fopen("players_online.txt", "a"); 
			fprintf(file, "%s\n", username);
			fclose(file);
			char GameStarts[MAX_DATA] = "### Game Starts ###";
			send(new, GameStarts, strlen(GameStarts), 0);
			data_len = recv(new, data, MAX_DATA, 0);
			data[data_len-1] = '\0';
			printf("%s - %s\n", data,username);

			//
			while(1){
				file = fopen("game_progress.txt", "r"); 
				if (file){	
					

					//printf("entered if here-----\n");
					fclose(file);
					FILE* file_game_progress = fopen("game_progress.txt", "r"); 
					fgets(user_status, sizeof(user_status), file_game_progress);
					fclose(file_game_progress);
					//printf("\n------------OK BRO--------------\n");

					//int x = number_of_players;
					//printf("%d\n",strlen(user_status));
					/*while(x > sizeof(user_status)/sizeof(char) ){
						file_game_progress = fopen("game_progress.txt", "r"); 
						fgets(user_status, sizeof(user_status), file_game_progress);
						fclose(file_game_progress);
					}*/
					//printf("\n-----------user_status,user_status[count]  in if : %s,%c\n", user_status,user_status[count]);

					while(user_status[count] != '1'){
						
						file_game_progress = fopen("game_progress.txt", "r"); 
						fgets(user_status, sizeof(user_status), file_game_progress);
						fclose(file_game_progress);
						/*if (strcmp(user_status,"0100")==0){
							printf("yeah----------in\n");
						}*/
						FILE* latest = fopen("word_now.txt", "r");
						if(latest){
							fgets(last_word, sizeof(last_word), latest);
							fclose(latest);
							if (strcmp(old_latest,last_word)!=0){
								send(new, last_word, strlen(last_word), 0);
								data_len = recv(new, temp, MAX_DATA, 0);
								temp[data_len-1] = '\0';
							}
							strcpy(old_latest,last_word);	
						}
					}
					/*if (strcmp(user_status,"0100")){
						printf("yeah----------out\n");
					}
					printf("---------------####---------------\n");
					*/
					file_game_progress = fopen("game_progress.txt", "r"); 
					fgets(user_status, sizeof(user_status), file_game_progress);
					fclose(file_game_progress);
					if(user_status[count] == '1'){
						printf("#%s : ", username);

						//printf("It's %s's turn\n",username);
						
						//send 
						char Turn[MAX_DATA] = "It's your turn\n";
						send(new, Turn, strlen(Turn), 0);
						
						//printf("sent it jus now to %s\n", username);

						//recv
						data_len = recv(new, data, MAX_DATA, 0);
						data[data_len-1] = '\0';
						printf("%s\n", data);


						//store in word_now.txt
						FILE* file_word_now = fopen("word_now.txt", "w");
						fprintf(file_word_now, "%s:%s", data,username);
						fclose(file_word_now);

						//add in words of user => append in words_{count}.txt
						char *filename = malloc(strlen("words_")+strlen(username)+1);
		        		strcpy(filename, "words_");
	    				strcat(filename, username);
						FILE* file_word_user = fopen(filename, "a");
						fprintf(file_word_user, "%s:%s\n", data,username);
						fclose(file_word_user);


						//update game_progress.txt
						FILE* file_w = fopen("game_progress.txt", "w"); 
						FILE* users_file = fopen("players_online.txt", "r"); 
						int i = 0;
						while (fgets(line, sizeof(line), users_file)) {
							if (count == number_of_players-1 && i == 0){
								fprintf(file_w, "%d", 1);
							}
							else if (count == i-1){
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
				else if (count == 0){
					printf("reached here-----\n");
					//fclose(file);
					FILE* file_w = fopen("game_progress.txt", "w"); 
					//FILE* users_file = fopen("players_online.txt", "r"); 
					int i = 0;
					int x = number_of_players;
					while (x--) {
						if (count == i){
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

					FILE* file_game_progress = fopen("game_progress.txt", "r"); 
					fgets(user_status, sizeof(user_status), file_game_progress);
					fclose(file_game_progress);
					printf("\n-----------user_status : %s", user_status);
					if(user_status[count] == '1'){
						printf("It's %s's turn\n",username);
						
						//send 
						char Turn[MAX_DATA] = "It's your turn\n";
						send(new, Turn, strlen(Turn), 0);
						
						//recv
						char word[MAX_DATA];
						data_len = recv(new, word, MAX_DATA, 0);
						word[data_len-1] = '\0';
						printf("%s\n", word);


						//store in word_now.txt
						FILE* file_word_now = fopen("word_now.txt", "w");
						fprintf(file_word_now, "%s:%s", word,username);
						fclose(file_word_now);

						//add in words of user => append in words_{count}.txt
						char *filename = malloc(strlen("words_")+strlen(username)+1);
		        		strcpy(filename, "words_");
	    				strcat(filename, username);
						FILE* file_word_user = fopen(filename, "a");
						fprintf(file_word_user, "%s:%s\n", word,username);
						fclose(file_word_user);

						//update game_progress.txt
						file_w = fopen("game_progress.txt", "w"); 
						FILE* users_file = fopen("players_online.txt", "r"); 
						int i = 0;
						while (fgets(line, sizeof(line), users_file)) {
							if (count == i-1){
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
				//fclose(file);
			}



			data_len = recv(new, data, MAX_DATA, 0);
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
			}

			printf("Client disconnected\n");
			close(new);
	    }
	}
	return 0;
}