/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
/* 
 * File:   startup.h
 * Author: Rodrigo
 *
 * Created on 31 de Março de 2020, 20:19
 */

#ifndef STARTUP_H
#define STARTUP_H

#include "lists.h"
#include "place.h"

PlaceList load_places(FILE *file);
PeopleListHead load_people(FILE *file);
void spread_people(PlaceList *places_list, PeopleListHead *people_list, PeopleListHead *people_list_not_simulating);

#endif /* STARTUP_H */

