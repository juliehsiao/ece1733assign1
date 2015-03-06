#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "findMinCover.h"
#include "cubical_function_representation.h"
#include "assign1.h"

extern int debug_flag;

//need to return:
// - a list of set_of_cubes + corresponding count
// - number of solutions
void branchAndBound(t_blif_cube ***set_of_cubes_list, int *EPIIndexList, int *solutionIdx, t_blif_cube **prevEPIs, int prevEPICount, int *costList,
        int numPIs, int numMinterms, int *minterms, int numValidPIs, bool *validPIs, bool *validMinterms, t_blif_cubical_function *f, bool **coverTable)
{
//printf("begin branch and bound... trying 2^%d combos\n", numValidPIs);
//int z; scanf("%d", &z);
    int i, j, k;
    t_blif_cube **PIs = f->set_of_cubes;

    for(i = 0; i < numValidPIs; i++) { //try to take one PI & compute cost
        int count = 0;
        for(j = 0; j < numPIs; j++) {
            if(!validPIs[j]) continue;
            if(count == i) break; //j is index of the PI we are taking
            count++;
        }

	    // Create temporary valid lists and copy the previous valid lists into them
	    bool *newValidMinterms = (bool *) malloc (numMinterms * sizeof(bool));
	    bool *newValidPIs = (bool *) malloc (numPIs * sizeof(bool));
	    memcpy (newValidMinterms, validMinterms, numMinterms * sizeof(bool));
	    memcpy (newValidPIs, validPIs, numPIs * sizeof(bool));
	    t_blif_cube **newEssentialPIs = (t_blif_cube **) malloc ((numValidPIs + prevEPICount) * sizeof(t_blif_cube *));
        memcpy(newEssentialPIs, prevEPIs, prevEPICount * sizeof(t_blif_cube *));
        int newEPICount = prevEPICount;

		// add essential PI to list
        //printf("%staking PI[%d]%s\n", BRED, j, KEND);
		newEssentialPIs[newEPICount] = (t_blif_cube *) malloc (sizeof(t_blif_cube));
		newEssentialPIs[newEPICount++][0] = PIs[j][0];
		// invalidate the PI and all minterm covered by the EPI to reduce the table
		newValidPIs[j] = false;

		for (k = 0; k < numMinterms; k++) {
			if (coverTable[j][k] == true) {
				//printf("newValidMinterms[%d] = false\t\t0x%x\n", k, &newValidMinterms[k]);
				newValidMinterms[k] = false;
			}
		}
        
        if(findMinCover(coverTable, numPIs, numMinterms, f, minterms, newValidPIs, newValidMinterms, newEssentialPIs, &newEPICount))
        {
            //taking this PI doesn't cause more branch & bound condition

            //free newValidPIs, newValidMinterms
            free(newValidPIs);
            free(newValidMinterms);
            
            // store set of cubes to the global array
            set_of_cubes_list[*solutionIdx] = newEssentialPIs;
            // store cost into global 
            int curCost = cover_cost(newEssentialPIs, newEPICount, f->input_count);
            costList[*solutionIdx] = curCost;
            // store the number of cubes associated with this solution
            EPIIndexList[(*solutionIdx)++] = newEPICount;

//            printf("press Enter...");
//int z; scanf("%d", &z);
        }
        else
        {
            //generate new numValidPIs
            int newNumValidPIs = 0;
		    for (k = 0; k < numPIs; k++) {
		    	if (newValidPIs[k] == true) {
		    		newNumValidPIs++;
		    	}
		    }

//            printf("press Enter...");
//int z; scanf("%d", &z);
            branchAndBound(set_of_cubes_list, EPIIndexList, solutionIdx, newEssentialPIs, newEPICount, costList, 
                numPIs, numMinterms, minterms, newNumValidPIs, newValidPIs, newValidMinterms, f, coverTable);
        }
    }
    return;
}


