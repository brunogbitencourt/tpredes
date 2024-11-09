# Nomes dos executáveis
TARGET_SERVER = server
TARGET_CLIENT = client

# Arquivos fonte
SRC_SERVER = server.c
SRC_CLIENT = client.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall

# Regra padrão para construir ambos os executáveis
all: $(TARGET_SERVER) $(TARGET_CLIENT)

# Compila o servidor
$(TARGET_SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(SRC_SERVER)

# Compila o cliente
$(TARGET_CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(SRC_CLIENT)

# Regra para executar o servidor
run_server: $(TARGET_SERVER)
	./$(TARGET_SERVER)

# Regra para executar o cliente
run_client: $(TARGET_CLIENT)
	./$(TARGET_CLIENT)

# Limpeza dos arquivos gerados
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT)
