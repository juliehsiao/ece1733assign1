
#include <stdbool.h>
#include "cubical_function_representation.h"


void findMinCover (bool **coverTable, int numPIs, int numMinterms, t_blif_cubical_function *f);
bool removeEmptyRow(bool **coverTable, int numCols, int numRows, bool *validPIs, 
        bool *validMinterms);
bool findEssentialPIs (bool **coverTable, int numMinterms, bool *validMinterms, 
        t_blif_cube **PIs, t_blif_cube ** essentialPIs, int *EPIIndex, int numPIs, bool *validPIs);
bool isCovered (bool *validMinterms, int numMinterms);
bool removeDominatedRow (bool **coverTable, int numRows, int numCols, bool *validPIs, 
        bool *validMinterms, int numInputs, t_blif_cube **set_of_cubes);
bool removeDominatedCol(bool **coverTable, int numRows, int numCols, bool *validPIs, 
        bool *validMinterms);




