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
#include <asm-generic/signal-defs.h>



#define MAX_CLIENTS 20 // Limite de 10 clientes por tipo (exibição e envio)
#define PORT 8080
#define MAX_TEXT_LEN 141

volatile sig_atomic_t timer_expired = 0;

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[MAX_TEXT_LEN];
} msg_t;

void setup_timer();
void send_periodic_message(fd_set *clients, int *client_fds);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    setup_timer();

    fd_set clients; // Estrutura de dados utilizada para represntar um conunto de sockets
    int client_fds[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }
    
    // AF_INET -> IPv4, SOCK_STREAM -> TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha na criação do socket");
        exit(EXIT_FAILURE);
    }
   
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // I Indica qualquer endereco IP
    address.sin_port = htons(PORT);

    // Associação do socket a porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Erro no bind");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_CLIENTS) <  0){
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }


    FD_ZERO(&clients); // Inicializa estrutura de clientes
    FD_SET(server_fd, &clients);


    while(1){

        fd_set read_fds = clients; // Mantem um registro dos clientes conectados aos sockets

        int activity =  select(FD_SETSIZE, &read_fds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("Erro no select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Erro ao aceitar conexão");
                exit(EXIT_FAILURE);
            }

            // Adiciona o novo cliente ao array client_fds
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == -1) {
                    client_fds[i] = new_socket;
                    FD_SET(new_socket, &clients);
                    printf("Cliente conectado no socket %d\n", new_socket);
                    break;
                }
            }
        }

        if (timer_expired) {
            send_periodic_message(&clients, client_fds);
            timer_expired = 0;
        }


        // Loop para verificar atividade dos clientes conectados
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int client_fd = client_fds[i];
            if (client_fd != -1 && FD_ISSET(client_fd, &read_fds)) {
                // Aqui você pode ler dados do cliente ou processar a conexão
            }
        }


    }
   
   
   
   return 0;
}

void handle_timer(int signum) {
    timer_expired = 1;
}


void setup_timer() {
    struct sigaction sa;
    struct itimerval timer;

    sa.sa_handler = &handle_timer;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);

    timer.it_value.tv_sec = 60;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 60;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}

void send_periodic_message(fd_set *clients, int *client_fds) {
    msg_t message;
    message.type = 2;              // Tipo 2 pode representar uma mensagem de status periódica
    message.orig_uid = 0;           // ID do servidor (0 para indicar origem do servidor)
    message.dest_uid = 0;           // 0 indica que a mensagem é para todos os exibidores
    snprintf((char*)message.text, MAX_TEXT_LEN, "Servidor ativo: mensagem periódica.");
    message.text_len = strlen((char*)message.text) + 1; // Inclui o caractere nulo

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int client_fd = client_fds[i];
        if (client_fd != -1 && FD_ISSET(client_fd, clients)) {
            // Envia a mensagem para o cliente
            if (send(client_fd, &message, sizeof(message), 0) < 0) {
                perror("Falha ao enviar mensagem periódica");
            }
        }
    }

    printf("Mensagem periódica enviada para todos os clientes conectados.\n");
}
