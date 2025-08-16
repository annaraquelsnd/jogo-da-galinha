#include <stdio.h>

typedef struct {
    int posicao;
} tCarro;

tCarro InstanciarCarro(int posicao);
int ObtemPosicaoCarro(tCarro carro);

typedef struct {
    char direcao;
    int velocidade;
    int num_carros;

    tCarro carros[10];

} tPista;

tPista InstanciarPista(char direcao, int velocidade, int num_carros, tCarro carros[]);

char ObtemDirecaoPista(tPista pista);
int ObtemVelocidadePista(tPista pista);
int ObtemNumCarrosPista(tPista pista);
tCarro ObtemCarroPista(tPista pista, int index);
tPista AndarCarrosPista(tPista pista, int tam_pista);

typedef struct {
    int x;
    int y; 
    int vidas;

} tGalinha;

tGalinha InstanciarGalinha(int x, int y, int vidas);
int ObtemXGalinha(tGalinha galinha);
int ObtemYGalinha(tGalinha galinha);
int ObtemVidasGalinha(tGalinha galinha){
    return galinha.vidas;
}

typedef struct {
    int status; // 0 = em andamento; 1 = ganhou; -1 = perdeu;

    int animacao;
    int largura_mapa;
    int qtd_pistas;

    char viewCarro[2][3];
    char viewGalinha[2][3];

    tPista pistas[12];
    tGalinha galinha;

    char mapa[35][102];

} tJogo;

tJogo LerJogo(FILE * config, FILE * personagens);
void ImprimirMapaJogo(tJogo jogo);
tJogo ProximoInstJogo(tJogo jogo);
int ObtemStatusJogo(tJogo jogo);
tJogo FicarParadaGalinhaJogo(tJogo jogo);
tJogo IrParaFrenteGalinhaJogo(tJogo jogo);
tJogo IrParaTrasGalinhaJogo(tJogo jogo);
tJogo HouveColisaoGalinhaJogo(tJogo jogo, tGalinha galinha);

void Debugar(tJogo jogo);

int main(int argc, char * argv[]){
    char entrada;

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


    while (ObtemStatusJogo(jogo) == 0) {
        ImprimirMapaJogo(jogo);
        // Criar LerEvento(jogo);

        do {
            scanf("%c", &entrada);
        } while (entrada == '\n');
        
        if (entrada == ' ') {
            jogo =  FicarParadaGalinhaJogo(jogo);
            // Verificar se houve colisão
        } else if (entrada == 'w') {
            jogo = IrParaFrenteGalinhaJogo(jogo);
        } else if (entrada == 's') {
            jogo = IrParaTrasGalinhaJogo(jogo);
        }

        //Debugar(jogo);

    }
    ImprimirMapaJogo(jogo);

    if (ObtemStatusJogo(jogo) == -1) {
        // perdeu
        printf("Perdeu");
    } else if (ObtemStatusJogo(jogo) == 1) {
        // ganhou
        printf("Ganhou");
    } else {
        // erro
        printf("ERRO: Status jogo não existe");
    }
    
    return 0;
}

