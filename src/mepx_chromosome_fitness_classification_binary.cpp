// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//---------------------------------------------------------------------------
#include <math.h>
#include <float.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#include "mep_chromosome.h"
#include "mep_functions.h"
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_binary_classification(const t_mep_data& mep_dataset, int* random_subset_indexes,
	int random_subset_selection_size,
	double** cached_variables_eval_matrix, double* cached_sum_of_errors, double* cached_threashold,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double, s_value_class* tmp_value_class, t_seed& seed)
{
	fitness_binary_classification_double_cache_all_training_data(mep_dataset,
		random_subset_indexes, random_subset_selection_size,
		cached_variables_eval_matrix, cached_sum_of_errors, cached_threashold,
		num_actual_variables, actual_enabled_variables,
		eval_matrix_double, tmp_value_class, seed);
}
//---------------------------------------------------------------------------
void t_mep_chromosome::fitness_binary_classification_double_cache_all_training_data(
	const t_mep_data& mep_dataset,
	int* random_subset_indexes, int random_subset_selection_size,
	double** cached_variables_eval_matrix, double* cached_sum_of_errors,
	double* cached_threashold,
	int num_actual_variables, int* actual_enabled_variables,
	double** eval_matrix_double, s_value_class* tmp_value_class, t_seed& seed)
{

	// evaluate a_chromosome
	// partial results are stored and used later in other sub-expressions

	double** data = mep_dataset.get_data_matrix_double();
	int num_rows = mep_dataset.get_num_rows();

	fitness = DBL_MAX;
	index_best_genes[0] = -1;
	num_incorrectly_classified = 100;// max

	int* line_of_constants = NULL;
	if (num_constants) {
		line_of_constants = new int[num_constants];// line where a constant was firstly computed
		for (int i = 0; i < num_constants; i++)
			line_of_constants[i] = -1;
	}

	compute_eval_matrix_double(num_rows, cached_variables_eval_matrix,
		num_actual_variables, actual_enabled_variables, line_of_constants, eval_matrix_double, seed);

	double best_threshold;
	for (int i = 0; i < code_length; i++) {   // read the t_mep_chromosome from top to down
		double sum_of_errors;
		if (prg[i].op >= 0)// a vairable
			if (prg[i].op < num_total_variables) { // a variable, which is cached already
				sum_of_errors = cached_sum_of_errors[prg[i].op];
				best_threshold = cached_threashold[prg[i].op];
			}
			else {// a constant
				if (mep_dataset.get_num_items_class_0() < num_rows - mep_dataset.get_num_items_class_0()) {// i must classify everything as 1
					sum_of_errors = mep_dataset.get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][0] - 1;
				}
				else {// less of 1, I must classify everything as class 0
					sum_of_errors = num_rows - mep_dataset.get_num_items_class_0();
					best_threshold = eval_matrix_double[line_of_constants[prg[i].op - num_total_variables]][0];
				}
				sum_of_errors /= (double)num_rows;
				sum_of_errors *= 100;
			}
		else {// an operator
			double* eval = eval_matrix_double[i];

			int num_0_incorrect = 0;
			for (int k = 0; k < random_subset_selection_size; k++) {
				tmp_value_class[k].value = eval[random_subset_indexes[k]];
				tmp_value_class[k].data_class = (int)data[random_subset_indexes[k]][num_total_variables];
				if (data[random_subset_indexes[k]][num_total_variables] < 0.5)
					num_0_incorrect++;
			}
			qsort((void*)tmp_value_class, random_subset_selection_size, sizeof(s_value_class), sort_function_value_class);

			//			int num_0_incorrect = mep_dataset->get_num_items_class_0();
			int num_1_incorrect = 0;
			best_threshold = tmp_value_class[0].value - 1;// all are classified to class 1 in this case
			sum_of_errors = num_0_incorrect;

			for (int t = 0; t < random_subset_selection_size; t++) {
				int j = t + 1;
				while (j < random_subset_selection_size && fabs(tmp_value_class[t].value - tmp_value_class[j].value) < 1e-6)// toate care sunt egale ca sa pot stabili thresholdul
					j++;

				// le verific pe toate intre i si j si le cataloghez ca apartinant la clasa 0
				for (int k = t; k < j; k++)
					if (tmp_value_class[k].data_class == 0)
						num_0_incorrect--;
					else
						if (tmp_value_class[k].data_class == 1) {
							//num_0_incorrect--;
							num_1_incorrect++;
						}
				if (num_0_incorrect + num_1_incorrect < sum_of_errors) {
					sum_of_errors = num_0_incorrect + num_1_incorrect;
					best_threshold = tmp_value_class[t].value;
				}
				t = j;
				t--;
			}
			sum_of_errors /= (double)random_subset_selection_size;
			sum_of_errors *= 100;
		}

		if (fitness > sum_of_errors) {
			fitness = sum_of_errors;
			index_best_genes[0] = i;
			best_class_threshold = best_threshold;
			num_incorrectly_classified = fitness;
		}
	}

	max_index_best_genes = index_best_genes[0];
	if (line_of_constants)
		delete[] line_of_constants;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_binary_classification_error_on_double_data(double** data, int num_data,
	int output_col, double& error)
{
	error = 0;
	double actual_output_double[1];

	//	int num_valid = 0;
	int index_error_gene;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			if (actual_output_double[0] <= best_class_threshold)
				error += data[k][output_col];
			else
				error += 1 - data[k][output_col];
		}
		else
			error++;
	}
	error /= num_data;
	error *= 100;

	return true;
}
//---------------------------------------------------------------------------
bool t_mep_chromosome::compute_binary_classification_error_on_double_data_return_error(
	double** data, int num_data, int output_col, double& error, int& index_error_gene)
{
	error = 0;
	double actual_output_double[1];

	//	int num_valid = 0;
	for (int k = 0; k < num_data; k++) {
		if (evaluate_double(data[k], actual_output_double, index_error_gene)) {
			if (actual_output_double[0] <= best_class_threshold)
				error += data[k][output_col];
			else
				error += 1 - data[k][output_col];
		}
		else
			return false;
	}
	error /= num_data;
	error *= 100;

	return true;
}
//---------------------------------------------------------------------------
