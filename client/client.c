#include <stdio.h>
#include <stdlib.h>      
#include <sys/socket.h>
#include <sys/types.h>    
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ERROR -1
#define BUFFER 1024
#define PORT_NUMBER 9052
// ./client harshan:123456789@127.0.0.1
int main(int argc, char *argv[])
{
	struct sockaddr_in remote_server;
	int sock;
	char input[BUFFER];
	char output[BUFFER];
	char myusername[BUFFER];
	int len;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
		perror("socket : ");
		exit(-1);
	}

	char * str = argv[1];
	//printf("%s\n", str);
	char * username = strtok(str,":");
	char * password = strtok(NULL, "@");
	char * ip_addr = strtok(NULL, "@");
	char * word_now;;

	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(PORT_NUMBER);
	remote_server.sin_addr.s_addr = inet_addr(ip_addr);
	bzero(&remote_server.sin_zero, 8);

	if ((connect(sock, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == ERROR){
		perror("connect : ");
		exit(-1);
	}

	send(sock, username, strlen(username), 0);
	len = recv(sock, output, BUFFER, 0);
	output[len] = '\0';
	printf("%s\n", output);

	send(sock, password, strlen(password), 0);
	len = recv(sock, output, BUFFER, 0);
	output[len] = '\0';
	printf("%s\n", output);

	char thank[BUFFER] = "Thanks ";
	send(sock, thank, strlen(thank), 0);

	strcpy(myusername,username);
//Game Starts message
	len = recv(sock, output, BUFFER, 0);
	output[len] = '\0';
	printf("%s\n", output);	
	char thanks[BUFFER] = "Agreed ";
	send(sock, thanks, strlen(thanks), 0);

	char message[BUFFER];
	char * user;
	int i = 0;
	while(1){
		len = recv(sock, message, BUFFER, 0);
		if (i >0 && strcmp(word_now,"yesterday")==0 ){
			printf("here goes......................3\n");
		}
		if (strcmp(message,"It's your turn\n")==0){
			printf("IF ---- %s\n", message);	
			message[len] = '\0';
			printf("---------------------------------------It's your turn\nProvide a word #%s :", myusername);	
			fgets(input, BUFFER, stdin);
			//printf("~~~~~~~~~~~~~~~~ %s\n",input);
			send(sock, input, strlen(input), 0);
			for (int i = 0; i < BUFFER; ++i){
				message[i] = '\0';
			}
		}
		else{
			printf("ElSE ---- %s\n", message);	
			word_now = strtok(message,":");
			i++;
			user = strtok(NULL, ":");
			//user[] = '\0';
			printf("#%s : %s\n",user,word_now);
			send(sock, thanks, strlen(thanks), 0);
			if (strcmp(word_now,"yesterday")==0){
				printf("here goes......................1\n");
			}
			//char * ip_addr = strtok(NULL, "@");
			/*for (int i = 0; i < strlen(message); ++i){
				message[i] = '\0';
			}
			for (int i = 0; i < strlen(user); ++i){
				user[i] = '\0';
			}*/
			for (int i = 0; i < BUFFER; ++i){
				message[i] = '\0';
			}
			if (strcmp(word_now,"yesterday")==0){
				printf("here goes......................2\n");
			}
		}
	}
//

/*	fgets(input, BUFFER, stdin);
	send(sock, input, strlen(input), 0);

	len = recv(sock, output, BUFFER, 0);
	output[len] = '\0';
	printf("%s\n", output);	

	send(sock, thank, strlen(thank), 0);

	if (strcmp(output, "File Not Found") != 0){
		input[strlen(input)-1] = '\0';
		FILE* file = fopen(input, "w"); 

		int i=1;
		while(1){
			len = recv(sock, output, BUFFER, 0);
			output[len] = '\0';

			//printf("%s\n", output);	
			if (len==0){
				break;
			}
			fprintf(file, "%s", output);

			char success[BUFFER] = "Received Line ";
			send(sock, success, strlen(success), 0);
			//printf("%s : %d\n", success, i);	
			//printf(".\n");
			i++;
		}

		printf("------------------------%s Downloaded..!\n", input);
		fclose(file);
	}
*/
	close(sock);

	return 0;
}