// Funções de tJogo:
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
void ImprimirMapaJogo(tJogo jogo){
    int i = 0, j = 0, i_pistas = 0, j_carros = 0, posicao = 0, i_view = 0, j_view = 0, coluna = 0, j_final = 0;
    char mapaAtual[(jogo.qtd_pistas*3)-1][jogo.largura_mapa];
    tCarro carro;

    // Copiando mapa para ser alterado localmente:
    for (i = 0; i < (jogo.qtd_pistas*3)-1; i++) {
        for (j = 0; j < jogo.largura_mapa; j++) {
            mapaAtual[i][j] = jogo.mapa[i][j];
        }
    }
    
    // Alterando a cópia do mapa com os elementos nas posições atuais:

    // Percorrer pistas e ler os carros:
    for (i_pistas = 0; i_pistas < jogo.qtd_pistas; i_pistas++) {
        // Verificar se pista é vazia:
        if (ObtemDirecaoPista(jogo.pistas[i_pistas]) != '0') {
            for (j_carros = 0; j_carros < ObtemNumCarrosPista(jogo.pistas[i_pistas]); j_carros++) {
                carro = ObtemCarroPista(jogo.pistas[i_pistas], j_carros);
                posicao = ObtemPosicaoCarro(carro);
                
                i = i_pistas*3; // linha em relação a pista:
                j = posicao-2; // Começa no início do desenho
                
                for (i_view = 0; i_view < 2; i_view++) {
                    for (j_view = 0; j_view < 3; j_view++) {
                        j_final = ((j + j_view) % jogo.largura_mapa + jogo.largura_mapa ) % jogo.largura_mapa;
                        //printf("i: %d; j: %d; ", i+i_view, j_final);
                        mapaAtual[i+i_view][j_final] = jogo.viewCarro[i_view][j_view];
                    }
                    
                }
                //printf("\n");
                
            }

        }

        // Colocando galinha:
        posicao = ObtemXGalinha(jogo.galinha);
        int pistaGalinha = ObtemYGalinha(jogo.galinha);

        i = pistaGalinha*3; // linha em relação a pista:
        j = posicao-2; // Começa no início do desenho

        for (i_view = 0; i_view < 2; i_view++) {
            //printf("Galinha: ");
            for (j_view = 0; j_view < 3; j_view++) {
                j_final = ((j + j_view) % jogo.largura_mapa + jogo.largura_mapa ) % jogo.largura_mapa;
                //printf("i: %d; j: %d; ", i+i_view, j_final);
                mapaAtual[i+i_view][j_final] = jogo.viewGalinha[i_view][j_view];
            }
            
        }
        //printf("\n");
    }
    


    printf("|");
    for (j = 0; j < jogo.largura_mapa; j++) {
        printf("-");
    }
    printf("|\n");
    
    for (i = 0; i < (jogo.qtd_pistas*3)-1; i++) {
        printf("|");
        for (j = 0; j < jogo.largura_mapa; j++) {
            printf("%c", mapaAtual[i][j]);
        }
        printf("|\n");
    }

    printf("|");
    for (j = 0; j < jogo.largura_mapa; j++) {
        printf("-");
    }
    printf("|\n");
    
}
tJogo LerJogo(FILE * config, FILE * personagens){
    tJogo jogo;
    int i = 0, j = 0, c = 0;
    char inicioLinha, direcao = 0;
    int velocidade = 0, num_carros = 0, posicao_carro = 0;
    tCarro carros[10];
    int x = 0, y = 0, vidas = 0;

    jogo.status = 0;

    fscanf(config, "%d", &jogo.animacao);
    fscanf(config, "%d %d", &jogo.largura_mapa, &jogo.qtd_pistas);

    inicioLinha = fgetc(config);
    printf("Char depois de qtdPistas: %c\n", inicioLinha);

    for (i = 0; i < jogo.qtd_pistas; i++) {

        inicioLinha = fgetc(config);

        printf("Inicio linha: %c\n", inicioLinha);

        if (inicioLinha == 'E' || inicioLinha == 'D') {
            // É pista
            // Ler direção:
            direcao = inicioLinha;

            // Ler velocidade e num de carros:
            fscanf(config, "%d %d", &velocidade, &num_carros);

            // Ler carros:
            for (c = 0; c < num_carros; c++) { // Trocar
                fscanf(config, "%d", &posicao_carro);
                carros[c] = InstanciarCarro(posicao_carro);
            }

            jogo.pistas[i] = InstanciarPista(direcao, velocidade, num_carros, carros);
            fgetc(config);
            
        } else {
            // Pista vazia
            direcao = '0';
            num_carros = -1;
            velocidade = -1;
            jogo.pistas[i] = InstanciarPista(direcao, velocidade, num_carros, carros);

            if (inicioLinha == 'G') {
                fscanf(config, "%d %d", &x, &vidas);          
                y = jogo.qtd_pistas-1;
                jogo.galinha = InstanciarGalinha(x, y, vidas);
            }
        }

    }

    // Ler view personagens:
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            fscanf(personagens, "%c", &jogo.viewGalinha[i][j]);
        }
        fscanf(personagens, "%*[ \n]");
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            fscanf(personagens, "%c", &jogo.viewCarro[i][j]);
        }
        fscanf(personagens, "%*[ \n]");
    }

    // Definir mapa base
    for (i = 0; i < (jogo.qtd_pistas*3)-1; i++) {
        for (j = 0; j < jogo.largura_mapa; j++) {
            if ((i+1)%3 != 0) {
                jogo.mapa[i][j] = ' ';
            } else {
                if ((j+1)%3 != 0) {
                    jogo.mapa[i][j] = '-';
                } else {
                    jogo.mapa[i][j] = ' ';
                }
            }
        }
    }
    

    return jogo;
    
}
int ObtemStatusJogo(tJogo jogo){
    return jogo.status;
}
tJogo ProximoInstJogo(tJogo jogo){
    // Mudar posição dos carros
    int i_pistas = 0;

    for (i_pistas = 0; i_pistas < jogo.qtd_pistas; i_pistas++) {
        // Verificar se pista é vazia:
        if (ObtemDirecaoPista(jogo.pistas[i_pistas]) != '0') {
            jogo.pistas[i_pistas] = AndarCarrosPista(jogo.pistas[i_pistas], jogo.largura_mapa);
        }
    }
    
    return jogo;
}

tJogo FicarParadaGalinhaJogo(tJogo jogo){

    jogo = ProximoInstJogo(jogo);
    jogo = HouveColisaoGalinhaJogo(jogo, jogo.galinha);

    return jogo;

}

tJogo IrParaFrenteGalinhaJogo(tJogo jogo){
    tGalinha galinhaCopy = jogo.galinha;
    galinhaCopy.y -= 1;

    jogo = ProximoInstJogo(jogo);
    jogo = HouveColisaoGalinhaJogo(jogo, galinhaCopy);

    return jogo;
    
}

