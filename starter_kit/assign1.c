////////////////////////////////////////////////////////////////////////
// Solution to assignment #1 for ECE1733.
// This program implements the Quine-McCluskey method for 2-level
// minimization. 
////////////////////////////////////////////////////////////////////////

/**********************************************************************/
/*** HEADER FILES *****************************************************/
/**********************************************************************/

#include <stdlib.h>
//#include <conio.h>
//#include <curses.h> //replaces conio.h
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "common_types.h"
#include "blif_common.h"
#include "cubical_function_representation.h"
#include "findPI.h"
#include "findMinCover.h"
#include "assign1.h"

/**********************************************************************/
/*** DATA STRUCTURES DECLARATIONS *************************************/
/**********************************************************************/

/**********************************************************************/
/*** DEFINE STATEMENTS ************************************************/
/**********************************************************************/

/**********************************************************************/
/*** GLOBAL VARIABLES *************************************************/
/**********************************************************************/

/**********************************************************************/
/*** FUNCTION DECLARATIONS ********************************************/
/**********************************************************************/




/**********************************************************************/
/*** BODY *************************************************************/
/**********************************************************************/


/**********************************************************************/
/*** COST FUNCTIONS ***************************************************/
/**********************************************************************/


int cube_cost(t_blif_cube *cube, int num_inputs)
/* Wires and inverters are free, everything else is #inputs+1*/
{
	int index;
	int cost = 0;

	for (index = 0; index < num_inputs; index++)
	{
		if (read_cube_variable(cube->signal_status, index) != LITERAL_DC)
		{
			cost++;
		}
	}
	if (cost > 1)
	{
		cost++;
	}
	return cost;
}


int function_cost(t_blif_cubical_function *f)
{
	int cost = 0;
	int index;
	
	if (f->cube_count > 0)
	{
		for(index = 0; index < f->cube_count; index++)
		{
			cost += cube_cost(f->set_of_cubes[index], f->input_count);
		}
		if (f->cube_count > 1)
		{
			cost += (f->cube_count+1);
		}
	}

	return cost;
}


int cover_cost(t_blif_cube **cover, int num_cubes, int num_inputs)
{
	int result = 0;
	int index;

	for (index = 0; index < num_cubes; index++)
	{
		result += cube_cost(cover[index], num_inputs);
	}
	if (num_cubes > 1)
	{
		result += num_cubes+1;
	}
	return result;
}


/**********************************************************************/
/*** helper functions for MINIMIZATION CODE ***************************/
/**********************************************************************/
void printCoverTable(bool **coverTable, int numRows, int numCols, int *minterms)
{
    int i, j;
    printf("**********************************************\n");
    for(j = 0; j < numCols; j++) {
        printf("|%2d ", minterms[j]);
    }
    printf("|\n");
    for(i = 0; i < numRows; i++) {
        for(j = 0; j < numCols; j++) {
            //printf("| %s%c%s ", BGRN, (coverTable[i][j])?(tick):(' '), KEND);
			printf("| ");
			if (coverTable[i][j]) printf("%s\xE2\x9C\x93%s", BRED, KEND);
			else printf(" ");
			printf(" ");
        }
        printf("|\n");
    }
    printf("**********************************************\n");
}

void printValidCoverTable(bool **coverTable, int numRows, int numCols, 
        bool *validPIs, bool*validMinterms)
{
    int i, j;
    printf("**********************************************\n");
    for(i = 0; i < numRows; i++) {
        for(j = 0; j < numCols; j++) {
            if(validPIs[i] && validMinterms[j]) {
                printf("| %c ", (coverTable[i][j])?('v'):(' '));
            }
            else {
                printf("|---");
            }
        }
        printf("|\n");
    }
    printf("**********************************************\n");
}

/**********************************************************************/
/*** MINIMIZATION CODE ************************************************/
/**********************************************************************/


