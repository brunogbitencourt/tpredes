# Usa uma imagem base com suporte a C
FROM gcc:latest

# Define o diretório de trabalho no container
WORKDIR /app

# Copia o código para dentro do container
COPY server.c .

# Compila o código
RUN gcc -o server server.c

# Define a porta para escutar conexões
EXPOSE 8080

# Comando para rodar o servidor
CMD ["./server"]
