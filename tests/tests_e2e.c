#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../budget.h"

// Test statistics
static int e2e_passed = 0;
static int e2e_failed = 0;

// Colors
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

// Test macros
#define E2E_TEST_START(name) printf("\n" BLUE "=== E2E Test: %s ===" RESET "\n", name)
#define E2E_PASS(msg) do { printf(GREEN "  ✓ %s" RESET "\n", msg); e2e_passed++; } while(0)
#define E2E_FAIL(msg) do { printf(RED "  ✗ %s" RESET "\n", msg); e2e_failed++; } while(0)
#define E2E_ASSERT(cond, msg) if (cond) E2E_PASS(msg); else E2E_FAIL(msg)

// Helper: Create input file for simulation
void create_input_file(const char* filename, const char* content) {
    FILE* f = fopen(filename, "w");
    if (f) {
        fputs(content, f);
        fclose(f);
    }
}

// Helper: Read file content
int file_contains(const char* filename, const char* search_str) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    
    char line[1000];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, search_str)) {
            found = 1;
            break;
        }
    }
    fclose(f);
    return found;
}

// Helper: Count data lines in CSV
int count_csv_lines(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return -1;
    
    int count = 0;
    char line[1000];
    fgets(line, sizeof(line), f); // Skip header
    
    while (fgets(line, sizeof(line), f)) {
        count++;
    }
    fclose(f);
    return count;
}

// Helper: Simulate stdin
void redirect_stdin(const char* input_file) {
    freopen(input_file, "r", stdin);
}

void restore_stdin() {
#ifdef _WIN32
    freopen("CON", "r", stdin);
#else
    freopen("/dev/tty", "r", stdin);
#endif
}

