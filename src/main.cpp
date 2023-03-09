#include<iostream>
#include<stdlib.h>
#include <windows.h>
using namespace std;

void TemplateDemo(void);
void ClassTemplateDemo(void);
void NonTypeTemplateParameterDemo(void);
int main(void)
{
    SetConsoleOutputCP(65001);
    NonTypeTemplateParameterDemo();
    return 0;
}
