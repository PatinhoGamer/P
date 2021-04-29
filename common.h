/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
/* 
 * File:   common.h
 * Author: Rodrigo
 *
 * Created on 3 de Abril de 2020, 20:38
 */

#ifndef COMMON_H
#define COMMON_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_PATH 256

#define SIMULATE 1
#define SHOW_STATS 2
#define ADD_SICK 3
#define TRANSFER_PEOPLE 4
#define GO_BACK 5
#define EXIT_SAVE 6


// Taxa de disseminação
#define SPREAD_RATE 0.75
// Probabilidade de recuperação
#define PROB_RECOV(age) 1.0/age
// Duração máxima da infeção
#define MAX_SICK_DAYS(age) floor(age/10) + 5
// Taxa de imunidade
#define IMMUNITY_PROB 0.2

#define MAXSAVEDITERATIONS 3

#define INITIALLISTSIZE 4
#define LISTEXPANDMULTIPLIER 2

#endif /* COMMON_H */