void simplify_function(t_blif_cubical_function *f)
/* This function simplifies the function f. The minimized set of cubes is
 * returned though a field in the input structure called set_of_cubes.
 * The number of cubes is stored in the field cube_count.
 */
{
    //=====================================================
    // [1] store the minterms for use later
    //=====================================================
    int numMinTerms = 0;
    int i, j, k;
    for(i = 0; i < f->cube_count; i++)
    {
        if(!f->set_of_cubes[i]->is_DC)
        {
            int numX = 0;
            for(j = 0; j < f->input_count; j++) {
                if(read_cube_variable(f->set_of_cubes[i]->signal_status, j) == LITERAL_DC) {
                    numX++;
                }
            }
            numMinTerms+= (1 << numX);
        }
    }

    int * minterms = (int *) malloc((numMinTerms + 1) * sizeof(int));
    int minTermIndex = 0;
    printSetOfCubes(f->set_of_cubes, f->input_count, f->cube_count);

    for(i = 0; i < f->cube_count; i++) {
        if (!f->set_of_cubes[i]->is_DC) {
            minTermIndex = enumerateAllMinterms(f->set_of_cubes[i], minterms, minTermIndex, f->input_count); 
        }
    }
    numMinTerms = minTermIndex;

    //=====================================================
    // [2] merge cubes to set of PIs
    //=====================================================
    t_blif_cube ** PIs = (t_blif_cube **) malloc (f->cube_count * sizeof(t_blif_cube *));
    findPI(f, PIs); //f->set_of_cubes will be freed in findPI, PIs is the only valid list
    f->set_of_cubes = PIs;
    printSetOfCubes(f->set_of_cubes, f->input_count, f->cube_count);

    //=====================================================
    // [3] construct the cover table
    //=====================================================
    bool **coverTable = (bool **) malloc( f->cube_count * sizeof(bool *) );
    for(i = 0; i < f->cube_count; i++) {
        coverTable[i] = (bool *) malloc( numMinTerms * sizeof(bool) );
        memset(coverTable[i], false, numMinTerms);
    }

    for(i = 0; i < f->cube_count; i++) {
        int PICovers[64] = {0};
        int numCovered = enumerateAllMinterms(f->set_of_cubes[i], PICovers, 0, f->input_count);
        for(j = 0; j < numCovered; j++) {
            for(k = 0; k < numMinTerms; k++) {
                if(minterms[k] == PICovers[j]) {
                    coverTable[i][k] = true;
                }
            }    
        }
printf("\n");
    } 
    printCoverTable(coverTable, f->cube_count, numMinTerms, minterms);

    //=====================================================
    // [4] find all minimal covers
    //=====================================================
    findMinCover(coverTable, f->cube_count, numMinTerms, f);




}


/**********************************************************************/
/*** MAIN FUNCTION ****************************************************/
/**********************************************************************/


int main(int argc, char* argv[])
{
	t_blif_logic_circuit *circuit = NULL;

	if (argc != 2)
	{
		printf("Usage: %s <source BLIF file>\r\n", argv[0]);
		return 0;
	}
	printf("Quine-McCluskey 2-level logic minimization program.\r\n");

	/* Read BLIF circuit. */
	printf("Reading file %s...\n",argv[1]);
	circuit = ReadBLIFCircuit(argv[1]);

	if (circuit != NULL)
	{
		int index;

		/* Minimize each function, one at a time. */
		printf("Minimizing logic functions\n");
		for (index = 0; index < circuit->function_count; index++)
		{
			t_blif_cubical_function *function = circuit->list_of_functions[index];

			simplify_function(function);
		}

		/* Print out synthesis report. */
		printf("Report:\r\n");
		for (index = 0; index < circuit->function_count; index++)
		{
			t_blif_cubical_function *function = circuit->list_of_functions[index];

			/* Print function information. */
			printf("Function %i: #inputs = %i; #cubes = %i; cost = %i\n", index+1, function->input_count, function->cube_count, function_cost(function)); 
		}

		/* Finish. */
		printf("Done.\r\n");
		DeleteBLIFCircuit(blif_circuit);
	}
	else
	{
		printf("Error reading BLIF file. Terminating.\n");
	}
	return 0;
}

