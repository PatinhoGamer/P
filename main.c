/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
#include <locale.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <time.h>
#include "common.h"

#include "main.h"
#include "utils.h"
#include "person.h"
#include "place.h"
#include "lists.h"
#include "startup.h"
#include "state.h"

/*
 * 
 */

int menu();
void simulate(PlaceList *place_list, PeopleListHead *people_list, int * days_passed);
void show_stats(FILE * output_stream, PlaceList *places_list, PeopleListHead *people_list, PeopleListHead *people_list_not_simulating);
void add_sick(PlaceList *places_list, PeopleListHead *people_list);
void transfer_people(PlaceList *place_list, PeopleListHead *people_list);
void exit_save(PlaceList *place_list, PeopleListHead *people_list, PeopleListHead *people_list_not_simulating);

int main(int argc, char** argv) {

   setlocale(LC_ALL, "Portuguese");
   initRandom();

   PlaceList places_list;
   PeopleListHead people_list;
   PeopleListHead people_list_not_simulating;
   SavedIterations saved_iterations;
   int days_passed = 0;

   int good_answer;

   const int n_predefined_places = 3;
   char places_paths[][32] = {"E1.bin", "E2.bin", "E3.bin"};
   char path_places[MAX_PATH];

   do {
      good_answer = 1;
      int option = 0;

      while (option < 1 || option > n_predefined_places + 1) {
	 printf("\n**** Selecione o ficheiro dos espacos ****\n");
	 for (int i = 0; i < n_predefined_places; i++)
	    printf("%d - %s\n", i + 1, places_paths[i]);
	 printf("%d - Outro\nOpcao: ", n_predefined_places + 1);
	 option = get_user_number();
      }

      if (option <= n_predefined_places) {
	 strcpy(path_places, places_paths[option - 1]);
      } else {
	 printf("Indique o caminho: ");
	 scanf("%[^\n]", path_places);
	 getchar();
      }

      FILE *file = fopen(path_places, "rb");
      if (file == NULL) {
	 printf("Erro a abrir ficheiro dos locais. Verifique a existencia do mesmo\n");
	 good_answer = 0;
      } else {
	 places_list = load_places(file); // file gets closed inside function

	 printf("\nLocais lidos do ficheiro binario:\n");
	 for (int i = 0; i < places_list.size; i++)
	    Place_imprime(stdout, places_list.arr + i);
      }
   } while (good_answer == 0);


   const int n_predefined_people = 4;
   char people_paths[][32] = {"pessoasA.txt", "pessoasB.txt", "pessoasC.txt", "pessoasD.txt"};
   char path_people[MAX_PATH];


   do {
      good_answer = 1;
      int option = 0;

      while (option < 1 || option > n_predefined_people + 1) {
	 printf("\n**** Selecione o ficheiro das pessoas ****\n");
	 for (int i = 0; i < n_predefined_people; i++)
	    printf("%d - %s\n", i + 1, people_paths[i]);
	 printf("%d - Outro\nOpcao: ", n_predefined_people + 1);
	 option = get_user_number();
      }

      if (option <= n_predefined_people) {
	 strcpy(path_people, people_paths[option - 1]);
      } else {
	 printf("Indique o caminho: ");
	 scanf("%[^\n]", path_people);
	 getchar();
      }

      FILE *file = fopen(path_people, "r");
      if (file == NULL) {
	 printf("Erro a abrir ficheiro das pessoas. Verifique a existencia do mesmo\n");
	 good_answer = 0;
      } else {
	 people_list = load_people(file); // file gets closed inside function

	 printf("\nPessoas lidas do ficheiro:\n");

	 pPerson curr_person = people_list.first_person;
	 while (curr_person != NULL) {
	    Person_imprime(stdout, curr_person);
	    curr_person = curr_person->next_person;
	 }
      }
   } while (good_answer == 0);

   //spreads people and puts the rest that didn't fit inside people_list_not_simulating
   spread_people(&places_list, &people_list, &people_list_not_simulating);

   //ready up saved_iterations
   saved_iterations.n_saved_iterations = 0;
   saved_iterations.iteration_states_head = NULL;

   int option;
   do {
      option = menu(days_passed);
      switch (option) {
	 case SIMULATE:
	 {
	    /* Avançar 1 iteração na simulação: a aplicação aplica o modelo de propagação à
	    população e atualiza o estado das pessoas. */
	    int n_loops = -1;
	    int good_answer = 0;
	    while (good_answer == 0) {
	       good_answer = 1;
	       printf("Numero de dias a passar: ");
	       n_loops = get_user_number();
	       if (n_loops < 1) {
		  good_answer = 0;
	       }
	    }
	    printf("A guardar a iteracao atual\n");
	    save_iteration_state(&places_list, &people_list, &saved_iterations, days_passed);
	    //print_backup_states(&saved_iterations);

	    printf("A simular %d dias\n", n_loops);
	    for (int i = 0; i < n_loops; i++) {
	       simulate(&places_list, &people_list, &days_passed);
	    }
	    printf("Terminado\n");
	    break;
	 }
	 case SHOW_STATS:
	 {
	    /* Apresentar estatística: a aplicação apresenta dados estatísticos do estado atual da
	    simulação. Entre outros resultados, deve mostrar: distribuição das pessoas por sala,
	    taxa de saudáveis, taxa de infetados, taxa de imunes, ... */
	    show_stats(stdout, &places_list, &people_list, &people_list_not_simulating);
	    break;
	 }
	 case ADD_SICK:
	 {
	    /*Adicionar doente: 1 nova pessoa doente é adicionada a um local. O utilizador
	    especifica o id do local e o identificador, a idade e o número de dias de infeção da
	    nova pessoa.*/
	    add_sick(&places_list, &people_list);
	    break;
	 }
	 case TRANSFER_PEOPLE:
	 {
	    /* Transferir pessoas: N pessoas são movidas de um local para outro dentro do espaço,
	    desde que estes locais tenham uma ligação direta. Os ids dos locais de origem e de
	    destino e o valor N são indicados pelo utilizador (por exemplo, mover 3 pessoas do
	    local 1 para o local 3). O programa escolhe N pessoas de forma aleatória, de entre as
	    que se encontram no local de origem. */
	    transfer_people(&places_list, &people_list);
	    break;
	 }
	 case GO_BACK:
	 {
	    printf("\n**** Reverter Iteracoes ****\n\n");
	    printf("0 - Nao retroceder\n");

	    pIterationState iterations[saved_iterations.n_saved_iterations];

	    pIterationState curr_iteration = saved_iterations.iteration_states_head;
	    int i = 0;
	    while (curr_iteration != NULL) {
	       int iter_days = curr_iteration->days_from_start;
	       int days_ago = days_passed - iter_days;
	       printf("%d - %d dias atras\n", i + 1, days_ago);

	       iterations[i] = curr_iteration;
	       curr_iteration = curr_iteration->next_iteration;
	       i++;
	    }

	    int times_to_go_back = 0;
	    int good_answer = 0;
	    while (good_answer == 0) {
	       good_answer = 1;
	       printf("Numero de iteracoes a retroceder (entre 0 a %d): ", saved_iterations.n_saved_iterations);
	       times_to_go_back = get_user_number();
	       if (times_to_go_back < 0 || times_to_go_back > saved_iterations.n_saved_iterations) {
		  good_answer = 0;
	       }
	    }

	    if (times_to_go_back == 0) {
	       printf("Operacao cancelada\n");
	       break;
	    }

	    IterationState * desiredIteration = iterations[times_to_go_back - 1];

	    printf("A reverter\n");
	    revert_to_iteration(&places_list, &people_list, desiredIteration, &days_passed);

	    delete_reverted_iterations(&saved_iterations, times_to_go_back);

	    printf("Terminado\n");
	    break;
	 }
	 case EXIT_SAVE:
	 {
	    /* Terminar Simulação: Ao terminar a simulação, a aplicação gera um relatório final.
	    Fica ao critério do aluno decidir que informação deve ser incluída, mas convém que
	    seja completa. Este relatório é guardado num ficheiro de texto com nome report.txt.
	    Além disso, a população existente na última iteração deve ser guardada num ficheiro
	    de texto. O nome deste ficheiro é indicado pelo utilizador. */
	    exit_save(&places_list, &people_list, &people_list_not_simulating);
	 }
      };
   } while (option != EXIT_SAVE);

   return 0;
}

