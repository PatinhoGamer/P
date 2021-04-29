/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
#ifndef LISTS_C
#define LISTS_C

#include "lists.h"
#include "utils.h"

void PlaceList_print(PlaceList * list) {
   for (int i = 0; i < list->size; i++)
      Place_print(&list->arr[i]);
}

void PlaceList_imprime(PlaceList * list) {
   for (int i = 0; i < list->size; i++)
      Place_imprime(stdout, &list->arr[i]);
}

int PeopleList_add(PeopleList *list, Person *new_elem) {
   int error = 0;
   if (list->size == list->capacity)
      error = PeopleList_expand(list);

   list->arr[list->size] = new_elem;
   list->size++;
   return error;
}

void PeopleListHead_add(PeopleListHead *list, Person *new_elem) {
   new_elem->next_person = list->first_person;
   list->first_person = new_elem;
   list->size++;
}

int PlacesList_add(PlaceList *list, PlaceFile *new_struct) {
   int error = 0;
   if (list->size == list->capacity)
      error = PlaceList_expand(list);

   // copy all variables
   pPlace new_place = &list->arr[list->size];
   new_place->id = new_struct->id;
   new_place->capacity = new_struct->capacity;
   memcpy(new_place->connects, new_struct->connects, sizeof (new_place->connects));

   list->size++;

   return error;
}

int PeopleList_removeAt(PeopleList *list, int index) {
   if (index > list->size) return 1;
   //get the person from the end to the place where we took the other from
   list->arr[index] = list->arr[list->size - 1];
   //decrease apparent size
   list->size--;
   return 0;
}

int PeopleList_clear(PeopleList *list) {
   list->size = 0;
}

int PlaceList_expand(PlaceList *list) {
   int new_capacity = list->capacity * LISTEXPANDMULTIPLIER;
   void* new_ptr = realloc(list->arr, new_capacity * sizeof (Place));
   if (new_ptr == NULL) {
      printf("Erro a alocar memoria!! Expancao abortada\n");
      return -1;
   }
   list->arr = new_ptr;
   list->capacity = new_capacity;
   return 0;
}

int PeopleList_expand(PeopleList *list) {
   int new_capacity = list->capacity * LISTEXPANDMULTIPLIER;
   void* new_ptr = realloc(list->arr, new_capacity * sizeof (Person *));
   if (new_ptr == NULL) {
      printf("Erro a alocar memoria!! Expancao abortada\n");
      return -1;
   }
   list->arr = new_ptr;
   list->capacity = new_capacity;
   return 0;
}

int PlaceList_shrink(PlaceList *list) {
   int new_capacity = list->size;
   if (new_capacity == 0) {
      return 0;
   }
   void* new_ptr = realloc(list->arr, new_capacity * sizeof (Place));
   if (new_ptr == NULL) {
      printf("Erro a alocar memoria!! Tarefa abortada\n");
      return -1;
   }
   list->arr = new_ptr;
   list->capacity = new_capacity;
   return 0;
}

void free_places(PlaceList *list) {
   // free all arrays of people that are inside the places
   for (int i = 0; i < list->size; i++)
      free(list->arr[i].people_inside.arr);
   // free the places struct array
   free(list->arr);
}

void free_people(PeopleListHead *list) {
   pPerson curr_person = list->first_person;
   while (curr_person != NULL) {
      // get a pointer to a person grab the next one and delete the previous one
      pPerson previous_person = curr_person;
      curr_person = curr_person->next_person;
      free(previous_person);
   }
}


#endif