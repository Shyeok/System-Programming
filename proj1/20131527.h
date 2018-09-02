#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<dirent.h>
// 선언한 구조체
typedef struct link // 실행한 명령어 저장 node 
{
    char sainstr[100]; // 명령어 저장
    struct link *nextinstr;
}sainstrs;
typedef struct hlink // hash table 공간
{
    char smne[100];
    int valuemne; 
    char mod[100];
    struct hlink *nextmne;
}hashcell;
// 함수 원형
void inputinstr();
void helpinstr();
void dirinstr();
void hiinstr();
void dumpinstr();
void editinstr();
void fillinstr();
void resetinstr();
int findvalue(char *svalue);
void inputopc();
void opcodeinstr();
void opcodelistinstr();
void printhex(int sui);
void cm();
