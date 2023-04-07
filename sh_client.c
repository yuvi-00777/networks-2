
/*    THE CLIENT PROCESS */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void receivefrom(int sockfd)
{

	char a[20];
	bzero(a, 20);
	int t = 0;
	while ((t = recv(sockfd, a, 10, 0)) > 0)
	{

		if (a[t - 1] == '\0')
		{
			printf("%s\n", a);
			break;
		}
		printf("%s", a);
	}
}
void receivefrommnoprint(int sockfd, char buf[])
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
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in serv_addr;

	int i;
	char buf[200], ans[200];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Unable to create socket\n");
		exit(0);
	}

	/* Recall that we specified INADDR_ANY when we specified the server
	   address in the server. Since the client can run on a different
	   machine, we must specify the IP address of the server.

	   In this program, we assume that the server is running on the
	   same machine as the client. 127.0.0.1 is a special address
	   for "localhost" (this machine)

	/* IF YOUR SERVER RUNS ON SOME OTHER MACHINE, YOU MUST CHANGE
		   THE IP ADDRESS SPECIFIED BELOW TO THE IP ADDRESS OF THE
		   MACHINE WHERE YOU ARE RUNNING THE SERVER.
		*/

	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	// serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_port = htons(20000);

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *)&serv_addr,
				 sizeof(serv_addr))) < 0)
	{
		perror("Unable to connect to server\n");
		exit(0);
	}

	/* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/
	for (i = 0; i < 50; i++)
		buf[i] = '\0';
	for (i = 0; i < 50; i++)
		ans[i] = '\0';
	// receiving the login from the server
	receivefrom(sockfd);
	// printf("%s\n", buf);
	for (i = 0; i < 50; i++)
		buf[i] = '\0';
	fgets(buf, 50, stdin);
	buf[strlen(buf) - 1] = '\0';
	// getting the username from the user
	// send(sockfd, buf, strlen(buf) + 1, 0);
	sendtooo(buf, sockfd);
	bzero(buf, 200);
	receivefrommnoprint(sockfd, buf);
	if (strcmp(buf, "FOUND") == 0)
	{
		printf("%s\n", buf);
	}
	// if the username is invalid then close the socket descriptor
	if (strcmp(buf, "NOT FOUND") == 0)
	{

		printf("NOT FOUND\nInvalid username\n", buf);
		close(sockfd);
		return 0;
	}
	while (1)
	{
		// get the shell commands to be executed from the user
		printf("Enter the shell command to be executed:");
		for (i = 0; i < 200; i++)
			buf[i] = '\0';
		fgets(buf, 200, stdin);
		buf[strlen(buf) - 1] = '\0';
		if (strcmp(buf, "exit") == 0)
		{
			close(sockfd);
			return 0;
		}
		sendtooo(buf, sockfd);
		int t;
		bzero(buf, 200);
		receivefrom(sockfd);
	}

	close(sockfd);
	return 0;
}
