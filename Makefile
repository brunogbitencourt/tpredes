# Nome do executável
TARGET = server

# Arquivos fonte
SRC = server.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall

# Regras do Makefile

# Regra padrão para construir o executável
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regra para executar o servidor
run: $(TARGET)
	./$(TARGET)

# Limpeza dos arquivos gerados
clean:
	rm -f $(TARGET)
