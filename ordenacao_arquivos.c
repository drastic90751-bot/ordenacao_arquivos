/* ======================================================================
   Trabalho: Algoritmos de Ordenacao + Manipulacao de Arquivos em C
   Disciplina: Plano de aula 08 (arquivos), 09 (ponteiros),
               11 (ordenacao), 12 (switch)

   O programa:
     1) Cria e grava um arquivo com dados numericos desordenados
     2) Le esses dados do arquivo
     3) Permite escolher o tipo de ordenacao (Bubble, Insertion,
        Selection, Merge ou Quick Sort)
     4) Ordena os dados e salva o resultado em um novo arquivo
     5) Permite alterar um valor dentro do arquivo e reordenar
   ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARQ_ORIGINAL   "dados.txt"
#define ARQ_ORDENADO   "dados_ordenados.txt"
#define MAX_VALORES    100

/* ---------------------------------------------------------------------
   Prototipos das funcoes (todas trabalham com ponteiros, como pede o
   material de "ponteiros em c")
   --------------------------------------------------------------------- */
int  lerArquivoParaVetor(const char *nomeArquivo, int *vetor, int max);
void gravarVetorEmArquivo(const char *nomeArquivo, int *vetor, int n);
void gerarArquivoDesordenado(void);
void mostrarVetor(int *vetor, int n);

void bubbleSort(int *v, int n);
void insertionSort(int *v, int n);
void selectionSort(int *v, int n);
void mergeSort(int *v, int esq, int dir);
void merge(int *v, int esq, int meio, int dir);
void quickSort(int *v, int esq, int dir);
int  particiona(int *v, int esq, int dir);

void menuOrdenar(void);
void menuAlterarValor(void);
void trocar(int *a, int *b);

/* =======================================================================
   MAIN - menu principal (usa switch, conforme "Plano de aula 12")
   ======================================================================= */
