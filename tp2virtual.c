#include "tp2virtual.h"

// Variaveis globais
// proximo fifo é utilizado no algorirmo de substituição segunda chance
int proximo_fifo = 0;

void check_algoritmo_substituicao(char *algoritmo_check) {
    // O programa utiliza 4 algoritmos de substituição: 2a, fifo, lru ou random
    // Essa função confere se essa entrada feita por linha de comando do nome do algoritmo
    // é valida
    if(strcmp(algoritmo_check, "2a") == 0 || strcmp(algoritmo_check, "fifo") == 0 || strcmp(algoritmo_check, "random") == 0 || strcmp(algoritmo_check, "lru") == 0) {
        return;
    }
    else {
        printf("Algoritmo de substituição inválido!\n Favor executar novamente escolhendo '2A', 'FIFO', 'LRU' ou 'RANDOM' como algoritmo.\n");
        exit(1);  
    };
}

void check_arquivo_entrada(char *arquivo_check) {
    // O programa deve ler um arquivo de entrada com extensão .log
    // Os arquivos disponíveis são: compilador.log, compressor.log, matriz.log e simulador.log
    // Essa função confere se essa entrada feita por linha de comando do nome do arquivo
    // é valida
    if(strcmp(arquivo_check, "compilador.log") == 0 || strcmp(arquivo_check, "compressor.log") == 0 ||
        strcmp(arquivo_check, "matriz.log") == 0 || strcmp(arquivo_check, "simulador.log") == 0) {
        return;
    }
    else {
        printf("Arquivo de entrada de memória inválido!\n Favor executar novamente escolhendo 'compilador.log', 'compressor.log', 'matriz.log' ou 'simulador.log' como arquivo de entrada.\n");
        exit(1);  
    };
}

void check_tamanho_quadro_memoria(int check_tamanho_quadro) {
    // O tamanho do quadro da memória deve ser um valor positivo maior que zero e múltiplo de 2
    // Além disso, um tamanho razoável para o quadro de memória está na faixa de 2KB a 64KB
    // Esta função valida se essa entrada está dentro destas duas faixas
    if(!(check_tamanho_quadro % 2) && check_tamanho_quadro >= 2 && check_tamanho_quadro <= 64){
        return;
    }
    else {
        printf("Tamanho de quadro de memória inválido!\n");
        if(check_tamanho_quadro < 2 || check_tamanho_quadro > 64){
            printf("Valor fora da faixa razoável! Favor entrar com um valor maior ou igual a 2KB e menor ou igual a 64KB.\n");
        }
        if(check_tamanho_quadro % 2){
            printf("Valor não múltiplo de 2. Favor entrar com um valor que seja divisível por 2.\n");
        }
        if(check_tamanho_quadro < 0){
            printf("Valor negativo. Favor entrar com um valor que seja maior que zero.\n");
        }
        exit(1);  
    };
    }
void check_tamanho_memoria_total(int check_tamanho_memoria) {
    // O tamanho da memória total deve ser um valor positivo maior que zero e múltiplo de 2
    // Além disso, um tamanho razoável paraa memória total está na faixa de 128KB a 16384KB
    // Esta função valida se essa entrada está dentro destas duas faixas
    if(!(check_tamanho_memoria % 2) && check_tamanho_memoria >= 128 && check_tamanho_memoria <= 16384){
        return;
    }
    else {
        printf("Tamanho de memória total inválido!\n");
        if(check_tamanho_memoria < 128 || check_tamanho_memoria> 16384){
            printf("Valor fora da faixa razoável! Favor entrar com um valor maior ou igual a 128KB e menor ou igual a 16384KB.\n");
        }
        if(check_tamanho_memoria % 2){
            printf("Valor não múltiplo de 2. Favor entrar com um valor que seja divisível por 2.\n");
        }
        if(check_tamanho_memoria < 0){
            printf("Valor negativo. Favor entrar com um valor que seja maior que zero.\n");
        }
        exit(1);  
    };
}

