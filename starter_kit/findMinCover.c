#include <stdbool.h>
#include <string.h>
#include <assert.h>
//#include "findMinCover.h"
#include "cubical_function_representation.h"

void findMinCover () {
    
    // [1] setup validMinterms, validPIs
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

	printf("Found %d essential PIs\n", EPIIndex);
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


void removeDominatedRow () {
	return;
}



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

