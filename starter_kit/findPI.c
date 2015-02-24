#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "findPI.h"


// Orders the set_of_cubes by cardinality of 1's
void orderSetOfCubes (t_blif_cubical_function *f, t_blif_cube **ordered)
{
    t_blif_cube **orig = f->set_of_cubes;
    int index = 0;
    assert(orig);
    int i;
    for (i = 0; i <= f->input_count; i++) {
        int j;
        for (j = 0; j < f->cube_count; j++) {
            if (findCardinality(f->set_of_cubes[j], f->input_count) == i) {
                ordered[index++] = f->set_of_cubes[j];
            }
        }
    }
    assert(index == f->cube_count);
}

// Returns cardinality of a cube cube, size is the number of inputs for the function
int findCardinality (t_blif_cube *cube, int size)
{
    int i;
    int ret = 0;
    for (i = 0; i < size; i++) {
        if (read_cube_variable(cube->signal_status, i) == LITERAL_1) ret++;
    }
    return ret;
}


void findPI(t_blif_cubical_function *f, t_blif_cube **PIs)
{
    t_blif_cube ** tmpPIs;
    t_blif_cube ** orderedSetOfCubes = (t_blif_cube **) malloc (f->cube_count * sizeof(t_blif_cube *));
    orderSetOfCubes(f, orderedSetOfCubes);
    bool mergedCubes = true;
    int PIIndex = 0;
    // Loop to find the PIs
    while (mergedCubes) {
    printSetOfCubes(orderedSetOfCubes, f->input_count, f->cube_count);

        mergedCubes = false;

        bool *used = (bool *) malloc (f->cube_count * sizeof(bool));
        memset(used, false, f->cube_count * sizeof(bool));

        int newCubeCount = 0;

        // Malloc a "set_of_cubes" that is square of the number of cubes in the set
        // This is the largest possible number of cubes we can have
        tmpPIs = (t_blif_cube **) malloc (f->cube_count * f->cube_count * sizeof (t_blif_cube *));

        int i, j;
        for (i = 0; i < f->cube_count-1; i++) {
            for (j = i+1; j < f->cube_count; j++) {
                if (findCardinality(orderedSetOfCubes[i], f->input_count)+1 != findCardinality(orderedSetOfCubes[j], f->input_count)) continue;
                t_blif_cube *mergedImplicant = mergeImplicants(orderedSetOfCubes[i], orderedSetOfCubes[j], f->input_count);
                if (mergedImplicant != NULL) {
                    used[i] = true;
                    used[j] = true;
                    tmpPIs[newCubeCount++] = mergedImplicant;
                    mergedCubes = true;
                }

            }
        }
        for(i = 0; i < f->cube_count; i++) {
            if ((used[i] == false) && !isRedundantPI(PIs, f->input_count, PIIndex, orderedSetOfCubes[i])) {
                PIs[PIIndex] = (t_blif_cube *) malloc(sizeof(t_blif_cube));
                PIs[PIIndex++][0] = orderedSetOfCubes[i][0]; 
            }
        }
        
        if (newCubeCount != 0) {
            freeSetOfCubes(orderedSetOfCubes, f->cube_count);
            orderedSetOfCubes = tmpPIs;
            f->cube_count = newCubeCount;
        }

        free(used);
    }


    f->cube_count = PIIndex;
    
    free(tmpPIs);

}

