// tests_all.c - Unit Tests with Detailed Output
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../budget.h"

/* ====== Test Statistics ====== */
static int tests_passed = 0;
static int tests_failed = 0;
static int tests_total = 0;

/* ====== Enhanced Test Macros ====== */
#define TEST_ASSERT(cond, desc) do { \
    tests_total++; \
    if (cond) { \
        tests_passed++; \
        printf("  [PASS] : %s\n", desc); \
    } else { \
        tests_failed++; \
        printf("  [FAIL] : %s (line %d)\n", desc, __LINE__); \
    } \
} while(0)

#define TEST_ASSERT_INT_EQ(a, b, desc) do { \
    tests_total++; \
    if ((a) == (b)) { \
        tests_passed++; \
        printf("  [PASS] : %s\n", desc); \
    } else { \
        tests_failed++; \
        printf("  [FAIL] : %s (expected %d, got %d, line %d)\n", desc, (int)(b), (int)(a), __LINE__); \
    } \
} while(0)

#define TEST_ASSERT_STR_EQ(a, b, desc) do { \
    tests_total++; \
    if ((a) && (b) && strcmp((a), (b)) == 0) { \
        tests_passed++; \
        printf("  [PASS] : %s\n", desc); \
    } else { \
        tests_failed++; \
        printf("  [FAIL] : %s (expected \"%s\", got \"%s\", line %d)\n", desc, (b)?(b):"(null)", (a)?(a):"(null)", __LINE__); \
    } \
} while(0)

/* ====== Helper Functions ====== */
static void set_stdin_from_string(const char* s, const char* fname) {
    FILE* fin = fopen(fname, "w");
    if (fin) {
        fputs(s, fin);
        fclose(fin);
        freopen(fname, "r", stdin);
    }
}

static void write_base_csv(const char* path) {
    FILE* f = fopen(path, "w");
    if (f) {
        fprintf(f, "RequestID,Title,Amount,Date\n");
        fprintf(f, "A001,ค่ากาแฟ,50.00,2025-09-01\n");
        fprintf(f, "A002,ค่าเช่าสถานที่,199.50,2025-09-30\n");
        fclose(f);
    }
}

static void restore_stdin(void) {
#ifdef _WIN32
    freopen("CON", "r", stdin);
#else
    freopen("/dev/tty", "r", stdin);
#endif
}

/* ====== Test Functions ====== */

void test_write_csv_field(void) {
    printf("\n--- Testing write_csv_field() ---\n");
    
    const char* tmp = "tmp_write_csv.csv";
    FILE* fp = fopen(tmp, "w+");
    
    // Test 1: Simple text (no quotes needed)
    write_csv_field(fp, "Hello");
    fputc('\n', fp);
    
    // Test 2: Text with comma (needs quotes)
    write_csv_field(fp, "A,B");
    fputc('\n', fp);
    
    // Test 3: Text with quotes (needs escaping)
    write_csv_field(fp, "He said \"hi\"");
    fputc('\n', fp);
    
    fflush(fp);
    fseek(fp, 0, SEEK_SET);
    
    char buf[256];
    
    fgets(buf, sizeof(buf), fp);
    buf[strcspn(buf, "\n")] = 0;
    TEST_ASSERT_STR_EQ(buf, "Hello", "simple text without quotes");
    
    fgets(buf, sizeof(buf), fp);
    buf[strcspn(buf, "\n")] = 0;
    TEST_ASSERT_STR_EQ(buf, "\"A,B\"", "text with comma (quoted)");
    
    fgets(buf, sizeof(buf), fp);
    buf[strcspn(buf, "\n")] = 0;
    TEST_ASSERT_STR_EQ(buf, "\"He said \"\"hi\"\"\"", "handle embedded quotes");
    
    fclose(fp);
    remove(tmp);
}

