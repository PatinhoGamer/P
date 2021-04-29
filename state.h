/*
 * Programação
 * Aluno : Rodrigo Cavadas Mendes
 * Nº : 2019114072
 *
 */
/* 
 * File:   state.h
 * Author: Rodrigo
 *
 * Created on 6 de Maio de 2020, 19:32
 */

#ifndef STATE_H
#define STATE_H

#include "common.h"
#include "lists.h"
#include "place.h"
#include "person.h"
#include "utils.h"

typedef struct backup_person BackupPerson, *pBackupPerson;

struct backup_person {
    Person person;
    pPlace place;
};

typedef struct iteration_state IterationState, *pIterationState;

struct iteration_state {
    BackupPerson *people_states;
    int n_people;
    int days_from_start;
    pIterationState next_iteration;
};

typedef struct saved_iterations SavedIterations, *pSavedIterations;

struct saved_iterations {
    IterationState *iteration_states_head;
    int n_saved_iterations;
};


void print_backup_states(SavedIterations *saves);
int save_iteration_state(PlaceList *place_list, PeopleListHead *people_list, SavedIterations *saved_iterations,int days_passed);
void revert_to_iteration(PlaceList *place_list, PeopleListHead *people_list, IterationState *iteration,int *days_passed);

void delete_reverted_iterations(SavedIterations * saved_iterations,int amount_to_delete);

#endif /* STATE_H */