// =============================================================================
// E2E Test 1: Complete Add Flow
// =============================================================================
void e2e_test_add_complete_flow() {
    E2E_TEST_START("Add Complete Flow");
    
    const char* test_file = "e2e_add_test.csv";
    set_data_filename(test_file);
    
    // Clean start
    remove(test_file);
    
    // Simulate user input for adding
    create_input_file("e2e_input_add.txt", 
        "ค่าอาหารกลางวัน\n"      // Title
        "150.50\n"                // Amount
        "2025-10-15\n"            // Date
        "\n"                      // Enter to continue
    );
    
    redirect_stdin("e2e_input_add.txt");
    addContent();
    restore_stdin();
    
    printf("\n"); // เพิ่ม newline หลังจากฟังก์ชันเสร็จ
    
    // Verify file exists
    E2E_ASSERT(access(test_file, F_OK) == 0, "CSV file created");
    
    // Verify content
    E2E_ASSERT(file_contains(test_file, "A001"), "Auto-generated ID A001");
    E2E_ASSERT(file_contains(test_file, "ค่าอาหารกลางวัน"), "Title saved");
    E2E_ASSERT(file_contains(test_file, "150.50"), "Amount saved");
    E2E_ASSERT(file_contains(test_file, "2025-10-15"), "Date saved");
    E2E_ASSERT(count_csv_lines(test_file) == 1, "Exactly 1 record added");
    
    // Cleanup
    remove(test_file);
    remove("e2e_input_add.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 2: Add Multiple Records
// =============================================================================
void e2e_test_add_multiple_records() {
    E2E_TEST_START("Add Multiple Records");
    
    const char* test_file = "e2e_multiple.csv";
    set_data_filename(test_file);
    remove(test_file);
    
    // Add first record
    create_input_file("e2e_input1.txt", 
        "ค่าเช่าสถานที่\n500.00\n2025-10-01\n\n");
    redirect_stdin("e2e_input1.txt");
    addContent();
    restore_stdin();
    printf("\n");
    
    // Add second record
    create_input_file("e2e_input2.txt", 
        "ค่าอาหาร\n200.00\n2025-10-02\n\n");
    redirect_stdin("e2e_input2.txt");
    addContent();
    restore_stdin();
    printf("\n");
    
    // Add third record
    create_input_file("e2e_input3.txt", 
        "ค่าเครื่องเขียน\n75.50\n2025-10-03\n\n");
    redirect_stdin("e2e_input3.txt");
    addContent();
    restore_stdin();
    printf("\n");
    
    // Verify
    E2E_ASSERT(count_csv_lines(test_file) == 3, "3 records added");
    E2E_ASSERT(file_contains(test_file, "A001"), "First ID: A001");
    E2E_ASSERT(file_contains(test_file, "A002"), "Second ID: A002");
    E2E_ASSERT(file_contains(test_file, "A003"), "Third ID: A003");
    
    // Cleanup
    remove(test_file);
    remove("e2e_input1.txt");
    remove("e2e_input2.txt");
    remove("e2e_input3.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 3: Search Flow
// =============================================================================
void e2e_test_search_flow() {
    E2E_TEST_START("Search Flow");
    
    const char* test_file = "e2e_search.csv";
    set_data_filename(test_file);
    
    // Create test data
    FILE* f = fopen(test_file, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A001,ค่าอาหาร,100.00,2025-10-01\n");
    fprintf(f, "A002,ค่าเช่าสถานที่,500.00,2025-10-02\n");
    fprintf(f, "A003,ค่าอาหารเย็น,150.00,2025-10-03\n");
    fclose(f);
    
    // Test: Search by title "อาหาร"
    create_input_file("e2e_search_input.txt",
        "2\n"          // Search by title
        "อาหาร\n"      // Search term
        "\n"           // Enter to continue
    );
    
    redirect_stdin("e2e_search_input.txt");
    printf("  (Running search...)\n");
    searchContent();
    restore_stdin();
    printf("\n");
    
    E2E_PASS("Search by title executed");
    
    // Test: Search by ID
    create_input_file("e2e_search_id.txt",
        "1\n"          // Search by ID
        "A002\n"       // ID
        "\n"
    );
    
    redirect_stdin("e2e_search_id.txt");
    printf("  (Running search by ID...)\n");
    searchContent();
    restore_stdin();
    printf("\n");
    
    E2E_PASS("Search by ID executed");
    
    // Cleanup
    remove(test_file);
    remove("e2e_search_input.txt");
    remove("e2e_search_id.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 4: Edit Flow
// =============================================================================
void e2e_test_edit_flow() {
    E2E_TEST_START("Edit Flow");
    
    const char* test_file = "e2e_edit.csv";
    set_data_filename(test_file);
    
    // Create initial record
    FILE* f = fopen(test_file, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A001,ค่าอาหาร,100.00,2025-10-01\n");
    fclose(f);
    
    // Edit: Change amount only
    create_input_file("e2e_edit_input.txt",
        "1\n"          // Search by ID
        "A001\n"       // Find A001
        "1\n"          // Select first result
        "3\n"          // Edit amount only
        "250.75\n"     // New amount
        "y\n"          // Confirm
        "\n"
    );
    
    redirect_stdin("e2e_edit_input.txt");
    editContent();
    restore_stdin();
    printf("\n");
    
    // Verify changes
    E2E_ASSERT(file_contains(test_file, "250.75"), "Amount updated to 250.75");
    E2E_ASSERT(file_contains(test_file, "ค่าอาหาร"), "Title unchanged");
    E2E_ASSERT(file_contains(test_file, "A001"), "ID unchanged");
    E2E_ASSERT(count_csv_lines(test_file) == 1, "Still 1 record");
    
    // Cleanup
    remove(test_file);
    remove("e2e_edit_input.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 5: Delete Flow
// =============================================================================
void e2e_test_delete_flow() {
    E2E_TEST_START("Delete Flow");
    
    const char* test_file = "e2e_delete.csv";
    set_data_filename(test_file);
    
    // Create test records
    FILE* f = fopen(test_file, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A001,ค่าอาหาร,100.00,2025-10-01\n");
    fprintf(f, "A002,ค่าเช่า,500.00,2025-10-02\n");
    fprintf(f, "A003,ค่าน้ำ,50.00,2025-10-03\n");
    fclose(f);
    
    int before_count = count_csv_lines(test_file);
    
    // Delete A002
    create_input_file("e2e_delete_input.txt",
        "1\n"          // Search by ID
        "A002\n"       // Find A002
        "1\n"          // Select first result
        "y\n"          // Confirm delete
        "\n"
    );
    
    redirect_stdin("e2e_delete_input.txt");
    deleteContent();
    restore_stdin();
    printf("\n");
    
    // Verify
    int after_count = count_csv_lines(test_file);
    E2E_ASSERT(after_count == before_count - 1, "Record count decreased by 1");
    E2E_ASSERT(!file_contains(test_file, "A002"), "A002 deleted");
    E2E_ASSERT(file_contains(test_file, "A001"), "A001 still exists");
    E2E_ASSERT(file_contains(test_file, "A003"), "A003 still exists");
    
    // Cleanup
    remove(test_file);
    remove("e2e_delete_input.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 6: Full CRUD Cycle
// =============================================================================
void e2e_test_full_crud_cycle() {
    E2E_TEST_START("Full CRUD Cycle");
    
    const char* test_file = "e2e_crud.csv";
    set_data_filename(test_file);
    remove(test_file);
    
    // CREATE: Add record
    create_input_file("e2e_crud_add.txt", 
        "ค่าทดสอบ\n100.00\n2025-10-01\n\n");
    redirect_stdin("e2e_crud_add.txt");
    addContent();
    restore_stdin();
    printf("\n");
    E2E_ASSERT(file_contains(test_file, "ค่าทดสอบ"), "CREATE: Record added");
    
    // READ: Display all
    create_input_file("e2e_crud_read.txt", "\n");
    redirect_stdin("e2e_crud_read.txt");
    printf("  (Displaying all...)\n");
    displayAllContent();
    restore_stdin();
    printf("\n");
    E2E_PASS("READ: Display executed");
    
    // UPDATE: Edit amount
    create_input_file("e2e_crud_update.txt",
        "1\nA001\n1\n3\n200.00\ny\n\n");
    redirect_stdin("e2e_crud_update.txt");
    editContent();
    restore_stdin();
    printf("\n");
    E2E_ASSERT(file_contains(test_file, "200.00"), "UPDATE: Amount changed");
    
    // DELETE: Remove record
    create_input_file("e2e_crud_delete.txt",
        "1\nA001\n1\ny\n\n");
    redirect_stdin("e2e_crud_delete.txt");
    deleteContent();
    restore_stdin();
    printf("\n");
    E2E_ASSERT(count_csv_lines(test_file) == 0, "DELETE: Record removed");
    
    // Cleanup
    remove(test_file);
    remove("e2e_crud_add.txt");
    remove("e2e_crud_read.txt");
    remove("e2e_crud_update.txt");
    remove("e2e_crud_delete.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 7: Data Validation Flow
// =============================================================================
void e2e_test_validation_flow() {
    E2E_TEST_START("Data Validation Flow");
    
    const char* test_file = "e2e_validation.csv";
    set_data_filename(test_file);
    remove(test_file);
    
    // Test: Invalid amount (should retry)
    create_input_file("e2e_val_amount.txt",
        "ทดสอบ\n"
        "abc\n"        // Invalid amount
        "100.00\n"     // Valid amount
        "2025-10-01\n"
        "\n"
    );
    
    redirect_stdin("e2e_val_amount.txt");
    printf("  (Testing invalid amount...)\n");
    addContent();
    restore_stdin();
    printf("\n");
    E2E_ASSERT(file_contains(test_file, "100.00"), "Valid amount accepted after retry");
    
    // Test: Invalid date (should retry)
    remove(test_file);
    create_input_file("e2e_val_date.txt",
        "ทดสอบ2\n"
        "100.00\n"
        "2025-13-01\n"  // Invalid month
        "2025-10-01\n"  // Valid date
        "\n"
    );
    
    redirect_stdin("e2e_val_date.txt");
    printf("  (Testing invalid date...)\n");
    addContent();
    restore_stdin();
    printf("\n");
    E2E_ASSERT(file_contains(test_file, "2025-10-01"), "Valid date accepted after retry");
    
    // Cleanup
    remove(test_file);
    remove("e2e_val_amount.txt");
    remove("e2e_val_date.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 8: ID Sequencing
// =============================================================================
void e2e_test_id_sequencing() {
    E2E_TEST_START("ID Sequencing");
    
    const char* test_file = "e2e_id_seq.csv";
    set_data_filename(test_file);
    remove(test_file);
    
    // Add records and verify ID sequence
    for (int i = 1; i <= 5; i++) {
        char input_file[50];
        sprintf(input_file, "e2e_id_input%d.txt", i);
        create_input_file(input_file, "Item\n10.00\n2025-10-01\n\n");
        
        redirect_stdin(input_file);
        addContent();
        restore_stdin();
        printf("\n");
        
        remove(input_file);
    }
    
    // Verify sequence
    E2E_ASSERT(file_contains(test_file, "A001"), "A001 exists");
    E2E_ASSERT(file_contains(test_file, "A002"), "A002 exists");
    E2E_ASSERT(file_contains(test_file, "A003"), "A003 exists");
    E2E_ASSERT(file_contains(test_file, "A004"), "A004 exists");
    E2E_ASSERT(file_contains(test_file, "A005"), "A005 exists");
    E2E_ASSERT(count_csv_lines(test_file) == 5, "5 records with sequential IDs");
    
    // Delete middle record
    create_input_file("e2e_id_delete.txt", "1\nA003\n1\ny\n\n");
    redirect_stdin("e2e_id_delete.txt");
    deleteContent();
    restore_stdin();
    printf("\n");
    
    // Add new record (should be A006, not A003)
    create_input_file("e2e_id_new.txt", "New\n10.00\n2025-10-01\n\n");
    redirect_stdin("e2e_id_new.txt");
    addContent();
    restore_stdin();
    printf("\n");
    
    E2E_ASSERT(file_contains(test_file, "A006"), "New ID is A006 (not reusing A003)");
    E2E_ASSERT(!file_contains(test_file, "A003"), "A003 not reused");
    
    // Cleanup
    remove(test_file);
    remove("e2e_id_delete.txt");
    remove("e2e_id_new.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 9: Special Characters in Data
// =============================================================================
void e2e_test_special_characters() {
    E2E_TEST_START("Special Characters in Data");
    
    const char* test_file = "e2e_special.csv";
    set_data_filename(test_file);
    remove(test_file);
    
    // Test comma in title
    create_input_file("e2e_special_comma.txt",
        "ค่า,อาหาร,และ,เครื่องดื่ม\n100.00\n2025-10-01\n\n");
    redirect_stdin("e2e_special_comma.txt");
    addContent();
    restore_stdin();
    printf("\n");
    
    E2E_ASSERT(file_contains(test_file, "ค่า,อาหาร,และ,เครื่องดื่ม"), 
               "Comma in title handled");
    
    // Verify it's properly quoted in CSV
    FILE* f = fopen(test_file, "r");
    char line[500];
    fgets(line, sizeof(line), f); // header
    fgets(line, sizeof(line), f); // data
    E2E_ASSERT(strstr(line, "\"ค่า,อาหาร,และ,เครื่องดื่ม\"") != NULL,
               "Comma field is quoted");
    fclose(f);
    
    // Cleanup
    remove(test_file);
    remove("e2e_special_comma.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// E2E Test 10: Cancel Operations
// =============================================================================
void e2e_test_cancel_operations() {
    E2E_TEST_START("Cancel Operations");
    
    const char* test_file = "e2e_cancel.csv";
    set_data_filename(test_file);
    
    // Create test data
    FILE* f = fopen(test_file, "w");
    fprintf(f, "RequestID,Title,Amount,Date\n");
    fprintf(f, "A001,ค่าทดสอบ,100.00,2025-10-01\n");
    fclose(f);
    
    int initial_count = count_csv_lines(test_file);
    
    // Test: Cancel delete
    create_input_file("e2e_cancel_delete.txt",
        "1\nA001\n1\nn\n\n");  // n = cancel
    redirect_stdin("e2e_cancel_delete.txt");
    deleteContent();
    restore_stdin();
    printf("\n");
    
    E2E_ASSERT(count_csv_lines(test_file) == initial_count, 
               "Delete cancelled - count unchanged");
    E2E_ASSERT(file_contains(test_file, "A001"), "Record still exists after cancel");
    
    // Test: Cancel edit
    create_input_file("e2e_cancel_edit.txt",
        "1\nA001\n1\n0\n");  // 0 = cancel
    redirect_stdin("e2e_cancel_edit.txt");
    editContent();
    restore_stdin();
    printf("\n");
    
    E2E_ASSERT(file_contains(test_file, "100.00"), "Edit cancelled - data unchanged");
    
    // Cleanup
    remove(test_file);
    remove("e2e_cancel_delete.txt");
    remove("e2e_cancel_edit.txt");
    set_data_filename("budget.csv");
}

// =============================================================================
// Main E2E Test Runner
// =============================================================================
void run_e2e_tests() {
    printf("\n");
    printf("========================================\n");
    printf("  END-TO-END TESTS\n");
    printf("========================================\n");
    
    e2e_passed = 0;
    e2e_failed = 0;
    
    e2e_test_add_complete_flow();
    e2e_test_add_multiple_records();
    e2e_test_search_flow();
    e2e_test_edit_flow();
    e2e_test_delete_flow();
    e2e_test_full_crud_cycle();
    e2e_test_validation_flow();
    e2e_test_id_sequencing();
    e2e_test_special_characters();
    e2e_test_cancel_operations();
    
    printf("\n========================================\n");
    printf("  E2E TEST SUMMARY\n");
    printf("========================================\n");
    printf("Passed: %d\n", e2e_passed);
    printf("Failed: %d\n", e2e_failed);
    printf("Total:  %d\n", e2e_passed + e2e_failed);
    printf("========================================\n\n");
    
    if (e2e_failed == 0) {
        printf("🎉 All E2E tests passed! 🎉\n");
    } else {
        printf("⚠️  Some E2E tests failed!\n");
    }
    printf("\n");
}