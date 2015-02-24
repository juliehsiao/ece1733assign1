
#include "cubical_function_representation.h"

int cube_cost(t_blif_cube *cube, int num_inputs);
int function_cost(t_blif_cubical_function *f);
int cover_cost(t_blif_cube **cover, int num_cubes, int num_inputs);

void simplify_function(t_blif_cubical_function *f);


void printCoverTable(bool **coverTable, int numRows, int numCols, int *minterms);
void printValidCoverTable(bool **coverTable, int numRows, int numCols, 
        bool *validPIs, bool*validMinterms);

