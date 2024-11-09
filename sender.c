#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 9254
#define MAX_TEXT_LEN 141

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[MAX_TEXT_LEN];
} msg_t;

void send_message(int sock, int sender_id);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <identificador> <endereço IP do servidor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sender_id = atoi(argv[1]);
    char *server_ip = argv[2];
    int sock;
    struct sockaddr_in serv_addr;
    msg_t message;

    if (sender_id < 1001 || sender_id > 1999) {
        fprintf(stderr, "Erro: O identificador deve estar entre 1001 e 1999.\n");
        exit(EXIT_FAILURE);
    }

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

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Falha na conexão");
        return -1;
    }

    // Envia uma mensagem "OI" para se registrar no servidor
    message.type = 0;  // Tipo 0 representa "OI"
    message.orig_uid = sender_id;
    message.dest_uid = 0;
    strcpy((char*)message.text, "Cliente de envio conectado.");
    message.text_len = strlen((char*)message.text) + 1;

    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("Erro ao enviar mensagem de identificação");
        close(sock);
        return -1;
    }

    // Aguarda a confirmação "OI" do servidor
    if (recv(sock, &message, sizeof(message), 0) > 0 && message.type == 0) {
        printf("Conectado ao servidor como sender com ID %d.\n", sender_id);
    } else {
        printf("Falha ao receber confirmação do servidor. Encerrando conexão.\n");
        close(sock);
        return -1;
    }

    // Entra no loop de envio de mensagens
    send_message(sock, sender_id);

    // Envia a mensagem "TCHAU" ao desconectar
    message.type = 1;  // Tipo 1 representa "TCHAU"
    message.orig_uid = sender_id;
    message.dest_uid = 0;
    strcpy((char*)message.text, "Cliente de envio se desconectando.");
    message.text_len = strlen((char*)message.text) + 1;

    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("Erro ao enviar mensagem de saída");
    }

    close(sock);
    return 0;
}

// Função para enviar mensagens ao servidor
void send_message(int sock, int sender_id) {
    msg_t message;
    message.type = 2;  // Tipo 2 representa "MSG"
    message.orig_uid = sender_id;

    while (1) {
        printf("Digite o ID do destinatário (0 para todos): ");
        int dest_id;
        scanf("%d", &dest_id);
        getchar();  // Remove o caractere de nova linha do buffer
        message.dest_uid = dest_id;

        printf("Digite sua mensagem (máximo %d caracteres): ", MAX_TEXT_LEN - 1);
        fgets((char*)message.text, MAX_TEXT_LEN, stdin);

        // Remove o '\n' da mensagem, se presente
        message.text_len = strlen((char*)message.text);
        if (message.text[message.text_len - 1] == '\n') {
            message.text[message.text_len - 1] = '\0';
            message.text_len--;
        }

        message.text_len++;  // Inclui o caractere nulo

        // Envia a mensagem para o servidor
        if (send(sock, &message, sizeof(message), 0) < 0) {
            perror("Erro ao enviar mensagem");
            break;
        }

        printf("Mensagem enviada para %d: %s\n", dest_id, message.text);
    }
}
