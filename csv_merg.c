/*
 * csv_merge.c
 *
 * AUTHOR        Jethro Swanson
 * DATE          Oct 24, 2021
 *
 * PURPOSE: Takes in 2 csv files and preformes a natrual, left, and full outer join on them
 *          storing the results in files named after the join that was preformed .
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define MAX_LINE 1000 //maximum size of any line in an input csv file
#define MAX_COL 100  //maximum number of columbs an input csv can have
#define null "NULL" // "NULL" is the expected entry for any null values in the csv
#define EMPTY "csv_merge.c - empty csv_row identifier" //placeholder for empty rows in an input file

//names of the two files to be processed, they must be in the same directory as this program.
#define FILENAME1 "input1.txt"
#define FILENAME2 "input2.txt"

typedef struct CSV_COL
{
	char* value;
} Csv_col;

typedef struct CSV_ROW
{
	Csv_col* col[MAX_COL];
} Csv_row;

/**
 * PURPOSE: uses assertions to ensure the values variable contains appropriate data
 * INPUT PARAMETERS:
 *    values: values variable to be validated
 *    values_size: number of items in the values array
 */
void validate_values(char values[MAX_COL][MAX_LINE], int values_size)
{
	for (int i = 0; i < values_size; i++)
	{
		assert(NULL != values[i]);
	}
}


/**
 * PURPOSE: counts the number of lines in a file
 * INPUT PARAMETERS:
 *    filename: contains the name of the file that must be counted
 * OUTPUT PARAMETERS:
 *    returns an int representing the number of lines contained within the file
 */
int count_lines(char* filename)
{
	FILE* input;
	char line[MAX_LINE] = "\0";
	int line_count = 0;

	input = fopen(filename, "r");

	while (NULL != fgets(line, MAX_LINE, input))
	{
		line_count++;
	}

	fclose(input);
	return line_count;
}

/**
 * PURPOSE: counts the number of columbs in the first line of a csv
 * INPUT PARAMETERS:
 *    filename: contains the name of the file that must be counted
 *    SEPERATORS: string of seperators to break up the line on
 * OUTPUT PARAMETERS:
 *    returns an int representing the number of columbs contained within the file
 */
int count_columbs(char* filename, char* SEPERATORS)
{
	FILE* input;
	char line[MAX_LINE] = "\0";
	int col_count = 0;
	char* token;

	input = fopen(filename, "r");

	if (NULL != fgets(line, MAX_LINE, input))
	{
		token = strtok(line, SEPERATORS);
		while (NULL != token)
		{
			col_count++;
			token = strtok(NULL, SEPERATORS);
		}
	}
	else
	{
		col_count = -1;
	}

	fclose(input);
	return col_count;
}


/**
 * PURPOSE: creates a new Csv_col struct dynamically allocating memeory for it and containing the input information
 * INPUT PARAMETERS:
 *    col: the name of the columb this item represents
 * OUTPUT PARAMETERS:
 *    returns a Csv_col struct containing the input information
 */
Csv_col* new_csv_col(char* col)
{
	Csv_col* new_csv_col = NULL;
	new_csv_col = malloc(sizeof(Csv_col));

	if (NULL != new_csv_col)
	{
		new_csv_col->value = malloc(strlen(col) + 1);
		if (NULL != new_csv_col->value)
		{
			strcpy(new_csv_col->value, col);
		}
		else
		{
			free(new_csv_col);
			new_csv_col = NULL;
		}
	}
	return new_csv_col;
}

/**
 * PURPOSE: creates a new Csv_row struct dynamically allocating memeory for it and containing the input information
 * INPUT PARAMETERS:
 *    values: an array of strings to used as each columbs value
 *    value_count: The number of items contained in values
 * OUTPUT PARAMETERS:
 *    returns a Csv_row struct containing the input information
 */
Csv_row* new_csv_row(char values[MAX_COL][MAX_LINE], int value_count)
{
	Csv_row* new_csv_row = NULL;
	new_csv_row = malloc(sizeof(Csv_row));

	if (NULL != new_csv_row)
	{
		for (int i = 0; i < value_count; i++)
		{
			new_csv_row->col[i] = new_csv_col(values[i]);
		}
	}
	return new_csv_row;
}

