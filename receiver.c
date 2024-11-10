#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define PORT 8080
#define MAX_TEXT_LEN 141

int sock;
int client_id;

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[MAX_TEXT_LEN];
} msg_t;

void handle_message(msg_t message) {
    // Exibe a origem e o conteúdo da mensagem recebida
    if (message.dest_uid == 0) {
        printf("RECEIVER......: Mensagem pública de %d: %s\n", message.orig_uid, message.text);
    } else {
        printf("RECEIVER......: Mensagem privada de %d para %d: %s\n", message.orig_uid, message.dest_uid, message.text);
    }
}

void send_exit_message(int sock, int client_id) {
    msg_t message;
    message.type = 1;  // Tipo 1 representa "tchau"
    message.orig_uid = client_id;  // ID do cliente
    message.dest_uid = 0;

    //printf("Enviando mensagem de saída (tchau) do cliente %d para o servidor.\n", client_id);
    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("RECEIVER......: Erro ao enviar mensagem de saída");
    } else {
        printf("RECEIVER......: Enviando TCHAU ao servidor.\n");
    }
}


void sigint_handler(int signum) {
    printf("RECEIVER......: Encerrando o sender...\n");        
        
    send_exit_message(sock, client_id);
    sleep(5);  
    close(sock);  
    exit(0); 
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "RECEIVER......: Uso: %s <identificador> <endereço IP do servidor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int receiver_id = atoi(argv[1]);
    client_id = receiver_id;
    char *server_ip = argv[2];
    struct sockaddr_in serv_addr;
    msg_t message;

    

    // Validação do ID do cliente
    if (receiver_id < 1 || receiver_id > 999) {
        fprintf(stderr, "RECEIVER......: Erro: O identificador deve estar entre 1 e 999.\n");
        exit(EXIT_FAILURE);
    }

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("RECEIVER......: Erro ao criar o socket");
        return -1;
    }

    signal(SIGINT, sigint_handler);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("RECEIVER......: Endereço inválido");
        return -1;
    }

    // Conexão ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("RECEIVER......: Falha na conexão");
        return -1;
    }

    // Envia mensagem de identificação "OI"
    message.type = 0;  // Tipo 0 representa "OI"
    message.orig_uid = receiver_id;  // Identificador único do cliente
    message.dest_uid = 0;  // Destinatário 0 para indicar que é para todos
    strcpy((char*)message.text, "Cliente de recepção conectado.");
    message.text_len = strlen((char*)message.text) + 1;

    if (send(sock, &message, sizeof(message), 0) < 0) {
        perror("RECEIVER......: Erro ao enviar mensagem de identificação");
        close(sock);
        return -1;
    }

    // Aguarda confirmação do servidor
    if (recv(sock, &message, sizeof(message), 0) > 0 && message.type == 0) {
        printf("RECEIVER......: Conectado ao servidor com sucesso! ID: %d.\n", receiver_id);
    } else {
        printf("RECEIVER......: Falha ao receber confirmação do servidor. Encerrando conexão.\n");
        close(sock);
        return -1;
    }

    // Loop para receber mensagens continuamente
    while (1) {
        //printf("Aguardando nova mensagem do servidor...\n");
        int bytes_received = recv(sock, &message, sizeof(message), 0);

        if (bytes_received > 0) {
            //printf("Mensagem recebida, processando...\n");
            handle_message(message);
        } else if (bytes_received == 0) {
            printf("Conexão fechada pelo servidor.\n");
            break;
        } else {
            if (errno == EINTR) continue;  // Ignora interrupções por sinais
            perror("RECEIVER......: Erro ao receber mensagem");
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
        perror("RECEIVER......: Erro ao enviar mensagem de saída");
    }

    close(sock);
    return 0;
}
