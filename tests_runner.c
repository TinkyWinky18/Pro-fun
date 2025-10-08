#include <stdio.h>
#include "tests_helpers.h"
#include "budget.h"

// Global test counters
int tests_passed = 0;
int tests_failed = 0;

// เพิ่มส่วนนี้: ไม่ exit หลังรัน test เสร็จ
void run_all_tests() {
    printf("\n");
    printf("========================================\n");
    printf("  Budget Management System - Unit Tests\n");
    printf("========================================\n");
    
    tests_passed = 0;
    tests_failed = 0;
    
    // Run all tests
    test_validate_date();
    test_write_csv_field();
    test_generate_next_id();
    test_csv_operations();
    test_data_validation();
    
    // Print summary
    printf("\n========================================\n");
    printf("  TEST SUMMARY\n");
    printf("========================================\n");
    printf(COLOR_GREEN "  Passed: %d" COLOR_RESET "\n", tests_passed);
    if (tests_failed > 0) {
        printf(COLOR_RED "  Failed: %d" COLOR_RESET "\n", tests_failed);
    } else {
        printf("  Failed: %d\n", tests_failed);
    }
    printf("  Total:  %d\n", tests_passed + tests_failed);
    printf("========================================\n");
    
    if (tests_failed == 0) {
        printf(COLOR_GREEN "\n✓ All tests passed!\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "\n✗ Some tests failed!\n" COLOR_RESET);
    }
    printf("\n");
    
    // ลบบรรทัดนี้ออก: return tests_failed > 0 ? 1 : 0;
    // เพื่อให้กลับไปเมนูหลักได้
}