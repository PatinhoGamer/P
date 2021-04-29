/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
/* 
 * File:   lists.h
 * Author: Rodrigo
 *
 * Created on 3 de Abril de 2020, 20:21
 */

#ifndef LISTS_H
#define LISTS_H

#include "common.h"
#include "place.h"
#include "person.h"


typedef struct people_list_head PeopleListHead, *pPeopleListHead;

struct people_list_head {
    Person *first_person; // pointer to the first person on the list
    int size; // total number of linked people
};


void PlaceList_print(PlaceList *list);
void PlaceList_imprime(PlaceList * list);

// adds new Place to PlacesList
int PlacesList_add(PlaceList *list, PlaceFile * new_place);
// adds new Person * to PeopleList(used in people_inside, that is found on Place)
int PeopleList_add(PeopleList *list, Person *new_elem);
// puts new element on the beginning of the linked list
void PeopleListHead_add(PeopleListHead *list, Person *new_elem);

// expands list so it can fit list->capacity * LISTEXPANDMULTIPLIER elements
int PlaceList_expand(PlaceList *list);
// expands list so it can fit list->capacity * LISTEXPANDMULTIPLIER elements
int PeopleList_expand(PeopleList *list);

//removes element at index
int PeopleList_removeAt(PeopleList *list, int index);

//"clears" list. just changes apparent size
int PeopleList_clear(PeopleList *list);

// cuts unused space of array, list->capacity becomes list->size, freeing some memory
int PlaceList_shrink(PlaceList *list);


//deletes from memory everything that is inside
void free_places(PlaceList *list);
//deletes from memory everything that is inside
void free_people(PeopleListHead *list);

#endif /* LISTS_H */

