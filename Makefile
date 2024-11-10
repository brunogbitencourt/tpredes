# Nomes dos executáveis
SERVER = server
RECEIVER = receiver
SENDER = sender

# Arquivos fonte
SERVER_SRC = server.c
RECEIVER_SRC = receiver.c
SENDER_SRC = sender.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall

# Regra padrão para construir todos os executáveis
all: $(SERVER) $(RECEIVER) $(SENDER)

# Compilar o servidor
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Compilar o receiver
$(RECEIVER): $(RECEIVER_SRC)
	$(CC) $(CFLAGS) -o $(RECEIVER) $(RECEIVER_SRC)

# Compilar o sender
$(SENDER): $(SENDER_SRC)
	$(CC) $(CFLAGS) -o $(SENDER) $(SENDER_SRC)

# Regra para executar o servidor
run-server: $(SERVER)
	./$(SERVER)

# Regra para executar o receiver com parâmetros ID e IP
run-receiver: $(RECEIVER)
	@echo "Executando receiver com ID=$(ID) e IP=$(IP)"
	./$(RECEIVER) $(ID) $(IP)

# Regra para executar o sender com parâmetros ID e IP
run-sender: $(SENDER)
	@echo "Executando sender com ID=$(ID) e IP=$(IP)"
	./$(SENDER) $(ID) $(IP)

# Limpeza dos arquivos gerados
clean:
	rm -f $(SERVER) $(RECEIVER) $(SENDER)
