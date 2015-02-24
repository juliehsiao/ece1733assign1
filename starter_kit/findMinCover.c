#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "findMinCover.h"

void findMinCover () {
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
