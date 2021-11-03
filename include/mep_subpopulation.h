// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#ifndef MEP_SUBPOPULATION_H
#define MEP_SUBPOPULATION_H

#include "mep_chromosome.h"

//-----------------------------------------------------------------
class t_sub_population {
public:
	t_mep_chromosome* individuals; // array of individuals
	t_mep_chromosome offspring1, offspring2;
	int worst_index, best_index;

	t_sub_population();

	void compute_worst_index(int pop_size);

	void compute_best_index(int pop_size);
};
//-----------------------------------------------------------------
#endif