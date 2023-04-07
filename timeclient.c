#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[100];
    int retries = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8181);
    inet_aton("127.0.0.1", &servaddr.sin_addr);

    // Send request for time to server
    if (sendto(sockfd, "givemesometime", 14, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Error sending to socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Continuously try to receive time from server with timeout and retries
    while (1)
    {
        struct pollfd fds;
        int ret;
        // Setting up poll
        fds.fd = sockfd;
        fds.events = POLLIN;

        // Wait for server response with timeout
        ret = poll(&fds, 1, 3000);
        if (ret == -1)
        {
            printf("error int polling");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            retries++;
            // printf("%d", retries);
            if (retries == 5)
            {
                printf("Timeout exceeded\n");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            else
            {
                // Send request for time to server again
                if (sendto(sockfd, "givemesometime", 14, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
                {
                    perror("Error sending to socket");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
                continue;
            }
        }

        // Receive time from server and display on screen
        bzero(buffer, 100);
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL) > 0)
        {
            printf("Date and time received from server: %s\n", buffer);
            break;
        }
        else
        {
            perror("Error receiving from socket");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
    close(sockfd);
    return 0;
}