/**
 * PURPOSE: prints a columb out to the console for testing purposes
 * INPUT PARAMETERS:
 *    col: columb to be printed
 * OUTPUT PARAMETERS:
 *    prints value contained in columb to console
 */
void print_csv_col(Csv_col* col)
{
	printf("[%s]", col->value);
	fflush(stdout);
}

/**
 * PURPOSE: prints a csv row out to the console for testing purposes
 * INPUT PARAMETERS:
 *    row: row to be printed
 *    num_cols: number of columbs the row encompases
 * OUTPUT PARAMETERS:
 *    prints values contained in row to console
 */
void print_csv_row(Csv_row* row, int num_cols)
{
	for (int i = 0; i < num_cols; i++)
	{
		print_csv_col(row->col[i]);
		printf(", ");
		fflush(stdout);
	}
	printf("\n");
	fflush(stdout);
}

/**
 * PURPOSE: preformes a natural join on the two csvs represented by inputs and prints the resulting table as a csv named Natural_Join.txt
 * INPUT PARAMETERS:
 *    csv1_columbs: an array of Csv_col structs holding the names of the columbs in csv1
 *    csv1_rows: an array of Csv_row structs holding all rows of csv1
 *    csv1_col_count: number of columbs csv1 containes
 *    csv1_row_count: number of rows csv1 containes
 *    csv2_columbs: an array of Csv_col structs holding the names of the columbs in csv2
 *    csv2_rows: an array of Csv_row structs holding all rows of csv2
 *    csv2_col_count: number of columbs csv2 containes
 *    csv2_row_count: number of rows csv2 containes
 * OUTPUT PARAMETERS:
 *    creates a new file named Natural_Join.txt and puts the result of a natural joining the input csv values in it
 */