void relatorio_estatisticas(char *arquivo_entrada, int tamanho_quadro, int tamanho_memoria, char *algoritmo_substituicao, int acessos_totais,
                            int acessos_leitura, int acessos_escrita, int num_page_faults, int num_dirty_pages) {
    // Esta funcao imprime no terminal os parâmetros de entrada do programa e as estatísiticas coletadas durante a execução do simulador
    printf("--------------------------------\n");
    printf("Parametros de entrada:\n");
    printf("Arquivo de entrada: %s\n", arquivo_entrada);
    printf("Tamanho da memoria: %d KB\n", tamanho_memoria);
    printf("Tamanho das páginas: %d KB\n", tamanho_quadro);
    printf("Tecnica de reposição: %s\n", algoritmo_substituicao);
    printf("--------------------------------\n");
    printf("Estatísitcas:\n");
    printf("Acessos totais à memória: %d\n", acessos_totais);
    printf("Acessos leitura: %d\n", acessos_leitura);
    printf("Acessos escrita: %d\n", acessos_escrita);
    printf("Page faults: %d\n", num_page_faults);
    printf("Páginas escritas: %d\n", num_dirty_pages);
    printf("--------------------------------\n");
}

int determinar_pagina(int tamanho_quadro_memoria, unsigned addr) {
    unsigned s, tmp;

    // Converter tamanho do quadro de memória de kilobytes para bytes
    tmp = tamanho_quadro_memoria * 1024;  
    // Variável para armazenar o número de bits necessários para representar o tamanho do quadro de memória
    s = 0; 

    while (tmp > 1) {
        // Deslocar o valor de 'tmp' uma posição para a direita (equivalente a dividir por 2)
        tmp = tmp >> 1;  
        // Incrementar 's' em 1 a cada iteração, contando o número de bits necessários para representar o tamanho do quadro
        s++; 
    }

    // Calcular o número da página a partir do endereço de 32 bits
    int pagina = addr >> s;  // Desloca o endereço 'addr' para a direita em 's' posições (remove os bits menos significativos)
    
    // Retornar o número da página correspondente ao endereço
    return pagina;  
}


int substituicao_fifo(Pagina *tabela_de_paginas, Quadro *memoria_fisica, int indice_pagina, int numero_quadros) {
    // Implementação do algoritmo FIFO (First-In, First-Out)
    // Armazenar o índice da página que será substituída (a primeira página inserida)
    int pagina_reposta = memoria_fisica[0].indice;  

    // Deslocar os índices das páginas uma posição para a esquerda
    for (int i = 1; i < numero_quadros; i++) {
        memoria_fisica[i - 1].indice = memoria_fisica[i].indice;  
    }

    // Substituir o último quadro pelo novo
    memoria_fisica[numero_quadros - 1].indice = indice_pagina; 

    // Retornar o índice da página que foi substituída
    return pagina_reposta;  
}


int substituicao_lru(Pagina *tabela_de_paginas, Quadro *memoria_fisica, int indice_pagina, int numero_quadros, int tempo_atual) {
    // Implementação do algoritmo LRU (Least Recently Used)
    // Variável para armazenar o índice do quadro com a página menos recentemente utilizada
    int min_index = 0;  
    // Armazenar o tempo de acesso da primeira página na memória física
    int min_access_time = memoria_fisica[0].ultimo_acesso;  

    // Encontrar o quadro com o menor tempo de acesso (página menos recentemente utilizada)
    for (int i = 1; i < numero_quadros; i++) {
        if (memoria_fisica[i].ultimo_acesso < min_access_time) {
            // Atualiza o menor tempo de acesso
            min_access_time = memoria_fisica[i].ultimo_acesso; 
            // Armazena o índice do quadro com o menor tempo de acesso
            min_index = i;  
        }
    }

    // Armazenar o índice da página que será substituída
    int pagina_reposta = memoria_fisica[min_index].indice;  

    // Substituir o quadro encontrado pelo novo
    memoria_fisica[min_index].indice = indice_pagina;  
    // Atualizar o tempo de acesso do quadro para o tempo atual
    memoria_fisica[min_index].ultimo_acesso = tempo_atual;  

    // Retornar o índice da página que foi substituída pelo algoritmo LRU
    return pagina_reposta;  
}

int substituicao_random(Pagina *tabela_de_paginas, Quadro *memoria_fisica, int indice_pagina, int numero_quadros) {
    // Implementação do algoritmo aleatório
    // Gerar uma posição aleatória dentro do número de quadros
    int random_position = rand() % numero_quadros;  
    // Armazenar o índice da página que será substituída
    int pagina_reposta = memoria_fisica[random_position].indice;  

    // Substituir um quadro aleatório pelo novo índice
    memoria_fisica[random_position].indice = indice_pagina;  

    // Retornar o índice da página que foi substituída aleatoriamente
    return pagina_reposta;  
}


