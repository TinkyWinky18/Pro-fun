#ifndef BUDGET_H
#define BUDGET_H

#include <stdio.h>

// Global variables
extern int ch;
extern FILE *help;
extern char filename[256];

// Main functions
void menu();
void addContent();
void write_csv_field(FILE *fp, const char *s);
void searchContent();
void deleteContent();
void editContent();
void displayAllContent();
char* generate_next_id();
int validate_date(const char *date, int *year, int *month, int *day);

// Utility function
void set_data_filename(const char *path);

// Test functions
void run_all_tests(void);      // Unit tests
void run_e2e_tests(void);      // E2E tests

#endif