int menu(int days_passed) {
   int option;
   int res = 0;

   do {
      printf("\n\n**** Menu Principal ****\n");
      printf("%d - Simular N iteracoes -> dias desde o inicio : %d\n", SIMULATE, days_passed);
      printf("%d - Mostrar estatisticas\n", SHOW_STATS);
      printf("%d - Adicionar doente\n", ADD_SICK);
      printf("%d - Transferir pessoas\n", TRANSFER_PEOPLE);
      printf("%d - Voltar atras X iteracoes\n", GO_BACK);
      printf("%d - Terminar simulacao e guardar\n", EXIT_SAVE);
      printf("\nOpcao: ");
      fseek(stdin, 0, SEEK_END);
      res = scanf("%d", &option);
      fseek(stdin, 0, SEEK_END);

      printf("\n\n");
   } while (res != 1 || option < SIMULATE || option > EXIT_SAVE);
   return option;
}

void simulate(PlaceList *place_list, PeopleListHead *people_list, int *days_passed) {

   pPerson got_sick_list[people_list->size];
   int n_new_sick = 0;

   (*days_passed)++;

   // Spread the virus --------------------------------------
   int n_people_simulating = 0;
   for (int i = 0; i < place_list->size; i++) {

      pPlace place = &place_list->arr[i];
      pPeopleList people_inside = &place->people_inside;
      int n_people_inside = people_inside->size;
      n_people_simulating += n_people_inside;
      int people_to_infect = floor(SPREAD_RATE * n_people_inside);


      for (int j = 0; j < n_people_inside; j++) {

	 pPerson person = people_inside->arr[j];

	 if (person->state == Sick) {
	    int got_sick[people_to_infect];

	    for (int k = 0; k < people_to_infect; k++) {

	       int person_index = 0;
	       int already_chosen = 1;
	       while (already_chosen == 1) {
		  already_chosen = 0;
		  person_index = intUniformRnd(0, n_people_inside - 1);
		  for (int l = 0; l < k; l++) {
		     if (person_index == got_sick[l]) {
			already_chosen = 1;
			break;
		     }
		  }
	       }

	       pPerson infected = people_inside->arr[person_index];
	       if (infected->state == Healthy) {
		  got_sick_list[n_new_sick] = infected;
		  n_new_sick++;
	       }

	       got_sick[k] = person_index;
	    }
	 }
      }
   }
   // ------------------------------------------------------

   // Make people Sick
   for (int i = 0; i < n_new_sick; i++) {
      pPerson infected = got_sick_list[i];
      infected->state = Sick;
      infected->days_sick = 0;
   }
   // ------------------------------------------------------


   // Update people ages
   if (*days_passed % 365 == 0) {
      pPerson person = people_list->first_person;
      for (int i = 0; i < n_people_simulating; i++) {
	 person->age++;
	 person = person->next_person;
      }
   }
   // ---------------------------------------------------------


   // Day count, Recuperation phase and Immunity ----------------
   pPerson person = people_list->first_person;
   for (int i = 0; i < n_people_simulating; i++) {

      if (person->state == Sick) {

	 person->days_sick++;
	 int got_better = 0;

	 if (person->age != 0) {
	    float prob_recovery = PROB_RECOV(person->age);
	    got_better = probEvent(prob_recovery);
	 } else {
	    got_better = 1;
	 }

	 int max_sick_days = MAX_SICK_DAYS(person->age);

	 if (person->days_sick >= max_sick_days)
	    got_better = 1;

	 if (got_better == 1) {
	    int becomes_immune = probEvent(IMMUNITY_PROB);

	    if (becomes_immune) {
	       person->state = Immune;
	    } else {
	       person->state = Healthy;
	    }
	    person->days_sick = 0;
	 }
      }

      person = person->next_person;
   }
   // ---------------------------------------------------------
}

