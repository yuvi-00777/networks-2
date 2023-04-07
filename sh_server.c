/*
			NETWORK PROGRAMMING WITH SOCKETS

In this program we illustrate the use of Berkeley sockets for interprocess
communication across the network. We show the communication between a server
process and a client process.


*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

/* THE SERVER PROCESS */
#define USERNAME_LEN 32
#define USERS_FILE "users.txt"
// function for checking the username

bool check_username(char *username)
{
	FILE *fp = fopen(USERS_FILE, "r");
	if (fp == NULL)
	{
		perror("Error opening users file");
		return false;
	}

	char line[USERNAME_LEN];
	while (fgets(line, sizeof(line), fp))
	{
		// remove newline character from line
		int len = strlen(line);
		if (line[len - 1] == '\n')
		{
			line[len - 1] = '\0';
		}
		if (strcmp(line, username) == 0)
		{
			fclose(fp);
			return true;
		}
	}

	fclose(fp);
	return false;
}
void receivefrom(int sockfd, char buf[])
{
	// char *buf = malloc(sizeof(char) * 200);
	// bzero(buf, 200);
	char a[20];
	bzero(a, 20);
	int t = 0;
	while ((t = recv(sockfd, a, 10, 0)) > 0)
	{

		if (a[t - 1] == '\0')
		{
			strcat(buf, a);
			break;
		}
		strcat(buf, a);
	}
}
void sendtooo(char buf[], int newsockfd)
{

	// printf("sending||\n");
	int j = 0;
	char a[20];
	bzero(a, 20);
	while (j < strlen(buf))
	{
		bzero(a, 20);
		for (int i = 0; i < 9 && j < strlen(buf); i++)
		{
			a[i] = buf[j];
			j++;
		}
		if (j == strlen(buf))
		{
			send(newsockfd, a, strlen(a) + 1, 0);
			return;
		}
		else
		{
			send(newsockfd, a, strlen(a), 0);
		}
	}
}
void sendtooooo(char buf[], int newsockfd)
{

	// printf("sending||\n");
	int j = 0;
	char a[20];
	bzero(a, 20);
	while (j < strlen(buf))
	{
		bzero(a, 20);
		for (int i = 0; i < 9 && j < strlen(buf); i++)
		{
			a[i] = buf[j];
			j++;
		}
		send(newsockfd, a, strlen(a), 0);
	}
}
int main(int argc, char **argv)
{
	int sockfd, newsockfd; /* Socket descriptors */
	int clilen;
	struct sockaddr_in cli_addr, serv_addr;

	int i;
	char buf[200], ans[201]; /* We will use this buffer for communication */

	/* The following system call opens a socket. The first parameter
	   indicates the family of the protocol to be followed. For internet
	   protocols we use AF_INET. For TCP sockets the second parameter
	   is SOCK_STREAM. The third parameter is set to 0 for user
	   applications.
	*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Cannot create socket\n");
		exit(0);
	}

	/* The structure "sockaddr_in" is defined in <netinet/in.h> for the
	   internet family of protocols. This has three main fields. The
	   field "sin_family" specifies the family and is therefore AF_INET
	   for the internet family. The field "sin_addr" specifies the
	   internet address of the server. This field is set to INADDR_ANY
	   for machines having a single IP address. The field "sin_port"
	   specifies the port number of the server.
	*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_port = htons(20000);
	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call.
	*/
	if (bind(sockfd, (struct sockaddr *)&serv_addr,
			 sizeof(serv_addr)) < 0)
	{
		printf("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5); /* This specifies that up to 5 concurrent client
				  requests will be queued up while the system is
				  executing the "accept" system call below.
			   */

	/* In this program we are illustrating a concurrent server -- one
	   which forks to accept multiple client connections concurrently.
	   As soon as the server accepts a connection from a client, it
	   forks a child which communicates with the client, while the
	   parent becomes free to accept a new connection. To facilitate
	   this, the accept() system call returns a new socket descriptor
	   which can be used by the child. The parent continues with the
	   original socket descriptor.
	*/
	while (1)
	{

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
						   &clilen);

		if (newsockfd < 0)
		{
			printf("Accept error\n");
			exit(0);
		}

		/* Having successfully accepted a client connection, the
		   server now forks. The parent closes the new socket
		   descriptor and loops back to accept the next connection.
		*/
		if (fork() == 0)
		{

			/* This child process will now communicate with the
			   client through the send() and recv() system calls.
			*/
			close(sockfd); /* Close the old socket since all
					  communications will be through
					  the new socket.
				   */

			/* We initialize the buffer, copy the message to it,
			   and send the message to the client.
			*/

			strcpy(buf, "LOGIN:");
			sendtooo(buf, newsockfd);
			bzero(buf, strlen(buf));

			/* We again initialize the buffer, and receive a
			   message from the client.
			*/
			for (i = 0; i < 200; i++)
				buf[i] = '\0';
			// char *p = malloc(sizeof(char) * 200);
			// bzero(p, 200);

			receivefrom(newsockfd, buf);
			// buf[strlen(buf) - 1] = '\0';
			char username[USERNAME_LEN];
			strcpy(username, buf);
			for (i = 0; i < 50; i++)
				buf[i] = '\0';
			// If the username is present then we send the message "FOUND"
			// else we send the message "NOT FOUND"
			if (check_username(username))
			{

				strcpy(buf, "FOUND");
				sendtooo(buf, newsockfd);
				// send(newsockfd, buf, strlen(buf) + 1, 0);
			}
			else
			{
				strcpy(buf, "NOT FOUND");
				sendtooo(buf, newsockfd);
			}
			int t;
			bzero(ans, 50);
			bzero(buf, 50);
			while (1)
			{
				char command[200];
				bzero(command, 200);
				int z = 0;
				receivefrom(newsockfd, command);
				bzero(ans, 200);
				// command[strlen(command) - 1] = '\0';
				strcpy(ans, command);
				char *token1 = strtok(ans, " ");
				char *token2;
				if (token1 != NULL)
				{
					token2 = strtok(NULL, " ");
				}
				if (strcmp(token1, "pwd") == 0)
				{
					// printf("X1");
					char cwd[1000];
					bzero(cwd, 1000);
					// if (token2 != NULL)
					// {
					// 	bzero(buf, 200);
					// 	strcpy(buf, "####");
					// 	sendtooo(buf, newsockfd);
					// }
					if (getcwd(cwd, sizeof(cwd)) != NULL)
					{
						// get the cwd in the buffer named cwd and then send it to client in the form of small packets
						int j = 0;
						bzero(buf, 200);
						strcpy(buf, cwd);
						sendtooo(buf, newsockfd);
					}
					else
					{
						// if the command is pwd but the command is invalid then send "####"" to client
						bzero(buf, 200);
						strcpy(buf, "####");
						sendtooo(buf, newsockfd);
					}
					continue;
				}
				// dividing the received command into tokens divided with space

				// if the token is not dir or cd then the command is invalid so send "$$$$"

				else if (strcmp(token1, "dir") == 0)
				{
					// If the command is dir then execute the dir shell command using the opendir and readdir functions
					DIR *dir;
					struct dirent *entry;
					if (token2 == NULL)
					{
						dir = opendir(".");
						while ((entry = readdir(dir)) != NULL)
						{
							bzero(buf, 200);
							strcpy(buf, entry->d_name);
							strcat(buf, "\n");
							printf("%s\n", entry->d_name);
							sendtooooo(buf, newsockfd);
							// strcat(buf, "\n");
							// send(newsockfd, buf, strlen(buf), 0);
						}
						bzero(buf, 200);
						send(newsockfd, buf, strlen(buf) + 1, 0);
						closedir(dir);
						continue;
					}
					dir = opendir(token2);
					if (dir == NULL)
					{
						// if the command contains dir but the directory is not present then send "####" to the client
						bzero(buf, 200);
						strcpy(buf, "####");
						sendtooo(buf, newsockfd);
						continue;
					}

					while ((entry = readdir(dir)) != NULL)
					{
						bzero(buf, 200);
						strcpy(buf, entry->d_name);
						strcat(buf, "\n");
						printf("%s\n", entry->d_name);
						sendtooooo(buf, newsockfd);
						// strcat(buf, "\n");
						// send(newsockfd, buf, strlen(buf), 0);
					}
					bzero(buf, 200);
					send(newsockfd, buf, strlen(buf) + 1, 0);
					closedir(dir);
					continue;
				}
				else if (strcmp(token1, "cd") == 0)
				{
					// if the command is cd then usong the chdir command execute cd shell command
					if (token2 == NULL)
					{
						chdir("~");
						bzero(buf, 200);
						strcpy(buf, "Successfully changed directory");
						sendtooo(buf, newsockfd);
						continue;
					}
					if (chdir(token2) != 0)
					{
						// if the directory is invalid then send "####" to the client
						bzero(buf, 200);
						strcpy(buf, "####");
						sendtooo(buf, newsockfd);
					}
					else
					{
						bzero(buf, 200);
						strcpy(buf, "Successfully changed directory");
						sendtooo(buf, newsockfd);
					}
					continue;
				}

				else
				{
					bzero(buf, 200);
					strcpy(buf, "$$$$");
					sendtooo(buf, newsockfd);
					continue;
				}
			}

			close(newsockfd);
			exit(0);
		}

		close(newsockfd);
	}
	return 0;
}