void natural_join(Csv_col* csv1_columbs, Csv_row* csv1_rows, int csv1_col_count, int csv1_row_count, Csv_col* csv2_columbs, Csv_row* csv2_rows, int csv2_col_count, int csv2_row_count)
{
	const char* output_name = "Natural_Join.txt";
	int joined_row_count = 0;
	int joined_col_count = 0;
	char joined_cols[MAX_COL][MAX_LINE];
	Csv_row* joined_rows = calloc(csv1_row_count + csv2_row_count, sizeof(Csv_row));
	char values[MAX_COL][MAX_LINE];
	int values_size = 0;
	char joined_values[MAX_COL][MAX_LINE];
	int joined_values_size = 0;
	int is_joined_col = 0;
	FILE* output;
	int equivilent_counter = 0;


	for (int i = 0; i < csv1_col_count; i++)
	{
		for (int j = 0; j < csv2_col_count; j++)
		{
			if (0 == strcmp(csv1_columbs[i].value, csv2_columbs[j].value) && 0 != strcmp(csv1_columbs[i].value, null))
			{
				strcpy(joined_cols[joined_col_count], csv1_columbs[i].value);
				joined_col_count++;
			}
		}
	}

	for (int k = 0; k < csv1_row_count; k++)
	{
		for (int l = 0; l < csv2_row_count;l++)
		{
			for (int i2 = 0; i2 < joined_col_count; i2++)
			{
				for (int j2 = 0; j2 < csv1_col_count; j2++)
				{
					if (0 == strcmp(csv1_columbs[j2].value, joined_cols[i2]))
					{
						for (int k2 = 0; k2 < csv2_col_count; k2++)
						{
							if (0 == strcmp(csv2_columbs[k2].value, joined_cols[i2]) && 0 != strcmp(csv2_columbs[k2].value, null))
							{
								if (0 == strcmp(csv1_rows[k].col[j2]->value, csv2_rows[l].col[k2]->value) && 0 != strcmp(csv1_rows[k].col[j2]->value, null))
								{
									equivilent_counter++;
								}
							}
						}
					}
				}
			}
			if (equivilent_counter == joined_col_count)
			{
				for (int m = 0; m < csv1_col_count; m++)
				{
					is_joined_col = 0;
					for (int n = 0; n < joined_col_count; n++)
					{

						if (0 == strcmp(csv1_columbs[m].value, joined_cols[n]) && 0 != strcmp(csv1_columbs[m].value, null))
						{
							is_joined_col = 1;
							strcpy(joined_values[joined_values_size], csv1_rows[k].col[m]->value);
							joined_values_size++;
						}
					}
					if (!is_joined_col) {
						assert(NULL != csv1_rows[k].col[m]->value);
						strcpy(values[values_size], csv1_rows[k].col[m]->value);
						assert(NULL != values[values_size]);
						values_size++;
					}
				}

				is_joined_col = 0;
				for (int m = 0; m < csv2_col_count; m++)
				{
					is_joined_col = 0;
					for (int n = 0; n < joined_col_count; n++)
					{
						if (0 == strcmp(csv2_columbs[m].value, joined_cols[n]) && 0 != strcmp(csv2_columbs[m].value, null))
						{
							is_joined_col = 1;
						}
					}
					if (!is_joined_col)
					{
						assert(NULL != csv2_rows[l].col[m]->value);
						strcpy(values[values_size], csv2_rows[l].col[m]->value);
						assert(NULL != values[values_size]);
						values_size++;
					}
				}

				for (int m = 0; m < joined_values_size; m++)
				{
					assert(NULL != joined_values[m]);
					strcpy(values[values_size], joined_values[m]);
					assert(NULL != values[values_size]);
					values_size++;
				}
				joined_values_size = 0;

			}
			equivilent_counter = 0;

			validate_values(values, values_size);
			if (0 < values_size)
			{
				joined_rows[joined_row_count] = *new_csv_row(values, csv1_col_count + csv2_col_count - joined_col_count);
				joined_row_count++;
			}
			values_size = 0;
		}
	}
	output = fopen(output_name, "w");

	for (int i = 0; i < csv1_col_count; i++)
	{
		is_joined_col = 0;
		for (int n = 0; n < joined_col_count; n++)
		{
			if (0 == strcmp(csv1_columbs[i].value, joined_cols[n]))
			{
				is_joined_col = 1;
			}
		}
		if (!is_joined_col) {
			fprintf(output, "%s", csv1_columbs[i].value);
		}
		if (((i < csv1_col_count - 1) || (0 < csv2_col_count - joined_col_count)) && (!is_joined_col))
		{
			fprintf(output, ",");
		}
	}

	for (int j = 0; j < csv2_col_count; j++)
	{
		is_joined_col = 0;
		for (int n = 0; n < joined_col_count; n++)
		{
			if (0 == strcmp(csv2_columbs[j].value, joined_cols[n]))
			{
				is_joined_col = 1;
			}
		}
		if (!is_joined_col) {
			fprintf(output, "%s", csv2_columbs[j].value);
		}
		if (((j < csv2_col_count - 1) || (0 < joined_col_count)) && (!is_joined_col))
		{
			fprintf(output, ",");
		}
	}

	for (int i = 0; i < joined_col_count; i++)
	{
		fprintf(output, "%s", joined_cols[i]);

		if (i < joined_col_count - 1)
		{
			fprintf(output, ",");
		}
	}

	for (int i = 0; i < joined_row_count; i++)
	{
		fprintf(output, "\n");
		for (int j = 0; j < csv1_col_count + csv2_col_count - joined_col_count; j++)
		{
			fprintf(output, "%s", joined_rows[i].col[j]->value);
			if (j < csv1_col_count + csv2_col_count - joined_col_count - 1)
			{
				fprintf(output, ",");
			}
		}
	}

	fclose(output);

	for (int i = 0; i < joined_row_count; i++)
	{
		for (int j = 0; j < csv1_col_count + csv2_col_count - joined_col_count; j++)
		{
			free(joined_rows[i].col[j]);
		}
	}
	free(joined_rows);
}


/**
 * PURPOSE: preformes a left join on the two csvs represented by inputs and prints the resulting table as a csv named Left_Join.txt
 * INPUT PARAMETERS:
 *    csv1_columbs: an array of Csv_col structs holding the names of the columbs in csv1
 *    csv1_rows: an array of Csv_row structs holding all rows of csv1
 *    csv1_col_count: number of columbs csv1 containes
 *    csv1_row_count: number of rows csv1 containes
 *    csv2_columbs: an array of Csv_col structs holding the names of the columbs in csv2
 *    csv2_rows: an array of Csv_row structs holding all rows of csv2
 *    csv2_col_count: number of columbs csv2 containes
 *    csv2_row_count: number of rows csv2 containes
 * OUTPUT PARAMETERS:
 *    creates a new file named Left_Join.txt and puts the result of a left joining the input csv values in it
 */
