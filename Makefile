# Nomes dos executáveis
SERVER = server
CLIENT = client

# Arquivos fonte
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall

# Regra padrão para construir ambos executáveis
all: $(SERVER) $(CLIENT)

# Compilar o servidor
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Compilar o cliente
$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Regra para executar o servidor
run-server: $(SERVER)
	./$(SERVER)

# Regra para executar o cliente
run-client: $(CLIENT)
	./$(CLIENT)

# Limpeza dos arquivos gerados
clean:
	rm -f $(SERVER) $(CLIENT)
