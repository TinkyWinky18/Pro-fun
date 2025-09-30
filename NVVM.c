#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int ch; 
FILE *help;

void menu();
void addContent();
void escape_and_write(FILE *fp, const char *s);

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
                printf("(ยังไม่ได้ทำครับT-T)\n");
                break;
            case 4:
                printf("-แก้ไขรายการ-\n");
                printf("(ยังไม่ได้ทำครับT-T)\n");
                break;
            case 5:
                printf("-ลบรายการ-\n");
                printf("(ยังไม่ได้ทำครับT-T)\n");
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
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') { need_quotes = 1; break; }
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
    char date[16];
    char title[64];
    char id[128];
    char amount_text[64];
    double amount = -1.0;

    const char *filename = "budget.csv";

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
    printf("กรอกรหัสรายการ (เช่น B001): ");
    if (fgets(id, sizeof(id), stdin) == NULL) return;
    id[strcspn(id, "\n")] = '\0';

    
    printf("กรอกชื่อรายการ (เช่น ค่าสถานที่): ");
    if (fgets(title, sizeof(title), stdin) == NULL) return;
    title[strcspn(title, "\n")] = '\0';
    
    
    while (1) {
        printf("กรอกจำนวนเงิน (เช่น 199.50): ");
        if (fgets(amount_text, sizeof(amount_text), stdin) == NULL) return;
        amount_text[strcspn(amount_text, "\n")] = '\0';
        
        char *endp = NULL;
        amount = strtod(amount_text, &endp);
        
        if (endp != amount_text) {
            while (endp && *endp && isspace((unsigned char)*endp)) endp++;
        }
        
        if (endp && *endp == '\0' && amount >= 0.0) break;
        printf("รูปแบบจำนวนเงินไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n");
    }
    
    printf("กรอกวันที่ (เช่น 2025-09-30): ");
    if (fgets(date, sizeof(date), stdin) == NULL) return;
    date[strcspn(date, "\n")] = '\0';

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
}
