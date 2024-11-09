# Usa uma imagem base com suporte a C
FROM gcc:latest

# Define o diretório de trabalho no container
WORKDIR /app

# Copia o código do servidor e do cliente para dentro do container
COPY server.c .
COPY client.c .

# Compila o código do servidor e do cliente
RUN gcc -o server server.c
RUN gcc -o client client.c

# Define a porta para escutar conexões
EXPOSE 8080

# Comando padrão para rodar o servidor (para iniciar somente o servidor)
CMD ["./server"]
