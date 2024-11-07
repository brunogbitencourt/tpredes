#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Criação do socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha na criação do socket");
        exit(EXIT_FAILURE);
    }

    // Configurações do socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Falha na configuração do socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind do socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Falha no bind");
        exit(EXIT_FAILURE);
    }

    // Escuta por conexões
    if (listen(server_fd, 3) < 0) {
        perror("Falha na escuta");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escutando na porta 8080\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Falha ao aceitar conexão");
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, 1024);
    printf("Mensagem recebida: %s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Mensagem enviada: Hello from server\n");

    close(new_socket);
    close(server_fd);
    return 0;
}
