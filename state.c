/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */

#include "state.h"

void print_backup_states(SavedIterations *saves) {
   printf("Backups:\n\n");
   pIterationState iteration = saves->iteration_states_head;
   for (int i = 0; i < saves->n_saved_iterations; i++) {

      printf("Iteration Index : %d, number of saved people : %d\n", i, iteration->n_people);

      for (int j = 0; j < iteration->n_people; j++) {

	 pBackupPerson backup = &iteration->people_states[j];
	 printf("\t Person : name -> %s, place id -> %d\n", backup->person.name, backup->place->id);
      }
      printf("\n\n");

      iteration = iteration->next_iteration;
   }
}

pIterationState add_new_state(SavedIterations *saved_iterations, int n_people_save) {

   //remove iteration if the list is on the limit
   if (saved_iterations->n_saved_iterations == MAXSAVEDITERATIONS) {
      pIterationState previous_iteration, curr_iteration = saved_iterations->iteration_states_head;
      
      while (curr_iteration->next_iteration != NULL) {
	 previous_iteration = curr_iteration;
	 curr_iteration = curr_iteration->next_iteration;
      }

      free(curr_iteration->people_states);
      free(curr_iteration);
      previous_iteration->next_iteration = NULL;
   }

   pIterationState new_iteration = malloc(sizeof (IterationState));
   if (new_iteration == NULL) {
      alloc_error("new_iteration");
   }

   pIterationState previous_first = saved_iterations->iteration_states_head;
   saved_iterations->iteration_states_head = new_iteration;
   new_iteration->next_iteration = previous_first;


   new_iteration->n_people = n_people_save;
   new_iteration->people_states = malloc(new_iteration->n_people * sizeof (BackupPerson));
   if (new_iteration->people_states == NULL) {
      alloc_error("new iteration backup");
   }

   if (saved_iterations->n_saved_iterations != MAXSAVEDITERATIONS)
      saved_iterations->n_saved_iterations++;

   return new_iteration;
}

// Unused function
// replaced with : backup_person->person = *real_person;

void copy_person_from_to(Person *from, Person *to) {
   strcpy(to->name, from->name);
   to->age = from->age;
   to->state = from->state;
   to->days_sick = from ->days_sick;
}

int save_iteration_state(PlaceList *place_list, PeopleListHead *people_list, SavedIterations *saved_iterations, int days_passed) {

   int n_people_save = 0;
   for (int i = 0; i < place_list->size; i++) {
      n_people_save += place_list->arr[i].people_inside.size;
   }

   pIterationState new_state = add_new_state(saved_iterations, n_people_save);
   BackupPerson *backup_array = new_state->people_states;
   new_state->days_from_start = days_passed;
   int backup_cur_index = 0;

   for (int i = 0; i < place_list->size; i++) {

      pPlace place = &place_list->arr[i];
      pPeopleList people_inside = &place->people_inside;

      for (int j = 0; j < people_inside->size; j++) {

	 pPerson real_person = people_inside->arr[j];
	 pBackupPerson backup_person = &backup_array[backup_cur_index];

	 //copy_person_from_to(real_person, &backup_person->person);
	 backup_person->person = *real_person;
	 backup_person->place = place;

	 backup_cur_index++;
      }
   }
}

void revert_to_iteration(PlaceList *place_list, PeopleListHead *people_list, IterationState *iteration, int *days_passed) {

   *days_passed = iteration->days_from_start;
   BackupPerson *person_states = iteration->people_states;

   free_people(people_list);
   people_list->size = 0;

   for (int i = 0; i < place_list->size; i++) {
      PeopleList_clear(&place_list->arr[i].people_inside);
   }

   pPerson last_person = NULL;

   for (int i = 0; i < iteration->n_people; i++) {
      pBackupPerson current_person = &person_states[i];

      pPerson new_person = malloc(sizeof (Person));
      if (new_person == NULL) {
	 alloc_error("allocating person while iteration reverting");
      }

      copy_person_from_to(&current_person->person, new_person);
      PeopleList_add(&current_person->place->people_inside, new_person);

      new_person->next_person = NULL;
      if (last_person == NULL) {
	 people_list->first_person = new_person;
	 last_person = new_person;
      }
      last_person->next_person = new_person;
      last_person = new_person;
      people_list->size++;
   }
}

void delete_reverted_iterations(SavedIterations * iterations_head, int amount_to_delete) {

   pIterationState curr_iteration = iterations_head->iteration_states_head;
   pIterationState previous_iteration;

   for (int i = 0; i < amount_to_delete; i++) {
      previous_iteration = curr_iteration;
      curr_iteration = curr_iteration->next_iteration;
      iterations_head->iteration_states_head = curr_iteration;
      iterations_head->n_saved_iterations--;

      free(previous_iteration->people_states);
      free(previous_iteration);
   }
}

