/* Criado por: Brunofsec  */
/* Simples Port scanner usando C  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h> /* processar as opções de linha de comando  */

#define GREEN_TEXT "\033[0;32m"
#define RESET_TEXT "\033[0m"
#define RED_TEXT "\033[0;31m"
#define YELLOW_TEXT "\033[0;33m"
#define BLUE_TEXT "\033[0;34m"
#define MAGENTA_TEXT "\033[0;35m"
#define CYAN_TEXT "\033[0;36m"
#define WHITE_TEXT "\033[0;37m"


// mapeando portas a serviços
struct PortService {
    int port;
    const char *service;
};

struct PortService portServices[] = {
    {21, "FTP"},
    {22, "SSH"},
    {80, "HTTP"},
    {443, "HTTPS"},
    // Adicionar mais portas e serviços conforme necessário
    {0, NULL}
};

// Função para obter o serviço correspondente à porta
const char *getServiceName(int port) {
    for (int i = 0; portServices[i].service != NULL; i++) {
        if (portServices[i].port == port) {
            return portServices[i].service;
        }
    }
    return "Desconhecido"; // Se a porta não for encontrada nos serviços conhecidos
}

void scan_ports(const char *ip, int porta_inicial, int porta_final) {
    struct sockaddr_in server_addr;

    printf(YELLOW_TEXT "Iniciando o scan de portas em %s...\n"RESET_TEXT, ip);

    for (int porta = porta_inicial; porta <= porta_final; porta++) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("Erro ao criar o socket");
            return;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(porta);
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
            perror("Erro ao converter endereço IP");
            close(sockfd); // Fecha o socket em caso de erro
            return;
        }

        // Define um tempo limite de conexão curto para evitar atrasos longos
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1 segundo
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

        // Tenta se conectar à porta
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
            printf("\r" GREEN_TEXT "Verificando a porta %d em %s..." RESET_TEXT, porta, ip);
            const char *serviceName = getServiceName(porta);
            printf("\r" GREEN_TEXT "Porta %d está aberta em %s (%s)\n" RESET_TEXT, porta, ip, serviceName);
        } else {
            printf("\rVerificando a porta %d em %s...", porta, ip);
        }

        // Fecha o socket em qualquer caso
        close(sockfd);
    }

    printf("\nScan de portas em %s concluído.\n", ip);
}

int main(int argc, char *argv[]) {
    int option;
    int all_ports = 0; // Para verificar todas as portas
    int specific_port = 0; // Para verificar uma porta específica
    int port_to_check = 0; // A porta específica a ser verificada

    while ((option = getopt(argc, argv, "ap:")) != -1) {
        switch (option) {
            case 'a':
                all_ports = 1;
                break;
            case 'p':
                specific_port = 1;
                port_to_check = atoi(optarg);
                break;
            case 'h':
                fprintf(stderr, "Construindo...\n", argv[0]);
                break;
            default:
                fprintf(stderr, "Uso: %s [-a] [-p porta] <endereço_IP>\n", argv[0]);
                return 1;
        }
    }

    if (optind != argc - 1) {
        fprintf(stderr, "Uso: %s [-a] [-p porta] <endereço_IP>\n", argv[0]);
        return 1;
    }

    char *ip = argv[optind];
    int porta_inicial = 1;
    int porta_final = 65535;

    if (specific_port) {
        porta_inicial = port_to_check;
        porta_final = port_to_check;
    }

    if (all_ports) {
        scan_ports(ip, 1, 65535);
    } else {
        scan_ports(ip, porta_inicial, porta_final);
    }

    return 0;
}
