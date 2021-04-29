/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
/* 
 * File:   place.h
 * Author: Rodrigo
 *
 * Created on 31 de Março de 2020, 20:07
 */

#ifndef PLACE_H
#define PLACE_H

#include "common.h"
#include "person.h"

typedef struct people_list PeopleList, *pPeopleList;

struct people_list {
    pPerson *arr; // pointer to the array of something depending on the use
    int size; // current number of elements
    int capacity; // number of elements this list can take before needing to realloc
};

typedef struct place Place, *pPlace;

// Struct used in the execution to include the people that are inside

struct place {
    int id;
    int capacity;
    int connects[3]; // ids of places. -1 on unused
    PeopleList people_inside;
};

// Struct that comes in the binary files

typedef struct place_file {
    int id;
    int capacity;
    int connects[3]; // ids of places. -1 on unused
} PlaceFile;


typedef struct place_list PlaceList, *pPlaceList;

struct place_list {
    Place *arr; // pointer to the array of something depending on the use
    int size; // current number of elements
    int capacity; // number of elements this list can take before needing to realloc
};


void Place_print(Place *place); // Prints information
void Place_imprime(FILE * output_stream, Place *place); // prints information in Portuguese. Added later but no point in deleting the english one

// returns the place or NULL if doesn't exist
Place* Place_get_by_id(PlaceList *list, int id);
// returns 1 if place1 is connected to place2, else returns 0
int Place_is_connected(Place *place1, Place *place2);

#endif /* LOCAL_H */

