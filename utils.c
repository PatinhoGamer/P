// Trabalho Pratico Programacao - LEI
// DEIS-ISEC 2019-2020

/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */


#include <math.h>

#include "utils.h"

void initRandom() {
   srand(time(NULL));
}

int intUniformRnd(int a, int b) {
   return a + rand() % (b - a + 1);
}

int probEvent(float prob) {
   return prob > ((float) rand() / RAND_MAX);
}

void alloc_error(char *what) {
   printf("Erro a alocar memoria -> '%s'\nPrograma Terminado\n", what);
   exit(EXIT_FAILURE);
}

int get_user_number() {
   char str[16];
   fgets(str, 16, stdin);
   return atoi(str);
}