//returns if all minterms have been covered
bool findMinCover (bool **coverTable, int numPIs, int numMinterms, t_blif_cubical_function *f, int *minterms,
        bool *validPIs, bool *validMinterms, t_blif_cube **essentialPIs, int *EPIIndex) {
    


    bool emptyChanged   = true;
    bool ePIChanged     = true;
    bool rowDomChanged  = true;
    bool colDomChanged  = true;
    bool done           = false;

    if (debug_flag) {
	    printf("Initial Cover Table\n");
        printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
	    printf("\n");
    }

    while( emptyChanged || ePIChanged || rowDomChanged || colDomChanged )
    {
        emptyChanged   = false;
        ePIChanged     = false;
        rowDomChanged  = false;
        colDomChanged  = false;

        // [2] remove empty rows
        emptyChanged = removeEmptyRow(coverTable, numMinterms, numPIs, validPIs, validMinterms);
        if(emptyChanged && debug_flag) {
            printf("[Removed empty row]\n");
            printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
			printf("\n");
        }

        // [3] call findEssentialPIs
        ePIChanged = findEssentialPIs (coverTable, numMinterms, validMinterms, f->set_of_cubes, essentialPIs, EPIIndex, numPIs, validPIs);
        if(ePIChanged && debug_flag) {
            printf("[Removed essential PIs]\n");
            printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
			printf("\n");
        }
        // [4] check if function is covered
        if(isCovered (validMinterms, numMinterms)) {
            done = true;
            break;
        }

        // [5] call row dominance function
        rowDomChanged = removeDominatedRow (coverTable, numPIs, numMinterms, validPIs, validMinterms, f->input_count , f->set_of_cubes);
        if(rowDomChanged) {
            if (debug_flag) {
                printf("[Removed dominated rows]\n");
                printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
			    printf("\n");
            }
            // [6] try to findEssentialPIs again
            ePIChanged = findEssentialPIs (coverTable, numMinterms, validMinterms, f->set_of_cubes, essentialPIs, EPIIndex, numPIs, validPIs) || ePIChanged;
            if(isCovered (validMinterms, numMinterms)) {
                done = true;
                break;
            }
            if (debug_flag) {
                printf("[Removed essential PIs]\n");
                printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
			    printf("\n");
            }
        }
        // [7] call column dominance function
        colDomChanged = removeDominatedCol(coverTable, numPIs, numMinterms, validPIs, validMinterms);
        if(colDomChanged) {
            if (debug_flag) {
                printf("[Removed dominated columns]\n");
                printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
			    printf("\n");
            }
            // [8] try to findEssentialPIs again
            ePIChanged = findEssentialPIs (coverTable, numMinterms, validMinterms, f->set_of_cubes, essentialPIs, EPIIndex, numPIs, validPIs) || ePIChanged;
            if(isCovered (validMinterms, numMinterms)) {
                done = true;
                break;
            }
            if (debug_flag) {
                printf("[Removed essential PIs]\n");
                printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);
			    printf("\n");
            }
        }
    }
    //if(!done) {
    //printf("%sBranch and bound%s\n", BYEL, KEND);
    //}
    //printf("final cover table\n");
    //printValidCoverTable(coverTable, numPIs, numMinterms, validPIs, validMinterms, minterms, f->set_of_cubes, f->input_count);

	return done;
}

// marks any PI (row) that doesn't cover a remaining minterm as invalid
bool removeEmptyRow(bool **coverTable, int numCols, int numRows, bool *validPIs, bool *validMinterms)
{
    bool isEmpty = true;
    bool changed = false;
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
            changed = true;
        }
    }
    return changed;
}

// Inserts essential PIs into list and "reduces" the cover table
bool findEssentialPIs (bool **coverTable, int numMinterms, bool *validMinterms, t_blif_cube **PIs, 
        t_blif_cube ** essentialPIs, int *EPIIndex, int numPIs, bool *validPIs)
{
	int i, j, k;
    bool changed = false;

	// Create temporary valid lists and copy the previous valid lists into them
	bool *newValidMinterms = (bool *) malloc (numMinterms * sizeof(bool));
	bool *newValidPIs = (bool *) malloc (numPIs * sizeof(bool));
	memcpy (newValidMinterms, validMinterms, numMinterms * sizeof(bool));
	memcpy (newValidPIs, validPIs, numPIs * sizeof(bool));

	// Iterate through each PI, it is essential if it covers a minterm that is not covered by any other PI
	for (i = 0; i < numPIs; i++) {
		if (validPIs[i] == false) continue;
		bool essential = false;
		for (j = 0; j < numMinterms; j++) {
			if (validMinterms[j] == false || coverTable[i][j] == false) continue;
			essential = true;
			for (k = 0; k < numPIs; k++) {
				if (i == k) continue;
				if (coverTable[k][j] && validMinterms[j] && validPIs[k] == true) {
					essential = false;
				}
			}
			if (essential) break;
		}
		if (essential) {
			// add essential PI to list
			essentialPIs[*EPIIndex] = (t_blif_cube *) malloc (sizeof(t_blif_cube));
			essentialPIs[(*EPIIndex)++][0] = PIs[i][0];
            changed = true;

			// invalidate the PI and all minterm covered by the EPI to reduce the table
			newValidPIs[i] = false;

			for (k = 0; k < numMinterms; k++) {
				if (coverTable[i][k] == true) {
					//printf("newValidMinterms[%d] = false\t\t0x%x\n", k, &newValidMinterms[k]);
					newValidMinterms[k] = false;
				}
			}
		}
	}

	//printf("%sFound %d essential PIs%s\n", BGRN, *EPIIndex, KEND); 

	// Now copy back the new valid lists back
	memcpy (validMinterms, newValidMinterms, numMinterms * sizeof(bool));
	memcpy (validPIs, newValidPIs, numPIs * sizeof(bool));

	free(newValidPIs);
	free(newValidMinterms);

    return changed;
}