void left_join(Csv_col* csv1_columbs, Csv_row* csv1_rows, int csv1_col_count, int csv1_row_count, Csv_col* csv2_columbs, Csv_row* csv2_rows, int csv2_col_count, int csv2_row_count)
{
	const char* output_name = "Left_Join.txt";
	int joined_row_count = 0;
	int joined_col_count = 0;
	char joined_cols[MAX_COL][MAX_LINE];
	Csv_row* joined_rows = calloc(csv1_row_count + csv2_row_count, sizeof(Csv_row));
	char values[MAX_COL][MAX_LINE];
	int values_size = 0;
	int is_joined_col = 0; //boolean
	FILE* output;
	int equivilent_counter = 0;
	int row_matched = 0; //boolean
	int row_completed = 0;//boolean


	for (int i = 0; i < csv1_col_count; i++)
	{
		for (int j = 0; j < csv2_col_count; j++)
		{
			if (0 == strcmp(csv1_columbs[i].value, csv2_columbs[j].value) && 0 != strcmp(csv1_columbs[i].value, null))
			{
				strcpy(joined_cols[joined_col_count], csv1_columbs[i].value);
				joined_col_count++;
			}
		}
	}

	for (int k = 0; k < csv1_row_count; k++)
	{
		row_completed = 0;
		for (int l = 0; l < csv2_row_count;l++)
		{
			equivilent_counter = 0;
			for (int i2 = 0; i2 < joined_col_count; i2++)
			{
				for (int j2 = 0; j2 < csv1_col_count; j2++)
				{
					if (0 == strcmp(csv1_columbs[j2].value, joined_cols[i2]))
					{
						for (int k2 = 0; k2 < csv2_col_count; k2++)
						{
							if (0 == strcmp(csv2_columbs[k2].value, joined_cols[i2]) && 0 != strcmp(csv2_columbs[k2].value, null))
							{
								if (0 == strcmp(csv1_rows[k].col[j2]->value, csv2_rows[l].col[k2]->value)
									&& 0 != strcmp(csv1_rows[k].col[j2]->value, null)
									&& 0 != strcmp(csv2_rows[l].col[k2]->value, null)
									&& !(equivilent_counter == joined_col_count)
									&& !(row_completed))
								{

									equivilent_counter++;
								}
							}
						}
					}
				}
			}
			if (equivilent_counter == joined_col_count && !row_completed)
			{
				values_size = 0;
				row_matched = 1;

				for (int m = 0; m < csv1_col_count; m++)
				{
					for (int m2 = 0; m2 < joined_col_count; m2++)
					{
						if (0 == strcmp(csv1_columbs[m].value, joined_cols[m2]))
						{
							if (0 == strcmp(csv1_rows[k].col[m]->value, null))
							{
								row_matched = 0;
							}
						}
					}
					assert(NULL != csv1_rows[k].col[m]->value);
					strcpy(values[values_size], csv1_rows[k].col[m]->value);
					assert(NULL != values[values_size]);
					values_size++;
				}

				is_joined_col = 0;
				for (int m = 0; m < csv2_col_count; m++)
				{
					is_joined_col = 0;
					for (int n = 0; n < joined_col_count; n++)
					{
						if (0 == strcmp(csv2_columbs[m].value, joined_cols[n]) && 0 != strcmp(csv2_columbs[m].value, null))
						{
							if (0 == strcmp(csv2_rows[l].col[m]->value, null))
							{
								row_matched = 0;
							}
							is_joined_col = 1;
						}
					}
					if (!is_joined_col)
					{
						assert(NULL != csv2_rows[l].col[m]->value);
						strcpy(values[values_size], csv2_rows[l].col[m]->value);
						assert(NULL != values[values_size]);
						values_size++;
					}
				}

				//creates a completed row
				validate_values(values, values_size);
				if (0 < values_size && row_matched && !row_completed)
				{
					joined_rows[joined_row_count] = *new_csv_row(values, csv1_col_count + csv2_col_count - joined_col_count);
					joined_row_count++;
					row_completed = 1;
				}
				values_size = 0;

			}
			equivilent_counter = 0;

		}

		if (!row_matched && !row_completed) //handles if left row had no right counterpart
		{
			values_size = 0;
			for (int m = 0; m < csv1_col_count; m++)
			{
				assert(NULL != csv1_rows[k].col[m]->value);
				strcpy(values[values_size], csv1_rows[k].col[m]->value);
				assert(NULL != values[values_size]);
				values_size++;
			}
			for (int m = 0; m < csv2_col_count - joined_col_count; m++)
			{
				strcpy(values[values_size], null);
				assert(NULL != values[values_size]);
				values_size++;
			}
			joined_rows[joined_row_count] = *new_csv_row(values, csv1_col_count + csv2_col_count - joined_col_count);

			joined_row_count++;
			row_completed = 1;
		}

		row_matched = 0;
	}

	output = fopen(output_name, "w");

	//prints columbs to output file
	for (int i = 0; i < csv1_col_count; i++)
	{
		fprintf(output, "%s", csv1_columbs[i].value);

		if (((i < csv1_col_count - 1) || (0 < csv2_col_count)))
		{
			fprintf(output, ",");
		}
	}

	for (int j = 0; j < csv2_col_count; j++)
	{
		is_joined_col = 0;
		for (int n = 0; n < joined_col_count; n++)
		{
			if (0 == strcmp(csv2_columbs[j].value, joined_cols[n]))
			{
				is_joined_col = 1;
			}
		}
		if (!is_joined_col) {
			fprintf(output, "%s", csv2_columbs[j].value);
		}
		if ((j < csv2_col_count - 1) && (!is_joined_col))
		{
			fprintf(output, ",");
		}
	}

	for (int i = 0; i < joined_row_count; i++)
	{
		fprintf(output, "\n");
		for (int j = 0; j < csv1_col_count + csv2_col_count - joined_col_count; j++)
		{
			fprintf(output, "%s", joined_rows[i].col[j]->value);
			if (j < csv1_col_count + csv2_col_count - joined_col_count - 1)
			{
				fprintf(output, ",");
			}
		}
	}

	fclose(output);
}