void show_stats(FILE * output_stream, PlaceList *places_list, PeopleListHead *people_list, PeopleListHead *people_list_not_simulating) {
   fprintf(output_stream, "Estatisticas -------------------------------\nLocais: %d\n\tPessoas: %d\n\n", places_list->size, people_list->size);
   int n_people_simulating = 0;
   int n_healthy = 0, n_immune = 0, n_sick = 0;

   for (int i = 0; i < places_list->size; i++) {
      pPlace place = &places_list->arr[i];
      Place_imprime(output_stream, place);

      pPeopleList people = &place->people_inside;
      n_people_simulating += people->size;

      for (int j = 0; j < people->size; j++) {
	 pPerson person = people->arr[j];
	 fprintf(output_stream, "\t");
	 Person_imprime(output_stream, person);

	 if (person->state == Healthy) {
	    n_healthy++;
	 } else if (person->state == Immune) {
	    n_immune++;
	 } else if (person->state == Sick) {
	    n_sick++;
	 }
      }
      fprintf(output_stream, "\n");
   }
   fprintf(output_stream, "\n\n");

   fprintf(output_stream, "Pessoas que nao estao a participar na simulacao: %d\n", people_list_not_simulating->size);
   pPerson person = people_list_not_simulating->first_person;

   while (person != NULL) {
      fprintf(output_stream, "\t");
      Person_imprime(output_stream, person);
      person = person->next_person;
   }

   fprintf(output_stream, "\nTaxas: \n");
   float healthy_rate = n_healthy / (float) n_people_simulating;
   float immune_rate = n_immune / (float) n_people_simulating;
   float sick_rate = n_sick / (float) n_people_simulating;

   fprintf(output_stream, "\tSaudaveis : %f\n", healthy_rate * 100);
   fprintf(output_stream, "\tImmunes : %f\n", immune_rate * 100);
   fprintf(output_stream, "\tDoentes : %f\n", sick_rate * 100);
   fprintf(output_stream, "-----------------------------------------------\n");

   pPerson oldest_person = people_list->first_person;
   pPlace place_oldest_person = NULL;


   for (int i = 0; i < places_list->size; i++) {
      pPlace place = &places_list->arr[i];

      pPeopleList people = &place->people_inside;

      for (int j = 0; j < people->size; j++) {
	 pPerson person = people->arr[j];


	 if (person->age >= oldest_person->age) {
	    oldest_person = person;
	    place_oldest_person = place;
	 }
      }
   }


   fprintf(output_stream, "A pessoa mais velha esta na sala : %d\nIdade : %d, Nome : %s\n"
	   , place_oldest_person->id, oldest_person->age, oldest_person->name);
}

