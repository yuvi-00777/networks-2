#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define PORT 8888
#define MAX_RETRIES 5
#define TIMEOUT_MS 3000

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, client_addr;
    char buffer[256];
    // Create socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&client_addr, 0, sizeof(client_addr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8181);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Continuously wait for client requests
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Receive client request and send current date and time in response
        memset(buffer, 0, sizeof(buffer));
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len) < 0)
        {
            perror("Error receiving from socket");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strcpy(buffer, asctime(timeinfo));
        // sending time from server to client
        if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, client_len) < 0)
        {
            perror("Error sending to socket");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
    close(sockfd);
    return 0;
}
