/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
/* 
 * File:   person.h
 * Author: Rodrigo
 *
 * Created on 31 de Março de 2020, 20:08
 */

#ifndef PERSON_H
#define PERSON_H

#include "common.h"

enum health_state {
    Healthy = 'S', Sick = 'D', Immune = 'I'
};

typedef struct person Person, *pPerson;

struct person {
    char name[32];
    int age;
    enum health_state state;
    int days_sick;

    Person *next_person; // the person is part of a linked list so it needs a pointer to the next (at least)
};


void Person_print(Person *person);
void Person_imprime(FILE * output_stream, Person *person);
void Person_print_all(Person *person);

#endif /* PESSOA_H */

