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
    int mod[5];
    struct hlink *nextmne;
}hashcell;
typedef struct symlink // symbol table 
{
    char sym[10];
    int locv;
    struct symlink *nextsym;
}symcell;

typedef struct mlink // Modification Linked List
{
    int loc;
    struct mlink *nextm;
}mcell;

// 함수 원형
void inputinstr();
void helpinstr();
void dirinstr();
void hiinstr();
void dumpinstr();
void editinstr();
void fillinstr();
void resetinstr();
int findvalue(char *svalue, int ty);
void inputopc();
void opcodeinstr();
void opcodelistinstr();
int transstring(int sui, char *tstr, int len);
void symbolinstr();
void cm();
int pone(char *fn, int *bv, int *length);
int findopcode(char *str, hashcell **insihashhead);
int finddirect(char *str);
int ptwo(char *fn, int *bv, int *length);
int findsymbol(char *str, symcell **insisymhead);
int findmnenumber(char *str);
int commacheck(char *str1, char *str2, char *str3, char *str);
int xcheck(char *str1, char *str2, char *str3);
