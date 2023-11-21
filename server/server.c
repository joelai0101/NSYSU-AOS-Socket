#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 256
#define MAX_FILES 100

typedef struct {
    char filename[50];
    char owner[50];
    int group;
    char accessright[10];
    char same_group_mem[10][10];
    char other_group_mem[10][10];
    int flag; // lock, 0: doing nothing, 1: reading, 2: writing

} File;

File file[MAX_FILES];
int file_count = 0;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void ShowCapabilityList() {

	int i, j, k;
	for(i = 0; i < file_count; i++) {
        printf("---------------\n");
		printf("file : %s \n", file[i].filename);
        printf("---------------\n");
        printf("Owner : \n");
        printf("---------------\n");
		printf("%s |", file[i].owner);

		for(j = 0; j < 2; j++)
			printf(" %c", file[i].accessright[j]);

		printf("\n");
        printf("---------------\n");
        printf("Group Member : \n");
        printf("---------------\n");

		for(k = 0; k < 2; k++) {
			printf("%s |", file[i].same_group_mem[k]);

			for(j = 2;j < 4; j++)
				printf(" %c", file[i].accessright[j]);

			printf("\n");
		}

        printf("---------------\n");
        printf("Other : \n");
        printf("---------------\n");

		for(k = 0; k < 3; k++) {
			printf("%s |", file[i].other_group_mem[k]);

			for(j = 4; j < 6; j++)
				printf(" %c", file[i].accessright[j]);	

			printf("\n");
		}
        printf("---------------\n");
	}

}

