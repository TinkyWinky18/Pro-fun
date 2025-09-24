#include <stdio.h>

int ch;//choice
FILE *help;

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
int main(){

    menu();

    scanf("%d", &ch);
    switch (ch)
    {
    case 1:
        printf("-แสดงข้อมูลรายการในอดีต-\n");
        break;
    case 2:
        printf("-เพิ่มรายการ-\n");
        break;
    case 3:
        printf("-ค้นหารายการ-\n");
        break;
    case 4:
        printf("-แก้ไขรายการ-\n");
        break;
    case 5:
        printf("-ลบรายการ-\n");
        break;
    case 0:
        printf("-ออกจากระบบ-\n");
        break;
    default:
        printf("-กรอกข้อมูลผิดพลาด!-");//ตรงนี้ต้องมาเพิ่มให้ลูปกลับไปกรอกข้อมูลใหม่
        break;
    }


    return 0;
}