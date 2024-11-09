#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[141];
} msg_t;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    msg_t message;

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Erro ao criar o socket\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Conexão com o servidor no localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Endereço inválido\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Falha na conexão\n");
        return -1;
    }

    // Envia mensagem "OI" para o servidor
    message.type = 0; // Tipo 0 representa "OI"
    message.orig_uid = 1001; // Identificador único do cliente
    message.dest_uid = 0; // 0 para enviar a todos
    strcpy((char*)message.text, "Olá do cliente de envio!");
    message.text_len = strlen((char*)message.text) + 1;

    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("Erro ao enviar mensagem");
    }

    // Aguarda resposta do servidor
    if (recv(sock, &message, sizeof(message), 0) > 0) {
        printf("Mensagem recebida do servidor: %s\n", message.text);
    }

    close(sock);
    return 0;
}