void *connection_handler( void *sockfd ) {

    int clientfd = *(int*) sockfd;

    // send(Enter an user)
    send( clientfd, "Enter an user a1, a2, a3, c1, c2, c3 : ", 50, 0 );
    char name[10];

    // recv(user name)
    int name_temp = recv( clientfd, name, 10, 0 );
    printf("%s enter\n", name);

    // send(user group)
    int group;
    if (strcmp(name, "a1") == 0) { send(clientfd, "a1 is in AOS-students group\n", 40, 0); group = 1; }
	else if (strcmp(name, "a2") == 0) { send(clientfd, "a2 is in AOS-students group\n", 40, 0); group = 1; }
	else if (strcmp(name, "a3") == 0) { send(clientfd, "a3 is in AOS-students group\n", 40, 0); group = 1; }
	else if (strcmp(name, "c1") == 0) { send(clientfd, "c1 is in CSE-students group\n", 40, 0); group = 2; }
	else if (strcmp(name, "c2") == 0) { send(clientfd, "c2 is in CSE-students group\n", 40, 0); group = 2; }
	else if (strcmp(name, "c3") == 0) { send(clientfd, "c3 is in CSE-students group\n", 40, 0); group = 2; }

    char buf[BUFFER_SIZE] = "";
    int i;

    while(1) {
        bzero(buf, BUFFER_SIZE); // init
        recv( clientfd, buf, sizeof(buf),0);
       
        // 切 cmd to token
        char token[4][20];
		char *delim = " ";
		char *pch;
		pch = strtok(buf, delim);
		i = 0;
		while(pch != NULL) {
		    strcpy(token[i], pch);		
			pch = strtok(NULL, delim);
			i++;
		}
        
        if (strcmp(token[0], "exit") == 0) {
            // send(cmd_exit) 
            send(clientfd, "exit", 10, 0);	
            printf("%s leave\n", name);
            break;
        }    

        else if (strcmp(token[0], "create") == 0) {
            // send(cmd_create) 
            send(clientfd, "create", 10, 0);	
            
            int exist = 0; 
            // check file is existed
            for( i = 0; i < file_count; i++ ) {
                if ( strcmp( token[1], file[i].filename) == 0 ) {
                    exist = 1;
                    break;
                }    
            } 

            if ( exist ) 
                // send(create_msg)
                send(clientfd, "file exist", 20, 0 );
            
            else {
                // create file
				FILE *fp = fopen(token[1], "w");

                strcpy(file[file_count].filename, token[1] );
                strcpy(file[file_count].accessright, token[2] );
                strcpy(file[file_count].owner, name );
                file[file_count].group = group;
                file[file_count].flag = 0;
                if (strcmp(name, "a1") == 0 ) {
                    strcpy(file[file_count].same_group_mem[0], "a2");
                    strcpy(file[file_count].same_group_mem[1], "a3");
                    strcpy(file[file_count].other_group_mem[0], "c1");
                    strcpy(file[file_count].other_group_mem[1], "c2");
                    strcpy(file[file_count].other_group_mem[2], "c3");
                }

                else if (strcmp(name, "a2") == 0 ) {
                    strcpy(file[file_count].same_group_mem[0], "a1");
                    strcpy(file[file_count].same_group_mem[1], "a3");
                    strcpy(file[file_count].other_group_mem[0], "c1");
                    strcpy(file[file_count].other_group_mem[1], "c2");
                    strcpy(file[file_count].other_group_mem[2], "c3");
                }

                else if (strcmp(name, "a3") == 0 ) {
                    strcpy(file[file_count].same_group_mem[0], "a2");
                    strcpy(file[file_count].same_group_mem[1], "a1");
                    strcpy(file[file_count].other_group_mem[0], "c1");
                    strcpy(file[file_count].other_group_mem[1], "c2");
                    strcpy(file[file_count].other_group_mem[2], "c3");
                }

                else if (strcmp(name, "c1") == 0 ) {
                    strcpy(file[file_count].same_group_mem[0], "c2");
                    strcpy(file[file_count].same_group_mem[1], "c3");
                    strcpy(file[file_count].other_group_mem[0], "a1");
                    strcpy(file[file_count].other_group_mem[1], "a2");
                    strcpy(file[file_count].other_group_mem[2], "a3");
                }

                else if (strcmp(name, "c2") == 0 ) {
                    strcpy(file[file_count].same_group_mem[0], "c1");
                    strcpy(file[file_count].same_group_mem[1], "c3");
                    strcpy(file[file_count].other_group_mem[0], "a1");
                    strcpy(file[file_count].other_group_mem[1], "a2");
                    strcpy(file[file_count].other_group_mem[2], "a3");
                }

                else if (strcmp(name, "c3") == 0 ) {
                    strcpy(file[file_count].same_group_mem[0], "c2");
                    strcpy(file[file_count].same_group_mem[1], "c1");
                    strcpy(file[file_count].other_group_mem[0], "a1");
                    strcpy(file[file_count].other_group_mem[1], "a2");
                    strcpy(file[file_count].other_group_mem[2], "a3");
                }

                fclose(fp);
                file_count++;
                // send(create_msg)
                send(clientfd, "create success", 20 , 0 );

            }

            ShowCapabilityList();

        }    

        else if (strcmp(token[0], "read") == 0) {
            // send(cmd_read) 
            send(clientfd, "read", 10, 0);

            int target = -1; 
            // check file is existed
            for( i = 0; i < file_count; i++ ) {
                if ( strcmp( token[1], file[i].filename) == 0 ) {
                    target = i;
                    break;
                }    
            } 

            if ( target == -1 ) 
                // send(create_msg)
                send(clientfd, "file not exist", 40, 0 );

            else {

                int permission = 0;

                // owner
                if ( strcmp(name, file[target].owner) == 0 ) {
                    if ( file[target].accessright[0] == 'r' )
                        permission = 1;    
                }

                // group member
                else if ( group == file[target].group ) {
                    if ( file[target].accessright[2] == 'r' )
                        permission = 1;
                }

                // other
                else {
                    if ( file[target].accessright[4] == 'r' )
                        permission = 1;
                }

                if ( permission == 0 )
                    send(clientfd, "Permission deny", 40, 0 );

                else {
                   
                    if ( file[target].flag == 0 || file[target].flag == 1 ) {
                        
                        send(clientfd, "Permission accept", 40, 0 );
                        
                        file[target].flag = 1;
                        bzero(buf, BUFFER_SIZE);
                        FILE *fp = fopen(token[1], "r");
                        char content[500];
                        bzero(content, 500);

                        while( fgets(buf, sizeof(buf), fp ) )
                            strcat(content,buf);

                        send(clientfd, content, 500, 0 );

                        send(clientfd, "Download the file", 40, 0 );
                        sleep(2);
                        send(clientfd, "Download complete!", 40, 0 );    
                        fclose(fp);	
                        file[target].flag = 0;

                    }

                    else
                       send(clientfd, "access denied, some one is writing", 40, 0 ); 
               
                }    

                ShowCapabilityList();
            }

        }

        else if (strcmp(token[0], "write") == 0) {
            // send(cmd_write) 
            send(clientfd, "write", 10, 0);

            int target = -1; 
            // check file is existed
            for( i = 0; i < file_count; i++ ) {
                if ( strcmp( token[1], file[i].filename) == 0 ) {
                    target = i;
                    break;
                }    
            } 

            if ( target == -1 ) 
                // send(create_msg)
                send(clientfd, "file not exist", 40, 0 );

            else {

                int permission = 0;

                // owner
                if ( strcmp(name, file[target].owner) == 0 ) {
                    if ( file[target].accessright[1] == 'w' )
                        permission = 1;    
                }

                // group member
                else if ( group == file[target].group ) {
                    if ( file[target].accessright[3] == 'w' )
                        permission = 1;
                }

                // other
                else {
                    if ( file[target].accessright[5] == 'w' )
                        permission = 1;
                }

                if ( permission == 0 )
                    send(clientfd, "Permission deny", 40, 0 );

                else {
                   
                    if ( file[target].flag == 0 ) {
                        
                        send(clientfd, "Permission accept", 40, 0 );  
                        file[target].flag = 2; // write
                        bzero(buf, BUFFER_SIZE);

                        char content[500];
                        bzero(content, 500);

                        if ( strcmp(token[2], "o") == 0 ) {
                            FILE *fp = fopen(token[1], "w");
                            int file_len = recv(clientfd, content, 500, 0 );
                            fprintf(fp, "%s", content );
                            fclose(fp);
                        } 

                        else {
                            FILE *fp = fopen(token[1], "a");
                            int file_len = recv(clientfd, content, 500, 0 );
                            fprintf(fp, "%s", content );
                            fclose(fp);
                        }    

                        
                        send(clientfd, "Upload the file", 40, 0 );
                        sleep(2);
                        send(clientfd, "Upload complete!", 40, 0 );    
                        file[target].flag = 0;

                    }

                    else
                       send(clientfd, "access denied, someone is reading or writing", 40, 0 ); 
               
                }    

                ShowCapabilityList();
            }

        }

        else if (strcmp(token[0], "changemode") == 0) {
            // send(cmd_changemode) 
            send(clientfd, "changemode", 10, 0);

            int target = -1; 
            // check file is existed
            for( i = 0; i < file_count; i++ ) {
                if ( strcmp( token[1], file[i].filename) == 0 ) {
                    target = i;
                    break;
                }    
            } 

            if ( target == -1 ) 
                // send(create_msg)
                send(clientfd, "file not exist", 20, 0 );

            else {
                
                strcpy( file[target].accessright, token[2] );
                send(clientfd, "changemode success", 20, 0 );
            }

            ShowCapabilityList();
        }

        else
            send(clientfd, "Error!", 20, 0 );
    }


    free(sockfd);

    return 0;
}

int main(int argc, char *argv[]) {
    int sockfd, clientfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &serv_addr, sizeof(serv_addr));
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
        return 1;
    }
    puts( "Bind done\n");

    // Listen
    listen(sockfd, MAX_CLIENTS);
    puts( "Waiting for connections\n" );
    clilen = sizeof(cli_addr);

    while(1) {
        clientfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (clientfd < 0) 
            error("ERROR on accept");
        puts( "Connection accepted\n" );

        pthread_t sniffer_thread;
        int *new_sock;
        new_sock = malloc(1);
        *new_sock = clientfd;
         
        if ( pthread_create( &sniffer_thread , NULL,  connection_handler , (void*) new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
         
        // Now join the thread , so that we dont terminate before the thread
        // pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    return 0; 
}
