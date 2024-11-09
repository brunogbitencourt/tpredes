#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>

void start_server();

int main() {
    start_server();
    return 0;
}

void start_server() {
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

    // Loop principal para aceitar conexões e processar mensagens
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Falha ao aceitar conexão");
            exit(EXIT_FAILURE);
        }

        int valread = read(new_socket, buffer, 1024);
        buffer[valread] = '\0';  // Garante que o buffer termina em null

        printf("Mensagem recebida: %s\n", buffer);

        // Verifica se a mensagem é "exit"
        if (strcmp(buffer, "exit_server") == 0) {
            printf("Encerrando o servidor.\n");
            send(new_socket, "Servidor encerrado", strlen("Servidor encerrado"), 0);
            printf("\n");  // Quebra de linha após o send
            close(new_socket);
            break;  // Sai do loop para encerrar o servidor
        }

        // Responde com uma mensagem padrão se não for "exit"
        send(new_socket, hello, strlen(hello), 0);
        printf("\n");  // Quebra de linha após o send

        close(new_socket);  // Fecha o socket para esta conexão
    }

    // Fecha o socket do servidor
    close(server_fd);
}
