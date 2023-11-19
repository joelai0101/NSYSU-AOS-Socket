#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFFER_SIZE 256

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];
    char username[10];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        close(sockfd);
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "ERROR connecting to the server\n");
        close(sockfd);
        exit(1);
    }
    puts("Connected!");

    // Receive and display initial message
    bzero(buffer, BUFFER_SIZE);
    n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n", buffer);

    // Prompt for username
    printf("Enter your username: ");
    bzero(username, sizeof(username));
    fgets(username, sizeof(username) - 1, stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove newline character

    // Send the username to the server
    n = send(sockfd, username, strlen(username), 0);
    if (n < 0)
        error("ERROR writing to socket");

    // Receive and display user group message
    bzero(buffer, BUFFER_SIZE);
    n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n", buffer);

    int i = 0;
    // Menu
    while(1) {

        bzero(buffer, BUFFER_SIZE);
        printf("%s can execute the following commands:\n", username);
        printf("1. create [filename] [access right]\n");
        printf("2. read [filename]\n");
        printf("3. write [filename] [o/a]\n");
        printf("4. changemode [filename] [access right]\n");
        printf("5. exit\n");        
        fgets(buffer, sizeof(buffer) - 1, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        
        send(sockfd, buffer, strlen(buffer), 0 );

        // 切 buffer to token
        char token[4][20] ;
        char *delim = " ";
        char *pch ;
        pch = strtok(buffer,delim);
        i = 0 ;
		while(pch != NULL) {
		    strcpy(token[i], pch);
            pch = strtok(NULL,delim);
			i++;
		}

        bzero(buffer, BUFFER_SIZE);

        // recv(buffer)
        recv(sockfd, buffer, 10, 0 );

        if (strcmp(buffer, "exit") == 0) {
            bzero(buffer, BUFFER_SIZE);
            break ;
        }

        else if (strcmp(buffer, "create") == 0) {
            bzero(buffer, BUFFER_SIZE);
            // recv(create_msg);
            recv(sockfd, buffer, 20, 0 );
            printf("%s\n", buffer);
   
        }    

        else if (strcmp(buffer, "read") == 0) {
            bzero(buffer, BUFFER_SIZE);
            // recv(read_msg);
            recv(sockfd, buffer, 40, 0 );
            printf("%s\n", buffer);

            if ( strcmp(buffer, "Permission accept") == 0 ) {

                char content[500] = {""} ;
                recv(sockfd, content, 500, 0 );
                FILE *fp = fopen(token[1], "w+"); 
                fprintf( fp, "%s", content );
                fclose(fp);
                printf("%s\n", content);

                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);
                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);

            }

        }

        else if (strcmp(buffer, "write") == 0) {

            bzero(buffer, BUFFER_SIZE);
            // recv(write_msg);
            recv(sockfd, buffer, 40, 0 );
            printf("%s\n", buffer);

            if (strcmp(buffer, "Permission accept") == 0 ) {

                char content[500] = {""} ;
                printf("Please enter the content:\n");
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0'; // Remove newline character
                send(sockfd, content, sizeof(content), 0);
   
                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);
                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);

            }
        }

        else if (strcmp(buffer, "changemode") == 0) {
            bzero(buffer, BUFFER_SIZE);
            // recv(changemode_msg) 
            // recv(changemode_msg)
            // 使用循環確保接收完整的消息
            int total_received = 0;
            int expected_length = 20;
            while (total_received < expected_length) {
                int received = recv(sockfd, buffer + total_received, expected_length - total_received, 0);
                if (received < 0) {
                    perror("Error receiving changemode message");
                    break;
                } else if (received == 0) {
                    // Connection closed
                    break;
                } else {
                    total_received += received;
                }
            }
            printf("%s\n", buffer);
        }

        else {
            bzero(buffer, BUFFER_SIZE);
            printf("Error!\n");
        }    


    }

    close(sockfd);

    return 0;
}
