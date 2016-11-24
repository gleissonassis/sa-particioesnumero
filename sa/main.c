#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


#define copiar_array(DST,SRC,LEN) \
{ size_t TMPSZ = sizeof(*(SRC)) * (LEN); \
if ( ((DST) = malloc(TMPSZ)) != NULL ) \
memcpy((DST), (SRC), TMPSZ); }

/*
 * Struct: solucao
 * -----------------------------------------------------------------------------
 *   Representação de uma solução factível para o problema de Partição de Números.
 
 *   tamanho: tamanho do vetor.
 *   numeros: vetor contendo os números que serão particionados.
 *   representacao: vetor que representa um particionamento.
 */
struct solucao {
    int tamanho;
    int *numeros;
    int *representacao;
};

void selection_sort(int *array, int n);
void ler_arquivo(struct solucao *s, char arquivo[2000]);
void imprimir_particao(int *p, int tamanho);
void imprimir_solucao(struct solucao s);

void construir_solucao_inicial(struct solucao *s);
int calcular_valor_solucao(int *numeros, int *representacao, int tamanho);
int* calcular_melhor_vizinho(struct solucao s);
int* gerar_vizinho_aleatorio(struct solucao s);
int* sa(double a, double T0, struct solucao solucao, int SAmax);


// -----------------------------------------------------------------------------
//
//   Bloco de funções auxiliares para resolução do problema.
//
// -----------------------------------------------------------------------------

/*
 * Function: selection_sort
 * -----------------------------------------------------------------------------
 *   Ordena de forma descendente um vetor passado como parâmetro, pois para a 
 *   implementação do problema é necessário que os números sejam computados de 
 *   forma descendente. Como o vetor é passado por ponteiro ele é alterado 
 *   diretamente, ou seja, a função não retorna um vetor ordenado e sim ordena
 *   diretamente o vetor informado como parâmetro.
 *
 *
 *   array: ponteiro para o vetor de inteiros.
 *   n: tamanho do vetor.
 */
void selection_sort(int *array, int n) {
    int i, j;
    int min;
    int temp;
    
    for(i = 0; i < n - 1; i++) {
        min=i;
        for(j = i + 1; j < n; j++) {
            if(array[j] > array[min]) {
                min = j;
            }
        }
        
        temp = array[i];
        array[i] = array[min];
        array[min] = temp;
    }
}


/*
 * Function: ler_arquivo
 * -----------------------------------------------------------------------------
 *   Função auxiliar utilizada para imprimir no console uma solução.
 *
 *   s: ponteiro para a estrutura que representa uma construção de solução para 
 *   o problema.
 *   arquivo: caminho para o arquivo
 */
void ler_arquivo(struct solucao *s, char arquivo[2000]) {
    FILE* fp;
    int i, t;
    char c;
    
    fp = fopen(arquivo, "r");
    
    fscanf(fp, "%d\r\n", &t);
    fscanf(fp, "%c\r\n", &c);
    fscanf(fp, "%d\n", &s->tamanho);
    
    s->numeros = malloc(s->tamanho * sizeof(int));
    
    fscanf(fp, "%c\r\n", &c);
    fscanf(fp, "%d\r\n", &t);
    fscanf(fp, "%c\r\n", &c);
    
    for(i = 0; i < s->tamanho; i++) {
        fscanf(fp, "%d\r\n", &s->numeros[i]);
    }
}

/*
 * Function: imprimir_particao
 * -----------------------------------------------------------------------------
 *   Função auxiliar utilizada para imprimir no console uma representação de
 *   partição o componente representação da estrutura solução.
 *
 *   p: representação em vetor de inteiros de uma partição.
 *   tamanho: tamanho da partição.
 */
void imprimir_particao(int *p, int tamanho) {
    printf("{");
    for(int i = 0; i < tamanho; i++) {
        if(p[i] != 0) {
            printf("%d", p[i]);
            
            if(i < tamanho - 1 && p[i+1] != 0) {
                printf(",");
            }
        }
    }
    printf("};\r\n");
}


