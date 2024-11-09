# Nomes dos executáveis
SERVER = server
RECEIVER = receiver

# Arquivos fonte
SERVER_SRC = server.c
RECEIVER_SRC = receiver.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall

# Regra padrão para construir ambos executáveis
all: $(SERVER) $(RECEIVER)

# Compilar o servidor
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Compilar o receiver
$(RECEIVER): $(RECEIVER_SRC)
	$(CC) $(CFLAGS) -o $(RECEIVER) $(RECEIVER_SRC)

# Regra para executar o servidor
run-server: $(SERVER)
	./$(SERVER)

# Regra para executar o receiver com parâmetros ID e IP
run-receiver: $(RECEIVER)
	@echo "Executando receiver com ID=$(ID) e IP=$(IP)"
	./$(RECEIVER) $(ID) $(IP)

# Limpeza dos arquivos gerados
clean:
	rm -f $(SERVER) $(RECEIVER)
