#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <math.h>
#include <inttypes.h>

int n; // dim
int rainha; // queens
int extTh; // threads completas
unsigned long long somaFinal;
unsigned long possibilidades;

typedef struct info{
	int nDaThread;
	pthread_mutex_t *mutex;
	pthread_mutex_t *saida;
	int *posRainhas; // posicao a comecar para a proxima rainha
}info;

void* possibilidade(void* varThreads);
unsigned long long calculaPeso(int y, int x);
unsigned long long findQueens(char** tabuleiro);
int modulo(int num);
char** criaTabuleiro();
int findX(int pos);
int findY(int pos);
int findPlace(char** tabuleiro, int posIn);
void resetaTabuleiro(char** matriz);
int removeRainha(char** tabuleiro, int posRem);
void printTab(char** tab, int dim);
void placeQueen(char** tabuleiro, int y, int x);
unsigned long long nqueens(int dim, int queens);

//imprime tabuleiro (apenas pra teste)
void printTab(char** tab, int dim){
	int i, j;

	for(i = 0; i < dim; i++){
		printf("\n");
		for(j = 0; j < dim; j++){
			printf("%3.d  ", tab[i][j]);
		}
	}
}

/*
nqueens(int dim, int queens)
Para um tabuleiro de dim x dim, encontrar todas as combinações possiveis para "queens" rainhas para calcular o valor total, sendo que, cada combinação corresponde a um valor.
para cada rainha posicionada em uma casa, seu valor é 2^posicao, sendo que a posição é incrementada horizontalemte e depois verticalmente. EX: pos(2,3) = 2^(2 + 3*dim)
*/

// funcao principal
unsigned long long nqueens(int dim, int queens){

	// variaveis
	int i, j;
	pthread_t *thread;
	pthread_mutex_t mutex;
	pthread_mutex_t saida;
	info *varThreads;
	n = dim;
	rainha = queens;
	somaFinal = 0;
  	possibilidades = 0;
  	extTh = 0;

	// testes iniciais
	if(queens > dim){ // mais rainhas que linhas
		return 0;
	}
	if(queens < 1){
		return 0;
	}
	if(dim < 1){
		return 0;
	}

	// inicializacoes
	thread = malloc(dim * dim * sizeof(pthread_t));
	varThreads = malloc(dim * dim * sizeof(info));
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&saida, NULL);

	//inicializar varThreads
	for(i = 0; i < (dim * dim); i++){
		varThreads[i].nDaThread = i;
		varThreads[i].mutex = &mutex;
		varThreads[i].saida = &saida;
		varThreads[i].posRainhas = malloc(queens * sizeof(int));
		for(j = 0; j < rainha; j++){
			varThreads[i].posRainhas[j] = 0;
		}
		pthread_create(&thread[i], NULL, possibilidade, (void*)&varThreads[i]);
	}

	// running
	for(i = 0; i < (dim * dim); i++){
		pthread_join(thread[i], NULL);
	}
	// espera todas threads terminarem
	while(1){
		if(extTh == (dim * dim)){
			return somaFinal;
		}
	}
}

// encontra uma casa para colocar rainha
int findPlace(char** tabuleiro, int posIn){

	int i = findY(posIn);
	int j = findX(posIn);

	for( ; i < n; i++){
		if(i > findY(posIn)){
			j = 0;
		}
		for( ; j < n; j++){ // começa depois de colocar rainha.......depois juntar os 3 ifs
			if(tabuleiro[i][j] == '0'){
				return (i * n + j);
			}
		}
	}
	return 0;
}

// pra testar nova possiblidade, remove a rainha a ser alterada
int removeRainha(char** tabuleiro, int posRem){

	int i, j;
	int y = findY(posRem);
	int x = findX(posRem);

	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++){ // começa depois de colocar rainha.......depois juntar os 3 ifs
			if(i == y){
				tabuleiro[i][j] --; // mesma linha
			}else if(j == x){
				tabuleiro[i][j] --; // mesma coluna
			}else if(modulo(i - y) == modulo(x - j)){
				tabuleiro[i][j] --; // mesma diagonal
			}
		}
	}
	tabuleiro[y][x] = '0';

	return 1;
}

