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
    int sockfd, portno, n; // 定義 socket 文件描述符，端口號和讀/寫操作的返回值
    struct sockaddr_in serv_addr; // 定義伺服端位址結構
    struct hostent *server;  // 定義伺服器主機資訊

    char buffer[BUFFER_SIZE]; // 定義緩衝區
    char username[10]; // 定義使用者名稱

    if (argc < 3) { // 檢查 command line 參數數量
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]); // 從 command line 參數獲取端口號
    /**
     * socket(domain, type, protocol)
     * AF_INET：這是一個地址族，表示我們要使用的是 IPv4 網際網路協議。
     * SOCK_STREAM：這是一個 socket 類型，表示我們要使用的是 TCP 協議，
     * 該協議提供了一種可靠的、面向連接的服務。
     * 0：這是一個協議號，通常設置為 0，表示我們要使用的是默認的協議（在這裡默認的協議是 TCP）。
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // 創建 socket
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]); // 從 command line 參數獲取伺服器主機名
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        close(sockfd);
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr)); // 初始化伺服器位址結構
    serv_addr.sin_family = AF_INET; // 設置地址類型為 IPv4 網際網路協議。
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length); // 複製伺服器 IP 地址
    serv_addr.sin_port = htons(portno); // 設置端口號

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { // 連接到伺服器
        fprintf(stderr, "ERROR connecting to the server\n");
        close(sockfd);
        exit(1);
    }
    puts("Connected!");

    // Receive and display initial message
    bzero(buffer, BUFFER_SIZE);
    n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0); // 從伺服器接收消息
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n", buffer);

    // Prompt for username
    printf("Enter your username: ");
    bzero(username, sizeof(username)); // 清空使用者名稱
    fgets(username, sizeof(username) - 1, stdin); // 從標準輸入讀取使用者名稱
    username[strcspn(username, "\n")] = '\0'; // Remove newline character 

    // Send the username to the server
    n = send(sockfd, username, strlen(username), 0); // 將使用者名稱發送到伺服器
    if (n < 0)
        error("ERROR writing to socket");

    // Receive and display user group message
    bzero(buffer, BUFFER_SIZE);
    n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0); // 從伺服器接收消息
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n", buffer);

    int i = 0;
    // Menu
    while(1) {

        // Check if the user is unknown
        if (strcmp(buffer, "Unknown user\n") == 0) {
            puts("Please enter a valid username: a1, a2, a3, c1, c2, c3\n");
            bzero(buffer, BUFFER_SIZE);
            break;
        }

        bzero(buffer, BUFFER_SIZE);
        printf("%s can execute the following commands:\n", username); // 顯示使用者可以執行的命令
        printf("1. create [filename] [access right]\n");
        printf("2. read [filename]\n");
        printf("3. write [filename] [o/a]\n");
        printf("4. changemode [filename] [access right]\n");
        printf("5. exit\n");        
        fgets(buffer, sizeof(buffer) - 1, stdin); // 從標準輸入讀取命令
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        
        send(sockfd, buffer, strlen(buffer), 0 ); // 將命令發送到伺服器
        
        // 切 buffer to token
        char token[4][20]; // 存儲從 buffer 字串中分隔出來的命令和參數。
        char *delim = " "; // 分隔符
        char *pch; // 指向當前分隔的字串的指針
        pch = strtok(buffer, delim); // 將 buffer 字串按照 delim 字串（空格）進行分隔。
        i = 0; // token[i] 則代表第 i 個分隔出來的字串。
		while (pch != NULL) { // 繼續分隔直到沒有更多的參數
		    strcpy(token[i], pch); // 將當前參數複製到陣列中
            pch = strtok(NULL, delim); // 獲取下一個參數
			i++;
		}
        /**
         * 如果 buffer 中的內容是 "create file1 rwr---"，
         * 則 strtok 會首先返回 "create"，然後返回 "file1"，最後返回 "rwr---"。
         * 這些返回的字串會被複製到 token[0]、token[1] 和 token[2] 中。
         */

        bzero(buffer, BUFFER_SIZE);
        // recv(buffer)
        recv(sockfd, buffer, 10, 0 ); // 從伺服器接收響應

        // 非可執行指令的情況
        if (strcmp(token[0], "create") != 0 && strcmp(token[0], "read") != 0 && 
            strcmp(token[0], "write") != 0 && strcmp(token[0], "changemode") != 0 &&
            strcmp(token[0], "exit") != 0) {
            puts("Invalid command\n");
            bzero(buffer, BUFFER_SIZE);
            continue;  // 跳過這次迴圈的剩餘部分，並開始下一次迴圈
        }

        if (strcmp(token[0], "exit") == 0 ) { // 如果指令是 "exit"，則退出循環
            bzero(buffer, BUFFER_SIZE);
            break ;
        }

        else if (strcmp(token[0], "create") == 0 ) { // 如果指令是 "create"，則處理創建文件的響應
            bzero(buffer, BUFFER_SIZE);
            // recv(create_msg);
            recv(sockfd, buffer, 20, 0 ); // 從伺服器接收創建文件響應
            printf("%s\n", buffer);
            bzero(buffer, BUFFER_SIZE);
        }    

        else if (strcmp(token[0], "read") == 0 ) { // 如果指令是 "read"，則處理讀取文件的響應
            bzero(buffer, BUFFER_SIZE);
            // recv(read_msg);
            recv(sockfd, buffer, 40, 0 ); // 從伺服器接收讀取文件響應
            printf("%s\n", buffer);

            if ( strcmp(buffer, "Permission accept") == 0 ) { // 如果伺服器接受讀取請求，則接收文件內容並保存到本地

                char content[500] = {""} ;
                recv(sockfd, content, 500, 0 ); // 從伺服器接收文件內容
                FILE *fp = fopen(token[1], "w+"); // 打開本地文件
                fprintf( fp, "%s", content ); // 將文件內容寫入本地文件
                fclose(fp); // 關閉本地文件
                printf("%s\n", content); // 顯示文件內容

                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);
                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);

            }
            bzero(buffer, BUFFER_SIZE);

        }

        else if (strcmp(token[0], "write") == 0 ) { // 如果指令是 "write"，則處理寫入文件的響應
            bzero(buffer, BUFFER_SIZE);
            // recv(write_msg);
            recv(sockfd, buffer, 40, 0 ); // 從伺服器接收寫入文件響應
            printf("%s\n", buffer);

            if (strcmp(buffer, "Permission accept") == 0 ) { // 如果伺服器接受寫入請求，則讀取使用者輸入的內容並發送到伺服器

                char content[500] = {""} ;
                printf("Please enter the content:\n");
                fgets(content, sizeof(content), stdin); // 從標準輸入讀取內容
                content[strcspn(content, "\n")] = '\0'; // Remove newline character
                send(sockfd, content, sizeof(content), 0); // 將內容發送到伺服器
   
                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);
                recv(sockfd, buffer, 40, 0 );
                printf("%s\n", buffer);

            }

            bzero(buffer, BUFFER_SIZE);
        }

        else if (strcmp(token[0], "changemode") == 0 ) { // 如果指令是 "changemode"，則處理更改文件模式的響應
            bzero(buffer, BUFFER_SIZE);
            // recv(changemode_msg) 
            // recv(changemode_msg)
            // 使用循環確保接收完整的消息
            int total_received = 0; // 已接收的字節數
            int expected_length = 20; // 預期的訊息長度
            while (total_received < expected_length) { // 當已接收的字節數小於預期的訊息長度時，繼續接收
                /**
                 * sockfd：這是一個文件描述符，表示要從中接收數據的 socket。
                 * buffer + total_received：這是一個指向緩衝區的指針，用於存儲接收到的數據。buffer + total_received 表示緩衝區的當前位置，即已接收的數據之後的位置。
                 * expected_length - total_received：這是一個大小參數，表示要接收的最大字節數。expected_length - total_received 表示還需要接收的字節數。
                 * 0：這是一個標誌參數，用於控制接收操作的行為。在這裡，它被設置為 0，表示使用默認的行為。
                 */
                int received = recv(sockfd, buffer + total_received, expected_length - total_received, 0);
                if (received < 0) { // 如果接收失敗，則輸出錯誤訊息並退出循環
                    perror("Error receiving changemode message");
                    break;
                } else if (received == 0) { // 如果接收到的字節數為 0，則表示連接已關閉，退出循環
                    // Connection closed
                    break;
                } else { // 如果成功接收到訊息，則將已接收的字節數加上剛接收到的字節數
                    total_received += received;
                }
            }
            printf("%s\n", buffer);
            bzero(buffer, BUFFER_SIZE);
        }

        else {
            
            printf("Error!\n");
            bzero(buffer, BUFFER_SIZE);
            
        }    

    }

    close(sockfd); // 關閉 socket

    return 0;
}
