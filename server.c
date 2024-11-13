#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <bits/sigaction.h>
#include <fcntl.h>  
#include <asm-generic/signal-defs.h>



#define MAX_CLIENTS 20 
#define PORT 8080
#define MAX_TEXT_LEN 141
#define PERIODIC_INTERVAL 5  // Intervalo em milissegundos para envio periódico

int client_uids[MAX_CLIENTS];

int client_fds[MAX_CLIENTS];

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[MAX_TEXT_LEN];
} msg_t;

// void send_periodic_message(fd_set *clients, int *client_fds);
void handle_message(int client_fd, int *client_fds, fd_set *clients);
void timer_handler(int signum);
void setup_timer(int milliseconds);


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);


    fd_set clients;  // Conjunto de sockets de clientes
    // int client_fds[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
        client_uids[i] = -1; 
    }

    // Configuração do socket do servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("SERVIDOR......: Falha na criação do socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("SERVIDOR......: Erro no bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("SERVIDOR......: Erro no listen");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&clients);
    FD_SET(server_fd, &clients);

    setup_timer(PERIODIC_INTERVAL);

    while (1) {
        fd_set read_fds = clients;

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        

        int activity = select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);

        if (activity < 0) {
            if (errno == EINTR) {
                continue; // Ignora a interrupção do sinal e continua
            }
            perror("SERVIDOR......: Erro no select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                if (errno == EINTR) {
                    continue; // Ignora a interrupção do sinal e continua
                }
                perror("SERVIDOR......: Erro ao aceitar conexão");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == -1) {
                    client_fds[i] = new_socket;
                    client_uids[i] = -1;
                    FD_SET(new_socket, &clients);
                    break;
                }
            }
        }


        // Loop para verificar atividade dos clientes conectados
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int client_fd = client_fds[i];
            // Verifique se o descritor ainda é válido
            if (client_fd != -1 && FD_ISSET(client_fd, &read_fds)) {
                // Se o descritor for inválido, remova-o de clients
                if (fcntl(client_fd, F_GETFD) == -1) {
                    //printf("Removendo descritor inválido %d do conjunto clients.\n", client_fd);
                    FD_CLR(client_fd, &clients);
                    client_fds[i] = -1;
                    client_uids[i] = -1;
                    continue;
                }
                // Trata a mensagem recebida de acordo com o envio
                handle_message(client_fd, client_fds, &clients);
            }
        }
    }  
    return 0;
}

void timer_handler(int signum) {
    printf("SERVIDOR......: Enviando mensagem periódica para todos os clientes.\n");

    msg_t periodic_msg;
    periodic_msg.type = 2; // tipo de mensagem periódica
    periodic_msg.orig_uid = 0;
    periodic_msg.dest_uid = 0;
    snprintf((char*)periodic_msg.text, MAX_TEXT_LEN, "Mensagem periódica do servidor");

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int client_fd = client_fds[i];
        if (client_fd != -1) {
            send(client_fd, &periodic_msg, sizeof(periodic_msg), 0);
        }
    }
}

void setup_timer(int seconds) {
    struct sigaction sa;
    struct itimerval timer;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGALRM, &sa, NULL);

    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = seconds;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}

void handle_message(int client_fd, int *client_fds, fd_set *clients) {
    msg_t message;
    int bytes_received = recv(client_fd, &message, sizeof(message), 0);

    if (bytes_received <= 0) {
        close(client_fd);
        FD_CLR(client_fd, clients);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] == client_fd) {
                client_fds[i] = -1;
                client_uids[i] = -1;
                break;
            }
        }
        return;
    }

    switch (message.type) {
        case 0:
            //printf("Cliente %d identificado com ID %d \n", client_fd, message.orig_uid);
            printf("SERVIDOR......: Cliente %d diz OI.\n", message.orig_uid);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == client_fd) {
                    client_uids[i] = message.orig_uid;  // Salva o UID do cliente
                    break;
                }
            }
            send(client_fd, &message, sizeof(message), 0);
            break;
        case 1:
            close(client_fd);
            FD_CLR(client_fd, clients);  // Certifique-se de remover de `clients`
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == client_fd) {
                    client_fds[i] = -1;
                    client_uids[i] = -1;
                    break;
                }
            }
            printf("SERVIDOR......: Cliente %d diz TCHAU.\n", message.orig_uid);
            //printf("Cliente %d removido com sucesso.\n", client_fd);
            break;
        case 2:
            if (message.dest_uid == 0) {
                // Mensagem pública: enviada para todos os clientes, exceto o remetente
                //printf("Mensagem pública de %d: %s\n", message.orig_uid, message.text);
                printf("SERVIDOR......: Cliente %d publica para todos.\n", message.orig_uid);

                for (int i = 0; i < MAX_CLIENTS; i++) {
                    int dest_fd = client_fds[i];
                    if (dest_fd != -1 && dest_fd != client_fd) {
                        send(dest_fd, &message, sizeof(message), 0);
                    }
                }
            } else {
                // Mensagem privada: enviada apenas para o cliente com dest_uid correspondente
                //printf("Mensagem privada de %d para %d: %s\n", message.orig_uid, message.dest_uid, message.text);
                printf("SERVIDOR......: Cliente %d publica para %d : %s.\n", message.orig_uid, message.dest_uid, message.text);
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] != -1 && client_uids[i] == message.dest_uid) {
                        send(client_fds[i], &message, sizeof(message), 0);
                        break; // Envia apenas para o cliente específico
                    }
                }
            }
            break;
        default:
            printf("SERVIDOR......: Tipo de mensagem desconhecido.\n");
            break;
    }
}