int main(void) {
    int opcao;

    do {
        printf("\n=================================================\n");
        printf("   ORDENACAO + MANIPULACAO DE ARQUIVOS EM C\n");
        printf("=================================================\n");
        printf(" 1 - Criar/gravar arquivo com dados desordenados\n");
        printf(" 2 - Ler e mostrar dados do arquivo (%s)\n", ARQ_ORIGINAL);
        printf(" 3 - Ordenar dados e salvar em novo arquivo\n");
        printf(" 4 - Alterar um valor no arquivo e reordenar\n");
        printf(" 0 - Sair\n");
        printf("-------------------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            /* limpa entrada invalida */
            while (getchar() != '\n');
            opcao = -1;
        }

        switch (opcao) {
            case 1:
                gerarArquivoDesordenado();
                break;

            case 2: {
                int vetor[MAX_VALORES];
                int n = lerArquivoParaVetor(ARQ_ORIGINAL, vetor, MAX_VALORES);
                if (n > 0) {
                    printf("\nDados lidos de \"%s\":\n", ARQ_ORIGINAL);
                    mostrarVetor(vetor, n);
                }
                break;
            }

            case 3:
                menuOrdenar();
                break;

            case 4:
                menuAlterarValor();
                break;

            case 0:
                printf("\nEncerrando o programa...\n");
                break;

            default:
                printf("\nOpcao invalida! Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}

/* =======================================================================
   1) CRIA E GRAVA ARQUIVO COM DADOS NUMERICOS DESORDENADOS
   ======================================================================= */
void gerarArquivoDesordenado(void) {
    int n;
    int vetor[MAX_VALORES];

    printf("\nQuantos valores deseja gerar/gravar? (max %d): ", MAX_VALORES);
    scanf("%d", &n);

    if (n <= 0 || n > MAX_VALORES) {
        printf("Quantidade invalida.\n");
        return;
    }

    int escolha;
    printf("Gerar valores aleatoriamente (1) ou digitar manualmente (2)? ");
    scanf("%d", &escolha);

    if (escolha == 1) {
        srand((unsigned int) time(NULL));
        for (int i = 0; i < n; i++) {
            vetor[i] = rand() % 1000;  /* valores entre 0 e 999 */
        }
    } else {
        for (int i = 0; i < n; i++) {
            printf("Digite o valor [%d]: ", i + 1);
            scanf("%d", &vetor[i]);
        }
    }

    gravarVetorEmArquivo(ARQ_ORIGINAL, vetor, n);

    printf("\nArquivo \"%s\" criado/gravado com sucesso!\n", ARQ_ORIGINAL);
    printf("Dados gravados (desordenados): ");
    mostrarVetor(vetor, n);
}

/* =======================================================================
   Grava um vetor de inteiros em arquivo texto.
   Primeira linha = quantidade de elementos (facilita a leitura depois).
   ======================================================================= */
void gravarVetorEmArquivo(const char *nomeArquivo, int *vetor, int n) {
    FILE *fp = fopen(nomeArquivo, "w");
    if (fp == NULL) {
        printf("Erro ao abrir/criar o arquivo \"%s\".\n", nomeArquivo);
        return;
    }

    fprintf(fp, "%d\n", n);
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\n", *(vetor + i));  /* uso de ponteiro/aritmetica */
    }

    fclose(fp);
}

/* =======================================================================
   2) LE OS DADOS DO ARQUIVO PARA UM VETOR
   Retorna a quantidade de elementos lidos (0 se erro).
   ======================================================================= */
int lerArquivoParaVetor(const char *nomeArquivo, int *vetor, int max) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        printf("Erro: arquivo \"%s\" nao encontrado. Crie-o primeiro (opcao 1).\n",
               nomeArquivo);
        return 0;
    }

    int n;
    if (fscanf(fp, "%d", &n) != 1 || n <= 0 || n > max) {
        printf("Erro ao ler cabecalho do arquivo.\n");
        fclose(fp);
        return 0;
    }

    for (int i = 0; i < n; i++) {
        fscanf(fp, "%d", (vetor + i));
    }

    fclose(fp);
    return n;
}

void mostrarVetor(int *vetor, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", *(vetor + i));
    }
    printf("\n");
}

/* =======================================================================
   3) e 4) MENU DE ORDENACAO (switch) - le do arquivo original,
   ordena com o metodo escolhido e grava em novo arquivo
   ======================================================================= */
void menuOrdenar(void) {
    int vetor[MAX_VALORES];
    int n = lerArquivoParaVetor(ARQ_ORIGINAL, vetor, MAX_VALORES);
    if (n <= 0) return;

    printf("\nDados antes da ordenacao: ");
    mostrarVetor(vetor, n);

    int opcao;
    printf("\nEscolha o algoritmo de ordenacao:\n");
    printf(" 1 - Bubble Sort\n");
    printf(" 2 - Insertion Sort\n");
    printf(" 3 - Selection Sort\n");
    printf(" 4 - Merge Sort\n");
    printf(" 5 - Quick Sort\n");
    printf("Opcao: ");
    scanf("%d", &opcao);

    switch (opcao) {
        case 1:
            bubbleSort(vetor, n);
            printf("\n[Bubble Sort aplicado]\n");
            break;
        case 2:
            insertionSort(vetor, n);
            printf("\n[Insertion Sort aplicado]\n");
            break;
        case 3:
            selectionSort(vetor, n);
            printf("\n[Selection Sort aplicado]\n");
            break;
        case 4:
            mergeSort(vetor, 0, n - 1);
            printf("\n[Merge Sort aplicado]\n");
            break;
        case 5:
            quickSort(vetor, 0, n - 1);
            printf("\n[Quick Sort aplicado]\n");
            break;
        default:
            printf("\nOpcao invalida. Nenhuma ordenacao foi realizada.\n");
            return;
    }

    printf("Dados apos a ordenacao: ");
    mostrarVetor(vetor, n);

    gravarVetorEmArquivo(ARQ_ORDENADO, vetor, n);
    printf("Resultado salvo em \"%s\"\n", ARQ_ORDENADO);
}