/*
 * Function: imprimir_solucao
 * -----------------------------------------------------------------------------
 *   Função auxiliar utilizada para imprimir no console uma solução.
 *
 *   s: uma solução do problema contendo os componentes da estrutura solução.
 */
void imprimir_solucao(struct solucao s) {
    printf("Numeros = ");
    imprimir_particao(s.numeros, s.tamanho);
    printf("Representacao = ");
    imprimir_particao(s.representacao, s.tamanho);
    printf("Valor = %d\r\n", calcular_valor_solucao(s.numeros, s.representacao, s.tamanho));
    
}

// -----------------------------------------------------------------------------
//
//   Bloco de funções diretamente ligadas à resolução do problema.
//
// -----------------------------------------------------------------------------

/*
 * Function: construir_solucao_inicial
 * -----------------------------------------------------------------------------
 *   A função constroi uma solução inicial factível para o problema de Partição 
 *   de Números e atualiza o componente representação da estrutura solução.
 *
 *   s: estrutura do problema contendo os números e o tamanho da instância.
 */
void construir_solucao_inicial(struct solucao *s) {
    s->representacao = malloc(s->tamanho * sizeof(int));
    selection_sort(s->numeros, s->tamanho);
    
    int peso_p1 = 0;
    int peso_p2 = 0;
    
    for(int i = 0; i < s->tamanho; i++) {
        if(peso_p1 <= peso_p2) {
            peso_p1 += s->numeros[i];
            s->representacao[i] = 1;
        } else {
            peso_p2 += s->numeros[i];
            s->representacao[i] = 2;
        }
    }
}

/*
 * Function: calcular_valor_solucao
 * -----------------------------------------------------------------------------
 *   Calcula o valor de uma solução que é a diferença absoluta entre as duas 
 *   partições.
 *
 *   p: representação em vetor de inteiros de uma partição.
 *   tamanho: tamanho da partição.
 */
int calcular_valor_solucao(int *numeros, int *representacao, int tamanho) {
    int valor_p1 = 0;
    int valor_p2 = 0;
    
    for(int i = 0; i < tamanho; i++) {
        if(representacao[i] == 1) {
            valor_p1 += numeros[i];
        } else {
            valor_p2 += numeros[i];
        }
    }
    
    return abs(valor_p1 - valor_p2);
}




/*
 * Function: calcular_melhor_vizinho
 * -----------------------------------------------------------------------------
 *   Função que explora a vizinhanção de uma solução realizando uma busca local 
 *   do tipo Best Improvement.
 *
 *   s: estrutura que representa uma construção de solução para o problema.
 */
int* calcular_melhor_vizinho(struct solucao s) {
    int *r;
    int *melhor_vizinho;
    int melhor_avaliacao = calcular_valor_solucao(s.numeros, s.representacao, s.tamanho);
    int tmp;
    //printf("Movimentos de mudanca de particao\r\n\r\n");
    
    copiar_array(melhor_vizinho, s.representacao, s.tamanho);
    
    for(int i = 0; i < s.tamanho; i ++) {
        copiar_array(r, s.representacao, s.tamanho);
        
        r[i] = r[i] == 1 ? 2 : 1;
        
        int valor_vizinnho = calcular_valor_solucao(s.numeros, r, s.tamanho);
        
        //imprimir_particao(r, s.tamanho);
        //printf("=%d\r\n", valor_vizinnho);
        
        if(valor_vizinnho < melhor_avaliacao) {
            copiar_array(melhor_vizinho, r, s.tamanho);
            melhor_avaliacao = valor_vizinnho;
        }
        
        free(r);
    }
    
    //printf("Movimentos de troca de particao\r\n\r\n");
    
    for(int i = 0; i < s.tamanho; i ++) {
        for(int j = i + 1; j < s.tamanho; j++) {
            copiar_array(r, s.representacao, s.tamanho);
            
            tmp = r[i];
            r[i] = r[j];
            r[j] = tmp;
            
            int valor_vizinnho = calcular_valor_solucao(s.numeros, r, s.tamanho);
            
            //imprimir_particao(r, s.tamanho);
            //printf("=%d\r\n", valor_vizinnho);
            
            if(valor_vizinnho < melhor_avaliacao) {
                copiar_array(melhor_vizinho, r, s.tamanho);
                melhor_avaliacao = valor_vizinnho;
            }
            
            free(r);
        }
    }
    
    return melhor_vizinho;
}

