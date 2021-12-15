// Author: Mihai Oltean, mihai.oltean@gmail.com
// https://mepx.org
// https://github.com/mepx
// License: MIT
//-----------------------------------------------------------------
#ifndef mep_DATA_CLASS_H
#define mep_DATA_CLASS_H

//-----------------------------------------------------------------
#include "pugixml.hpp"
#include "mep_rands.h"
#include "csv_utils.h"
//-----------------------------------------------------------------

#define MEP_DATA_DOUBLE 0
#define MEP_DATA_STRING 1
//-----------------------------------------------------------------

#define MEP_E_OK 0
#define MEP_E_CANNOT_MOVE_DATA_OF_DIFFERENT_TYPES 1
#define MEP_E_NOT_ENOUGH_DATA_TO_MOVE 2
#define MEP_E_DEST_AND_SOURCE_MUST_HAVE_THE_SAME_NUMBER_OF_COLUMNS 3

//-----------------------------------------------------------------
class t_mep_data: public t_setter_data_base {
private:
	unsigned int num_cols;
	unsigned int num_data;

	double **_data_double;

	char* **_data_string;

	unsigned int num_outputs;

	unsigned int num_class_0;

	unsigned int num_classes;

	unsigned char data_type; // 0-double, 1- string

	char list_separator;

	bool _modified;

	bool from_csv_double(const char *file_name);
	bool from_csv_string(const char *file_name);
	bool from_csv_string_old(const char* file_name);

	void delete_double_data(void);
	void delete_string_data(void);

	// tries to convert strings to real values
	bool to_double(void);
	void add_string_data(unsigned int row, unsigned int col, const char* data);
	void add_string_data_to_row(unsigned int row, unsigned int col, const char* data);

	void set_to_minus_one(void);

public:

	t_mep_data(void);
	~t_mep_data();

	// fills the internal matrix wiht data from string s which must contain tab separated data
	void from_tab_separated_string(const char* s);
	
	// returns the number of rows training data
	unsigned int get_num_rows(void)const;

	// returns the number of columns of the training data
	unsigned int get_num_cols(void)const;

	// returns the data type of the data: 
	// 0 for real
	// 1 for string
	unsigned int get_data_type(void)const;

	unsigned int get_num_outputs(void)const;
	void set_num_outputs(unsigned int new_num);

	// returns a training data as double (if data type is 0)
	// assumes that row and col are valid; no test for out of range are performed
	double** get_data_matrix_double(void) const;

	// returns a training data as string (if data type is 1)
	// assumes that row and col are valid; no test for out of range are performed
	char*** get_data_matrix_string(void) const;
	
	// returns an entire row as a pointer to double
	// data type must be 0,
	double* get_row(unsigned int row)const;

	double get_value_double(unsigned int row, unsigned int col)const;
	char* get_value_string(unsigned int row, unsigned int col)const;

	// clears the data internal structures and deletes memory
	void clear_data(void);
	void delete_data(void);

	// init data
    void init(void);

    int from_xml(pugi::xml_node parent);
    int to_xml(pugi::xml_node parent);

	// saves the training data to a csv files
	bool to_csv(const char *file_name, char ListSeparator) const;
	
	// loads the data from a csv file
	bool from_csv(const char *filename);

	// loads from PROBEN1 format
	// num_classes must be known in advanced because it is an "1 of m" format
	// if num_classes is 0, it means that is a regression problem
	bool from_one_of_m_format(const char *filename, unsigned int num_classes);

	// transform string values to real values
	int to_numeric(t_mep_data *other_data1, t_mep_data* other_data2);

	// scale to interval in the entire matrix
	int to_interval_everywhere(double min, double max, t_mep_data *other_data1, t_mep_data* other_data2);
	// scale to interval in all variables
	int to_interval_all_variables(double min, double max, t_mep_data *other_data1, t_mep_data* other_data2);
	// scale to interval in the selected column
	int to_interval_selected_col(double min, double max, unsigned int col, t_mep_data *other_data1, t_mep_data* other_data2);
    
	// replace a given string with another string; returns the number of replacements
	int replace_symbol_from_selected_col(const char *find_what, const char* replace_with, unsigned int col, bool use_regular);
	int replace_symbol_from_all_variables(const char *find_what, const char* replace_with, bool use_regular);
	int replace_symbol_everywhere(const char *s_find_what, const char* s_replace_with, bool use_regular);

	// returns the number of occurences of a given string
	int find_symbol_from_selected_col(const char *find_what, unsigned int col, bool use_regular);
	int find_symbol_from_all_variables(const char *find_what, bool use_regular);
	int find_symbol_everywhere(const char *s_find_what, bool use_regular);

	// randomly changes the rows of the data
	void shuffle(t_seed &seed);

	bool detect_list_separator(const char *file_name);

	// moves last count rows to another t_mep_data object
	int move_to(t_mep_data *, unsigned int count);

	// computes how many row belong to the first class
	void count_0_class(unsigned int target_col);

	// returns true if the problem is a binary classification problem
	bool is_binary_classification_problem(void)const;

	// returns true if the problem is a multi classs classification problem
	bool is_multi_class_classification_problem(void)const;

	// remove_empty_rows
	void remove_empty_rows(void);

	// returns true if data have been modified
	bool is_modified(void)const;

	// returns the number of items belonging to class 0
	unsigned int get_num_items_class_0(void)const;

	// counts the number of classes and stores them into an internal variable
	void count_num_classes(unsigned int target_col);

	// returns the number of classes
	unsigned int get_num_classes(void)const;

	// returns true is data could be a time serie
	bool could_be_time_serie(void) const;

	// transform the (1 row or 1 column) data into a matrix based on window size
	// do not call it from here
	// use the method from the t_mep class because validation and test must be empty
	// also the method from t_mep sets the problem_type to MEP_PROBLEM_TIME_SERIE
	bool to_time_serie(unsigned int window_size);

	// append another data and steal all rows from it
	void append_and_steal(t_mep_data& other);

	void reverse_time_serie(void);
};
//-----------------------------------------------------------------
#endif // DATA_CLASS_H_INCLUDED
