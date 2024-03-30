#include "syscall.h"
#include "copyright.h"

int main() {
  char str[50];
	int length = 50;
	PrintString("------------CHUONG TRINH TEST (READ & PRINT)-------------\n\n");
	PrintString("Nhap chuoi: ");
	ReadString(str, length);
	PrintString("Chuoi vua nhap: ");
	PrintString(str);
	PrintString("\n\n");
  return 0;
}
