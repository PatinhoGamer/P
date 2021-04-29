/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */

#include "person.h"

void Person_print(Person *person) {
   printf("Person {name: %s, age: %d, health_state: ", person->name, person->age);
   if (person->state == Healthy)
      printf("Healthy}\n");
   else if (person->state == Immune)
      printf("Immune}\n");
   else
      printf("Sick, days_sick : %d}\n", person->days_sick);
}

void Person_imprime(FILE * output_stream,Person *person) {
   fprintf(output_stream,"%s,\t idade: %d,\t estado de saude: ", person->name, person->age);
   if (person->state == Healthy)
      fprintf(output_stream,"saudavel\n");
   else if (person->state == Immune)
      fprintf(output_stream,"imune\n");
   else
      fprintf(output_stream,"doente, dias doente: %d\n", person->days_sick);
}

void Person_print_all(Person *person) {
   while (person != NULL) {
      Person_print(person);
      person = person->next_person;
   }
}