/**
 * PURPOSE: preformes a full outer join on the two csvs represented by inputs and prints the resulting table as a csv named Full_Outer_Join.txt
 * INPUT PARAMETERS:
 *    csv1_columbs: an array of Csv_col structs holding the names of the columbs in csv1
 *    csv1_rows: an array of Csv_row structs holding all rows of csv1
 *    csv1_col_count: number of columbs csv1 containes
 *    csv1_row_count: number of rows csv1 containes
 *    csv2_columbs: an array of Csv_col structs holding the names of the columbs in csv2
 *    csv2_rows: an array of Csv_row structs holding all rows of csv2
 *    csv2_col_count: number of columbs csv2 containes
 *    csv2_row_count: number of rows csv2 containes
 * OUTPUT PARAMETERS:
 *    creates a new file named Full_Outer_Join.txt and puts the result of a full outer joining the input csv values in it
 */
void full_outer_join(Csv_col* csv1_columbs, Csv_row* csv1_rows, int csv1_col_count, int csv1_row_count, Csv_col* csv2_columbs, Csv_row* csv2_rows, int csv2_col_count, int csv2_row_count)
{
	const char* output_name = "Full_Outer_Join.txt";
	int joined_row_count = 0;
	int joined_col_count = 0;
	char joined_cols[MAX_COL][MAX_LINE];
	Csv_row* joined_rows = calloc(csv1_row_count + csv2_row_count, sizeof(Csv_row));
	Csv_row* csv2_unjoined_rows = calloc(csv2_row_count, sizeof(Csv_row));
	char values[MAX_COL][MAX_LINE];
	int values_size = 0;
	int is_joined_col = 0; //boolean
	FILE* output;
	int equivilent_counter = 0;
	int row_matched = 0; //boolean
	int empty_row_len = 1;
	char empty_values[1][MAX_LINE] = { EMPTY };

	//duplicates the csv2 row list
	for (int i = 0; i < csv2_row_count; i++)
	{
		csv2_unjoined_rows[i] = csv2_rows[i];
	}

	for (int i = 0; i < csv1_col_count; i++)
	{
		for (int j = 0; j < csv2_col_count; j++)
		{
			if (0 == strcmp(csv1_columbs[i].value, csv2_columbs[j].value) && 0 != strcmp(csv1_columbs[i].value, null))
			{
				strcpy(joined_cols[joined_col_count], csv1_columbs[i].value);
				joined_col_count++;
			}
		}
	}

	for (int k = 0; k < csv1_row_count; k++)
	{
		for (int l = 0; l < csv2_row_count;l++)
		{
			for (int i2 = 0; i2 < joined_col_count; i2++)
			{
				for (int j2 = 0; j2 < csv1_col_count; j2++)
				{
					if (0 == strcmp(csv1_columbs[j2].value, joined_cols[i2]))
					{
						for (int k2 = 0; k2 < csv2_col_count; k2++)
						{
							if (0 == strcmp(csv2_columbs[k2].value, joined_cols[i2]) && 0 != strcmp(csv2_columbs[k2].value, null))
							{
								if (0 == strcmp(csv1_rows[k].col[j2]->value, csv2_rows[l].col[k2]->value) && 0 != strcmp(csv1_rows[k].col[j2]->value, null))
								{
									equivilent_counter++;
								}
							}
						}
					}
				}
			}
			if (equivilent_counter == joined_col_count)
			{
				row_matched = 1;
				values_size = 0;
				csv2_unjoined_rows[l] = *new_csv_row(empty_values, empty_row_len);

				for (int m = 0; m < csv1_col_count; m++)
				{
					for (int m2 = 0; m2 < joined_col_count; m2++)
					{
						if (0 == strcmp(csv1_columbs[m].value, joined_cols[m2]))
						{
							if (0 == strcmp(csv1_rows[k].col[m]->value, null))
							{
								row_matched = 0;
							}
						}
					}
					assert(NULL != csv1_rows[k].col[m]->value);
					strcpy(values[values_size], csv1_rows[k].col[m]->value);
					assert(NULL != values[values_size]);
					values_size++;
				}


				is_joined_col = 0;
				for (int m = 0; m < csv2_col_count; m++)
				{
					is_joined_col = 0;
					for (int n = 0; n < joined_col_count; n++)
					{
						if (0 == strcmp(csv2_columbs[m].value, joined_cols[n]) && 0 != strcmp(csv2_columbs[m].value, null))
						{
							is_joined_col = 1;
						}
					}
					if (!is_joined_col)
					{
						assert(NULL != csv2_rows[l].col[m]->value);
						strcpy(values[values_size], csv2_rows[l].col[m]->value);
						assert(NULL != values[values_size]);
						values_size++;
					}
				}

				//creates a completed row
				validate_values(values, values_size);
				if (0 < values_size && row_matched)
				{
					joined_rows[joined_row_count] = *new_csv_row(values, csv1_col_count + csv2_col_count - joined_col_count);
					joined_row_count++;
				}
				values_size = 0;

			}
			equivilent_counter = 0;

		}

		if (!row_matched) //handles if left row had no right counterpart
		{
			values_size = 0;
			for (int m = 0; m < csv1_col_count; m++)
			{
				assert(NULL != csv1_rows[k].col[m]->value);
				strcpy(values[values_size], csv1_rows[k].col[m]->value);
				assert(NULL != values[values_size]);
				values_size++;
			}
			for (int m = 0; m < csv2_col_count - joined_col_count; m++)
			{
				strcpy(values[values_size], null);
				assert(NULL != values[values_size]);
				values_size++;
			}
			joined_rows[joined_row_count] = *new_csv_row(values, csv1_col_count + csv2_col_count - joined_col_count);
			joined_row_count++;
		}

		row_matched = 0;
	}

	//handles all right rows with no left counterpart
	values_size = 0;
	for (int i = 0; i < csv2_row_count; i++)
	{
		if (0 != strcmp(csv2_unjoined_rows[i].col[0]->value, EMPTY))
		{
			for (int j = 0; j < csv1_col_count; j++)
			{
				is_joined_col = 0;
				for (int n = 0; n < joined_col_count && !is_joined_col; n++)
				{
					if (0 == strcmp(csv1_columbs[j].value, joined_cols[n]) && 0 != strcmp(csv1_columbs[j].value, null))
					{
						is_joined_col = 1;
					}
				}
				if (!is_joined_col)
				{
					strcpy(values[values_size], null);
					values_size++;
				}
				else
				{
					for (int k = 0; k < csv2_col_count; k++)
					{
						if (0 == strcmp(csv1_columbs[j].value, csv2_columbs[k].value))
						{
							strcpy(values[values_size], csv2_unjoined_rows[i].col[k]->value);
							values_size++;
						}
					}

				}
			}
			for (int j = 0; j < csv2_col_count; j++)
			{
				is_joined_col = 0;
				for (int n = 0; n < joined_col_count && !is_joined_col; n++)
				{
					if (0 == strcmp(csv2_columbs[j].value, joined_cols[n]) && 0 != strcmp(csv2_columbs[j].value, null))
					{
						is_joined_col = 1;
					}
				}
				if (!is_joined_col)
				{
					strcpy(values[values_size], csv2_unjoined_rows[i].col[j]->value);
					values_size++;
				}
			}

			joined_rows[joined_row_count] = *new_csv_row(values, csv1_col_count + csv2_col_count - joined_col_count);
			joined_row_count++;
			values_size = 0;
		}
	}

	output = fopen(output_name, "w");

	//prints columbs to output file
	for (int i = 0; i < csv1_col_count; i++)
	{
		fprintf(output, "%s", csv1_columbs[i].value);

		if (((i < csv1_col_count - 1) || (0 < csv2_col_count)))
		{
			fprintf(output, ",");
		}
	}

	for (int j = 0; j < csv2_col_count; j++)
	{
		is_joined_col = 0;
		for (int n = 0; n < joined_col_count; n++)
		{
			if (0 == strcmp(csv2_columbs[j].value, joined_cols[n]))
			{
				is_joined_col = 1;
			}
		}
		if (!is_joined_col) {
			fprintf(output, "%s", csv2_columbs[j].value);
		}
		if ((j < csv2_col_count - 1) && (!is_joined_col))
		{
			fprintf(output, ",");
		}
	}

	for (int i = 0; i < joined_row_count; i++)
	{
		fprintf(output, "\n");
		for (int j = 0; j < csv1_col_count + csv2_col_count - joined_col_count; j++)
		{
			fprintf(output, "%s", joined_rows[i].col[j]->value);
			if (j < csv1_col_count + csv2_col_count - joined_col_count - 1)
			{
				fprintf(output, ",");
			}
		}
	}

	fclose(output);
}