// Returns true when the entire function is covered (i.e. all minterms have been invalidated)
bool isCovered (bool *validMinterms, int numMinterms) {
	int i;
	for (i = 0; i < numMinterms; i++) {
		if (validMinterms[i] == true) {
			return false;
		}
	}
	//printf(">>>>>> All minterms have been covered! <<<<<<\n\n");
	return true;
}


// Dominated Row has less ticks
// Rows of PIs, Cols of Minterms
bool removeDominatedRow (bool **coverTable, int numRows, int numCols, bool *validPIs, bool *validMinterms, int numInputs, t_blif_cube **set_of_cubes) {
	
	// Create temporary valid lists and copy the previous valid lists into them`
	bool *newValidPIs = (bool *) malloc (numRows * sizeof(bool));
	memcpy (newValidPIs, validPIs, numRows * sizeof(bool));

    bool changed = false;

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
                changed = true;
			}
		}
	}

	// Update and free the valid list
	memcpy (validPIs, newValidPIs, numRows * sizeof(bool));
	free(newValidPIs);
    return changed;
}


// Dominated Col has more ticks
bool removeDominatedCol(bool **coverTable, int numRows, int numCols, bool *validPIs, bool *validMinterms) {
    
	// Create temporary valid lists and copy the previous valid lists into them
	bool *newValidMinterms = (bool *) malloc (numCols * sizeof(bool));
	memcpy (newValidMinterms, validMinterms, numCols * sizeof(bool));

    int i, j, k;
    bool changed = false;
	// Iterate through each minterm, a minterm is dominated if there is another minterm is the superset of it 
	for (i = 0; i < numCols; i++) { // for every column
		if (validMinterms[i] == false) continue; //skip if it's not valid

        // calculate the number of PIs that covers it
		int numCovered1 = 0;
		for (k = 0; k < numRows; k++) {
			if (validPIs[k] == false) continue;
			if (coverTable[k][i] == true) {
				numCovered1++;
			}
        }

		int numCovered2 = 0;
	    for (j = i+1; j < numCols; j++) { // look at each of the subsequent columns
		    if (validMinterms[j] == false) continue; //skip if it's not valid
            // calculate the number of PIs that covers it
            numCovered2 = 0;
		    for (k = 0; k < numRows; k++) {
			    if (validPIs[k] == false) continue;
			    if (coverTable[k][j] == true) {
			    	numCovered2++;
			    }
            }

            int dominator = (numCovered1 < numCovered2)?(i):(j); //minterm with less PIs covering it is the domincator (stays)
            int dominated = (numCovered1 < numCovered2)?(j):(i); // minterm with more PIs covering it is dominated (removed)
            bool isDominated = true;
            for(k = 0; k < numRows; k++) {
		    	if (validPIs[k] == false) continue;
		    	if ( (coverTable[k][dominator] == true) && (coverTable[k][dominated] == false) ) {
		            isDominated = false;
		    	}
            }
            
            if(isDominated) {
                newValidMinterms[dominated] = false; 
                changed = true;
	            //printf("Dominated col %d with col %d \n", dominated, dominator);
            }
		}
	}

	memcpy (validMinterms, newValidMinterms, numCols * sizeof(bool));
	free(newValidMinterms);
    return changed;
}

