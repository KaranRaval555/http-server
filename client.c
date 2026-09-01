#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8088

int main(int argc, char *argv[]) {

    char *address = argv[1];
    char request[] = "GET / HTTP/1.0\r\nConnection: close\r\n\r\n";
    char response[4096];

    int client_sock;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in remote_address;
    remote_address.sin_port = htons(PORT);
    remote_address.sin_family = AF_INET;

    inet_aton(address, &remote_address.sin_addr);

    int status = connect(client_sock, (struct sockaddr*) &remote_address, sizeof(remote_address));

    if (status == -1) {
        printf("Failed to connect on port: %d\n", PORT);
        return 1;
    }
    printf("Succesfully connected on port %d\n",PORT);

    if (send(client_sock, request, strlen(request), 0) == -1){
        printf("Request failed");
    }

    int n = recv(client_sock, response, sizeof(response) - 1, 0);
    if(n == -1){
        printf("Failed to receive response\n");
        return 1;
    }

    printf("Response sent by the server:\n\n%s",response);

    close(client_sock);

    return 0;
}