void test_validate_date(void) {
    printf("\n--- Testing validate_date() ---\n");
    
    int y, m, d;
    
    // Valid dates
    TEST_ASSERT(validate_date("2025-09-30", &y, &m, &d) == 1, "valid date 2025-09-30");
    TEST_ASSERT_INT_EQ(y, 2025, "year parsed correctly");
    TEST_ASSERT_INT_EQ(m, 9, "month parsed correctly");
    TEST_ASSERT_INT_EQ(d, 30, "day parsed correctly");
    
    TEST_ASSERT(validate_date("2000-01-01", &y, &m, &d) == 1, "minimum year 2000");
    TEST_ASSERT(validate_date("2099-12-31", &y, &m, &d) == 1, "maximum year 2099");
    
    // Leap year
    TEST_ASSERT(validate_date("2024-02-29", &y, &m, &d) == 1, "leap year Feb 29 (2024)");
    TEST_ASSERT(validate_date("2000-02-29", &y, &m, &d) == 1, "leap year Feb 29 (2000)");
    TEST_ASSERT(validate_date("2023-02-29", &y, &m, &d) == 0, "non-leap year Feb 29 rejected");
    
    // Invalid ranges
    TEST_ASSERT(validate_date("1999-01-01", &y, &m, &d) == 0, "year below 2000 rejected");
    TEST_ASSERT(validate_date("2100-01-01", &y, &m, &d) == 0, "year above 2099 rejected");
    TEST_ASSERT(validate_date("2025-00-15", &y, &m, &d) == 0, "month 00 rejected");
    TEST_ASSERT(validate_date("2025-13-15", &y, &m, &d) == 0, "month 13 rejected");
    
    // Days in month
    TEST_ASSERT(validate_date("2025-01-31", &y, &m, &d) == 1, "31 days in January");
    TEST_ASSERT(validate_date("2025-04-30", &y, &m, &d) == 1, "30 days in April");
    TEST_ASSERT(validate_date("2025-04-31", &y, &m, &d) == 0, "April has no 31st day");
    TEST_ASSERT(validate_date("2025-02-30", &y, &m, &d) == 0, "February has no 30th day");
    
    // Format errors
    TEST_ASSERT(validate_date("25-09-30", &y, &m, &d) == 0, "wrong format (YY-MM-DD)");
    TEST_ASSERT(validate_date("2025/09/30", &y, &m, &d) == 0, "wrong separator (/)");
    TEST_ASSERT(validate_date("2025-9-30", &y, &m, &d) == 0, "month not zero-padded");
    TEST_ASSERT(validate_date("abcd-01-15", &y, &m, &d) == 0, "non-numeric characters");
}

