#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

// Test counter
extern int tests_passed;
extern int tests_failed;

// Color codes
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_RESET "\033[0m"

// Test macros
#define TEST_START(name) printf("\n" COLOR_YELLOW "Testing: %s" COLOR_RESET "\n", name)
#define TEST_PASS(msg) do { printf(COLOR_GREEN "  ✓ PASS: %s" COLOR_RESET "\n", msg); tests_passed++; } while(0)
#define TEST_FAIL(msg) do { printf(COLOR_RED "  ✗ FAIL: %s" COLOR_RESET "\n", msg); tests_failed++; } while(0)
#define ASSERT_TRUE(cond, msg) if (cond) TEST_PASS(msg); else TEST_FAIL(msg)
#define ASSERT_FALSE(cond, msg) if (!(cond)) TEST_PASS(msg); else TEST_FAIL(msg)
#define ASSERT_EQUAL(a, b, msg) if ((a) == (b)) TEST_PASS(msg); else TEST_FAIL(msg)
#define ASSERT_STR_EQUAL(a, b, msg) if (strcmp(a, b) == 0) TEST_PASS(msg); else TEST_FAIL(msg)

// Test function declarations
void test_validate_date();
void test_write_csv_field();
void test_generate_next_id();
void test_csv_operations();
void test_data_validation();

// E2E test function
void run_e2e_tests();

#endif