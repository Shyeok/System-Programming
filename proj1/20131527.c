#include"20131527.h"

// 전역 변수
int instr; // 명령어
int starti, endi, addressi, valuei; // 입력 받은 값
int startp, endp; // 이전 dump에서 출력한 번지
unsigned char meno[65536][16]; // 메모리
sainstrs *head; // history Linked_list head 주소 값
hashcell *hashhead[20]; // hashtable 각 줄의 head 주소 값

// 함수 : hash_function()
// 목적 : 입력받은 값의 hashtable의 index값을 return
int hash_function(char *s)
{
    int i, sum;
    for(i = 0, sum = 0; i <strlen(s); i++)
    {
        sum += s[i];
    }
    return sum % 20;
}
// 프로그램 main
int main()
{
    head = NULL; // 초기화
    startp = 0;
    endp = -1;
    inputopc();
    for(;;)
    {
        instr = 0;
        inputinstr();
        if(instr == 1)helpinstr();
        else if(instr == 2)dirinstr();
        else if(instr == 3)break;
        else if(instr == 4)hiinstr();
        else if(instr == 5)dumpinstr();
        else if(instr == 6)editinstr();
        else if(instr == 7)fillinstr();
        else if(instr == 8)resetinstr(); 
        else if(instr == 9)opcodeinstr();
        else if(instr == 10)opcodelistinstr();
    }
    cm();
    return 0;
}
// 함수 : inputinstr()
// 목적 : 명령어 입력 및 명령어 오류 판단
void inputinstr()
{
    char sinstr[500], insiinstr[100];
    char *stinstr;
    int i, len, stlen, hashindex;
    int eflag = 0;
    sainstrs *newhis, *insihead;
    hashcell *insihashhead;
    printf("sicsim> ");
    fgets(sinstr,sizeof(sinstr),stdin);
    len = strlen(sinstr);
    len = len - 1;
    sinstr[len] = 0;
    for(i = 0; i <= len; i++)insiinstr[i] = sinstr[i]; // 입력 받은 값을 따로 저장 
    stlen = 0;
    for(i = 0; i <= len; i++)// 명령어를 원하는 형식으로 바꿈
    {
        if(insiinstr[i] == ',')
        {
            sinstr[stlen] = ' ';
            stlen = stlen + 1;
        }
        sinstr[stlen] = insiinstr[i];
        stlen = stlen + 1;
        if(insiinstr[i] == ',')
        {
            sinstr[stlen] = ' ';
            stlen = stlen + 1;
        }
    }
    stinstr = strtok(sinstr," \t");
    if(stinstr != NULL)// 명령어 구분
    {
        if(strcmp(stinstr,"h") == 0 || strcmp(stinstr,"help") == 0)
        {   
            stinstr = strtok(NULL," \t");
            if(stinstr == NULL)instr = 1;
        }
        else if(strcmp(stinstr,"d") == 0 || strcmp(stinstr,"dir") == 0)
        {
            stinstr = strtok(NULL," \t");
            if(stinstr == NULL)instr = 2;
        }
        else if(strcmp(stinstr,"q") == 0 || strcmp(stinstr,"quit") == 0)
        {      
            stinstr = strtok(NULL," \t");
            if(stinstr == NULL)instr = 3;
        }
        else if(strcmp(stinstr,"hi") == 0 || strcmp(stinstr,"history") == 0)
        {
            stinstr = strtok(NULL," \t");
            if(stinstr == NULL)instr = 4;
        }
        else if(strcmp(stinstr,"du") == 0 || strcmp(stinstr,"dump") == 0)
        {
            starti = -1; // start 값과 end 값 입력 판단 위해 -1 저장
            endi = -1;
            stinstr = strtok(NULL," \t");
            if(stinstr == NULL)instr = 5;
            else
            {
                starti = findvalue(stinstr);
                stinstr = strtok(NULL," \t");
                if(stinstr == NULL)
                {
                    if(starti >= 0 && starti < 65536 * 16)instr = 5; // 입력 값의 범위 검사
                    else
                    {
                        eflag = 1; // 오류 표시 
                    }
                }
                else if(stinstr != NULL && strcmp(stinstr,",") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        endi = findvalue(stinstr);
                        stinstr = strtok(NULL," \t");
                        if(stinstr == NULL)
                        {
                            if(starti >= 0 && starti < 65536 * 16 && endi >= 0 && endi < 65536 * 16)//입력 값의 범위 검사
                            {
                                if(starti <= endi)instr = 5;// start 값이 end 값보다 작거나 같은지 검사
                                else
                                {
                                    eflag = 3;// 오류 표시
                                }
                            }
                            else
                            {
                                eflag = 1;// 오류 표시
                            }
                        }
                    }
                }
            }
        }
        else if(strcmp(stinstr,"e") == 0 || strcmp(stinstr,"edit") == 0)
        {
            stinstr = strtok(NULL," \t");
            if(stinstr != NULL)
            {
                addressi = findvalue(stinstr);
                stinstr = strtok(NULL," \t");
                if(stinstr != NULL && strcmp(stinstr,",") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        valuei = findvalue(stinstr);
                        stinstr = strtok(NULL," \t");
                        if(stinstr == NULL)
                        {
                            if(addressi >= 0 && addressi < 16 * 65536 && valuei >= 0 && valuei < 256)instr = 6;//입력 값의 범위 검사
                            else
                            {
                                eflag = 1;//오류 표시
                            }
                        }
                    }
                }
            }
        }
        else if(strcmp(stinstr,"f") == 0 || strcmp(stinstr,"fill") == 0)
        {
            stinstr = strtok(NULL," \t");
            if(stinstr != NULL)
            {
                starti = findvalue(stinstr);
                stinstr = strtok(NULL," \t");
                if(stinstr != NULL && strcmp(stinstr,",") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        endi = findvalue(stinstr);
                        stinstr = strtok(NULL," \t");
                        if(stinstr != NULL && strcmp(stinstr,",") == 0)
                        {
                            stinstr = strtok(NULL," \t");
                            valuei = findvalue(stinstr);
                            stinstr = strtok(NULL," \t");
                            if(stinstr == NULL)
                            {
                                if(starti >= 0 && starti < 65536 * 16 && endi >= 0 && endi < 65536 * 16 && valuei >= 0 && valuei < 256)// 입력 값의 범위 검사
                                {
                                    if(starti <= endi)instr = 7;//start 값이 end 값보다 작거나 같은지 검사
                                    else
                                    {
                                        eflag = 3;//오류 표시
                                    }
                                }
                                else
                                {
                                    eflag = 1;//오류표시
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(strcmp(stinstr,"reset") == 0)instr = 8;
        else if(strcmp(stinstr,"opcode") == 0)
        {
            stinstr = strtok(NULL," \t");
            if(stinstr != NULL)
            {
                hashindex = hash_function(stinstr);
                insihashhead = hashhead[hashindex];
                valuei = -1;
                for(;;)//hashtable에 있는지 판단
                {
                    if(insihashhead == NULL)break;
                    if(strcmp(stinstr,insihashhead->smne) == 0)
                    {
                        valuei = insihashhead->valuemne;
                        break;
                    }
                    insihashhead = insihashhead->nextmne;
                }
                stinstr = strtok(NULL," \t");
                if(stinstr == NULL && valuei != -1)instr = 9;
                else eflag = 2;//오류 표시
            }
        }
        else if(strcmp(stinstr,"opcodelist") == 0)
        {
            stinstr = strtok(NULL," \t");
            if(stinstr == NULL)instr = 10;
        }
    }
    if(instr != 0)//올바른 명령어일 경우 history에 저장
    {
        newhis = (sainstrs *)malloc(sizeof(sainstrs));
        newhis->nextinstr = NULL;
        for(i = 0; i < len; i++)
        {
            newhis->sainstr[i] = insiinstr[i];
        }
        if(head == NULL)
        {
            head = newhis;
        }
        else
        {
            insihead = head;
            for(;;)
            {
                if(insihead->nextinstr == NULL)break;
                insihead = insihead->nextinstr;
            }
            insihead->nextinstr = newhis;
        }
    }
    else // 올바른 명령어가 아닐 경우 오류 경고 출력
    {
        if(eflag == 0)printf("Wrong instruction\n");
        else if(eflag == 1)printf("Out of range\n");
        else if(eflag == 2)printf("Input memonic is not exist\n");
        else if(eflag == 3)printf("Start is greater than End\n");
    }
}
// 함수 : helpinstr()
// 목적 : 명령어 list 출력
void helpinstr()
{
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}
// 함수 : dirinstr()
// 목적 : 현재 디렉토리 안에 있는 파일 이름 출력
void dirinstr()
{
    DIR *di = NULL;
    struct dirent *din = NULL;
    struct stat dimo;
    di = opendir(".");
    for(;;)
    {
        din = readdir(di);
        if(din == NULL)break;
        lstat(din->d_name,&dimo);
        printf("%s",din->d_name);// 파일 이름 출력
        if(S_ISDIR(dimo.st_mode))printf("/");// 파일이 디렉토리인지 판단
        else if(dimo.st_mode & S_IXUSR)printf("*");
        printf("\t");
    }
    printf("\n");
}
// 함수 : hiinstr()
// 목적 : 현재까지 입력한 명령어 출력
void hiinstr()
{
    int cnt = 0;
    sainstrs *insihead;
    insihead = head;
    for(;;)
    {
        if(insihead == NULL)break;
        cnt = cnt + 1;
        printf("%d\t%s\n",cnt,insihead->sainstr);
        insihead = insihead->nextinstr;
    }
}
// 함수 : findvalue()
// 목적 : 문자열로 전달된 16진수를 10진수로 변경하여 return한다
int findvalue(char *svalue)
{
    int i, suvalue;
    int flag, len;
    flag = 0;
    suvalue = 0;
    len = strlen(svalue);
    for(i = 0; i < len; i++)
    {
        suvalue = suvalue * 16;
        if(svalue[i] >= '0' && svalue[i] <= '9')// 변경 및 전달된 16진수 범위 검사
        {
            suvalue = suvalue + (int)(svalue[i] - '0');
        }
        else if(svalue[i] >= 'A' && svalue[i] <= 'F')
        {
            suvalue = suvalue + (int)(svalue[i] - 'A') + 10;
        }
        else if(svalue[i] >= 'a' && svalue[i] <= 'f')
        {
            suvalue = suvalue + (int)(svalue[i] - 'a') + 10;
        }
        else// 문자열로 전달된 16진수가 잘못 되었을 경우
        {
            flag = 1;//오류 표시
            break;
        }
    }
    if(flag == 1)suvalue = -1;//오류일 경우 suvalue에 -1 저장
    return suvalue;
}
// 함수 : dumpinstr()
// 목적 : 메모리를 start번지 부터 end 번지 까지 출력
void dumpinstr()
{
    int i, j;
    int sline, eline;
    int hex, addhex;
    int cnt, values;
    char saddress[6];
    cnt = 0;
    if(starti == -1 && endi == -1)//start 값과 end 값 이 입력 되지 않았을 경우
    {
        startp = endp + 1;
        if(startp >= 16 * 65536)startp = 0;// 시작 번지가 메모리 범위를 초과할 경우 시작번지에 0 저장
        endp = startp + 159;// 마지막 번지는 시작 번지 + 159
    }
    else if(endi == -1)//start 값만 입력이 된 경우
    {
        startp = starti;
        endp = startp + 159;// 마지막 번지는 시작 번지 + 159
    }
    else// start 값과 end 값이 모두 입력된 경우
    {
        startp = starti;
        endp = endi;
    }
    if(endp >= 16 * 65536)endp = (16 * 65536 - 1);// 마지막 번지가 메모리 범위를 초과할 경우 마지막 번지에 마지막 번지 저장
    sline = startp / 16;
    eline = endp / 16;
    cnt = sline * 16;
    for(i = sline; i <= eline; i++)
    {
        saddress[5] = 0;// 주소를 문자열로 된 16진수로 바꿈
        saddress[4] = '0';
        hex = 16 * 16 * 16;
        addhex = i;
        for(j = 0; j <= 3; j++)
        {
            if((addhex / hex) <= 9)saddress[j] = (char)((addhex / hex) + '0');
            else
            {
                saddress[j] = (char)((addhex / hex) - 10 + 'A');
            }
            addhex = addhex % hex;
            hex = hex / 16;
        }
        printf("%s ",saddress);// 주소 출력
        for(j = 0; j < 16; j++)
        {
            if(cnt >= startp && cnt <= endp)// 출력하는 범위에 포함되는지 판단
            {
                values = (int)(meno[i][j]);
                printhex(values);//저장되어 있는 값을 16진수로 바꾸어 출력
                printf(" ");
            }
            else
            {
                printf("   ");
            }
            cnt = cnt + 1;
        }
        printf(" ; ");
        cnt = cnt - 16;
        for(j = 0; j < 16; j++)
        {
            if(cnt >= startp && cnt <= endp && (int)(meno[i][j]) >= 32 && (int)(meno[i][j]) <= 126)// 출력하는 범위에 포함되는지 판단
            {
                printf("%c",meno[i][j]);
            }
            else
            {
                printf(".");
            }
            cnt = cnt + 1;
        }
        printf("\n");
    }
}
// 함수 : editinstr()
// 목적 : 메모리의 address번지의 값을 value 값으로 변경
void editinstr()
{
    int row, col;
    row = addressi / 16;
    col = addressi % 16;
    meno[row][col] = valuei;
}
// 함수 : fillinstr()
// 목적 : 메모리의 start 번지 부터 end 번지 까지 value 값으로 변경
void fillinstr()
{
    int cnt;
    cnt = starti;
    for(;;)
    {
        meno[cnt/16][cnt%16] = valuei;
        if(cnt == endi)break;
        cnt = cnt + 1;
    }
}
// 함수 : resetinstr()
// 목적 : 메모리 전체를 0으로 변경
void resetinstr()
{
    int i, j;
    for(i = 0; i < 65536; i++)
    {
        for(j = 0; j < 16; j++)
        {
            meno[i][j] = 0;
        }
    }
}
// 함수 : inputopc()
// 목적 : opcode.txt로 부터 정보를 입력 받고 hashtable를 완성한다.
void inputopc()
{
    FILE *inp = fopen("opcode.txt","r");
    char inputstring[100], svalue[100];
    hashcell *newhash, *insihashhead;
    int i, hashindex;
    for(i = 0; i < 20; i++)// hashtable의 head 값 초기화
    {
        hashhead[i] = NULL;
    }
    for(;;)
    {
        if(fgets(inputstring,sizeof(inputstring),inp) == NULL)break;
        newhash = (hashcell*)malloc(sizeof(hashcell));
        newhash->nextmne = NULL;
        sscanf(inputstring,"%s %s %s",svalue,newhash->smne,newhash->mod);
        newhash->valuemne = findvalue(svalue);
        hashindex = hash_function(newhash->smne);
        if(hashhead[hashindex] == NULL)// hashtable에 추가
        {
            hashhead[hashindex] = newhash;
        }
        else
        {
            insihashhead = hashhead[hashindex];
            for(;;)
            {
                if(insihashhead->nextmne == NULL)break;
                insihashhead = insihashhead->nextmne;
            }
            insihashhead->nextmne = newhash;
        }
    }
}
// 함수 : opcodeinstr()
// 목적 : 입력 받은 값의 opcode를 출력한다.
void opcodeinstr()
{
    printf("opcode is ");
    printhex(valuei);
    printf("\n");
}
// 함수 : opcodelistinstr()
// 목적 : hashtable를 출력한다.
void opcodelistinstr()
{
    int i;
    hashcell *insihashhead;
    for(i = 0; i < 20; i++)
    {
        insihashhead = hashhead[i];
        printf("%d : ",i);
        for(;;)
        {
            if(insihashhead == NULL)break;
            printf("[%s,",insihashhead->smne);
            printhex(insihashhead->valuemne);
            printf("]");
            insihashhead = insihashhead->nextmne;
            if(insihashhead != NULL)printf(" -> ");
        }
        printf("\n");
    }
}
// 함수 : printhex()
// 목적 : 전달된 10진수를 16진수로 바꾸어 출력한다.
void printhex(int sui)
{
    int i, su;
    su = 16;
    for(i = 0; i < 2; i++)
    {
        if(sui / su <= 9)printf("%c",(char)(sui / su + '0'));
        else
        {
            printf("%c",(char)((sui / su) - 10 + 'A'));
        }
        sui = sui % su;
        su = su / 16;
    }
}
// 함수 : cm()
// 목적 : 할당 받은 메로리를 해체한다.
void cm()
{
    int i;
    hashcell *insihashhead;
    sainstrs *insihead;
    for(i = 0; i < 20; i++)
    {
        for(;;)
        {
            if(hashhead[i] == NULL)break;
            insihashhead = hashhead[i];
            hashhead[i] = hashhead[i]->nextmne;
            free(insihashhead);
        }
    }
    for(;;)
    {
        if(head == NULL)break;
        insihead = head;
        head = head->nextinstr;
        free(insihead);
    }
}