// encontra coordenada x
int findX(int pos){
	return pos % n;
}

// encontra coordenada y
int findY(int pos){
	return pos / n;
}

// gera o tabuleiro
char** criaTabuleiro(){
	char** matriz;
	int i;
	matriz = malloc(n * sizeof(char*));
	for(i = 0; i < n; i++){
		matriz[i] = malloc(n * sizeof(char));
	}
	return matriz;
}

// funcao das threads
void* possibilidade(void* varThreads){
	// verifica conteudo na posicao, se livre, poe rainha, marca posicoes, bloqueia buffer, atualiza resultado, desbloqueia buffer
	info * conteudo = (info*)varThreads;
	char** tabuleiro;
	int existePos = 1;
	int rainhaAtual = 1;
	unsigned long long parcial = 0;

	tabuleiro = criaTabuleiro();
	resetaTabuleiro(tabuleiro);

	conteudo->posRainhas[0] = conteudo->nDaThread;
	placeQueen(tabuleiro, findY(conteudo->nDaThread), findX(conteudo->nDaThread));

	while(existePos){
		while(rainhaAtual < rainha){ // busca proxima pos
			if(conteudo->posRainhas[rainhaAtual] != 0){ // se já executou alguma vez nessa rainha
				conteudo->posRainhas[rainhaAtual] = findPlace(tabuleiro, conteudo->posRainhas[rainhaAtual] + 1);
			}
			else{ // se mudou de rainha
				conteudo->posRainhas[rainhaAtual] = findPlace(tabuleiro, conteudo->posRainhas[rainhaAtual - 1] + 1);
			}
			if(conteudo->posRainhas[rainhaAtual] == 0){ // se não achou espaço livre
				existePos = 0;
				break;
			}
			else{ // coloca rainha e vai para a proxima posicao
				placeQueen(tabuleiro, findY(conteudo->posRainhas[rainhaAtual]), findX(conteudo->posRainhas[rainhaAtual]));
				rainhaAtual ++;
			}
		}
		rainhaAtual --;
		if(existePos){ // achou combinacao, calcula resultado
            parcial += findQueens(tabuleiro);
			pthread_mutex_lock(conteudo->mutex);
 			possibilidades++;
 			somaFinal += parcial;
			pthread_mutex_unlock(conteudo->mutex);
			if(rainha == 1){
				break;
			}
		}
		else{
            if(rainhaAtual > 0){
                existePos = 1;
            }
		}
		removeRainha(tabuleiro, conteudo->posRainhas[rainhaAtual]);
	}
	pthread_mutex_lock(conteudo->saida);
	extTh ++; // atualiza numero de threads encerradas
	pthread_mutex_unlock(conteudo->saida);
	return (void*) 0; // não vai haver mais rainhas naquela coluna
}

// peso na rainha naquela casa
unsigned long long calculaPeso(int y, int x){
	return pow(2, n * y + x);
}

// localiza todas rainhas no tabuleiro peso total
unsigned long long findQueens(char** tabuleiro){
	int i, j;
	unsigned long long resultado = 0;
	for(i = 0; i < n; i++){
		for(j = 0;  j < n; j++){
			if(tabuleiro[i][j] == 'x'){ //rainha
				resultado += calculaPeso(i, j);
			}
		}
	}
	return resultado;
}

// limpa o tabuleiro
void resetaTabuleiro(char **matriz){
	int i, j;
	char *aux;
	for(i = 0; i < n; i++){
		aux = matriz[i];
		for(j = 0; j < n; j++){
			aux[j] = '0'; //48
		}
	}
}

// marca x na rainha, e i nas casas ameaçadas
void placeQueen(char** tabuleiro, int y, int x){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++){ // começa depois de colocar rainha.......depois juntar os 3 ifs
			if(i == y){
				tabuleiro[i][j] ++; // mesma linha
			}else if(j == x){
				tabuleiro[i][j] ++; // mesma coluna
			}else if(modulo(i - y) == modulo(x - j)){
				tabuleiro[i][j] ++; // mesma diagonal
			}
		}
	}
	tabuleiro[y][x] = 'x';
}

// calcula módulo do numero
int modulo(int num){
	if(num < 0){
		return num = num * (-1);
	}else{
		return num;
	}
}
