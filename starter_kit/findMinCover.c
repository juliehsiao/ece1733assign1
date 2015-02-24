#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "findMinCover.h"
#include "cubical_function_representation.h"

void findMinCover (bool **coverTable, int numPIs, int numMinterms,t_blif_cubical_function *f) {
    
    // [1] setup validMinterms, validPIs
    bool * validPIs = (bool *) malloc(numPIs * sizeof(bool));
    memset(validPIs, true, numPIs * sizeof(bool));
    bool * validMinterms = (bool *) malloc(numMinterms * sizeof(bool));
    memset(validMinterms, true, numMinterms * sizeof(bool));

    // [2] remove empty rows
    // [3] call findEssentialPIs
    // [4] check if function is covered
    // [5] call row dominance function
    // [6] check if function is covered
    // [7] call column dominance function
    // [8] check if function is covered
    // [9] repeat from 2
	return;
}

// marks any PI (row) that doesn't cover a remaining minterm as invalid
void removeEmptyRow(bool **coverTable, int numCols, int numRows, bool *validPIs, bool *validMinterms)
{
    bool isEmpty = true;
    int i, j;
    for(i = 0; i < numRows; i++) { //for every PI
        if(!validPIs[i]) continue;
        isEmpty = true;
        for(j = 0; ((j < numCols) && isEmpty); j++) { //check with every minterm (until we find that it's not empty)
            if(!validMinterms[j]) continue;
            if(coverTable[i][j]) { //if this PI covers a valid minterm, then the row is not empty
                isEmpty = false;
            }
        }

        if(isEmpty) { // if this PI doesn't cover any of the remaining minterms, mark it as invalid 
            validPIs[i] = false;
        }
    }
}

// Inserts essential PIs into list and "reduces" the cover table
void findEssentialPIs (bool **coverTable, int *minterms, int numMinterms, 
		bool *validMinterms, t_blif_cube **PIs, int numPIs, bool *validPIs)
{
	int i, j, k;

	// At most as many essential PIs as valid PIs
	int numValidPIs = 0;
	for (i = 0; i < numPIs; i++) {
		if (validPIs[i]) {
			numValidPIs++;
		}
	}

	printf("There are %d valid PIs\n", numValidPIs);

	t_blif_cube **essentialPIs = (t_blif_cube **) malloc (numValidPIs * sizeof(t_blif_cube *));
	int EPIIndex = 0;

	// Create temporary valid lists and copy the previous valid lists into them
	bool *newValidMinterms = (bool *) malloc (numMinterms * sizeof(bool));
	bool *newValidPIs = (bool *) malloc (numPIs * sizeof(bool));
	memcpy (newValidMinterms, validMinterms, numMinterms * sizeof(bool));
	memcpy (newValidPIs, validPIs, numPIs * sizeof(bool));

	// Iterate through each minterm, a PI is essential if it is the only PI
	// that covers the minterm
	for (i = 0; i < numMinterms; i++) { // col of minterms
		if (validMinterms[i] == false) continue;
		int numCovered = 0;
		int index = 0;
		for (j = 0; j < numPIs; j++) {
			if (validPIs[j] == false) continue;
			if (coverTable[j][i] == true) {
				numCovered++;
				index = j;
			}
		}
		if (numCovered == 1) { // An essential PI is found
			// add essential PI to list
			essentialPIs[EPIIndex] = (t_blif_cube *) malloc (sizeof(t_blif_cube));
			essentialPIs[EPIIndex++][0] = PIs[index][0];

			// invalidate the PI and all minterm covered by the EPI to reduce the table
			newValidPIs[index] = false;

			for (k = 0; k < numMinterms; k++) {
				if (coverTable[index][k] == true) newValidMinterms[k] = false;
			}
		}
	}

	printf("Found %d essential PIs\n", EPIIndex); //TODO: return this value? or pass f->cube_count to this function to update it here?
	// TODO: need to append the essentialPIs to the cover
	
	// Now copy back the new valid lists back
	memcpy (validMinterms, newValidMinterms, numMinterms * sizeof(bool));
	memcpy (validPIs, newValidPIs, numPIs * sizeof(bool));

	free(newValidMinterms);
	free(newValidPIs);
}


