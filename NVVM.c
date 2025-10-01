#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ch; 
FILE *help;
const char *filename = "budget.csv";

void menu();
void addContent();
void write_csv_field(FILE *fp, const char *s);
void searchContent();
void deleteContent();
void editContent();

int main(){
    do {
        menu();

        if (scanf("%d", &ch) != 1) ch = -1;
        
        int c; while ((c = getchar()) != '\n' && c != EOF) {}

        switch (ch)
        {
            case 1:
                printf("-แสดงข้อมูลรายการในอดีต-\n");
                printf("(ยังไม่ได้ทำครับT-T)\n");
                break;
            case 2:
                printf("-เพิ่มรายการ-\n");
                addContent();
                break;
            case 3:
                printf("-ค้นหารายการ-\n");
                searchContent();
                break;
            case 4:
                printf("-แก้ไขรายการ-\n");
                editContent();;
                break;
            case 5:
                printf("-ลบรายการ-\n");
                deleteContent();
                break;
            case 0:
                printf("-ออกจากระบบ-\n");
                break;
            default:
                printf("-กรอกข้อมูลผิดพลาด!-\n"); 
                break;
        }
        printf("\n");
    } while (ch != 0);

    return 0;
}

void menu(){
    printf("ระบบจัดการข้อมูลการใช้จ่ายงบประมาณ\n");
    printf("[1]แสดงข้อมูลรายการในอดีต\n");
    printf("[2]เพิ่มรายการ\n");
    printf("[3]ค้นหารายการ\n");
    printf("[4]แก้ไขรายการ\n");
    printf("[5]ลบรายการ\n");
    printf("[0]ออกจากระบบ\n");
    printf("โปรดเลือกตัวเลือกที่ต้องการดำเนินการ\n");
}

void write_csv_field(FILE *fp, const char *s) {
    int need_quotes = 0;
    for (const char *p = s; *p; ++p) {
        if (*p == ',' || *p == '"'){ 
            need_quotes = 1; break; 
        }
    }
    if (!need_quotes) {
        fputs(s, fp);
    } else {
        fputc('"', fp);
        for (const char *p = s; *p; ++p) {
            if (*p == '"') fputc('"', fp);
            fputc(*p, fp);
        }
        fputc('"', fp);
    }
}

void addContent(){
    char date[20];
    char title[100];
    char id[20];
    char amount_text[50];
    double amount = -1.0;

    help = fopen(filename, "r");
    if (help == NULL) {
        help = fopen(filename, "w");
        if (help) {
            fprintf(help, "RequestID,Title,Amount,Date\n");
            fclose(help);
        } else {
            perror("สร้างไฟล์ไม่สำเร็จ");
            return;
        }
    } else {
        fclose(help);
    }

     while (1) {
        printf("กรอกรหัสรายการ (เช่น B001): ");
        if (fgets(id, sizeof(id), stdin) == NULL) return;
        id[strcspn(id, "\n")] = '\0';
        
        int valid = 0;
        
        for (int i = 0; id[i] != '\0'; i++) {
            if (id[i] != ' ' && id[i] != '\t') {
                valid = 1;
                break;
            }
        }

        if (valid) break;
        printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
    }

   while (1) {
        printf("กรอกชื่อรายการ (เช่น ค่าสถานที่): ");
        if (fgets(title, sizeof(title), stdin) == NULL) return;
        title[strcspn(title, "\n")] = '\0';
        
        int valid = 0;

        for (int i = 0; title[i] != '\0'; i++) {
            if (title[i] != ' ' && title[i] != '\t') {
                valid = 1;
                break;
            }
        }

        if (valid) break;
        printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
    }
    
    while (1) {
        printf("กรอกจำนวนเงิน (เช่น 199.50): ");
        if (fgets(amount_text, sizeof(amount_text), stdin) == NULL) return;
        amount_text[strcspn(amount_text, "\n")] = '\0';
        
        char *endp = NULL;
        amount = strtod(amount_text, &endp);
        
        if (endp != amount_text) {
            while (endp && *endp && (*endp == ' ' || *endp == '\t')) endp++;
        }
        
        if (endp && *endp == '\0' && amount >= 0.0) break;
        printf("รูปแบบจำนวนเงินไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n");
    }
    
    while (1) {
        printf("กรอกวันที่ (เช่น 2025-09-30): ");
        if (fgets(date, sizeof(date), stdin) == NULL) return;
        date[strcspn(date, "\n")] = '\0';
        
        int valid = 0;
        for (int i = 0; date[i] != '\0'; i++) {
            if (date[i] != ' ' && date[i] != '\t') {
                valid = 1;
                break;
            }
        }
        
        if (valid) break;
        printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
    }

    help = fopen(filename, "a");
    if (!help) {
        perror("เปิดไฟล์เขียนไม่ได้");
        return;
    }

    write_csv_field(help, id);    fputc(',', help);
    write_csv_field(help, title); fputc(',', help);
    fprintf(help, "%.2f", amount); fputc(',', help);
    write_csv_field(help, date); fputc('\n', help);

    

    fclose(help);
    printf("บันทึกข้อมูลเรียบร้อยลงไฟล์ %s\n", filename);

    printf("\nกด Enter เพื่อดำเนินการต่อ...");
    getchar();
}

