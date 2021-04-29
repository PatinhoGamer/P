/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */

#include "startup.h"
#include "utils.h"
#include "main.h"

PlaceList load_places(FILE *file) {

   PlaceFile buffer;
   PlaceList places_list;

   places_list.arr = malloc(INITIALLISTSIZE * sizeof (Place));
   if (places_list.arr == NULL) {
      fclose(file);
      alloc_error("placelist->arr");
   }
   places_list.capacity = INITIALLISTSIZE;
   places_list.size = 0;

   while (1) {
      int read_amount = fread(&buffer, sizeof (PlaceFile), 1, file);
      if (read_amount == 0)
	 break;
      PlacesList_add(&places_list, &buffer);

   }
   fclose(file);

   if (places_list.size == 0) {
      printf("Ficheiro de locais invalido\n");
      exit(EXIT_FAILURE);
   }

   // allocate space for people that are going to be inside
   for (int i = 0; i < places_list.size; i++) {
      pPeopleList people_list = &places_list.arr[i].people_inside;
      people_list->arr = malloc(INITIALLISTSIZE * sizeof (Person *));
      if (people_list->arr == NULL) {
	 alloc_error("peoplelist->arr");
      }
      people_list->capacity = INITIALLISTSIZE;
      people_list->size = 0;
   }

   // free unused bytes (this is static information for the rest of the execution)
   PlaceList_shrink(&places_list);

   //Verify connections
   for (int i = 0; i < places_list.size; i++) {
      pPlace place = &places_list.arr[i];

      for (int j = 0; j < 3; j++) {
	 if (i != j && place->id == places_list.arr[j].id) {
	    printf("Local com IDs repetidos '%d'\n", place->id);
	    exit(EXIT_FAILURE);
	 }
      }

      if (Place_is_connected(place, place) == 1) {
	 printf("Local com ID '%d' esta ligado a si proprio\n", place->id);
	 exit(EXIT_FAILURE);
      }

      for (int j = 0; j < 3; j++) {
	 int other_id = place->connects[j];
	 if (other_id == -1)
	    continue;

	 // check if it has repeated connections
	 for (int k = 0; k < 3; k++) {
	    if (j != k && place->connects[j] == place->connects[k]) {
	       printf("Local com ID '%d' tem conexoes repetidas\n", place->id);
	       exit(EXIT_FAILURE);
	    }
	 }

	 if (place->capacity <= 0) {
	    printf("Local com ID '%d' tem capacidade 0 ou menor", place->id);
	    exit(EXIT_FAILURE);
	 }

	 pPlace otherPlace = Place_get_by_id(&places_list, other_id);
	 if (otherPlace == NULL) {
	    printf("Ficheiro Binario Inconsistente. Local com IDs '%d' nao existe e esta ligado ao '%d'\n", other_id, place->id);
	    exit(EXIT_FAILURE);
	 }
	 if (Place_is_connected(otherPlace, place) == 0) { // if place gotten by Id is not connected to this one
	    printf("Ficheiro Binario Inconsistente. Locais com IDs {'%d','%d'} nao apresentam ligacoes corretas\n", place->id, other_id);
	    exit(EXIT_FAILURE);
	 }
      }
   }

   return places_list;
}

PeopleListHead load_people(FILE *file) {

   char temp_char;
   int read_amount = 0;
   PeopleListHead people_list;
   people_list.first_person = NULL;
   people_list.size = 0;
   pPerson previous_person = NULL;
   int n_linha = 0;

   while (1) {

      Person buffer;
      buffer.age = -1;
      read_amount = fscanf(file, "%s %d %c", buffer.name, &buffer.age, &temp_char);
      n_linha++;

      if (read_amount != 3)
	 break;


      if (buffer.age == -1) {
	 printf("Ficheiro de pessoas com erro! Na linha : %d ou na anterior\n", n_linha);
	 fclose(file);
	 exit(EXIT_FAILURE);
      }

      if (temp_char == Healthy) {
	 buffer.state = Healthy;
      } else if (temp_char == Immune) {
	 buffer.state = Immune;
      } else if (temp_char == Sick) {
	 buffer.state = Sick;
	 int good_read = fscanf(file, " %d \n", &buffer.days_sick);
	 if(good_read == 1){
	    
	 }
	 int max_sick_days = MAX_SICK_DAYS(buffer.age);
	 if (buffer.days_sick < 0 || buffer.days_sick > max_sick_days) {
	    printf("Ficheiro de pessoas com erro! Na linha : %d\t Numero de dias doente invalido para a idade. Aceitados [0,%d]",
		    n_linha, max_sick_days);
	    fclose(file);
	    exit(EXIT_FAILURE);
	 }
      } else {
	 printf("Ficheiro de pessoas com erro! Na linha : %d\n", n_linha);
	 fclose(file);
	 exit(EXIT_FAILURE);
      }


      buffer.next_person = NULL;

      pPerson this_person = malloc(sizeof (Person));
      if (this_person == NULL) {
	 fclose(file);
	 alloc_error("pessoa");
      }
      memcpy(this_person, &buffer, sizeof (Person));
      //*this_person = buffer;
 
      if (previous_person == NULL) {
	 people_list.first_person = this_person;
      } else {
	 previous_person->next_person = this_person;
      }

      //Person_print(this_person);
      previous_person = this_person;
      people_list.size++;
   }
   fclose(file);

   return people_list;
}

void spread_people(PlaceList *places_list, PeopleListHead *people_list, PeopleListHead *people_list_not_simulating) {
   int total_capacity = 0;
   int i;
   int free_spaces[places_list->size];
   int n_free_spaces = places_list->size;

   for (i = 0; i < places_list->size; i++) {
      pPlace place = &places_list->arr[i];
      total_capacity += place->capacity;
      free_spaces[i] = i;
   }

   pPerson last_person_in = NULL;

   pPerson curr_person = people_list->first_person;
   while (curr_person != NULL) {

      int random_place = intUniformRnd(0, n_free_spaces - 1);
      int index = free_spaces[random_place];
      pPlace destination = &places_list->arr[index];

      pPeopleList people_inside = &destination->people_inside;

      PeopleList_add(people_inside, curr_person);
      total_capacity--;


      if (people_inside->size == destination->capacity) {
	 free_spaces[random_place] = free_spaces[n_free_spaces - 1];
	 n_free_spaces--;
      }

      if (total_capacity == 0) {
	 printf("Locais cheios\nAs Pessoas restantes ficarao de fora na simulacao. Mas continuarao a existir\n");
	 last_person_in = curr_person;
	 break;
      }

      curr_person = curr_person->next_person;
   }


   people_list_not_simulating->size = 0;
   people_list_not_simulating->first_person = NULL;

   if (curr_person == NULL) {
      printf("Todas as pessoas foram inseridas em locais\n");
      return;
   }

   pPerson first_person_not_in = last_person_in->next_person;
   last_person_in->next_person = NULL;

   if (people_list_not_simulating->first_person == NULL) {
      people_list_not_simulating->first_person = first_person_not_in;
   }
   // update the size of the list
   curr_person = first_person_not_in;
   while (curr_person != NULL) {
      curr_person = curr_person->next_person;
      people_list_not_simulating->size++;
   }

}