void add_sick(PlaceList *places_list, PeopleListHead *people_list) {
   pPlace place = NULL;
   //check for free space
   int free_space = 0;
   for (int i = 0; i < places_list->size; i++) {
      int slots_open = places_list->arr[i].capacity - places_list->arr[i].people_inside.size;
      if (slots_open > 0) {
	 free_space = 1;
	 break;
      }
   }
   if (free_space == 0) {
      printf("Nao existem locais para colocar o novo doente\n");
      return;
   }

   pPerson person = malloc(sizeof (Person));
   if (person == NULL) {
      alloc_error("pessoa");
   }
   strcpy(person->name, "");
   person->age = -1;
   person->state = Sick;
   person->days_sick = -1;

   int option;
   int res = 0;
   while (1) {
      do {
	 printf("\n\n**** Personalizar novo doente ****\n");
	 printf("1 - Id do local de destino\n");
	 printf("2 - Nome\n");
	 printf("3 - Idade\n");
	 printf("4 - Dias doente\n");
	 printf("5 - Confirmar\n");
	 printf("6 - Cancelar\n");
	 printf("\nid local: %d, nome: '%s', idade: %d, dias doente: %d\n",
		 (place == NULL) ? -1 : place->id, person->name, person->age, person->days_sick);
	 printf("\nOpcao: ");
	 fseek(stdin, 0, SEEK_END);
	 res = scanf("%d", &option);
	 fseek(stdin, 0, SEEK_END);
	 printf("\n\n");


      } while (res != 1 || option < 1 || option > 6);


      switch (option) {
	 case 1:
	 {
	    int good_place = 0;
	    while (good_place == 0) {
	       printf("Indique o ID do local para o novo doente: ");
	       int place_id = -1;
	       place_id = get_user_number();
	       place = Place_get_by_id(places_list, place_id);

	       if (place == NULL) {
		  printf("Local nao existe\nEscolha outro\n");
	       } else if (place->capacity <= place->people_inside.size) {
		  printf("Local sem capacidade para o doente\nEscolha outro\n");
	       } else {
		  good_place = 1;
	       }
	    }
	    break;
	 }
	 case 2:
	 {
	    printf("Indique o nome do doente: ");
	    scanf("%s", person->name);
	    break;
	 }
	 case 3:
	 {
	    int good_age = 0;
	    while (good_age == 0) {
	       printf("Indique a idade do doente: ");
	       person->age = get_user_number();
	       if (person->age < 0 || person->age > 125) {
		  printf("Introduza uma idade entre os 0 e 125 anos de idade\n");
	       } else {
		  good_age = 1;
	       }
	    }
	    break;
	 }
	 case 4:
	 {
	    if (person->age < 0) {
	       printf("Indique primeiramente a idade do doente\n");
	       break;
	    }
	    int good_days_sick = 0;
	    while (good_days_sick == 0) {
	       printf("Indique o numero de dias do estado de doente: ");
	       person->days_sick = get_user_number();

	       int max_sick_days = MAX_SICK_DAYS(person->age);

	       if (person->days_sick > max_sick_days) {
		  printf("Uma pessoa com %d anos de idade nao pode estar mais de %d dias doente\n", person->age, max_sick_days);
	       } else if (person->days_sick < 0) {
		  printf("Uma pessoa nao pode estar menos de 0 dias doente para ser considerado doente\n");
	       } else {
		  good_days_sick = 1;
	       }
	    }
	    break;
	 }
	 case 5:
	 {
	    if (place == NULL) {
	       printf("Escolha o local onde o doente vai ser colocado\n");
	       break;
	    }
	    // if fields haven't been filled
	    if (person->age == -1 || person->days_sick == -1 || strlen(person->name) == 0) {
	       printf("Complete as informacoes do novo doente\n");
	       break;
	    }
	    // add person to the people_inside list of that place
	    PeopleList_add(&place->people_inside, person);
	    // add person to the linked list
	    PeopleListHead_add(people_list, person);

	    printf("Doente adicionado\n-------");
	    return;
	 }
	 case 6:
	 {
	    free(person);
	    return;
	 }
      }
   }
}

