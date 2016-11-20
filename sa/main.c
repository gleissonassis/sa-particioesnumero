#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


#define copiar_array(DST,SRC,LEN) \
{ size_t TMPSZ = sizeof(*(SRC)) * (LEN); \
if ( ((DST) = malloc(TMPSZ)) != NULL ) \
memcpy((DST), (SRC), TMPSZ); }

struct solucao {
    int tamanho;
    int *numeros;
    int *representacao;
};

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

int calcular_peso_particao(int *particao, int tamanho) {
    int peso = 0;
    
    for(int i = 0; i < tamanho; i++) {
        peso += particao[i];
    }
    
    return peso;
}

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

void imprimir_solucao(struct solucao s) {
    printf("Numeros = ");
    imprimir_particao(s.numeros, s.tamanho);
    printf("Representacao = ");
    imprimir_particao(s.representacao, s.tamanho);
    printf("Valor = %d\r\n", calcular_valor_solucao(s.numeros, s.representacao, s.tamanho));
    
}

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



int* sa(double a, double T0, struct solucao solucao, int SAmax) {
    int* s_estrela;
    int* s;
    int* sl;
    double x;
    double e = 2.71828;
    
    int IterT = 0;
    double T = T0;
    double delta = 0;
    
    copiar_array(s, solucao.representacao, solucao.tamanho);
    copiar_array(s_estrela, s, solucao.tamanho);
    
    while(T > 0.0001) {
        while(IterT < SAmax) {
            IterT = IterT + 1;
            
            sl = gerar_vizinho_aleatorio(solucao);
            
            int fsl = calcular_valor_solucao(solucao.numeros, sl, solucao.tamanho);
            int fs = calcular_valor_solucao(solucao.numeros, s, solucao.tamanho);
            delta = fsl - fs;
            
            if(delta < 0) {
                copiar_array(s, sl, solucao.tamanho);
                copiar_array(solucao.representacao, s, solucao.tamanho);
                
                int fs_estrela = calcular_valor_solucao(solucao.numeros, s_estrela, solucao.tamanho);
                
                if(fsl < fs_estrela) {
                    copiar_array(s_estrela, sl, solucao.tamanho);
                }
            } else {
                x = (double)rand() / (double)RAND_MAX;
                double tmp1 = -delta / T;
                double tmp = pow(e, tmp1);
                
                if(x < tmp) {
                    copiar_array(s, sl, solucao.tamanho);
                }
            }
        }
        
        IterT = 0;
        T = T - (a * T);
    }
    
    copiar_array(sl, s, solucao.tamanho);
    
    return s;
}


int main(int argc, const char * argv[]) {
    srand(10);
    struct solucao s;
    
    ler_arquivo(&s, "/Users/gleissonassis/Documents/Mestrado/HelloWorldC/HelloWorldC/Particao-200-4-150.txt");
    construir_solucao_inicial(&s);
    
    imprimir_solucao(s);
    
    s.representacao = sa(0.0001, 10000, s, 1);
    
    imprimir_solucao(s);
    
    //s.representacao = calcular_melhor_vizinho(s);
    
    //
}
