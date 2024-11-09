#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void start_sender(const char *message, const char *sender_ip);  // Função alterada para aceitar sender_ip

int main() {
    char message[1024];
    const char *sender_ip = "127.0.0.1"; // Endereço IP do sender (alterado para localhost)

    printf("IP do sender: %s\n", sender_ip);  // Exibe o IP do sender

    while (1) {
        printf("Digite uma mensagem para enviar ao servidor (ou 'exit_sender' para sair): ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0; // Remove a quebra de linha

        // Verifica se a mensagem é "exit_sender" para encerrar o loop
        if (strcmp(message, "exit_sender") == 0) {
            printf("Encerrando o sender...\n");
            break;
        }

        // Envia a mensagem para o servidor
        start_sender(message, sender_ip);  // Passa o IP do sender
    }

    return 0;
}

void start_sender(const char *message, const char *sender_ip) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Falha na criação do socket \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Converte o endereço IP do sender para o formato binário
    if (inet_pton(AF_INET, sender_ip, &serv_addr.sin_addr) <= 0) {
        printf("\n Endereço inválido ou não suportado \n");
        close(sock);
        return;
    }

    // Conecta ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Conexão falhou \n");
        close(sock);
        return;
    }

    // Envia a mensagem
    send(sock, message, strlen(message), 0);
    printf("Mensagem enviada: %s\n", message);

    // Lê a resposta do servidor
    int valread = read(sock, buffer, sizeof(buffer) - 1);
    if (valread > 0) {
        buffer[valread] = '\0';  // Garante que o buffer termina em null
        printf("Mensagem recebida do servidor: %s\n", buffer);
    }

    // Fecha o socket
    close(sock);
}
