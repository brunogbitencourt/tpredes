#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void start_receiver();  // Função para iniciar o receiver

int main() {
    start_receiver();  // Inicia o receiver para escutar conexões
    return 0;
}

void start_receiver() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024] = {0};

    // Criação do socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);  // Porta do receiver (diferente da do servidor)
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Aceitar conexões de qualquer IP

    // Vincula o socket à porta
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Falha ao associar o socket");
        exit(EXIT_FAILURE);
    }

    // Coloca o servidor para escutar
    if (listen(server_sock, 3) < 0) {
        perror("Falha ao colocar o servidor em escuta");
        exit(EXIT_FAILURE);
    }

    printf("Receiver escutando na porta 8081\n");

    // Aceita conexões do servidor
    while (1) {
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Falha ao aceitar a conexão");
            exit(EXIT_FAILURE);
        }

        // Recebe dados do servidor
        int valread = read(client_sock, buffer, sizeof(buffer) - 1);
        if (valread > 0) {
            buffer[valread] = '\0';  // Garante que o buffer termine com null
            printf("Mensagem recebida do servidor: %s\n", buffer);
        }

        // Fecha o socket
        close(client_sock);
    }

    close(server_sock);
}
