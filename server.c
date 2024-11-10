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



#define MAX_CLIENTS 20 // Limite de 10 clientes por tipo (exibição e envio)
#define PORT 9254
#define MAX_TEXT_LEN 141
#define PERIODIC_INTERVAL 5000

int client_uids[MAX_CLIENTS];

volatile sig_atomic_t timer_expired = 0; 

typedef struct {
    unsigned short int type;
    unsigned short int orig_uid;
    unsigned short int dest_uid;
    unsigned short int text_len;
    unsigned char text[MAX_TEXT_LEN];
} msg_t;

void send_periodic_message(fd_set *clients, int *client_fds);
void handle_message(int client_fd, int *client_fds, fd_set *clients);
void timer_handler(int signum);
void setup_timer(int milliseconds);


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    //setup_handler();

    fd_set clients; // Estrutura de dados utilizada para represntar um conunto de sockets
    int client_fds[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
        client_uids[i] = -1; 
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

    // Configura o tratador de sinal para o temporizador e o temporizador
    signal(SIGALRM, timer_handler);
    setup_timer(PERIODIC_INTERVAL);



    while(1){
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set read_fds = clients; // Mantem um registro dos clientes conectados aos sockets
        
        // O select agora não bloqueará indefinidamente
        int activity = select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);

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
                    client_uids[i] = -1;  // Inicialize o UID
                    FD_SET(new_socket, &clients);
                    printf("Cliente conectado no socket %d\n", new_socket);
                    break;
                }
            }
        }

        // if (timer_expired) {
        //     printf("Timer expirado, preparando para enviar mensagem periódica.\n");
        //     send_periodic_message(&clients, client_fds);
        //     timer_expired = 0;
        //     setup_timer(5000);
        // }      



        // Loop para verificar atividade dos clientes conectados
        for (int i = 0; i < MAX_CLIENTS; i++) {
        int client_fd = client_fds[i];
        
        // Verifique se o descritor ainda é válido
        if (client_fd != -1 && FD_ISSET(client_fd, &read_fds)) {
            // Se o descritor for inválido, remova-o de clients
            if (fcntl(client_fd, F_GETFD) == -1) {
                printf("Removendo descritor inválido %d do conjunto clients.\n", client_fd);
                FD_CLR(client_fd, &clients);
                client_fds[i] = -1;
                client_uids[i] = -1;
                continue;
            }

            handle_message(client_fd, client_fds, &clients);
        }
    }


    }
   
   
   
   return 0;
}

void timer_handler(int signum) {
    timer_expired = 1;
}



void setup_timer(int milliseconds) {
    struct itimerval timer;

    timer.it_value.tv_sec = milliseconds / 1000;
    timer.it_value.tv_usec = (milliseconds % 1000) * 1000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}


void send_periodic_message(fd_set *clients, int *client_fds) {
    printf("Enviando mensagem periódica para todos os clientes conectados.\n");
    msg_t message;
    message.type = 2;
    message.orig_uid = 0;
    message.dest_uid = 0;
    snprintf((char*)message.text, MAX_TEXT_LEN, "Servidor ativo: mensagem periódica.");
    message.text_len = strlen((char*)message.text) + 1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int client_fd = client_fds[i];
        if (client_fd != -1 && FD_ISSET(client_fd, clients)) {
            printf("Enviando mensagem para cliente no socket %d\n", client_fd);
            if (send(client_fd, &message, sizeof(message), 0) < 0) {
                perror("Falha ao enviar mensagem periódica");
            }
        }
    }
}

void handle_message(int client_fd, int *client_fds, fd_set *clients) {
    msg_t message;
    int bytes_received = recv(client_fd, &message, sizeof(message), 0);

    if (bytes_received == 0) {
        // Cliente fechou a conexão
        printf("Cliente %d fechou a conexão.\n", client_fd);
        close(client_fd);
        FD_CLR(client_fd, clients);  // Remova do conjunto `clients`, não `read_fds`
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] == client_fd) {
                client_fds[i] = -1;
                client_uids[i] = -1;
                break;
            }
        }
        return;
    } else if (bytes_received < 0) {
        perror("Erro ao receber dados");
        close(client_fd);
        FD_CLR(client_fd, clients);  // Remova do conjunto `clients`
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] == client_fd) {
                client_fds[i] = -1;
                client_uids[i] = -1;
                break;
            }
        }
        return;
    }

    printf("Mensagem recebida do cliente %d, tipo %d.\n", client_fd, message.type);

    switch (message.type) {
        case 0:
            printf("Cliente %d identificado com ID %d.\n", client_fd, message.orig_uid);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == client_fd) {
                    client_uids[i] = message.orig_uid;  // Salva o UID do cliente
                    break;
                }
            }
            send(client_fd, &message, sizeof(message), 0);
            break;

        case 1:
            printf("Mensagem de desconexão recebida do cliente %d.\n", client_fd);
            printf("Cliente %d se desconectando.\n", client_fd);
            close(client_fd);
            FD_CLR(client_fd, clients);  // Certifique-se de remover de `clients`
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == client_fd) {
                    client_fds[i] = -1;
                    client_uids[i] = -1;
                    break;
                }
            }
            printf("Cliente %d removido com sucesso.\n", client_fd);
            break;

        default:
            printf("Tipo de mensagem desconhecido.\n");
            break;
    }
}
