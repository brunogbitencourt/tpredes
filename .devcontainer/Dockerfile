# Use uma imagem base com GCC instalado
FROM gcc:latest

# Instale o GDB para depuração
RUN apt-get update && apt-get install -y gdb

# Defina o diretório de trabalho dentro do contêiner
WORKDIR /workspace

# Copie todos os arquivos do projeto para dentro do contêiner
COPY . /workspace

# Exponha a porta 8080 para comunicação (caso seja necessário para seu servidor)
EXPOSE 8080

# Comando padrão do contêiner (pode ser substituído pelo VSCode para depuração)
CMD ["/bin/bash"]
