#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[141];
} msg_t;

void handle_message(msg_t message) {
    // Exibe a origem e o conteúdo da mensagem recebida
    if (message.dest_uid == 0) {
        printf("Mensagem pública de %d: %s\n", message.orig_uid, message.text);
    } else {
        printf("Mensagem privada de %d para %d: %s\n", message.orig_uid, message.dest_uid, message.text);
    }
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <identificador> <endereço IP do servidor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int receiver_id = atoi(argv[1]);
    char *server_ip = argv[2];
    int sock;
    struct sockaddr_in serv_addr;
    msg_t message;

     // Validação do ID do cliente
    if (receiver_id < 1 || receiver_id > 999) {
        fprintf(stderr, "Erro: O identificador deve estar entre 1 e 999.\n");
        exit(EXIT_FAILURE);
    }

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar o socket");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Endereço inválido");
        return -1;
    }

    // Conexão ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Falha na conexão");
        return -1;
    }

    message.type = 0;  // Tipo 0 representa "OI"
    message.orig_uid = receiver_id;  // Identificador único do cliente
    message.dest_uid = 0;  // Destinatário 0 para indicar que é para todos
    strcpy((char*)message.text, "Cliente de recepção conectado.");
    message.text_len = strlen((char*)message.text) + 1;

    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("Erro ao enviar mensagem de identificação");
        close(sock);
        return -1;
    }

    // Aguarda mensagem do Servidor
    if (recv(sock, &message, sizeof(message), 0) > 0 && message.type == 0) {
        printf("Conectado ao servidor com sucesso! ID: %d.\n", receiver_id);
    } else {
        printf("Falha ao receber confirmação do servidor. Encerrando conexão.\n");
        close(sock);
        return -1;
    }



    while (1) {
        int bytes_received = recv(sock, &message, sizeof(message), 0);
        
        if (bytes_received > 0) {
            handle_message(message);
        } else if (bytes_received == 0) {
            printf("Conexão fechada pelo servidor.\n");
            break;
        } else {
            if (errno == EINTR) continue;
            perror("Erro ao receber mensagem");
            break;
        }
    }
    // Envia mensagem "TCHAU" ao desconectar
    message.type = 1;  // Tipo 1 representa "TCHAU"
    message.orig_uid = receiver_id;
    message.dest_uid = 0;
    strcpy((char*)message.text, "Cliente se desconectando.");
    message.text_len = strlen((char*)message.text) + 1;

    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("Erro ao enviar mensagem de saída");
    }

    close(sock);
    return 0;
}