void searchContent(){
    char search_term[100];
    char line[500];
    int search_type;
    int found = 0;
    
    help = fopen(filename, "r");
    if (help == NULL) {
        printf("ไม่พบไฟล์ข้อมูล กรุณาเพิ่มรายการก่อน\n");
        return;
    }
    
    while (1) {
        printf("เลือกประเภทการค้นหา:\n");
        printf("[1] ค้นหาจากรหัสรายการ\n");
        printf("[2] ค้นหาจากชื่อรายการ\n");
        printf("เลือก: ");
        
        if (scanf("%d", &search_type) != 1) {
            printf("กรุณากรอกตัวเลข\n");
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        
        if (search_type == 1 || search_type == 2) {
            break;
        }
        
        printf("ตัวเลือกไม่ถูกต้อง กรุณาเลือก 1 หรือ 2\n");
    }
    
    printf("กรอกคำค้นหา: ");
    if (fgets(search_term, sizeof(search_term), stdin) == NULL) {
        fclose(help);
        return;
    }
    search_term[strcspn(search_term, "\n")] = '\0';
    
    int valid = 0;
    for (int i = 0; search_term[i] != '\0'; i++) {
        if (search_term[i] != ' ' && search_term[i] != '\t') {
            valid = 1;
            break;
        }
    }
    
    if (!valid) {
        printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
        fclose(help);
        return;
    }
    
    if (fgets(line, sizeof(line), help) == NULL) {
        printf("ไฟล์ว่างเปล่า\n");
        fclose(help);
        return;
    }
    
    printf("\n--- ผลการค้นหา ---\n");
    
    while (fgets(line, sizeof(line), help) != NULL) {
        char temp_line[500];
        strcpy(temp_line, line);
        
        char *id = strtok(temp_line, ",");
        char *title = strtok(NULL, ",");
        char *amount = strtok(NULL, ",");
        char *date = strtok(NULL, ",\n");
        
        if (id == NULL || title == NULL || amount == NULL || date == NULL) {
            continue;
        }
        
        if (id[0] == '"') {
            id++;
            if (id[strlen(id)-1] == '"') id[strlen(id)-1] = '\0';
        }
        if (title[0] == '"') {
            title++;
            if (title[strlen(title)-1] == '"') title[strlen(title)-1] = '\0';
        }
        
        int match = 0;
        
        if (search_type == 1) {

            if (strstr(id, search_term) != NULL) {
                match = 1;
            }
        } else if (search_type == 2) {

            if (strstr(title, search_term) != NULL) {
                match = 1;
            }
        }
        
        if (match) {
            printf("รหัสรายการ: %s\n", id);
            printf("ชื่อรายการ: %s\n", title);
            printf("จำนวนเงิน: %s บาท\n", amount);
            printf("วันที่: %s\n", date);
            printf("------------------------\n");
            found++;
        }
    }
    
    if (found == 0) {
        printf("ไม่พบข้อมูลที่ค้นหา\n");
    } else {
        printf("พบทั้งหมด %d รายการ\n", found);
    }

    printf("\nกด Enter เพื่อดำเนินการต่อ...");
    getchar();
    
    fclose(help);
}

void deleteContent(){
    char search_term[100];
    char line[500];
    int search_type;
    int found = 0;
    
    help = fopen(filename, "r");
    if (help == NULL) {
        printf("ไม่พบไฟล์ข้อมูล กรุณาเพิ่มรายการก่อน\n");
        return;
    }
    
    while (1) {
        printf("เลือกประเภทการค้นหา:\n");
        printf("[1] ค้นหาจากรหัสรายการ\n");
        printf("[2] ค้นหาจากชื่อรายการ\n");
        printf("เลือก: ");
        
        if (scanf("%d", &search_type) != 1) {
            printf("กรุณากรอกตัวเลข\n");
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        
        if (search_type == 1 || search_type == 2) {
            break;
        }
        
        printf("ตัวเลือกไม่ถูกต้อง กรุณาเลือก 1 หรือ 2\n");
    }
    
    printf("กรอกคำค้นหา: ");
    if (fgets(search_term, sizeof(search_term), stdin) == NULL) {
        fclose(help);
        return;
    }
    search_term[strcspn(search_term, "\n")] = '\0';
    
    int valid = 0;
    for (int i = 0; search_term[i] != '\0'; i++) {
        if (search_term[i] != ' ' && search_term[i] != '\t') {
            valid = 1;
            break;
        }
    }
    
    if (!valid) {
        printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
        fclose(help);
        return;
    }
    
    if (fgets(line, sizeof(line), help) == NULL) {
        printf("ไฟล์ว่างเปล่า\n");
        fclose(help);
        return;
    }
    
    printf("\n--- ผลการค้นหา ---\n");
    
    char all_lines[100][500];
    int match_indices[100];
    char display_info[100][500];
    int total_lines = 0;
    int match_count = 0;
    
    while (fgets(line, sizeof(line), help) != NULL) {
        strcpy(all_lines[total_lines], line);
        
        char temp_line[500];
        strcpy(temp_line, line);
        
        char *id = strtok(temp_line, ",");
        char *title = strtok(NULL, ",");
        char *amount = strtok(NULL, ",");
        char *date = strtok(NULL, ",\n");
        
        if (id == NULL || title == NULL || amount == NULL || date == NULL) {
            total_lines++;
            continue;
        }
        
        if (id[0] == '"') {
            id++;
            if (id[strlen(id)-1] == '"') id[strlen(id)-1] = '\0';
        }
        if (title[0] == '"') {
            title++;
            if (title[strlen(title)-1] == '"') title[strlen(title)-1] = '\0';
        }
        
        int match = 0;
        
        if (search_type == 1) {
            if (strstr(id, search_term) != NULL) {
                match = 1;
            }
        } else if (search_type == 2) {
            if (strstr(title, search_term) != NULL) {
                match = 1;
            }
        }
        
        if (match) {
            printf("[%d] รหัสรายการ: %s\n", match_count + 1, id);
            printf("    ชื่อรายการ: %s\n", title);
            printf("    จำนวนเงิน: %s บาท\n", amount);
            printf("    วันที่: %s\n", date);
            printf("------------------------\n");
            
            snprintf(display_info[match_count], sizeof(display_info[match_count]), 
                     "รหัส: %s, ชื่อ: %s, จำนวน: %s, วันที่: %s", 
                     id, title, amount, date);
            match_indices[match_count] = total_lines;
            match_count++;
        }
        
        total_lines++;
    }
    
    fclose(help);
    
    if (match_count == 0) {
        printf("ไม่พบข้อมูลที่ค้นหา\n");
        printf("\nกด Enter เพื่อดำเนินการต่อ...");
        getchar();
        return;
    }
    
    printf("พบทั้งหมด %d รายการ\n\n", match_count);
    
    int select;
    while (1) {
        printf("เลือกหมายเลขรายการที่ต้องการลบ (0 = ยกเลิก): ");
        
        if (scanf("%d", &select) != 1) {
            printf("กรุณากรอกตัวเลข\n");
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        int c2; while ((c2 = getchar()) != '\n' && c2 != EOF) {}
        
        if (select == 0) {
            printf("ยกเลิกการลบ\n");
            return;
        }
        
        if (select < 1 || select > match_count) {
            printf("หมายเลขไม่ถูกต้อง กรุณาเลือก 1-%d\n", match_count);
            continue;
        }
        
        break;
    }
    
    char confirm;
    printf("\nยืนยันการลบรายการนี้? (y/n): ");
    printf("%s\n", display_info[select - 1]);
    scanf(" %c", &confirm);
    int c3; while ((c3 = getchar()) != '\n' && c3 != EOF) {}
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("ยกเลิกการลบ\n");
        printf("\nกด Enter เพื่อดำเนินการต่อ...");
        getchar();
        return;
    }
    
    help = fopen(filename, "w");
    if (!help) {
        perror("เปิดไฟล์เขียนไม่ได้");
        return;
    }
    
    fprintf(help, "RequestID,Title,Amount,Date\n");
    
    int delete_index = match_indices[select - 1];
    for (int i = 0; i < total_lines; i++) {
        if (i != delete_index) {
            fputs(all_lines[i], help);
        }
    }
    
    fclose(help);
    
    printf("ลบรายการเรียบร้อยแล้ว\n");
    printf("\nกด Enter เพื่อดำเนินการต่อ...");
    getchar();
}

void editContent(){
    char search_term[100];
    char line[500];
    int search_type;
    
    help = fopen(filename, "r");
    if (help == NULL) {
        printf("ไม่พบไฟล์ข้อมูล กรุณาเพิ่มรายการก่อน\n");
        return;
    }
    
    while (1) {
        printf("เลือกประเภทการค้นหา:\n");
        printf("[1] ค้นหาจากรหัสรายการ\n");
        printf("[2] ค้นหาจากชื่อรายการ\n");
        printf("เลือก: ");
        
        if (scanf("%d", &search_type) != 1) {
            printf("กรุณากรอกตัวเลข\n");
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        
        if (search_type == 1 || search_type == 2) {
            break;
        }
        
        printf("ตัวเลือกไม่ถูกต้อง กรุณาเลือก 1 หรือ 2\n");
    }
    
    printf("กรอกคำค้นหา: ");
    if (fgets(search_term, sizeof(search_term), stdin) == NULL) {
        fclose(help);
        return;
    }
    search_term[strcspn(search_term, "\n")] = '\0';
    
    int valid = 0;
    for (int i = 0; search_term[i] != '\0'; i++) {
        if (search_term[i] != ' ' && search_term[i] != '\t') {
            valid = 1;
            break;
        }
    }
    
    if (!valid) {
        printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
        fclose(help);
        return;
    }
    
    if (fgets(line, sizeof(line), help) == NULL) {
        printf("ไฟล์ว่างเปล่า\n");
        fclose(help);
        return;
    }
    
    printf("\n--- ผลการค้นหา ---\n");
    
    char all_lines[100][500];
    int match_indices[100];
    char match_ids[100][20];
    char match_titles[100][100];
    char match_amounts[100][50];
    char match_dates[100][20];
    int total_lines = 0;
    int match_count = 0;
    
    while (fgets(line, sizeof(line), help) != NULL) {
        strcpy(all_lines[total_lines], line);
        
        char temp_line[500];
        strcpy(temp_line, line);
        
        char *id = strtok(temp_line, ",");
        char *title = strtok(NULL, ",");
        char *amount = strtok(NULL, ",");
        char *date = strtok(NULL, ",\n");
        
        if (id == NULL || title == NULL || amount == NULL || date == NULL) {
            total_lines++;
            continue;
        }
        
        if (id[0] == '"') {
            id++;
            if (id[strlen(id)-1] == '"') id[strlen(id)-1] = '\0';
        }
        if (title[0] == '"') {
            title++;
            if (title[strlen(title)-1] == '"') title[strlen(title)-1] = '\0';
        }
        
        int match = 0;
        
        if (search_type == 1) {
            if (strstr(id, search_term) != NULL) {
                match = 1;
            }
        } else if (search_type == 2) {
            if (strstr(title, search_term) != NULL) {
                match = 1;
            }
        }
        
        if (match) {
            printf("[%d] รหัสรายการ: %s\n", match_count + 1, id);
            printf("    ชื่อรายการ: %s\n", title);
            printf("    จำนวนเงิน: %s บาท\n", amount);
            printf("    วันที่: %s\n", date);
            printf("------------------------\n");
            
            strcpy(match_ids[match_count], id);
            strcpy(match_titles[match_count], title);
            strcpy(match_amounts[match_count], amount);
            strcpy(match_dates[match_count], date);
            match_indices[match_count] = total_lines;
            match_count++;
        }
        
        total_lines++;
    }
    
    fclose(help);
    
    if (match_count == 0) {
        printf("ไม่พบข้อมูลที่ค้นหา\n");
        printf("\nกด Enter เพื่อดำเนินการต่อ...");
        getchar();
        return;
    }
    
    printf("พบทั้งหมด %d รายการ\n\n", match_count);
    
    int select;
    while (1) {
        printf("เลือกหมายเลขรายการที่ต้องการแก้ไข (0 = ยกเลิก): ");
        
        if (scanf("%d", &select) != 1) {
            printf("กรุณากรอกตัวเลข\n");
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        int c2; while ((c2 = getchar()) != '\n' && c2 != EOF) {}
        
        if (select == 0) {
            printf("ยกเลิกการแก้ไข\n");
            return;
        }
        
        if (select < 1 || select > match_count) {
            printf("หมายเลขไม่ถูกต้อง กรุณาเลือก 1-%d\n", match_count);
            continue;
        }
        
        break;
    }
    
    int selected_index = select - 1;
    
    printf("\n--- รายการที่เลือก ---\n");
    printf("รหัสรายการ: %s\n", match_ids[selected_index]);
    printf("ชื่อรายการ: %s\n", match_titles[selected_index]);
    printf("จำนวนเงิน: %s บาท\n", match_amounts[selected_index]);
    printf("วันที่: %s\n\n", match_dates[selected_index]);
    
    int edit_choice;
    while (1) {
        printf("เลือกข้อมูลที่ต้องการแก้ไข:\n");
        printf("[1] รหัสรายการ\n");
        printf("[2] ชื่อรายการ\n");
        printf("[3] จำนวนเงิน\n");
        printf("[4] วันที่\n");
        printf("[5] แก้ไขทั้งหมด\n");
        printf("[0] ยกเลิก\n");
        printf("เลือก: ");
        
        if (scanf("%d", &edit_choice) != 1) {
            printf("กรุณากรอกตัวเลข\n");
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }
        int c3; while ((c3 = getchar()) != '\n' && c3 != EOF) {}
        
        if (edit_choice >= 0 && edit_choice <= 5) {
            break;
        }
        
        printf("ตัวเลือกไม่ถูกต้อง กรุณาเลือก 0-5\n");
    }
    
    if (edit_choice == 0) {
        printf("ยกเลิกการแก้ไข\n");
        return;
    }
    
    char new_id[20], new_title[100], new_date[20];
    double new_amount;
    char amount_text[50];
    
    strcpy(new_id, match_ids[selected_index]);
    strcpy(new_title, match_titles[selected_index]);
    strcpy(new_date, match_dates[selected_index]);
    new_amount = atof(match_amounts[selected_index]);
    
    if (edit_choice == 1 || edit_choice == 5) {
        while (1) {
            printf("กรอกรหัสรายการใหม่ (เดิม: %s): ", match_ids[selected_index]);
            if (fgets(new_id, sizeof(new_id), stdin) == NULL) return;
            new_id[strcspn(new_id, "\n")] = '\0';
            
            int valid = 0;
            for (int i = 0; new_id[i] != '\0'; i++) {
                if (new_id[i] != ' ' && new_id[i] != '\t') {
                    valid = 1;
                    break;
                }
            }
            
            if (valid) break;
            printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
        }
    }
    
    if (edit_choice == 2 || edit_choice == 5) {
        while (1) {
            printf("กรอกชื่อรายการใหม่ (เดิม: %s): ", match_titles[selected_index]);
            if (fgets(new_title, sizeof(new_title), stdin) == NULL) return;
            new_title[strcspn(new_title, "\n")] = '\0';
            
            int valid = 0;
            for (int i = 0; new_title[i] != '\0'; i++) {
                if (new_title[i] != ' ' && new_title[i] != '\t') {
                    valid = 1;
                    break;
                }
            }
            
            if (valid) break;
            printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
        }
    }
    
    if (edit_choice == 3 || edit_choice == 5) {
        while (1) {
            printf("กรอกจำนวนเงินใหม่ (เดิม: %s): ", match_amounts[selected_index]);
            if (fgets(amount_text, sizeof(amount_text), stdin) == NULL) return;
            amount_text[strcspn(amount_text, "\n")] = '\0';
        
            char *endp = NULL;
            new_amount = strtod(amount_text, &endp);
        
            if (endp != amount_text) {
            while (endp && *endp && (*endp == ' ' || *endp == '\t')) endp++;
            }
        
            if (endp && *endp == '\0' && new_amount >= 0.0) break;
            printf("รูปแบบจำนวนเงินไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n");
        }
    }
    
    if (edit_choice == 4 || edit_choice == 5) {
        while (1) {
            printf("กรอกวันที่ใหม่ (เดิม: %s): ", match_dates[selected_index]);
            if (fgets(new_date, sizeof(new_date), stdin) == NULL) return;
            new_date[strcspn(new_date, "\n")] = '\0';
            
            int valid = 0;
            for (int i = 0; new_date[i] != '\0'; i++) {
                if (new_date[i] != ' ' && new_date[i] != '\t') {
                    valid = 1;
                    break;
                }
            }
            
            if (valid) break;
            printf("Error: กรุณากรอกข้อมูลให้ถูกต้อง\n");
        }
    }
    
    char confirm;
    printf("\nยืนยันการแก้ไข? (y/n): ");
    scanf(" %c", &confirm);
    int c4; while ((c4 = getchar()) != '\n' && c4 != EOF) {}
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("ยกเลิกการแก้ไข\n");
        printf("\nกด Enter เพื่อดำเนินการต่อ...");
        getchar();
        return;
    }
    
    help = fopen(filename, "w");
    if (!help) {
        perror("เปิดไฟล์เขียนไม่ได้");
        return;
    }
    
    fprintf(help, "RequestID,Title,Amount,Date\n");
    
    int edit_index = match_indices[selected_index];
    for (int i = 0; i < total_lines; i++) {
        if (i != edit_index) {
            fputs(all_lines[i], help);
        } else {
            write_csv_field(help, new_id);
            fputc(',', help);
            write_csv_field(help, new_title);
            fputc(',', help);
            fprintf(help, "%.2f", new_amount);
            fputc(',', help);
            write_csv_field(help, new_date);
            fputc('\n', help);
        }
    }
    
    fclose(help);
    
    printf("แก้ไขรายการเรียบร้อยแล้ว\n");
    printf("\nกด Enter เพื่อดำเนินการต่อ...");
    getchar();
}

