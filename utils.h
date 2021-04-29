// Trabalho Pratico Programacao - LEI
// DEIS-ISEC 2019-2020


/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */


#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "lists.h"
#include "place.h"
#include "person.h"

// Inicializa o gerador de numeros aleatorios.
// Esta funcao deve ser chamada apenas uma vez no inicio da execucao do programa
void initRandom();

//Devolve um valor inteiro aleatorio distribuido uniformemente entre [a, b]
int intUniformRnd(int a, int b);

// Devolve o valor 1 com probabilidade prob. Caso contrario, devolve 0
int probEvent(float prob);

void alloc_error(char *what);
int get_user_number();


#endif /* UTILS_H */
