/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */

#include "place.h"

void Place_print(Place *place) {
   printf("Place {id: %d, capacity: %d, connections: %d %d %d, \n\tpeople_inside: {",
	   place->id, place->capacity, place->connects[0], place->connects[1], place->connects[2]);
   pPeopleList list = &place->people_inside;
   for (int i = 0; i < list->size; i++) {
      printf("name : '%s',", list->arr[i]->name);
   }
   printf("}\n");
}

void Place_imprime(FILE * output_stream, Place *place) {
   fprintf(output_stream, "id: %d, numero de pessoas: %d, capacidade: %d, ligado: %d %d %d\n",
	   place->id, place->people_inside.size, place->capacity, place->connects[0], place->connects[1], place->connects[2]);
}

Place* Place_get_by_id(PlaceList *list, int id) {
   for (int i = 0; i < list->size; i++)
      if (id == list->arr[i].id)
	 return (list->arr + i);
   return NULL;
}

int Place_is_connected(Place *place1, Place *place2) {
   for (int i = 0; i < 3; i++)
      if (place1->connects[i] == place2->id)
	 return 1;
   return 0;
}