/* =======================================================================
   5) ALTERAR UM VALOR NO ARQUIVO E REORDENAR
   ======================================================================= */
void menuAlterarValor(void) {
    int vetor[MAX_VALORES];
    int n = lerArquivoParaVetor(ARQ_ORIGINAL, vetor, MAX_VALORES);
    if (n <= 0) return;

    printf("\nDados atuais: ");
    mostrarVetor(vetor, n);

    int pos, novoValor;
    printf("\nDigite a posicao a alterar (1 a %d): ", n);
    scanf("%d", &pos);

    if (pos < 1 || pos > n) {
        printf("Posicao invalida.\n");
        return;
    }

    printf("Digite o novo valor: ");
    scanf("%d", &novoValor);

    *(vetor + (pos - 1)) = novoValor;  /* altera via ponteiro */

    /* grava a alteracao de volta no arquivo original */
    gravarVetorEmArquivo(ARQ_ORIGINAL, vetor, n);
    printf("\nValor alterado! Novo conteudo do arquivo \"%s\": ", ARQ_ORIGINAL);
    mostrarVetor(vetor, n);

    /* pede o metodo de ordenacao para reordenar em seguida */
    printf("\nAgora escolha como deseja reordenar os dados:\n");
    menuOrdenar();
}

/* =======================================================================
   ALGORITMOS DE ORDENACAO (Plano de aula 11)
   Todos recebem ponteiro para o vetor, conforme material de ponteiros
   ======================================================================= */

void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* --- Bubble Sort --- */
void bubbleSort(int *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (*(v + j) > *(v + j + 1)) {
                trocar((v + j), (v + j + 1));
            }
        }
    }
}

/* --- Insertion Sort --- */
void insertionSort(int *v, int n) {
    for (int i = 1; i < n; i++) {
        int chave = *(v + i);
        int j = i - 1;
        while (j >= 0 && *(v + j) > chave) {
            *(v + j + 1) = *(v + j);
            j--;
        }
        *(v + j + 1) = chave;
    }
}

/* --- Selection Sort --- */
void selectionSort(int *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        int menor = i;
        for (int j = i + 1; j < n; j++) {
            if (*(v + j) < *(v + menor)) {
                menor = j;
            }
        }
        if (menor != i) {
            trocar((v + i), (v + menor));
        }
    }
}

/* --- Merge Sort --- */
void merge(int *v, int esq, int meio, int dir) {
    int n1 = meio - esq + 1;
    int n2 = dir - meio;

    int *L = (int *) malloc(n1 * sizeof(int));
    int *R = (int *) malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = *(v + esq + i);
    for (int j = 0; j < n2; j++) R[j] = *(v + meio + 1 + j);

    int i = 0, j = 0, k = esq;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            *(v + k) = L[i];
            i++;
        } else {
            *(v + k) = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) { *(v + k) = L[i]; i++; k++; }
    while (j < n2) { *(v + k) = R[j]; j++; k++; }

    free(L);
    free(R);
}

void mergeSort(int *v, int esq, int dir) {
    if (esq < dir) {
        int meio = esq + (dir - esq) / 2;
        mergeSort(v, esq, meio);
        mergeSort(v, meio + 1, dir);
        merge(v, esq, meio, dir);
    }
}

/* --- Quick Sort --- */
int particiona(int *v, int esq, int dir) {
    int pivo = *(v + dir);
    int i = esq - 1;

    for (int j = esq; j < dir; j++) {
        if (*(v + j) <= pivo) {
            i++;
            trocar((v + i), (v + j));
        }
    }
    trocar((v + i + 1), (v + dir));
    return i + 1;
}

void quickSort(int *v, int esq, int dir) {
    if (esq < dir) {
        int p = particiona(v, esq, dir);
        quickSort(v, esq, p - 1);
        quickSort(v, p + 1, dir);
    }
}
