#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>  // Para intptr_t

#define SOCKET_OPTION SO_REUSEADDR  // Definição do tipo de socket (SO_REUSEADDR para compatibilidade)

void start_server(void);
void *handle_sender(void *arg);

int main() {
    start_server();  // Chamada da função start_server
    return 0;
}

void start_server(void) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Criação do socket para o servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha na criação do socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do socket
    if (setsockopt(server_fd, SOL_SOCKET, SOCKET_OPTION, &opt, sizeof(opt)) != 0) {
        perror("Falha na configuração do socket");
        exit(EXIT_FAILURE);
    }

    // Configurações do endereço
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind do socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Falha no bind");
        exit(EXIT_FAILURE);
    }

    // Coloca o servidor em modo de escuta
    if (listen(server_fd, 3) < 0) {
        perror("Falha na escuta");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escutando na porta 8080\n");

    // Loop para aceitar várias conexões
    while (1) {
        // Aceita conexão do sender
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Falha ao aceitar conexão");
            exit(EXIT_FAILURE);
        }

        printf("Conexão recebida do sender.\n");

        // Cria uma nova thread para lidar com o sender
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_sender, (void *)(intptr_t)new_socket) != 0) {
            perror("Falha ao criar a thread");
            close(new_socket);
        }
        pthread_detach(thread_id);  // Desanexa a thread para não precisar esperar o término
    }

    close(server_fd);
}

void *handle_sender(void *arg) {
    int new_socket = (intptr_t)arg;  // Converte o argumento de volta para o socket
    char buffer[1024] = {0};
    int receiver_socket;  // Socket para o receiver
    struct sockaddr_in receiver_address;

    // Conectar ao receiver
    receiver_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (receiver_socket < 0) {
        perror("Falha ao criar socket para conectar ao receiver");
        close(new_socket);
        pthread_exit(NULL);
    }

    receiver_address.sin_family = AF_INET;
    receiver_address.sin_port = htons(8081);  // Porta do receiver
    receiver_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // IP do receiver

    if (connect(receiver_socket, (struct sockaddr *)&receiver_address, sizeof(receiver_address)) < 0) {
        perror("Falha ao conectar ao receiver");
        close(new_socket);
        close(receiver_socket);
        pthread_exit(NULL);
    }

    // Recebe a mensagem do sender
    int valread = read(new_socket, buffer, 1024);
    if (valread <= 0) {
        perror("Falha ao ler mensagem");
        close(new_socket);
        close(receiver_socket);
        pthread_exit(NULL);
    }
    buffer[valread] = '\0';  // Garante que o buffer é null-terminated
    printf("Mensagem recebida do sender: %s\n", buffer);

    // Envia a mensagem para o receiver
    send(receiver_socket, buffer, strlen(buffer), 0);
    printf("Mensagem encaminhada para o receiver: %s\n", buffer);

    // Fecha os sockets
    close(new_socket);
    close(receiver_socket);

    pthread_exit(NULL);  // Finaliza a thread
}