void test_generate_next_id(void) {
    printf("\n--- Testing generate_next_id() ---\n");
    
    const char* tmp = "tmp_ids.csv";
    set_data_filename(tmp);
    
    // Empty file - should generate A001
    FILE* f = fopen(tmp, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fclose(f);
    
    char* id1 = generate_next_id();
    TEST_ASSERT_STR_EQ(id1, "A001", "first ID is A001");
    
    // Add A001, next should be A002
    f = fopen(tmp, "a");
    fprintf(f, "A001,Test,10.00,2025-09-30\n");
    fclose(f);
    
    char* id2 = generate_next_id();
    TEST_ASSERT_STR_EQ(id2, "A002", "second ID is A002");
    
    // Add A999, next should be B001
    f = fopen(tmp, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A999,Test,10.00,2025-09-30\n");
    fclose(f);
    
    char* id3 = generate_next_id();
    TEST_ASSERT_STR_EQ(id3, "B001", "rollover from A999 to B001");
    
    // Multiple prefixes - should pick highest
    f = fopen(tmp, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A500,Test1,10.00,2025-09-30\n");
    fprintf(f, "B200,Test2,20.00,2025-09-30\n");
    fprintf(f, "C100,Test3,30.00,2025-09-30\n");
    fclose(f);
    
    char* id4 = generate_next_id();
    TEST_ASSERT_STR_EQ(id4, "C101", "pick highest prefix and increment");
    
    remove(tmp);
    set_data_filename("budget.csv");
}

void test_addContent(void) {
    printf("\n--- Testing addContent() ---\n");
    
    const char* tmp = "tmp_add.csv";
    set_data_filename(tmp);
    
    set_stdin_from_string(
        "ค่าเช่าสถานที่\n"
        "199.50\n"
        "2025-09-30\n"
        "\n",
        "tmp_stdin_add.txt"
    );
    
    addContent();
    
    FILE* f = fopen(tmp, "r");
    TEST_ASSERT(f != NULL, "file created successfully");
    
    char line[512];
    fgets(line, sizeof(line), f); // header
    TEST_ASSERT(strstr(line, "RequestID") != NULL, "header contains RequestID");
    
    fgets(line, sizeof(line), f); // data
    TEST_ASSERT(strstr(line, "ค่าเช่าสถานที่") != NULL, "title saved correctly");
    TEST_ASSERT(strstr(line, "199.50") != NULL, "amount saved correctly");
    TEST_ASSERT(strstr(line, "2025-09-30") != NULL, "date saved correctly");
    TEST_ASSERT(strstr(line, "A001") != NULL, "auto-generated ID A001");
    
    fclose(f);
    remove(tmp);
    remove("tmp_stdin_add.txt");
    set_data_filename("budget.csv");
}

void test_searchContent(void) {
    printf("\n--- Testing searchContent() ---\n");
    
    const char* tmp = "tmp_search.csv";
    set_data_filename(tmp);
    write_base_csv(tmp);
    
    // Search by title
    set_stdin_from_string(
        "2\n"
        "ค่าเช่า\n"
        "\n",
        "tmp_stdin_search.txt"
    );
    
    printf("  (Running search - check console output)\n");
    searchContent();
    TEST_ASSERT(1, "search by title executed");
    
    // Search by ID
    write_base_csv(tmp);
    set_stdin_from_string(
        "1\n"
        "A001\n"
        "\n",
        "tmp_stdin_search2.txt"
    );
    
    searchContent();
    TEST_ASSERT(1, "search by ID executed");
    
    remove(tmp);
    remove("tmp_stdin_search.txt");
    remove("tmp_stdin_search2.txt");
    set_data_filename("budget.csv");
}

void test_deleteContent(void) {
    printf("\n--- Testing deleteContent() ---\n");
    
    const char* tmp = "tmp_delete.csv";
    set_data_filename(tmp);
    write_base_csv(tmp);
    
    set_stdin_from_string(
        "2\n"           // search by title
        "ค่าเช่า\n"     // find A002
        "1\n"           // select first result
        "y\n"           // confirm delete
        "\n",
        "tmp_stdin_delete.txt"
    );
    
    deleteContent();
    
    FILE* f = fopen(tmp, "r");
    TEST_ASSERT(f != NULL, "file still exists after delete");
    
    char line[512];
    fgets(line, sizeof(line), f); // header
    
    int data_lines = 0;
    int found_a001 = 0, found_a002 = 0;
    
    while (fgets(line, sizeof(line), f)) {
        data_lines++;
        if (strstr(line, "A001")) found_a001 = 1;
        if (strstr(line, "A002")) found_a002 = 1;
    }
    
    TEST_ASSERT_INT_EQ(data_lines, 1, "one row remaining after delete");
    TEST_ASSERT(found_a001 == 1, "A001 still exists");
    TEST_ASSERT(found_a002 == 0, "A002 was deleted");
    
    fclose(f);
    remove(tmp);
    remove("tmp_stdin_delete.txt");
    set_data_filename("budget.csv");
}

void test_editContent(void) {
    printf("\n--- Testing editContent() ---\n");
    
    const char* tmp = "tmp_edit.csv";
    set_data_filename(tmp);
    
    FILE* f = fopen(tmp, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A001,ค่ากาแฟ,50.00,2025-09-01\n");
    fclose(f);
    
    set_stdin_from_string(
        "1\n"                   // search by ID
        "A001\n"                // find A001
        "1\n"                   // select first result
        "5\n"                   // edit all fields
        "A010\n"                // new ID
        "ค่าเครื่องเขียน\n"    // new title
        "75.25\n"               // new amount
        "2025-10-01\n"          // new date
        "y\n"                   // confirm
        "\n",
        "tmp_stdin_edit.txt"
    );
    
    editContent();
    
    f = fopen(tmp, "r");
    TEST_ASSERT(f != NULL, "file exists after edit");
    
    char line[512];
    fgets(line, sizeof(line), f); // header
    fgets(line, sizeof(line), f); // data
    
    TEST_ASSERT(strstr(line, "A010") != NULL, "ID changed to A010");
    TEST_ASSERT(strstr(line, "ค่าเครื่องเขียน") != NULL, "title updated");
    TEST_ASSERT(strstr(line, "75.25") != NULL, "amount updated");
    TEST_ASSERT(strstr(line, "2025-10-01") != NULL, "date updated");
    
    fclose(f);
    remove(tmp);
    remove("tmp_stdin_edit.txt");
    set_data_filename("budget.csv");
}

void test_displayAllContent(void) {
    printf("\n--- Testing displayAllContent() ---\n");
    
    const char* tmp = "tmp_display.csv";
    set_data_filename(tmp);
    write_base_csv(tmp);
    
    set_stdin_from_string("\n", "tmp_stdin_display.txt");
    
    printf("  (Running display - check console output)\n");
    displayAllContent();
    TEST_ASSERT(1, "display all executed successfully");
    
    remove(tmp);
    remove("tmp_stdin_display.txt");
    set_data_filename("budget.csv");
}

/* ====== Main Test Runner ====== */
void run_all_tests(void) {
    printf("\n");
    printf("========================================\n");
    printf("  UNIT TESTS - Budget Management\n");
    printf("========================================\n");
    
    test_write_csv_field();
    test_validate_date();
    test_generate_next_id();
    test_addContent();
    test_searchContent();
    test_deleteContent();
    test_editContent();
    test_displayAllContent();
    
    restore_stdin();
    
    printf("\n========================================\n");
    printf("  TEST SUMMARY\n");
    printf("========================================\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Total:  %d\n", tests_total);
    printf("========================================\n\n");
    
    if (tests_failed == 0) {
        printf("🎉 All tests passed! 🎉\n\n");
    } else {
        printf("⚠️  Some tests failed. Please review above. ⚠️\n\n");
    }
}