void transfer_people(PlaceList *place_list, PeopleListHead *people_list) {

   int from_place_id = -1, to_place_id = -1;
   int n_people = -1;
   pPlace from_place = NULL, to_place = NULL;

   while (1) {
      int option, res = 0;
      do {
	 printf("\n\n**** Transferencia de pessoas ****\n");
	 printf("1 - Id do local de origem\n");
	 printf("2 - Numero de pessoas\n");
	 printf("3 - Id do local de destino\n");
	 printf("4 - Confirmar\n");
	 printf("5 - Cancelar\n");
	 printf("\nOrigem Id: %d, Numero de pessoas: '%d', Destino Id: %d\n",
		 (from_place == NULL) ? -1 : from_place->id, n_people,
		 (to_place == NULL) ? -1 : to_place->id);
	 printf("\nOpcao: ");
	 fseek(stdin, 0, SEEK_END);
	 res = scanf("%d", &option);
	 fseek(stdin, 0, SEEK_END);
	 printf("\n\n");
      } while (res != 1 || option < 1 || option > 5);


      switch (option) {
	 case 1: // Origin
	 {
	    int good_answer = 0;
	    from_place_id = -1;
	    while (good_answer == 0) {
	       good_answer = 1;
	       PlaceList_imprime(place_list);
	       printf("\nIndique o id do local de origem: ");
	       from_place_id = get_user_number();
	       from_place = Place_get_by_id(place_list, from_place_id);

	       if (from_place == NULL) {
		  printf("Id nao existente\n");
		  good_answer = 0;
	       }
	    }
	    break;
	 }
	 case 2: // Number of people
	 {
	    int good_answer = 0;
	    while (good_answer == 0) {
	       good_answer = 1;
	       printf("Indique o numero de pessoas a transferir: ");
	       n_people = get_user_number();
	       if (n_people < 1) {
		  printf("Indique um valor superior a 0\n");
		  good_answer = 0;
	       }
	    }
	    break;
	 }
	 case 3: // Destination
	 {
	    int good_answer = 0;
	    to_place_id = -1;
	    while (good_answer == 0) {
	       good_answer = 1;
	       PlaceList_imprime(place_list);
	       printf("\nIndique o id do local de destino: ");
	       to_place_id = get_user_number();
	       to_place = Place_get_by_id(place_list, to_place_id);

	       if (to_place == NULL) {
		  printf("Id nao existente\n");
		  good_answer = 0;
	       }
	    }
	    break;
	 }
	 case 4: // Confirm
	 {
	    pPeopleList from_people = &from_place->people_inside;
	    pPeopleList to_people = &to_place->people_inside;

	    if (!Place_is_connected(from_place, to_place)) {
	       printf("Locais nao estao conectados\n");
	       break;
	    }
	    if (from_people->size < n_people) {
	       printf("O local de origem nao tem pessoas suficientes\n");
	       break;
	    }
	    if (to_place->capacity - to_people->size < n_people) {
	       printf("O local de destino nao tem capacidade para as novas pessoas\n");
	       break;
	    }

	    for (int i = 0; i < n_people; i++) {
	       int person_index = intUniformRnd(0, from_people->size - 1);
	       pPerson choosen_one = from_people->arr[person_index];

	       PeopleList_add(to_people, choosen_one);

	       PeopleList_removeAt(from_people, person_index);
	    }

	    printf("Transferencia de pessoas completa------------------------\n");
	    return;
	 }
	 case 5: // Cancel
	    return;
      }
   }
}