tJogo IrParaTrasGalinhaJogo(tJogo jogo) {
    tGalinha galinhaCopy = jogo.galinha;
    if (galinhaCopy.y < jogo.qtd_pistas-1) {
        galinhaCopy.y += 1;
    }

    jogo = ProximoInstJogo(jogo);
    jogo = HouveColisaoGalinhaJogo(jogo, galinhaCopy);

    return jogo;

}

tJogo HouveColisaoGalinhaJogo(tJogo jogo, tGalinha galinha){
    // Verifica e executa consequencias de colisão/não colisão

    int pistaGalinha = galinha.y;
    //printf("Pista galinha: %d\n", pistaGalinha);
    int i = 0, g_ocup = 0, c_ocup = 0, colisao = 0, colisao_carro = 0;

    int ocupacaoGalinha[3] = {galinha.x-1, galinha.x, galinha.x+1};
    //printf("Ocupação galinha: %d, %d, %d\n", galinha.x-1, galinha.x, galinha.x+1);
    int ocupacaoCarro[3];

    int qtd_carros = ObtemNumCarrosPista(jogo.pistas[pistaGalinha]);

    for (i = 0; i < qtd_carros; i++) {
        colisao_carro = 0;

        int ocupacaoCarro[3] = {ObtemPosicaoCarro(ObtemCarroPista(jogo.pistas[pistaGalinha], i))-1, ObtemPosicaoCarro(ObtemCarroPista(jogo.pistas[pistaGalinha], i)), ObtemPosicaoCarro(ObtemCarroPista(jogo.pistas[pistaGalinha], i))+1};
        //printf("Ocupação carro %d: %d, %d, %d\n", i, ObtemPosicaoCarro(ObtemCarroPista(jogo.pistas[pistaGalinha], i))-1, ObtemPosicaoCarro(ObtemCarroPista(jogo.pistas[pistaGalinha], i)), ObtemPosicaoCarro(ObtemCarroPista(jogo.pistas[pistaGalinha], i))+1);
        for (g_ocup = 0; g_ocup < 3; g_ocup++) {
            for (c_ocup = 0; c_ocup < 3; c_ocup++) {
                if (ocupacaoGalinha[g_ocup] == ocupacaoCarro[c_ocup]) {
                    // Printar no resumo
                    colisao_carro = 1;
                    //printf("Colisão do carro %d com Galinha. %d %d.\n", i, ocupacaoCarro[c_ocup], ocupacaoGalinha[g_ocup]);
                    break;
                }
                
            }

            if (colisao_carro == 1) {
                colisao = 1;
                break;
            }
        }
        
    }

    // Se colisao == 1: Houve colisão; Senão: Não houve colisão.

    if (colisao == 1) {
        galinha.vidas -= 1;
        galinha.y = jogo.qtd_pistas-1;

        if (ObtemVidasGalinha(galinha) == 0) {
            jogo.status = -1;
        }

        jogo.galinha = galinha;
        return jogo;
        
    } else {
        if (galinha.y == 0) {
            jogo.status = 1;
        }

        jogo.galinha = galinha;
        return jogo;
    }


    
}

// Funções de tPista:

int ObtemNumCarrosPista(tPista pista){
    return pista.num_carros;
}
char ObtemDirecaoPista(tPista pista){
    return pista.direcao;
}
tPista InstanciarPista(char direcao, int velocidade, int num_carros, tCarro carros[]){
    tPista pista;
    int i = 0;

    pista.direcao = direcao;
    pista.velocidade = velocidade;
    pista.num_carros = num_carros;

    for (i = 0; i < num_carros; i++) {
        pista.carros[i] = carros[i];
    }

    return pista;
}
tCarro ObtemCarroPista(tPista pista, int index){
    return pista.carros[index];
}
int ObtemPosicaoCarro(tCarro carro){
    return carro.posicao;
}
tPista AndarCarrosPista(tPista pista, int tam_pista){
    int i_carros = 0, movimento = 0, posicao = 0;
    if (pista.direcao == 'D') {
        movimento = pista.velocidade;
    } else if (pista.direcao == 'E') {
        movimento = -pista.velocidade;
    }
    
    for (i_carros = 0; i_carros < pista.num_carros; i_carros++) {
        posicao = pista.carros[i_carros].posicao + movimento;
        //printf("Posição1: %d\n", posicao);
        posicao = posicao%tam_pista;
        if (posicao < 0) {
            posicao = tam_pista + posicao;
        }
        //printf("Posição2: %d\n", posicao);
        pista.carros[i_carros].posicao = posicao;
    }

    return pista;
}

// Funções de tCarro:

tCarro InstanciarCarro(int posicao){
    tCarro carro;

    carro.posicao = posicao;

    return carro;

}

// Funções de tGalinha:

tGalinha InstanciarGalinha(int x, int y, int vidas){
    tGalinha galinha;

    galinha.x = x;
    galinha.y = y;
    galinha.vidas = vidas;

    return galinha;
}
int ObtemXGalinha(tGalinha galinha) {
    return galinha.x;
}
int ObtemYGalinha(tGalinha galinha){
    return galinha.y;
}