// Returns true when the entire function is covered (i.e. all minterms have been invalidated)
bool isCovered (bool *validMinterms, int numMinterms) {
	int i;
	for (i = 0; i < numMinterms; i++) {
		if (validMinterms[i] == true) {
			return false;
		}
	}
	printf("All minterms have been covered!!\n");
	return true;
}


// Dominated Row has less ticks
// Rows of PIs, Cols of Minterms
void removeDominatedRow (bool **coverTable, int numRows, int numCols, bool *validPIs, bool *validMinterms, int numInputs, t_blif_cube **set_of_cubes) {
	
	// Create temporary valid lists and copy the previous valid lists into them`
	bool *newValidPIs = (bool *) malloc (numRows * sizeof(bool));
	memcpy (newValidPIs, validPIs, numRows * sizeof(bool));

	bool jDomI = false;
	int i, j, k;
	// Iterate through each PI, a PI is dominated if:
	// There exists another PI that covers all the minterms it covers and more
	// A row is removed if it is dominated by another row AND the cost of the PI and the dominating PI
	// are the same
	for (i = 0; i < numRows; i++) {
		if (validPIs[i] == false) continue;
		for (j = 0; j < numRows; j++) {
			if (validPIs[j] == false) continue;
			// Compare two valid PI rows i and j, check to see if j dominates i
			for (k = 0; k < numCols; k++) {
				if (validMinterms[k] == false) continue;
				if (coverTable[i][k] && !coverTable[j][k]) {
					// row i covers a minterm not covered by j
					jDomI = false;
					break;
				}
				if (!coverTable[i][k] && coverTable[j][k]) {
					jDomI = true;
				}
			}
			// Only remove the dominated row (i) if the cost of PI[i] >= PI[j]
			if (jDomI && (cube_cost(set_of_cubes[i], numInputs) >= cube_cost(set_of_cubes[j], numInputs))) {
				newValidPIs[i] = false;
			}
		}
	}

	// Update and free the valid list
	memcpy (validPIs, newValidPIs, numRows * sizeof(bool));
	free(newValidPIs);
}


// Dominated Col has more ticks
void removeDominatedCol(bool **coverTable, int numRows, int numCols, bool *validPIs, bool *validMinterms) {
    
	// Create temporary valid lists and copy the previous valid lists into them
	bool *newValidMinterms = (bool *) malloc (numCols * sizeof(bool));
	memcpy (newValidMinterms, validMinterms, numCols * sizeof(bool));

    int i, j, k;
	// Iterate through each minterm, a minterm is dominated if there is another minterm is the superset of it 
	for (i = 0; i < numCols; i++) { // column 1
		if (validMinterms[i] == false) continue;
		int numCovered1 = 0;
		for (k = 0; k < numRows; k++) {
			if (validPIs[k] == false) continue;
			if (coverTable[k][i] == true) {
				numCovered1++;
			}
        }
		int numCovered2 = 0;
	    for (j = i+1; j < numCols; j++) { // column 2
            numCovered2 = 0;
		    for (k = 0; k < numRows; k++) {
			    if (validPIs[k] == false) continue;
			    if (coverTable[k][j] == true) {
			    	numCovered2++;
			    }
            }
		}
        int dominator = (numCovered1 < numCovered2)?(i):(j);
        int dominated = (numCovered1 < numCovered2)?(j):(i);
        bool isDominated = true;
        for(k = 0; k < numRows; k++) {
			if (validPIs[k] == false) continue;
			if ( (coverTable[k][dominator] == true) && (coverTable[k][dominated] == false) ) {
		        isDominated = false;
			}
        }
        
        if(isDominated) {
            newValidMinterms[dominated] = false; 
	        printf("Dominated col %d with col %d \n", dominated, dominator);
        }
	}

	memcpy (validMinterms, newValidMinterms, numCols * sizeof(bool));
	free(newValidMinterms);
}