void exit_save(PlaceList *places_list, PeopleListHead *people_list, PeopleListHead *people_list_not_simulating) {

   // ask if and where to store population
   int good_answer = 0;
   int should_save = 0;
   char answer[30];
   while (good_answer == 0) {
      printf("Deseja guardar a populacao? (S/N) -> ");
      scanf("%s", answer);

      if (strcmp(answer, "S") == 0) {
	 should_save = 1;
	 good_answer = 1;
      } else if (strcmp(answer, "N") == 0) {
	 good_answer = 1;
      }
   }

   if (should_save == 1) {
      printf("Indique o caminho para o ficheiro : ");
      char path[MAX_PATH];

      FILE *file = NULL;
      while (file == NULL) {
	 fseek(stdin, 0, SEEK_END);
	 scanf(" %[^\n]", path); // picks everything except \n which should even be there

	 file = fopen(path, "w");
	 if (file == NULL) {
	    printf("Erro a abrir criar ficheiro da populacao final. ");
	    printf("Indique um caminho correto.\n");
	 }
      }

      //Save population 1 by 1 until there are no more
      pPerson person = people_list->first_person;
      while (person != NULL) {
	 fprintf(file, "%s\t%d\t", person->name, person->age);
	 if (person->state == Healthy) {
	    fprintf(file, "S\n");
	 } else if (person->state == Immune) {
	    fprintf(file, "I\n");
	 } else if (person->state == Sick) {
	    fprintf(file, "D\t%d\n", person->days_sick);
	 }
	 person = person->next_person;
      }

      fflush(file);
      fclose(file);
   }


   FILE *rel_file = fopen("report.txt", "w");

   if (rel_file == NULL) {
      perror("Erro a criar ficheiro do relatorio final. Ficheiro nao criado\n");
   } else {

      // Might not work in MAC OS

      /*int file_fd = rel_file->_file;

      int stdout_fd = STDOUT_FILENO;
      int copy_stdout_fd = dup(stdout_fd);
      // Change stdout to the file so printf go to file
      _dup2(file_fd, stdout_fd);

      // Only prints around 3996 - 4000 characters to the file without the flush
      fflush(stdout);
      // even though the flush is on stdout instead of the rel_file
      fclose(rel_file);
      //put stdout back
      _dup2(copy_stdout_fd, stdout_fd);
      //printf("Nome do ficheiro do relatorio: %s\n", path);*/

      show_stats(rel_file, places_list, people_list, people_list_not_simulating);
   }

   printf("A limpar memoria\n");

   free_places(places_list);
   free_people(people_list);

   printf("Terminado\nPressione enter para sair ...\n");
   fflush(stdout);
   char trash[256];
   fgets(trash, 255, stdin);
   fgets(trash, 255, stdin);
   exit(EXIT_SUCCESS);
}