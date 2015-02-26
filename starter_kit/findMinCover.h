
#include <stdbool.h>
#include "cubical_function_representation.h"


void branchAndBound(t_blif_cube ***set_of_cubes_list, int *EPIIndexList, int *solutionIdx, t_blif_cube **prevEPIs, int prevEPICount, int *costList,
        int numPIs, int numMinterms, int *minterms, int numValidPIs, bool *validPIs, bool *validMinterms, t_blif_cubical_function *f, bool **coverTable);
bool findMinCover (bool **coverTable, int numPIs, int numMinterms, t_blif_cubical_function *f, int *minterms,
        bool *validPIs, bool *validMinterms, t_blif_cube **essentialPIs, int *EPIIndex);
bool removeEmptyRow(bool **coverTable, int numCols, int numRows, bool *validPIs, 
        bool *validMinterms);
bool findEssentialPIs (bool **coverTable, int numMinterms, bool *validMinterms, 
        t_blif_cube **PIs, t_blif_cube ** essentialPIs, int *EPIIndex, int numPIs, bool *validPIs);
bool isCovered (bool *validMinterms, int numMinterms);
bool removeDominatedRow (bool **coverTable, int numRows, int numCols, bool *validPIs, 
        bool *validMinterms, int numInputs, t_blif_cube **set_of_cubes);
bool removeDominatedCol(bool **coverTable, int numRows, int numCols, bool *validPIs, 
        bool *validMinterms);




