#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8088

int main() {

    FILE *input_file;
    input_file = fopen("index.html", "r");

    if (input_file == NULL) {
        printf("Failed to open index.html\n");
        return 1;
    }

    char response[8192] = "";
    char line[1024];

    while (fgets(line, sizeof(line), input_file)) {
        strcat(response, line);
    }

    fclose(input_file);

    char http_response[2048];

    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        strlen(response));
        strcat(http_response,response);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock == -1) {
        printf("Failed to create socket\n");
        return 1;
    }

    printf("Socket created successfully\n");

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY); // any ip address on local machine

    if (bind(server_sock, (struct sockaddr *)&host_addr, host_addrlen) == -1) {
        printf("Failed to bind socket\n");
        return 1;
    }
    printf("Socket successfully bound to address\n");

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        printf("Listen failed\n");
        return 1;
    }
    printf("Server listening for connections on port %d\n",PORT);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    while(1) {
        // Accept incoming connections
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addrlen);

        if (client_sock == -1) {
            printf("Failed to accept\n");
            continue;
        }
        printf("connection accepted\n");

        if (send(client_sock, http_response, strlen(http_response), 0) == -1) {
            printf("Failed to send data\n");
        }
        printf("Data sent successfully\n");

        close(client_sock);
    }

    return 0;
}
