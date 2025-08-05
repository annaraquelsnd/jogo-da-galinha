#include <stdio.h>

typedef struct {
    int posicao;
} tCarro;

typedef struct {
    char direcao;
    int velocidade;
    int num_carros;

    tCarro carros[10];

} tPista;

typedef struct {
    int x;
    int y; 
    int vidas;

} tGalinha;


typedef struct {
    int animacao;
    int largura_mapa;
    int qtd_pistas;

    char viewCarro[2][3];
    char viewGalinha[2][3];

    tPista pistas[12];
    tGalinha galinha;

    char mapa[35][100];

} tJogo;

tJogo LerJogo(FILE * config, FILE * personagens);

void Debugar(tJogo jogo);

int main(int argc, char * argv[]){
    // Verificando se parâmetros foram passados:
    if (argc <= 1){
        printf("ERRO: Informe o diretorio com os arquivos de configuracao.");
        return 1;
    }

    // Declaração de variáveis:
    tJogo jogo;
    FILE * configFile;
    FILE * personagensFile;
    char diretorio[1000];

    // Lendo arquivos:
    sprintf(diretorio, "%s/config_inicial.txt", argv[1]);
    configFile = fopen(diretorio, "r");
    if(!configFile) {
        printf("ERRO: Nao foi possivel abrir o arquivo '%s.'", diretorio);
        return 1;
    }

    sprintf(diretorio, "%s/personagens.txt", argv[1]);
    personagensFile = fopen(diretorio, "r");
    if(!personagensFile) {
        printf("ERRO: Nao foi possivel abrir o arquivo '%s.'", diretorio);
        return 1;
    } 

    jogo = LerJogo(configFile, personagensFile);
    fclose(configFile);
    fclose(personagensFile);
    Debugar(jogo);
    
    return 0;
}

void Debugar(tJogo jogo){
    int i = 0, j = 0, c = 0;

    printf("Animação: %d\n", jogo.animacao);
    printf("Largura mapa: %d\n", jogo.largura_mapa);
    printf("Quantidade pistas: %d\n", jogo.qtd_pistas);
    
    for (i = 0; i < jogo.qtd_pistas; i++) {
        printf("Pista %d - Direção: %c; Velocidade: %d; Num carros: %d; ", i, jogo.pistas[i].direcao, jogo.pistas[i].velocidade, jogo.pistas[i].num_carros);
        for (c = 0; c < jogo.pistas[i].num_carros; c++){
            printf("Carro %d: %d; ", c, jogo.pistas[i].carros[c].posicao);
        }
        printf("\n");
    }
    printf("\n");

    printf("Galinha - Posição: %d; Vidas: %d; Pista: %d\n\n", jogo.galinha.x, jogo.galinha.vidas, jogo.galinha.y);

    printf("Galinha:\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            printf("%c", jogo.viewGalinha[i][j]);
        }
        printf("\n");
    }

    printf("Carro:\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            printf("%c", jogo.viewCarro[i][j]);
        }
        printf("\n");
    }
    

}

tJogo LerJogo(FILE * config, FILE * personagens){
    tJogo jogo;
    int i = 0, j = 0, c = 0;
    char inicioLinha;

    fscanf(config, "%d", &jogo.animacao);
    fscanf(config, "%d %d", &jogo.largura_mapa, &jogo.qtd_pistas);

    jogo.pistas[i].direcao = '0';
    jogo.pistas[i].num_carros = -1;
    jogo.pistas[i].velocidade = -1;

    for (i = 1; i < jogo.qtd_pistas; i++) {

        do {
            fscanf(config, "%c", &inicioLinha);
        } while (inicioLinha == '\n');

        if (inicioLinha == 'E' || inicioLinha == 'D') {
            // É pista
            // Ler direção:
            jogo.pistas[i].direcao = inicioLinha;

            // Ler velocidade e num de carros:
            fscanf(config, "%d %d", &jogo.pistas[i].velocidade, &jogo.pistas[i].num_carros);

            // Ler carros:
            for (c = 0; c < jogo.pistas[i].num_carros; c++) {
                fscanf(config, "%d", &jogo.pistas[i].carros[c].posicao);
            }
            
        } else {
            jogo.pistas[i].direcao = '0';
            jogo.pistas[i].num_carros = -1;
            jogo.pistas[i].velocidade = -1;

            if (inicioLinha == 'G') {
                fscanf(config, "%d %d", &jogo.galinha.x, &jogo.galinha.vidas);          
                jogo.galinha.y = jogo.qtd_pistas-1;
            }
        }
        while (inicioLinha != '\n' && !feof(config)) {
            fscanf(config, "%c", &inicioLinha);
        }

    }


    // Ler view personagens:
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            fscanf(personagens, "%c", &jogo.viewGalinha[i][j]);
        }
        fscanf(personagens, "%*[^\n]\n");
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            fscanf(personagens, "%c", &jogo.viewCarro[i][j]);
        }
        fscanf(personagens, "%*[^\n]\n");
    }

    return jogo;
    
}