int substituicao_segunda_chance(Pagina *tabela_de_paginas, Quadro *memoria_fisica, int indice_pagina, int numero_quadros) {
    // Implementação do algoritmo Segunda Chance (Second Chance)
    // Variável para indicar se ocorreu a substituição
    int subs = 0;  
    // Variável para armazenar o índice do quadro atual
    int current_frame = -1;  

    while (subs == 0) {
        // Encontrar a página do próximo FIFO
        current_frame = memoria_fisica[proximo_fifo].indice;
        if (tabela_de_paginas[current_frame].bit_ref == 1) {
            // Página ganha uma segunda chance
            // Zera o bit de referência da página
            tabela_de_paginas[current_frame].bit_ref = 0; 
            // Circular para o próximo quadro 
            proximo_fifo = (proximo_fifo + 1) % numero_quadros;  
        } else {
            // Ocorreu a substituição
            subs = 1;  
            // Substituir o quadro selecionado pelo novo
            memoria_fisica[proximo_fifo].indice = indice_pagina; 
        }
    }
    // Retorna o índice do quadro substituído
    return current_frame;  
}


int main (int argc, char *argv[]){
    // tp2virtual lru arquivo.log 4 128
    printf("Executando o programa %s\n", argv[0]);
    
    printf("--------------------------------\n");

    // Validar o número de parâmetros de entrada (linha de comando)
    if(argc < 5){
        printf("O programa requer 4 entradas obrigatórias. Favor executar novamente com o número correto de parâmetros!\n");
        exit(1);
    };
    if(argc > 6){
        printf("O programa requer no máximo 5 entradas. Favor executar novamente com o número correto de parâmetros!\n");
        exit(1);
    };

    // Variaveis para rodar o programa no modo debug
    int debug = 0;
    // Caso o comando inclua um quinto argumento com valor debug, o programa imprime um relatório
    // detalhado do passo a passo so simulador
    if (argc == 6 && strcmp(argv[5],"debug") == 0){
        debug = 1;  
    }
    
    // Parâmetros de entrada (linha de comando)
    // Os parâmetros argv[1] e argv[2] são transformados para o lowercase (função tolower()) para facilitar a conferência
    char *algoritmo_substituicao_aux = argv[1];
    char algoritmo_substituicao[20];
    strcpy(algoritmo_substituicao, algoritmo_substituicao_aux);
    for (int i = 0; algoritmo_substituicao[i]; i++) {
        algoritmo_substituicao[i] = tolower(algoritmo_substituicao[i]);
    }
    // Tratamento caso a entrada do nome do algoritmo não esteja como sigla
    if (strcmp(algoritmo_substituicao, "segunda chance") == 0 || strcmp(algoritmo_substituicao, "segundachance") == 0) {
        strcpy(algoritmo_substituicao, "2a");
    }
    char* arquivo_entrada_memoria_aux = argv[2];
    char arquivo_entrada_memoria[20];
    strcpy(arquivo_entrada_memoria, arquivo_entrada_memoria_aux);

    for (int i = 0; arquivo_entrada_memoria[i]; i++) {
        arquivo_entrada_memoria[i] = tolower(arquivo_entrada_memoria[i]);
    }

    // Os parâmetros argv[3] e argv[4] são transformados em int (função atoi()) pois serão tratados como numéricos
    // pela lógica do código
    int tamanho_quadro_memoria = atoi(argv[3]);
    int tamanho_memoria_total = atoi(argv[4]);

    // Funções para validação das entradas 
    // Cancela a execução do programa caso alguma entrada não seja validada
    check_algoritmo_substituicao(algoritmo_substituicao);
    check_arquivo_entrada(arquivo_entrada_memoria);
    check_tamanho_quadro_memoria(tamanho_quadro_memoria);
    check_tamanho_memoria_total(tamanho_memoria_total);

    // Calcula o número de quadros na memória física
    int numero_quadros = tamanho_memoria_total / tamanho_quadro_memoria;

    // Aloca espaço para a tabela de páginas
    Pagina *tabela_de_paginas = (Pagina *) malloc(sizeof(Pagina) * (1 << 21));
    // Inicializa a tabela de páginas
    if (tabela_de_paginas != NULL) {
        // Calcula o tamanho de uma única estrutura Pagina e multiplica pelo resultado de (1 << 21),
        // que é equivalente a deslocar o valor 1 em 21 bits para a esquerda. 
        // Isso resulta no número total de bytes necessários para armazenar (1 << 21) estruturas Pagina.
        memset(tabela_de_paginas, 0, sizeof(Pagina) * (1 << 21));
    // Aponta o erro e encerra o programa caso a memória não tenha sido alocada corretamente
    } else {
        printf("Nâo foi possível alocar memória para tabela_de_paginas.\n");
        exit(1);
    }

    // Aloca espaço para a memória física
    Quadro *memoria_fisica = (Quadro *) malloc(sizeof(Quadro) * numero_quadros);
    // Inicializa a memória física
    if (memoria_fisica != NULL) {
        memset(memoria_fisica, 0, sizeof(Quadro) * numero_quadros);  
    // Aponta o erro e encerra o programa caso a memória não tenha sido alocada corretamente
    } else {
        printf("Não foi possível alocar memória para memoria_fisica.\n");
        exit(1);
    }

    char arquivo_entrada[100] = "./input_files/";
    strcat(arquivo_entrada, arquivo_entrada_memoria);

    // Abertura do arquivo de entrada com extensão .log no modo leitura
    FILE *fptr = fopen(arquivo_entrada, "r");
    // Aponta o erro e encerra o programa caso a leitura do arquivo acima não funcione corretamente 
    if(fptr== NULL){
        printf("Erro na abertura do arquivo de memória de entrada. Favor verificar seu arquivo!\n");
        exit(1);
    }

    // Variável de controle de ultimo acesso
    int clock = 0;

    // Seed para geração de números aleatórios com resultados reproduzíveis
    // Definir a semente para a função srand()
    srand(42);

    // Variáveis para coletar estatísticas
    int acessos_totais = 0;
    int acessos_leitura = 0;
    int acessos_escrita = 0;
    int num_page_faults = 0;
    int num_dirty_pages = 0;

    printf("Executando simulador ...\n");

    // Loop principal para processar os acessos à memória
    unsigned addr;
    char rw;
    int linhas_lidas;
    uint32_t endereco;
    
    // Lê o endereço e a operação (R ou W)
    while ((linhas_lidas = fscanf(fptr, "%x %c", &addr, &rw)) != EOF) {

        if (linhas_lidas != 2) {
            // A linha não está no formato correto
            printf("Uma linha fora do formato esperado foi encontrada. Favor verificar seu arquivo de entrada!\n");
            printf("O programa será encerrado agora!\n");
            exit(1);
        }
    
        endereco = addr;
        // Verificar se o endereço está dentro do intervalo esperado
        if (!(endereco >= 0x00000000 && endereco <= 0xFFFFFFFF)) {
            // O endereço não está no formato correto
            printf("Um endereço %x fora do formato esperado foi encontrado. Favor verificar seu arquivo de entrada!\n", endereco);
            printf("O programa será encerrado agora!\n");
            exit(1);
        }

        // Verificar se a operação é válida ('R' ou 'W')
        if (rw != 'R' && rw != 'W') {
            // A operação não é válida
            printf("Uma operação inválida foi encontrada. Favor verificar seu arquivo de entrada!\n");
            printf("O programa será encerrado agora!\n");
            exit(1);
        }
    
         // Incrementar o contador de acessosn totais
        acessos_totais++;
        
        // Incrementar o contador de tempo
        clock++;

        // Determinar o número da página a partir do endereço
        int indice_pagina = determinar_pagina(tamanho_quadro_memoria, addr);

        // Setar o bit de referência da página para 1 
        tabela_de_paginas[indice_pagina].bit_ref = 1;

        // Sempre que uma operação de escrita é realizada em uma página na memória, 
        // ela fica suja
        if (rw == 'W' || rw == 'w' ) {
            acessos_escrita++;
            tabela_de_paginas[indice_pagina].suja = 1;
        }  
        if (rw == 'R'|| rw == 'r') {
            acessos_leitura++;
        }  

        if (debug){
            printf("Linhas lidas: %d\n", acessos_totais);
            printf("Acessando página: %d\n", indice_pagina);
        }

        // Verifica se a página está na memória física
        if (tabela_de_paginas[indice_pagina].referencia != 0){
            // Página já está na memória física
            // Procurar o quadro ao qual ela está alocada
            int aux = -1;
            for(int i = 0; i <numero_quadros; i++){
                if(memoria_fisica[i].indice == indice_pagina){
                    aux = i;
                }
            }
            if (debug){
                printf("Página já está na memória física (Sem page fault). Está alocada no quadro: %d\n", aux);
            }

            // Atualizar último acesso da página e do quadro
            tabela_de_paginas[indice_pagina].ultimo_acesso = clock;
            memoria_fisica[aux].ultimo_acesso = clock;  
        } else {
            // Página não está na memória física, ocorre um page fault
            if (debug){
                printf("Página não está na memória física! Page fault\n");
            }
            
            // Incrementar o contador de page faults
            num_page_faults++;

            // Verificar se há um quadro vazio na memória física procurando algum quadro desocupado
            int quadro_vazio = -1;
            for (int i = 0; i < numero_quadros; i++) {
                if (memoria_fisica[i].ocupado == 0) {
                    if (debug){
                        printf("Quadro vazio encontrado! Memória alocada ao quadro: %d\n", i);
                    }
                    // Se achou um quadro livre, atualiza a memória física colocando o quadro como ocupado,
                    // o índice como o índice da página e o último acesso com o clock atual
                    memoria_fisica[i].ocupado = 1;
                    memoria_fisica[i].indice = indice_pagina;
                    memoria_fisica[i].ultimo_acesso = clock;
                    quadro_vazio = 1;
                    break;
                }
            }
            // Caso não exista um quadro vazio, utiliza-se o algoritmo de substituição selecionado para 
            // substituir a página  
            int pagina_reposta = -1;             
            if (quadro_vazio == -1) {
                if (debug){
                    printf("Sem quadro vazio! Vamos usar uma técnica de reposição\n");
                }
                if (strcmp(algoritmo_substituicao, "fifo") == 0) {
                    pagina_reposta = substituicao_fifo(tabela_de_paginas, memoria_fisica, indice_pagina, numero_quadros);
                } else if (strcmp(algoritmo_substituicao, "lru") == 0) {
                    pagina_reposta = substituicao_lru(tabela_de_paginas, memoria_fisica, indice_pagina, numero_quadros, clock);
                } else if (strcmp(algoritmo_substituicao, "random") == 0) {
                    pagina_reposta = substituicao_random(tabela_de_paginas, memoria_fisica, indice_pagina, numero_quadros);
                } else if (strcmp(algoritmo_substituicao, "2a") == 0) {
                    pagina_reposta = substituicao_segunda_chance(tabela_de_paginas, memoria_fisica, indice_pagina, numero_quadros);
                } 

                if(tabela_de_paginas[pagina_reposta].suja == 1){
                    // Página teve que ser escrita de volta no disco
                    num_dirty_pages++;
                }

                // Atualizar a tabela de páginas com o processo que não está presente
                // Setando seus atributos como zero
                tabela_de_paginas[pagina_reposta].referencia = 0;
                tabela_de_paginas[pagina_reposta].ultimo_acesso = 0;
                tabela_de_paginas[pagina_reposta].suja = 0;
                tabela_de_paginas[pagina_reposta].bit_ref = 0;

                if (debug){
                    printf("Página %d substituída\n", pagina_reposta);
                }
            }

            // Atualizar a tabela de páginas com os valores para o novo processo
            tabela_de_paginas[indice_pagina].referencia = 1;
            tabela_de_paginas[indice_pagina].ultimo_acesso = clock;      
        }
    }             
    
    // Função para apresentar o relatório com parâmetros de entrada e estatísitcas geradas durante a execução do simulador
    relatorio_estatisticas(arquivo_entrada_memoria, tamanho_quadro_memoria, tamanho_memoria_total, algoritmo_substituicao, acessos_totais,
                            acessos_leitura, acessos_escrita, num_page_faults,num_dirty_pages);

    // Liberar a memória alocada
    free(tabela_de_paginas);
    free(memoria_fisica);

    // Fechamento do arquivo
    fclose(fptr);

    return 0;
}