/*
 * Function: gerar_vizinho_aleatorio
 * -----------------------------------------------------------------------------
 *   Função que perturba uma solução com o objetivo de exporar de forma melhor 
 *   o espaço de soluções.
 *
 *   s: estrutura que representa uma construção de solução para o problema.
 */
int* gerar_vizinho_aleatorio(struct solucao s) {
    int *r;
    int tmp;
    
    copiar_array(r, s.representacao, s.tamanho);
    
    if(rand() % 50 > 25) {
        int posicao = rand() % s.tamanho;
        
        r[posicao] = r[posicao] == 1 ? 2 : 1;
    } else {
        int i = rand() % s.tamanho;
        int j = rand() % s.tamanho;
        
        tmp = r[i];
        r[i] = r[j];
        r[j] = tmp;
    }
    
    return r;
}


/*
 * Function: sa
 * -----------------------------------------------------------------------------
 *   Implementação da metaheurística Simulated Annealing.
 *
 *   a: fator de redução de temperatura.
 *   T0: temperatura inicial.
 *   s: estrutura que representa uma construção de solução para o problema.
 *   SAmax: quantidade de perturbações em uma iteração do Simulated Annealing
 *
 *   returns: um ponteiro para um vetor contendo uma representaçao de soluçao
 */
int* sa(double a, double T0, struct solucao solucao, int SAmax) {
    int* s_estrela;
    int* s;
    int* sl;
    
    int IterT = 0;
    double T = T0;
    double delta = 0;
    
    copiar_array(s, solucao.representacao, solucao.tamanho);
    copiar_array(s_estrela, s, solucao.tamanho);
    
    while(T > 0.0001) {
        while(IterT < SAmax) {
            IterT = IterT + 1;
            
            //A geração de um vizinho aleatório simula a perturbação de uma
            //solução
            sl = gerar_vizinho_aleatorio(solucao);
            
            int fsl = calcular_valor_solucao(solucao.numeros, sl, solucao.tamanho);
            int fs = calcular_valor_solucao(solucao.numeros, s, solucao.tamanho);
            delta = fsl - fs;
            
            //se a solução tem um custo melhor que a solução corrente ela é
            //automaticamente aceita. Observe que a solução corrente não é
            //necessáriamente a melhor solução (s_estrela)
            if(delta < 0) {
                copiar_array(s, sl, solucao.tamanho);
                copiar_array(solucao.representacao, s, solucao.tamanho);
                
                int fs_estrela = calcular_valor_solucao(solucao.numeros, s_estrela, solucao.tamanho);
                
                //Se a solução corrente é melhor de todos até o momento ela
                //passa ser s_estrela
                if(fsl < fs_estrela) {
                    copiar_array(s_estrela, sl, solucao.tamanho);
                }
            } else {
                double x = rand() / RAND_MAX;
                
                //aceitando uma solução de piora dada a função de probabilidade
                if(x < pow(M_E, -delta / T)) {
                    copiar_array(s, sl, solucao.tamanho);
                }
            }
        }
        
        //Reduzindo a temperatura através do fator de redução e zerando a contagem
        //de perturbações
        IterT = 0;
        T = T - (a * T);
    }
    
    copiar_array(s, s_estrela, solucao.tamanho);
    
    return s;
}


int main(int argc, const char * argv[]) {
    //iniciando a geração de números aleatorios com uma semente fixa para
    //possibilitar repetição de resultados nas diversas execuções do programa
    srand(10);
    
    struct solucao s;
    
    ler_arquivo(&s, "/Users/gleissonassis/Documents/Mestrado/HelloWorldC/HelloWorldC/Particao-200-4-150.txt");
    construir_solucao_inicial(&s);
    
    imprimir_solucao(s);
    
    s.representacao = sa(0.0001, 10000, s, 10);
    
    imprimir_solucao(s);
    
    //s.representacao = calcular_melhor_vizinho(s);
}