int main(void)
{
	char* SEPERATORS = ",\n";
	FILE* input1, * input2;
	int csv1_row_count = 0;
	int csv1_col_count = 0;
	int csv2_row_count = 0;
	int csv2_col_count = 0;
	int curr_row = 0;
	char line[MAX_LINE * MAX_COL] = "\0";
	char temp_col[MAX_LINE] = "\0";
	char* token;
	char values[MAX_COL][MAX_LINE];
	Csv_col* csv1_columbs;
	Csv_col* csv2_columbs;
	Csv_row* csv1_rows;
	Csv_row* csv2_rows;

	input1 = fopen(FILENAME1, "r");
	assert(NULL != input1);

	input2 = fopen(FILENAME2, "r");
	assert(NULL != input2);
	;

    //only attempts to process files if they both exist and can be opened 
	if (NULL != input1 && NULL != input2)
	{
		fclose(input1);
		fclose(input2);

		//gets basic information about both files such as row and columb counts to allow for merging 

		csv1_row_count = count_lines(FILENAME1) - 1;//-1 for columb row
		csv1_col_count = count_columbs(FILENAME1, SEPERATORS);
		assert(0 < csv1_row_count);
		assert(-1 < csv1_col_count);

		csv1_columbs = calloc(csv1_col_count, sizeof(Csv_col));
		csv1_rows = calloc(csv1_row_count, sizeof(Csv_row));
		assert(NULL != csv1_columbs);
		assert(NULL != csv1_rows);

		input1 = fopen(FILENAME1, "r");

		csv2_row_count = count_lines(FILENAME2) - 1;//-1 for collumb row
		csv2_col_count = count_columbs(FILENAME2, SEPERATORS);
		assert(-1 < csv2_row_count);
		assert(-1 < csv2_col_count);

		csv2_columbs = calloc(csv2_col_count, sizeof(Csv_col));
		csv2_rows = calloc(csv2_row_count, sizeof(Csv_row));
		assert(NULL != csv2_columbs);
		assert(NULL != csv2_rows);


		input2 = fopen(FILENAME2, "r");

		//converts the rows and columbs of the first input file to arrays to allow for merging 
		if (NULL != fgets(line, MAX_LINE * MAX_COL, input1))
		{
			token = strtok(line, SEPERATORS);
			for (int i = 0; i < csv1_col_count; i++)
			{
				if (NULL != token) {
					strcpy(temp_col, token);
					if (i == csv1_col_count - 1)
					{
						temp_col[strlen(temp_col) - 1] = '\0';
					}
					csv1_columbs[i] = *new_csv_col(temp_col);
					token = strtok(NULL, SEPERATORS);
				}
			}

			while (NULL != fgets(line, MAX_LINE * MAX_COL, input1))
			{
				token = strtok(line, SEPERATORS);
				for (int i = 0; i < csv1_col_count; i++)
				{
					if (NULL != token && MAX_LINE > strlen(token))
					{
						strcpy(values[i], token);

						//fixes error with strtok where an extra overwritting charecter is placed at the end of a line
						if ((i == csv1_col_count - 1) && (curr_row < csv1_row_count - 1))
						{
							values[i][strlen(values[i]) - 1] = '\0';
						}
						token = strtok(NULL, SEPERATORS);
					}
				}
				csv1_rows[curr_row] = *new_csv_row(values, csv1_col_count);
				curr_row++;
			}
		}

		curr_row = 0;

		//converts the rows and columbs of the second input file to arrays to allow for merging
		if (NULL != fgets(line, MAX_LINE * MAX_COL, input2))
		{
			token = strtok(line, SEPERATORS);
			for (int i = 0; i < csv2_col_count; i++)
			{
				if (NULL != token) {
					strcpy(temp_col, token);
					if (i == csv2_col_count - 1)
					{
						temp_col[strlen(temp_col) - 1] = '\0';
					}
					csv2_columbs[i] = *new_csv_col(temp_col);
					token = strtok(NULL, SEPERATORS);
				}
			}

			while (NULL != fgets(line, MAX_LINE * MAX_COL, input2))
			{
				token = strtok(line, SEPERATORS);
				for (int i = 0; i < csv2_col_count; i++)
				{
					if (NULL != token && MAX_LINE > strlen(token))
					{
						strcpy(values[i], token);

						if ((i == csv2_col_count - 1) && (curr_row < csv2_row_count - 1))
						{
							values[i][strlen(values[i]) - 1] = '\0';
						}
						token = strtok(NULL, SEPERATORS);
					}
				}
				csv2_rows[curr_row] = *new_csv_row(values, csv2_col_count);
				curr_row++;
			}
		}

		//preforms the associated joins, creating nessesary output files
		natural_join(csv1_columbs, csv1_rows, csv1_col_count, csv1_row_count, csv2_columbs, csv2_rows, csv2_col_count, csv2_row_count);
		left_join(csv1_columbs, csv1_rows, csv1_col_count, csv1_row_count, csv2_columbs, csv2_rows, csv2_col_count, csv2_row_count);
		full_outer_join(csv1_columbs, csv1_rows, csv1_col_count, csv1_row_count, csv2_columbs, csv2_rows, csv2_col_count, csv2_row_count);

		fclose(input1);
		fclose(input2);

		//frees all allocated memory
		for (int i = 0; i < csv1_row_count; i++)
		{
			for (int j = 0; j < csv1_col_count; j++)
			{
				free(csv1_rows[i].col[j]);
			}
		}
		free(csv1_rows);

		for (int i = 0; i < csv2_row_count; i++)
		{
			for (int j = 0; j < csv2_col_count; j++)
			{
				free(csv2_rows[i].col[j]);
			}
		}
		free(csv2_rows);

		free(csv1_columbs);
		free(csv2_columbs);
	}
	else
	{
		fprintf(stderr, "Unable to open an input file.\n");
	}

	printf("Program completed succesfully\n");
	return 0;
}