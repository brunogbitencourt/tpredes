# Projeto de Comunicação de Sockets em C

### Alunos:
- **Alberto Magno Machado**
- **Bruno Guimarães Bitencourt**
- **Oscar Dias**

### Professor:
- **Ricardo Carlini Sperandio**

## Descrição Geral

Este projeto implementa um sistema de comunicação utilizando sockets em C, dividido em três programas principais: `server.c`, `sender.c` e `receiver.c`. Cada um desses programas cumpre uma função distinta no sistema de comunicação, permitindo a troca de mensagens entre clientes e um servidor central. A aplicação foi configurada para operar via terminal, com comandos especificados no `Makefile` para facilitar a compilação e execução de cada componente.

### Programas Principais

- **`server.c`**: O servidor é responsável por gerenciar as conexões dos clientes (tanto `sender` quanto `receiver`) e estabelecer a lógica de envio e recebimento de mensagens. Ele é configurado para aceitar múltiplos clientes simultaneamente e processa mensagens individuais ou transmissões públicas para todos os clientes conectados ao servidor. A versão principal está configurada para funcionar de forma contínua e aceita conexões e mensagens sem interrupções.

- **`sender.c`**: O `sender` representa o cliente que envia mensagens para o servidor, que podem ser direcionadas a um cliente específico ou para todos os clientes conectados ao servidor. Através do `sender`, o usuário pode escolher o destinatário e o conteúdo da mensagem, possibilitando uma comunicação direcionada.

- **`receiver.c`**: O `receiver` é o cliente que recebe as mensagens enviadas pelo servidor, podendo visualizar tanto mensagens públicas, enviadas para todos os clientes, quanto mensagens privadas direcionadas ao seu identificador. Ele permanece ativo e pronto para receber mensagens enquanto a conexão com o servidor estiver ativa.

### Configuração e Execução com Docker

Para facilitar o desenvolvimento e garantir a compatibilidade do ambiente, o projeto foi configurado para ser executado em um contêiner Docker. O `Dockerfile` utilizado é baseado em uma imagem com o GCC e GDB instalados para compilar e depurar o código. 

1. **Requisitos**: Certifique-se de ter o Docker instalado em seu sistema. Caso contrário, instale o Docker seguindo as instruções oficiais: [https://docs.docker.com/get-docker/](https://docs.docker.com/get-docker/).

2. **Build do Contêiner**: No terminal, navegue até o diretório do projeto e execute o comando abaixo para construir o contêiner:
    ```bash
    docker build -t c-socket-app .
    ```

3. **Execução do Contêiner**: Após o build, inicie o contêiner com o comando:
    ```bash
    docker run -it --rm -v $(pwd):/workspace -w /workspace c-socket-app
    ```
   Isso iniciará o contêiner com o diretório atual montado como volume, permitindo acessar e modificar os arquivos do projeto dentro do contêiner.

### Executando o Projeto

Para compilar todos os programas, basta rodar o comando `make` no terminal. Para executar cada um dos componentes, use:

- `make run-server`: inicia o servidor para gerenciar conexões.
- `make run-sender ID=<ID> IP=<IP>`: inicia o cliente `sender`, substituindo `<ID>` pelo identificador único do cliente e `<IP>` pelo endereço IP do servidor.
- `make run-receiver ID=<ID> IP=<IP>`: inicia o cliente `receiver`, com o mesmo esquema de substituição de `<ID>` e `<IP>`.

### Detalhes da Branch e Temporizador

Na branch principal, o código é fornecido sem a implementação do temporizador. Esse recurso foi adicionado na branch `brunobitencourt/feat`, onde há uma tentativa de realizar transmissões periódicas de mensagens pelo servidor a cada 60 segundos. No entanto, o uso do temporizador causou conflitos no funcionamento do `select`, resultando em um desempenho inconsistente nas transmissões. 

Na branch `brunobitencourt/feat`, uma possível solução está sendo investigada, considerando o uso de threads (`pthreads`) para lidar com a transmissão periódica sem interferir no fluxo principal do servidor.
