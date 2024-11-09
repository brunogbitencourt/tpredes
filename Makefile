# Nomes dos executáveis
TARGET_SERVER = server
TARGET_RECEIVER = receiver
TARGET_SENDER = sender

# Caminho dos arquivos fonte
SRC_SERVER = src/server.c
SRC_RECEIVER = src/receiver.c
SRC_SENDER = src/sender.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall

# Regra padrão para construir todos os executáveis
all: $(TARGET_SERVER) $(TARGET_RECEIVER) $(TARGET_SENDER)

# Compila o servidor
$(TARGET_SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(SRC_SERVER)

# Compila o receiver
$(TARGET_RECEIVER): $(SRC_RECEIVER)
	$(CC) $(CFLAGS) -o $(TARGET_RECEIVER) $(SRC_RECEIVER)

# Compila o sender
$(TARGET_SENDER): $(SRC_SENDER)
	$(CC) $(CFLAGS) -o $(TARGET_SENDER) $(SRC_SENDER)

# Regra para executar o servidor
run_server: $(TARGET_SERVER)
	./$(TARGET_SERVER)

# Regra para executar o receiver
run_receiver: $(TARGET_RECEIVER)
	./$(TARGET_RECEIVER)

# Regra para executar o sender
run_sender: $(TARGET_SENDER)
	./$(TARGET_SENDER)

# Limpeza dos arquivos gerados
clean:
	rm -f $(TARGET_SERVER) $(TARGET_RECEIVER) $(TARGET_SENDER)
