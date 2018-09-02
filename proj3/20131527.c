#include"20131527.h"

// 전역 변수
int instr; // 명령어
int starti, endi, addressi, valuei; // 입력 받은 값
int startp, endp; // 이전 dump에서 출력한 번지
unsigned char memo[65536][16]; // 메모리
sainstrs *head; // history Linked_list head 주소 값
hashcell *hashhead[20]; // hashtable 각 줄의 head 주소 값
symcell *symhead1[100], *symhead2[100];
mcell *mhead = NULL;
int symbolflag = 0;
int startlc = 0;
int progaddr = 0; // loader 또는 run 명령어를 수행할 때 시작하는 주소
bpcell *bhead = NULL; // breakpoint를 저장할 Linked list의 head값을 저장한다.
esymcell *esymhead[30];
// 함수 : hash_function()
// 목적 : 입력받은 값의 hashtable의 index값을 return
int hash_function(char *s, int rown)
{
    int i, sum;
    for(i = 0, sum = 0; i <strlen(s); i++)
    {
        sum += s[i];
    }
    return sum % rown;
}

// 프로그램 main
int main()
{
    head = NULL; // 초기화
    startp = 0;
    endp = -1;
    progaddr = 0; 
    bhead = NULL;
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
        else if(instr == 13)symbolinstr();
        else if(instr == 16)printbp();
        else if(instr == 17)clearbp();
    }
    cm();
    return 0;
}

// 함수 : inputinstr()
// 목적 : 명령어 입력 및 명령어 오류 판단
void inputinstr()
{
    char sinstr[500], insiinstr[100];
    char *stinstr, readfile[100];
    char hstr[10]; // 16진수을 저장한다.
    int i, len, stlen, hashindex;
    int eflag = 0, basev = 0, length = 0;
    int bpaddr = 0, CSADDR = 0;
    sainstrs *newhis, *insihead;
    hashcell *insihashhead;
    bpcell *newbp; // 새로 breakpoint 정보가 저장된다.
    bpcell *insibhead;
    bpcell *phead;
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
                starti = findvalue(stinstr, 16);
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
                        endi = findvalue(stinstr, 16);
                        stinstr = strtok(NULL," \t");
                        if(stinstr == NULL)
                        {
                            if(starti >= 0 && starti < 65536 * 16 && endi >= 0 && endi < 65536 * 16)//입력 값의 범위 검사
                            {
                                if(starti <= endi)instr = 5;// start 값이 end 값보다 작거나 같은지 검사
                                else eflag = 3;// 오류 표시
                            }
                            else eflag = 1;// 오류 표시
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
                addressi = findvalue(stinstr, 16);
                stinstr = strtok(NULL," \t");
                if(stinstr != NULL && strcmp(stinstr,",") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        valuei = findvalue(stinstr, 16);
                        stinstr = strtok(NULL," \t");
                        if(stinstr == NULL)
                        {
                            if(addressi >= 0 && addressi < 16 * 65536 && valuei >= 0 && valuei < 256)instr = 6;//입력 값의 범위 검사
                            else eflag = 1;//오류 표시
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
                starti = findvalue(stinstr, 16);
                stinstr = strtok(NULL," \t");
                if(stinstr != NULL && strcmp(stinstr,",") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        endi = findvalue(stinstr, 16);
                        stinstr = strtok(NULL," \t");
                        if(stinstr != NULL && strcmp(stinstr,",") == 0)
                        {
                            stinstr = strtok(NULL," \t");
                            valuei = findvalue(stinstr, 16);
                            stinstr = strtok(NULL," \t");
                            if(stinstr == NULL)
                            {
                                if(starti >= 0 && starti < 65536 * 16 && endi >= 0 && endi < 65536 * 16 && valuei >= 0 && valuei < 256)// 입력 값의 범위 검사
                                {
                                    if(starti <= endi)instr = 7;//start 값이 end 값보다 작거나 같은지 검사
                                    else eflag = 3;//오류 표시
                                }
                                else eflag = 1;//오류표시
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
                hashindex = hash_function(stinstr, 20);
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
        else
        {
            for(i = 0; i < len; i++)sinstr[i] = insiinstr[i];
            sinstr[len] = 0;
            stinstr = strtok(sinstr," \t");
            if(stinstr != NULL)
            {
                if(strcmp(stinstr,"assemble") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    eflag = pone(stinstr,&basev,&length); // Pass One 호출
                    if(eflag == 0)eflag = ptwo(stinstr,&basev,&length); // Pass One 에서 오류가 발견 되지 않을 경우 Pass Two 호출
                    if(eflag == 0)
                    {
                        for(i = 0; i < 100; i++)symhead2[i] = symhead1[i]; // Symbol Table 저장
                        symbolflag = 1; // Symbol Table이 존재 한다는 표시
                    }
                    instr = 11;
                }
                else if(strcmp(stinstr,"type") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        FILE *rf = fopen(stinstr,"r");
                        stinstr = strtok(NULL," \t"); // 파일의 존재 유무 확인
                        if(rf != NULL && stinstr ==  NULL)
                        {
                            for(;;) // 바로 출력
                            {
                                if(fgets(readfile,sizeof(readfile),rf) == NULL)break;
                                len = strlen(readfile);
                                for(i = 0; i < len; i++)printf("%c",readfile[i]);
                            }
                            fclose(rf);
                            instr = 12;
                        }
                        else if(stinstr != NULL)eflag = 0;
                        else eflag = 4;
                    }
                }
                else if(strcmp(stinstr,"symbol") == 0)
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr == NULL && symbolflag == 1)instr = 13; // Symbol Table이 존재하는지 확인
                    else eflag = 5;
                }
                else if(strcmp(stinstr,"progaddr") == 0) // 시작 주소값 명령 처리
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr != NULL)
                    {
                        progaddr = findvalue(stinstr,16);
                        stinstr = strtok(NULL," \t");
                        if(progaddr >= 0 && stinstr == NULL)instr = 14;
                    }
                }
                else if(strcmp(stinstr,"loader") == 0) // loader 명령 처리
                {
                    CSADDR = progaddr;
                    for(i = 0; i < 30; i++)esymhead[i] = NULL; // 초기화
                    stinstr = strtok(NULL," \t");
                    for(;;)
                    {
                        if(stinstr == NULL)break;
                        eflag = linking(stinstr,&CSADDR);
                        if(eflag != 0)break;
                        stinstr = strtok(NULL," \t");
                    }
                    if(eflag == 0) // 오류가 없을 경우 계속 진행
                    {
                        CSADDR = progaddr;
                        for(i = 0; i < len; i++)sinstr[i] = insiinstr[i];
                        sinstr[len] = 0;
                        stinstr = strtok(sinstr," \t");
                        stinstr = strtok(NULL," \t");
                        for(;;)
                        {
                            if(stinstr == NULL)break;
                            eflag = loader(stinstr,&CSADDR);
                            if(eflag != 0)break;
                            stinstr = strtok(NULL," \t");
                        }
                        if(eflag == 0) // 오류가 없을 경우 loadmap를 출력해줌
                        {
                            instr = 15;
                            printloadmap();
                        }
                    }
                }
                else if(strcmp(stinstr,"bp") == 0) // breakpoint 명령 처리
                {
                    stinstr = strtok(NULL," \t");
                    if(stinstr == NULL)instr = 16; // breakpoint를 전부 출력 명령 처리
                    else if(stinstr != NULL)
                    {
                        if(strtok(stinstr,"clear") == 0) // clear 명령 처리
                        {
                            stinstr = strtok(NULL," \t");
                            if(stinstr == NULL)instr = 17;
                        }
                        else // breakpoint 추가 명령 처리
                        {
                            bpaddr = findvalue(stinstr,16);
                            for(i = 0; i < 4; i++)hstr[i] = stinstr[i];
                            hstr[4] = 0;
                            stinstr = strtok(NULL," \t");
                            if(bpaddr >= 0 && stinstr == NULL) // 형식 및 주소 입력 확인
                            {
                                newbp = (bpcell*)malloc(sizeof(bpcell));
                                newbp -> addr = bpaddr;
                                newbp -> nextbp = NULL;
                                if(bhead == NULL)bhead = newbp; // 새로운 breakpoint 정보
                                else
                                {
                                    if(bhead->addr > newbp->addr) // 오름차순으로 정렬
                                    {
                                        newbp->nextbp = bhead;
                                        bhead = newbp;
                                    }
                                    else
                                    {
                                        insibhead = bhead;
                                        phead = bhead;
                                        for(;;)
                                        {
                                            if(insibhead == NULL)break;
                                            if(insibhead -> addr > newbp -> addr)break;
                                            phead = insibhead;
                                            insibhead = insibhead->nextbp;
                                        }
                                        phead->nextbp = newbp;
                                        newbp->nextbp = insibhead;
                                    }
                                }
                                printf("\t[ok] create breapoint %s\n",hstr); // 메세지 출력
                                instr = 18;
                            }
                        }
                    }
                }
            }
        }
    }
    if(instr != 0)//올바른 명령어일 경우 history에 저장
    {
        newhis = (sainstrs *)malloc(sizeof(sainstrs));
        newhis->nextinstr = NULL;
        strcpy(newhis->sainstr,insiinstr);
        if(head == NULL)head = newhis;
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
        else if(eflag == 4)printf("Wrong file name\n");
        else if(eflag == 5)printf("Symbol is not exist\n");
        else if(eflag == 6)printf("Symbol is duplication\n");
        else if(eflag == 7)printf("Wrong object code\n");
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
    printf("assemble filename\n");
    printf("type filename\n");
    printf("symbol\n");
    printf("progaddr\n");
    printf("loader\n");
    printf("bp\n");
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
// 목적 : 문자열로 전달된 숫자들을 ty에 따라 10진수 또는 16진수로 변경해준다.
int findvalue(char *svalue, int ty)
{
    int i, suvalue;
    int flag, len;
    flag = 0;
    suvalue = 0;
    len = strlen(svalue);
    for(i = 0; i < len; i++)
    {
        suvalue = suvalue * ty;
        if(svalue[i] >= '0' && svalue[i] <= '9')// 변경 및 전달된 16진수 범위 검사
        {
            suvalue = suvalue + (int)(svalue[i] - '0');
        }
        else if(ty == 16 && svalue[i] >= 'A' && svalue[i] <= 'F')
        {
            suvalue = suvalue + (int)(svalue[i] - 'A') + 10;
        }
        else if(ty == 16 && svalue[i] >= 'a' && svalue[i] <= 'f')
        {
            suvalue = suvalue + (int)(svalue[i] - 'a') + 10;
        }
        else// 문자열로 전달된 16진수가 잘못 되었을 경우
        {
            flag = 1;//오류 표시
            break;
        }
    }
    if(flag == 1)suvalue = -1;
    return suvalue;
}
// 함수 : dumpinstr()
// 목적 : 메모리를 start번지 부터 end 번지 까지 출력
void dumpinstr()
{
    int i, j;
    int sline, eline;
    int addhex;
    int cnt, values;
    char hstr[10];
    cnt = 0;
    for(i = 0; i < 10; i++)hstr[i] = 0;
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
        addhex = i * 16;
        transstring(addhex, hstr, 5);
        printf("%s ",hstr);
        for(j = 0; j < 16; j++)
        {
            if(cnt >= startp && cnt <= endp)// 출력하는 범위에 포함되는지 판단
            {
                values = (int)(memo[i][j]);
                transstring(values, hstr, 2);
                printf("%s ",hstr);
            }
            else printf("   ");
            cnt = cnt + 1;
        }
        printf(" ; ");
        cnt = cnt - 16;
        for(j = 0; j < 16; j++)
        {
            if(cnt >= startp && cnt <= endp && (int)(memo[i][j]) >= 32 && (int)(memo[i][j]) <= 126)// 출력하는 범위에 포함되는지 판단
            {
                printf("%c",memo[i][j]);
            }
            else printf(".");
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
    memo[row][col] = valuei;
}
// 함수 : fillinstr()
// 목적 : 메모리의 start 번지 부터 end 번지 까지 value 값으로 변경
void fillinstr()
{
    int cnt;
    cnt = starti;
    for(;;)
    {
        memo[cnt/16][cnt%16] = valuei;
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
            memo[i][j] = 0;
        }
    }
}
// 함수 : inputopc()
// 목적 : opcode.txt로 부터 정보를 입력 받고 hashtable를 완성한다.
void inputopc()
{
    FILE *inp = fopen("opcode.txt","r");
    char inputstring[100], svalue[100];
    char imod[10], *stmod;
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
        for(i = 0; i < 3; i++)newhash->mod[i] = 0;
        sscanf(inputstring,"%s %s %s",svalue,newhash->smne,imod);
        newhash->valuemne = findvalue(svalue, 16);
        stmod = strtok(imod,"/ ");
        for(;;)
        {
            if(stmod == NULL)break;
            if(strcmp(stmod,"1") == 0)newhash->mod[0] = 1;
            else if(strcmp(stmod,"2") == 0)newhash->mod[1] = 1;
            else if(strcmp(stmod,"3") == 0)newhash->mod[2] = 1;
            else if(strcmp(stmod,"4") == 0)newhash->mod[3] = 1;
            stmod = strtok(NULL,"/ ");
        }
        hashindex = hash_function(newhash->smne, 20);
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
    fclose(inp);
}
// 함수 : opcodeinstr()
// 목적 : 입력 받은 값의 opcode를 출력한다.
void opcodeinstr()
{
    int i;
    char hstr[3];
    printf("opcode is ");
    for(i = 0; i < 3; i++)hstr[i] = 0;
    transstring(valuei, hstr, 2);
    printf("%s\n",hstr);
}
// 함수 : opcodelistinstr()
// 목적 : hashtable를 출력한다.
void opcodelistinstr()
{
    int i;
    char hstr[10];
    hashcell *insihashhead;
    for(i = 0; i < 10; i++)hstr[i] = 0;
    for(i = 0; i < 20; i++)
    {
        insihashhead = hashhead[i];
        printf("%d : ",i);
        for(;;)
        {
            if(insihashhead == NULL)break;
            transstring(insihashhead->valuemne, hstr, 2);
            printf("[%s,%s]",insihashhead->smne, hstr);
            insihashhead = insihashhead->nextmne;
            if(insihashhead != NULL)printf(" -> ");
        }
        printf("\n");
    }
}
// 함수 : transstring()
// 목적 : 전달된 숫자를 길이 len의 16진수 문자열로 변경해준다.
int transstring(int sui, char *tstr, int len)
{
    int i, hex = 1;
    int flag = 0;
    for(i = 0; i < len - 1; i++)hex = hex * 16;
    for(i = 0; i < len; i++)
    {
        if(sui / hex >= 0 && sui / hex <= 9)tstr[i] = (char)((sui / hex) + '0');
        else if(sui / hex >= 10 && sui / hex <= 15)tstr[i] = (char)(((sui / hex) - 10) + 'A');
        else
        {
            flag = -1;
            break;
        }
        sui = sui % hex;
        hex = hex / 16;
    }
    tstr[len] = 0;
    return flag;
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
// 함수 : symbolinstr()
// 목적 : Symbol Table에 있는 Symbol들을 내림차순으로 정렬하고 출력한다.
void symbolinstr()
{
    int i;
    char hstr[4];
    symcell *phead, *insisymhead;
    symcell *newsym, *insiphead, *insipphead;
    phead = NULL;
    for(i = 0; i < 100; i++)
    {
        insisymhead = symhead2[i];
        for(;;)
        {
            if(insisymhead == NULL)break;
            newsym = (symcell*)malloc(sizeof(symcell));
            newsym->nextsym = NULL;
            strcpy(newsym->sym,insisymhead->sym);
            newsym->locv = insisymhead->locv;
            if(phead == NULL)phead = newsym;
            else
            {
                insiphead = phead;
                if(strcmp(insiphead->sym,newsym->sym) < 0) // Symbol 정렬
                {
                    newsym -> nextsym = insiphead;
                    phead = newsym;
                }
                else
                {
                    for(;;)
                    {
                        if(insiphead == NULL)break;
                        if(strcmp(insiphead->sym,newsym->sym) < 0)break;
                        insipphead = insiphead; // 탐색한 노드 저장
                        insiphead = insiphead->nextsym;
                    }
                    insipphead->nextsym = newsym;
                    newsym->nextsym = insiphead;
                }
            }
            insisymhead = insisymhead->nextsym;
        }
    }
    for(i = 0; i < 4;i++)hstr[i] = 0;
    newsym = phead;
    for(;;) // 내림 차순으로 정렬한 Symbol 출력
    {
        if(newsym == NULL)break;
        transstring(newsym->locv,hstr,4);
        printf("\t%s\t%s\n",newsym->sym,hstr);
        newsym = newsym->nextsym;
        free(phead);
        phead = newsym;
        if(phead == NULL)break;
    }
}
// 함수 : pone()
// 목적 : symbol들을 Symbol Table에 저장하고 입력 받은 Code의 오류를 확인한다.
int pone(char *fn, int *bv, int *length)
{
    int len;
    int eflag = 0;
    char instrs[100];
    char *p1, *p2, *p3, *p4, *p5;
    int i, index = 0, location = 0;
    int tyflag = 0, returnv;
    int endflag = 0, baseflag = 0, rflag = 0;
    char bstr[100];
    int linev = 5, numflag = 0;
    FILE *mtxt1 = fopen("line.txt","w");
    FILE *mtxt2 = fopen("location.txt","w");
    hashcell *insihashhead;
    symcell *newsym, *insisymhead;
    len = strlen(fn);
    if(len >= 4 && strcmp(fn + len - 4,".asm") == 0) // .asm 파일인지 확인
    {
        FILE *in = fopen(fn,"r");
        if(in == NULL)eflag = 2;
        else
        {
            for(i = 0; i < 100; i++)symhead1[i] = NULL;
            fgets(instrs,sizeof(instrs),in);
            fprintf(mtxt1,"%d\n",linev);
            fprintf(mtxt2,"%d\n",location);
            linev = 5;
            len = strlen(instrs);
            if(instrs[len - 1] == '\n')instrs[len - 1] = 0;
            p1 = strtok(instrs," \t");
            p2 = strtok(NULL," \t");
            p3 = strtok(NULL," \t");
            if(p1 != NULL && p2 != NULL && p3 != NULL && strcmp(p2,"START") == 0)
            {
                *length = findvalue(p3,16);
                startlc = *length;
                location = *length;
                for(;;)
                {
                    if(eflag != 0)break;
                    linev = linev + 5;
                    if(fgets(instrs,sizeof(instrs),in) == NULL)break;
                    len = strlen(instrs);
                    if(instrs[len - 1] == '\n')instrs[len - 1] = 0;
                    p1 = NULL; // blank space 기준으로 문자열 구분
                    p2 = NULL;
                    p3 = NULL;
                    p4 = NULL;
                    p5 = NULL;
                    p1 = strtok(instrs," \t");
                    p2 = strtok(NULL," \t");
                    p3 = strtok(NULL," \t");
                    p4 = strtok(NULL," \t");
                    p5 = strtok(NULL," \t");
                    if(p1 == NULL && endflag == 1)break;
                    if(p1 != NULL)
                    {
                        fprintf(mtxt1,"%d\n",linev); // line 값과 location 값 따로 파일에 저장
                        fprintf(mtxt2,"%d\n",location);
                        if(endflag == 1)eflag = 2;
                        else if(p1[0] != '.')
                        {
                            insihashhead = NULL;
                            tyflag = 0;
                            if(p1[0] == '+')tyflag = 1;
                            findopcode(p1 + tyflag,&insihashhead);
                            rflag = finddirect(p1);
                            if(insihashhead == NULL && rflag == 0) // symbol 인지 검사
                            {
                                newsym = (symcell*)malloc(sizeof(symcell));
                                newsym->nextsym = NULL;
                                strcpy(newsym->sym,p1);
                                newsym->locv = location;
                                index = hash_function(p1,100);
                                if(symhead1[index] == NULL)symhead1[index] = newsym;
                                else
                                {
                                    insisymhead = symhead1[index];
                                    for(;;)
                                    {
                                        if(insisymhead->nextsym == NULL)break;
                                        if(strcmp(insisymhead->sym,newsym->sym) == 0)
                                        {
                                            eflag = 1;
                                            break;
                                        }
                                        insisymhead = insisymhead->nextsym;
                                    }
                                    insisymhead->nextsym = newsym;
                                }
                            }
                            else if(insihashhead != NULL || rflag != 0) // symbol이 존재 하지 않은 경우
                            {
                                p5 = p4;
                                p4 = p3;
                                p3 = p2;
                                p2 = p1;
                            }
                            insihashhead = NULL;
                            tyflag = 0;
                            if(p2 != NULL)
                            {
                                if(p2[0] == '+')tyflag = 1;
                                findopcode(p2 + tyflag,&insihashhead);
                                rflag = finddirect(p2);
                            }
                            if(insihashhead != NULL) // opcode일 경우
                            {
                                if(tyflag == 1 && insihashhead->mod[3] == 1) // 포맷 4
                                {
                                    if(strcmp(insihashhead->smne,"RSUB") == 0 && p3 == NULL)location = location + 4;
                                    if(strcmp(insihashhead->smne,"RSUB") != 0 && p3 != NULL)
                                    {
                                        numflag = xcheck(p3,p4,p5);
                                        if(numflag == -1)eflag = 3;
                                        else location = location + 4;
                                    }
                                }
                                else if(tyflag == 0 && insihashhead->mod[2] == 1) // 포맷 3
                                {
                                    if(strcmp(insihashhead->smne,"RSUB") == 0 && p3 == NULL)location = location + 3;
                                    else if(strcmp(insihashhead->smne,"RSUB") != 0 && p3 != NULL)
                                    {
                                        numflag = xcheck(p3,p4,p5);
                                        if(numflag == -1)eflag = 3;
                                        else location = location + 3;
                                    }
                                }
                                else if(tyflag == 0 && insihashhead->mod[1] == 1 && p3 != NULL) // 포맷 2
                                {
                                    numflag = commacheck(p3,p4,p5,insihashhead->smne);
                                    if(numflag == -1)eflag = 3;
                                    else location = location + 2;
                                }
                                else if(tyflag == 0 && insihashhead->mod[0] == 1)location = location + 1; // 포맷 1
                                else eflag = 3;
                            }
                            else if(insihashhead == NULL && rflag != 0) // directive 일 경우
                            {
                                if(rflag == 1)
                                {
                                    if(p3 != NULL)
                                    {
                                        len = strlen(p3);
                                        if(p3[0] == 'X' || p3[0] == 'x' || p3[0] == 'C' || p3[0] == 'c')
                                        {
                                            if(len >= 3 && p3[1] == (char)(39) && p3[len - 1] == (char)(39))
                                            {
                                                if(p3[0] == 'C' || p3[0] == 'c')location = location + (len - 3);
                                                else if((len - 3) % 2 == 0)location = location + ((len - 3) / 2);
                                                else eflag = 3;
                                            }
                                        }
                                    }
                                }
                                else if(rflag == 2)
                                {
                                    if(p3 != NULL)
                                    {
                                        returnv = findvalue(p3,10);
                                        if(returnv >= 0)location = location + 3;
                                        else eflag = 3;
                                    }
                                    else eflag = 3;
                                }
                                else if(rflag == 3)
                                {
                                    if(p3 != NULL)
                                    {
                                        returnv = findvalue(p3,10);
                                        if(returnv >= 0)location = location + returnv;
                                        else eflag = 3;
                                    }
                                    else eflag = 3;
                                }
                                else if(rflag == 4)
                                {
                                    if(p3 != NULL)
                                    {
                                        returnv = findvalue(p3,10);
                                        if(returnv >= 0)location = location + (returnv * 3);
                                        else eflag = 3;
                                    }
                                }
                                else if(rflag == 5)
                                {
                                    if(p3 != NULL)
                                    {
                                        strcpy(bstr,p3);
                                        baseflag = 1;
                                    }
                                }
                                else if(rflag == 6)endflag = 1;
                                else eflag = 3;
                            }
                            else eflag = 3; // 아무 것도 아닐 경우
                        }
                    }
                }
            }
            else eflag = 3;
            fclose(in);
        }
    }
    else eflag = 2;
    fclose(mtxt1);
    fclose(mtxt2);
    if(baseflag == 1) // BASE가 있을 경우 *bv에 BASE 값 저장
    {
        insisymhead = NULL;
        findsymbol(bstr,&insisymhead);
        if(insisymhead == NULL)eflag = 1;
        else (*bv) = insisymhead->locv;
    }
    if(eflag == 2)printf("File name is wrong\n"); // 오류 출력
    if(eflag == 3)printf("Line %d is Wrong\n",linev);
    *length = location - *length; // 길이 계산
    return eflag;
}
// 함수 : findopcode()
// 목적 : 넘겨 받은 문자열이 opcode인지 확인
int findopcode(char *str, hashcell **insihashhead)
{
    int index;
    index = hash_function(str,20);
    *insihashhead = hashhead[index];
    for(;;)
    {
        if((*insihashhead) == NULL)break;
        if(strcmp((*insihashhead)->smne,str) == 0)break;
        (*insihashhead) = (*insihashhead)->nextmne;
    }
    return 0;
}
// 함수 : finddirect()
// 목적 : 넘겨 받은 문자열이 directive인지 확인
int finddirect(char *str)
{
    int flag = 0;
    if(strcmp(str,"BYTE") == 0)flag = 1;
    else if(strcmp(str,"WORD") == 0)flag = 2;
    else if(strcmp(str,"RESB") == 0)flag = 3;
    else if(strcmp(str,"RESW") == 0)flag = 4;
    else if(strcmp(str,"BASE") == 0)flag = 5;
    else if(strcmp(str,"END") == 0)flag = 6;
    else if(strcmp(str,"START") == 0)flag = 7;
    return flag;
}
// 함수 : ptwo()
// 목적 : 입력 받은 code를 분석하여 .lst 파일과 .obj 파일을 만든다.
int ptwo(char *fn, int *bv, int *length)
{
    char obcode[100];
    int olen = 0, midv = 0;
    char *p1 = NULL, *p2 = NULL, *p3 = NULL, *p4 = NULL, *p5 = NULL;
    char instrs[100], instrs2[100];
    char hstr[15], obstr[200];
    hashcell *insihashhead;
    symcell *insisymhead;
    int len = 0;
    int rflag = 0, tyflag = 0;
    int i, objflag = 0, returnv = 0;
    int rlocv = 0, location = 0;
    int eflag = 0;
    int rline = 0, line = 0;
    int endflag = 0, oblen = 0;
    char fn1[100], fn2[100];
    mcell *insimhead, *mnew;
    len = strlen(fn);
    for(i = 0; i < len; i++)fn1[i] = fn[i];
    for(i = 0; i < len; i++)fn2[i] = fn[i];
    fn1[len - 3] = 'o';
    fn1[len - 2] = 'b';
    fn1[len - 1] = 'j';
    fn1[len] = 0;
    fn2[len - 3] = 'l';
    fn2[len - 2] = 's';
    fn2[len - 1] = 't';
    fn2[len] = 0;
    FILE *in = fopen(fn,"r");
    FILE *mtxt1 = fopen("line.txt","r");
    FILE *mtxt2 = fopen("location.txt","r");
    FILE *mtxt3 = fopen(fn2,"w"); // .lst 파일 생성
    FILE *mtxt4 = fopen(fn1,"w"); // .obj 파일 생성
    fscanf(mtxt1,"%d",&rline);
    fscanf(mtxt2,"%d",&rlocv);
    for(i = 0; i < 15; i++)hstr[i] = 0;
    for(i = 0; i < 10; i++)obcode[i] = 0;
    oblen = 0;
    for(;;)
    {
        if(eflag != 0)break;
        if(fgets(instrs,sizeof(instrs),in) == NULL)break;
        obcode[0] = 0;
        len = strlen(instrs);
        if(instrs[len - 1] == '\n')instrs[len - 1] = 0;
        strcpy(instrs2,instrs);
        line = rline;
        location = rlocv;
        fscanf(mtxt1,"%d",&rline);
        fscanf(mtxt2,"%d",&rlocv);
        p1 = strtok(instrs," \t");
        p2 = strtok(NULL," \t");
        p3 = strtok(NULL," \t");
        if(p1 == NULL && endflag == 1)break;
        if(p1 != NULL && p1[0] != '.')
        {
            insihashhead = NULL;
            tyflag = 0;
            if(p1[0] == '+')tyflag = 1;
            findopcode(p1 + tyflag,&insihashhead);
            rflag = finddirect(p1);
            if(insihashhead != NULL || rflag != 0)
            {
                p4 = p3;
                p3 = p2;
                p2 = p1;
            }
            insihashhead = NULL;
            tyflag = 0;
            if(p2[0] == '+')tyflag = 1; // 포맷 4인지 확인
            findopcode(p2 + tyflag,&insihashhead);
            rflag = finddirect(p2);
            if(rflag != 0) // directive 일 경우
            {
                objflag = 2;
                if(rflag == 1)
                {
                    len = strlen(p3);
                    if(p3[0] == 'X' || p3[0] == 'x')
                    {
                        olen = 0;
                        for(i = 2; i < len - 1; i++)
                        {
                            obcode[olen] = p3[i];
                            olen = olen + 1;
                        }
                        obcode[olen] = 0;
                    }
                    else
                    {
                        olen = 0;
                        for(i = 2; i < len - 1; i++)
                        {
                            transstring((int)(p3[i]),hstr,2);
                            obcode[olen] = hstr[0];
                            obcode[olen + 1] = hstr[1];
                            olen = olen + 2;
                        }
                        obcode[olen] = 0;
                    }
                }
                else if(rflag == 2)
                {
                    returnv = findvalue(p2,10);
                    transstring(returnv,hstr,6);
                    for(i = 0; i < 5; i++)obcode[i] = hstr[i];
                    obcode[6] = 0;
                }
                else
                {
                    obcode[0] = 0;
                    if(rflag == 3 || rflag == 4)objflag = 3;
                    if(rflag == 6)endflag = 1;
                    else if(rflag == 7)objflag = 1;
                }
            }
            else if(insihashhead != NULL)
            {
                objflag = 2;
                returnv = insihashhead->valuemne;
                if(insihashhead->mod[0] == 1)
                {
                    transstring(returnv,hstr,2);
                    obcode[0] = hstr[0];
                    obcode[1] = hstr[1];
                    obcode[2] = 0;
                }
                else if(insihashhead->mod[1] == 1)
                {
                    transstring(returnv,hstr,2);
                    obcode[0] = hstr[0];
                    obcode[1] = hstr[1];
                    p5 = strtok(NULL," \t");
                    returnv = commacheck(p3,p4,p5,p2);
                    transstring(returnv / 10,hstr,1);
                    obcode[2] = hstr[0];
                    transstring(returnv % 10,hstr,1);
                    obcode[3] = hstr[0];
                    obcode[4] = 0;
                }
                else
                {
                    if(p3 != NULL && p3[0] == '@')returnv = returnv + 2; // indirect addressing
                    else if(p3 != NULL && p3[0] == '#')returnv = returnv + 1; // immediate addressing
                    else returnv = returnv + 3; // simple addressing
                    transstring(returnv,hstr,2);
                    obcode[0] = hstr[0];
                    obcode[1] = hstr[1];
                    if(strcmp(p2,"RSUB") != 0) // RSUB가 아닐 시
                    {
                        p5 = strtok(NULL," \t");
                        if(xcheck(p3,p4,p5) == 1)midv = 8;
                        else midv = 0;
                        insisymhead = NULL;
                        tyflag = 0;
                        if(p3[0] == '#' || p3[0] == '@')tyflag = 1;
                        findsymbol(p3 + tyflag,&insisymhead);
                        if(insisymhead == NULL)returnv = findvalue(p3 + tyflag,10);
                        else returnv = insisymhead->locv;
                        if(p2[0] == '+' && returnv >= 0) // 포맷 4일 경우
                        {
                            midv = midv + 1;
                            transstring(returnv,hstr,5);
                            for(i = 0; i < 5; i++)obcode[i + 3] = hstr[i];
                            obcode[8] = 0;
                            transstring(midv,hstr,1);
                            obcode[2] = hstr[0];
                            if(insisymhead != NULL)
                            {
                                mnew = (mcell*)malloc(sizeof(mcell));
                                mnew->nextm = NULL;
                                mnew->loc = location + 1;
                                if(mhead == NULL)mhead = mnew;
                                else
                                {
                                    insimhead = mhead;
                                    for(;;)
                                    {
                                        if(insimhead->nextm == NULL)break;
                                        insimhead = insimhead->nextm;
                                    }
                                    insimhead->nextm = mnew;
                                }
                            }
                        }
                        else if(insisymhead != NULL)
                        {
                            returnv = returnv - rlocv;
                            if(returnv >= -2048 && returnv <= 2047) // pc relative
                            {
                                eflag = 0;
                                returnv = returnv & 0xfff;
                                transstring(returnv,hstr,3);
                                midv = midv + 2;
                            }
                            else // base relative
                            {
                                returnv = returnv + rlocv;
                                if(*bv == -1)eflag = 2;
                                else
                                {
                                    returnv = returnv - (*bv);
                                    if(returnv >= 0 && returnv <= 4095)
                                    {
                                        transstring(returnv,hstr,3);
                                        midv = midv + 4;
                                    }
                                    else eflag = 2;
                                }
                            }
                            if(eflag == 0)
                            {
                                for(i = 0; i < 3; i++)obcode[i + 3] = hstr[i];
                                obcode[6] = 0;
                            }
                        }
                        else if(returnv >= 0) // symbol이 아닌 숫자가 입력 되는 경우
                        {
                            transstring(returnv,hstr,3);
                            for(i = 0; i < 3; i++)obcode[i + 3] = hstr[i];
                            obcode[6] = 0;
                        }
                        else eflag = 3;
                        transstring(midv,hstr,1);
                        obcode[2] = hstr[0];
                    }
                    else // RSUB인 경우
                    {
                        if(p2[0] == '+')
                        {
                            for(i = 2; i < 8; i++)obcode[i] = '0';
                            obcode[8] = 0;
                        }
                        else
                        {
                            for(i = 2; i < 6; i++)obcode[i] = '0';
                            obcode[6] = 0;
                        }
                    }
                }
            }
            else eflag = 3;
        }
        fprintf(mtxt3,"\t%d",line); // .lst 파일 생성
        if(obcode[0] == 0)fprintf(mtxt3,"\t\t");
        else
        {
            transstring(location,hstr,4);
            fprintf(mtxt3,"\t%s",hstr);
        }
        fprintf(mtxt3,"\t%-80s",instrs2);
        if(obcode[0] != 0)fprintf(mtxt3,"\t%s",obcode);
        fprintf(mtxt3,"\n");
        if(objflag == 1) // .obj 파일 생성
        {
            fprintf(mtxt4,"H%-6s",p1);
            transstring(startlc,hstr,6);
            fprintf(mtxt4,"%s",hstr);
            transstring((*length),hstr,6);
            fprintf(mtxt4,"%s\n",hstr);
        }
        else
        {
            len = strlen(obcode);
            if(oblen == 0 && obcode[0] != 0)
            {
                obstr[0] = 'T';
                transstring(location,hstr,6);
                for(i = 0; i < 6; i++)obstr[i + 1] = hstr[i];
                oblen = 9;
            }
            if(objflag == 3) // RESW, RESB 가 입력 될 경우
            {
                if(oblen > 0)
                {
                    obstr[oblen] = 0;
                    transstring(((oblen - 9) / 2),hstr,2);
                    obstr[7] = hstr[0];
                    obstr[8] = hstr[1];
                    fprintf(mtxt4,"%s\n",obstr);
                }
                oblen = 0;
            }
            if(objflag == 2 && oblen + len < 69 && obcode[0] != 0) // object code 저장
            {
                for(i = 0; i < len; i++)obstr[oblen + i] = obcode[i];
                oblen = oblen + len;
            }
            else if(objflag == 2 && oblen + len >= 69 && obcode[0] != 0) // 길이가 초과 될 경우
            {
                obstr[oblen] = 0;
                transstring(((oblen - 9) / 2),hstr,2);
                obstr[7] = hstr[0];
                obstr[8] = hstr[1];
                fprintf(mtxt4,"%s\n",obstr);
                obstr[0] = 'T';
                oblen = 0;
                transstring(location,hstr,6);
                for(i = 0; i < 6; i++)obstr[i + 1] = hstr[i];
                for(i = 0; i < len; i++)obstr[i + 9] = obcode[i];
                oblen = 9 + len;
            }
        }
    }
    if(oblen != 0)
    {
        obstr[oblen] = 0;
        transstring(((oblen - 9) / 2),hstr,2);
        obstr[7] = hstr[0];
        obstr[8] = hstr[1];
        fprintf(mtxt4,"%s\n",obstr);
    }
    if(mhead != NULL) // Modification Record 출력 
    {
        insimhead = mhead;
        for(;;)
        {
            if(insimhead == NULL)break;
            transstring((insimhead->loc - startlc),hstr,6);
            fprintf(mtxt4,"M%s05\n",hstr);
            insimhead = insimhead->nextm;
            free(mhead);
            mhead = insimhead;
        }
    }
    transstring(startlc,hstr,6);
    fprintf(mtxt4,"E%s\n",hstr);
    fclose(mtxt1); // 파일 정리
    fclose(mtxt2);
    fclose(mtxt3);
    fclose(mtxt4);
    fclose(in);
    remove("line.txt");
    remove("location.txt");
    if(eflag != 0) // 오류가 발생 할 경우 생성 파일 삭제
    {
        remove(fn1);
        remove(fn2);
    }
    if(eflag == 2)printf("BASE is empty\n"); // 오류 문구 출력
    if(eflag == 3)printf("Line %d is syntax error\n",line);
    if(eflag == 0)
    {
        printf("\toutput file : [%s], [%s]\n",fn2,fn1);
    }
    return eflag;
}
// 함수 : findsymbol()
// 목적 : 전달 받은 문자열이 symbol인지 확인
int findsymbol(char *str, symcell **insisymhead)
{
    int index;
    index = hash_function(str,100);
    *insisymhead = symhead1[index];
    for(;;)
    {
        if((*insisymhead) == NULL)break;
        if(strcmp((*insisymhead)->sym,str) == 0)break;
        (*insisymhead) = (*insisymhead)->nextsym;
    }
    return 0;
}
// 함수 : findmnenumber()
// 목적 : 전달 받은 문자열의 Mnemonic number를 알려준다.
int findmnenumber(char *str)
{
    int flag = -1;
    if(strcmp(str,"A") == 0)flag = 0;
    else if(strcmp(str,"X") == 0)flag = 1;
    else if(strcmp(str,"L") == 0)flag = 2;
    else if(strcmp(str,"PC") == 0)flag = 8;
    else if(strcmp(str,"SW") == 0)flag = 9;
    else if(strcmp(str,"B") == 0)flag = 3;
    else if(strcmp(str,"S") == 0)flag = 4;
    else if(strcmp(str,"T") == 0)flag = 5;
    else if(strcmp(str,"F") == 0)flag = 6;
    return flag;
}
// 함수 : commacheck()
// 목적 : 포맷 2의 형식이 맞는지 확인하고 구성 레지스터를 구분한다.
int commacheck(char *str1, char *str2, char *str3, char *str)
{
    int i;
    int len = 0;
    int cflag = 0, flag1 = 0, flag2 = 0;
    int eflag = 0, mflag = 0;
    char *p1 = NULL, *p2 = NULL;
    len = strlen(str1);
    if(str1[len - 1] == ',') // 콤마 체크
    {
        str1[len - 1] = 0;
        cflag = 1;
    }
    else 
    {
        for(i = 0; i < len - 1; i++) // str1 사이에 있을 경우
        {
            if(str1[i] == ',')
            {
                mflag = 1;
                break;
            }
        }
        if(mflag == 1 && str2 == NULL)
        {
            p1 = strtok(str1,",");
            p2 = strtok(NULL,",");
            flag1 = findmnenumber(p1);
            flag2 = findmnenumber(p2);
        }
    }
    if(mflag == 0)flag1 = findmnenumber(str1);
    if(mflag == 0 && str2 != NULL)
    {
        if(strcmp(str2,",") == 0)
        {
            cflag = 1;
            if(str3 != NULL)flag2 = findmnenumber(str3);
            else eflag = 1;
        }
        else if(str2[0] == ',')
        {
            cflag = 1;
            flag2 = findmnenumber(str2 + 1);
            if(str3 != NULL)eflag = 1;
        }
        else if(cflag == 1)
        {
            if(str3 == NULL)flag2 = findmnenumber(str2);
            else eflag = 1;
        }
        else eflag = 1;
    }
    else if(str2 == NULL && cflag == 1)eflag = 1;
    if(eflag == 1)return -1;
    else if(strcmp(str,"CLEAR") != 0 && strcmp(str,"TIXR") != 0 && cflag == 0)return -1; // CLEAR와 TIXR은 뒤에 레지스터가 한개
    else return (flag1 * 10 + flag2); // 2자리로 보냄
}
// 함수 : xcheck()
// 목적 : x 레지스터를 사용하는지 확인한다.
int xcheck(char *str1, char *str2, char *str3)
{
    int cflag = 0;
    int len = 0;
    int flag = 0;
    len = strlen(str1);
    if(str1[len - 1] == ',')
    {
        str1[len - 1] = 0;
        cflag = 1;
    }
    if(len >= 2 &&(strcmp(str1 + len - 2,",x") == 0 || strcmp(str1 + len - 2,",X") == 0))
    {
        str1[len - 1] = 0;
        str1[len - 2] = 0;
        if(str2 == NULL)flag = 1;
        else flag = -1;
    }
    else
    {
        if(str2 != NULL)
        {
            if(strcmp(str2,",") == 0)
            {
                if(str3 != NULL)
                {
                    if(strcmp(str3,"x") == 0 || strcmp(str3,"X") == 0)flag = 1;
                    else flag = -1;
                }
                else flag = -1;
            }
            else if(strcmp(str2,",x") == 0 || strcmp(str2,",X") == 0)flag = 1;
            else if(cflag == 1)
            {
                if(strcmp(str2,"x") == 0 || strcmp(str2,"X") == 0)flag = 1;
                else flag = -1;
            }
            else flag = -1;
        }
    }
    if(cflag == 1 && flag == 0)flag = -1;
    return flag;
}
// 함수 : printbp()
// 목적 : 저장된 breakpoint를 출력
void printbp()
{
    bpcell *insibhead = NULL;
    char hstr[10];
    int i;
    for(i = 0; i < 10; i++)hstr[i] = 0;
    insibhead = bhead;
    if(bhead == NULL) // breakpoint가 없는 경우
    {
        printf("\tBreakpoint is not exit\n");
    }
    else // breakpoint 출력
    {
        printf("\tbreakpoint\n\t__________\n");
        for(;;)
        {
            if(insibhead == NULL)break;
            transstring(insibhead->addr,hstr,4);
            printf("\t%s\n",hstr);
            insibhead = insibhead->nextbp;
        }
    }
}
// 함수 : clearbp()
// 목적 : 저장된 breakpoint를 삭제
void clearbp()
{
    bpcell *insibhead = NULL;
    if(bhead == NULL) // breakpoint가 없을 경우
    {
        printf("\tBreakpoint is not exit\n");
    }
    else // breakpoint 삭제
    {
        printf("\t[ok] clear all breakpoints\n");
        insibhead = bhead;
        for(;;)
        {
            if(insibhead == NULL)break;
            insibhead = bhead->nextbp;
            free(bhead);
            bhead = insibhead;
        }
    }
}
// 함수 : linking()
// 목적 : 각 obj파일들을 link를 해준다.
// 리턴 값 : 오류 정보
int linking(char *filename, int *CSADDR)
{
    int CSLTH = 0;
    int eflag = 0, len = 0;
    int i, location = 0, length = 0;
    int index = 0;
    char str[150], hstr[10];
    esymcell *insiesymhead = NULL;
    esymcell *newesym;
    len = strlen(filename);
    if(len >= 4 && strcmp(filename + len - 4,".obj") == 0) // 파일 형식 확인
    {
        FILE *in = fopen(filename,"r");
        for(i = 0; i < 150; i++)str[i] = 0;
        for(i = 0; i < 10; i++)hstr[i] = 0;
        if(in == NULL)eflag = 4;
        else
        {
            for(;;)
            {
                fgets(str,sizeof(str),in);
                len = strlen(str);
                str[len] = 0;
                for(i = len - 1; i >= 0; i--) // object에 불필요한 공백을 없애주기 위해
                {
                    if(str[i] == ' ' || str[i] == '\n' || str[i] == '\t')str[i] = 0;
                    else break;
                }
                len = strlen(str);
                for(i = 0; i < len; i++) // blank space를 ' '로 통일
                {
                    if(str[i] == '\t')str[i] = ' ';
                }
                if(str[0] == 'H') // H가 나올때 이름과 길이를 알아낸다.
                {
                    if(strlen(str) < 18)
                    {
                        eflag = 7;
                        break;
                    }
                    newesym = (esymcell*)malloc(sizeof(esymcell));
                    newesym->nextesym = NULL;
                    newesym->typeflag = 0;
                    for(i = 1; i <= 6; i++)newesym->esym[i - 1] = str[i];
                    newesym->esym[6] = 0;
                    hstr[6] = 0;
                    for(i = 7; i <= 12; i++)hstr[i - 7] = str[i];
                    location = findvalue(hstr,16);
                    for(i = 13; i <= 18; i++)hstr[i - 13] = str[i];
                    length = findvalue(hstr,16);
                    newesym->locv = location + *CSADDR;
                    newesym->length = length;
                    CSLTH = length;
                    index = hash_function(newesym->esym,30);
                    esymhead[index] = newesym;
                }
                else if(str[0] == 'D') // D가 나올때 프로그램에서 선언한 symbol들을 알아낸다.
                {
                    len = 1;
                    for(;;)
                    {
                        if(eflag != 0)break;
                        if(len >= strlen(str))break;
                        newesym = (esymcell*)malloc(sizeof(esymcell));
                        newesym->nextesym = NULL;
                        newesym->typeflag = 1;
                        for(i = len; i <= 5 + len; i++)newesym->esym[i - len] = str[i];
                        newesym->esym[6] = 0;
                        hstr[6] = 0;
                        for(i = 6 + len; i <= 11 + len; i++)hstr[i - len - 6] = str[i];
                        location = findvalue(hstr,16);
                        newesym->locv = location + *CSADDR;
                        index = hash_function(newesym->esym,30);
                        if(esymhead[index] == NULL)esymhead[index] = newesym; // symbol hashtable에 추가
                        else
                        {
                            insiesymhead = esymhead[index];
                            for(;;)
                            {
                                if(strcmp(insiesymhead->esym,newesym->esym) == 0)
                                {
                                    eflag = 6;
                                    break;
                                }
                                if(insiesymhead->nextesym == NULL)break;
                                insiesymhead = insiesymhead->nextesym;
                            }
                            insiesymhead->nextesym = newesym;
                        }
                        len = len + 12;
                    }
                }
                else if(str[0] == 'E') // E가 나오면 탐색을 멈춘다.
                {
                    *CSADDR = *CSADDR + CSLTH;
                    if(*CSADDR > 0xfffff)
                    {
                        eflag = 1;
                        break;
                    }
                    break;
                }
                if(eflag != 0)break;
            }
            fclose(in);
        }
    }
    else eflag = 4;
    return eflag;
}
// 함수 : loader()
// 목적 : object code를 메모리 위에 올려준다.
// 리턴 값 : 오류 정보
int loader(char *filename, int *CSADDR)
{
    int refer[10], i, eflag = 0;
    int CSLTH = 0, len = 0, add = 0;
    int index = 0, location = 0, valuei = 0, rvaluei = 0;
    char str[150], hstr[10], esym[10];
    esymcell *insiesym = NULL;
    FILE *in = fopen(filename,"r");
    for(i = 0; i < 10; i++)refer[i] = 0;
    for(i = 0; i < 10; i++)hstr[i] = 0;
    for(i = 0; i < 150; i++)str[i] = 0;
    refer[1] = *CSADDR;
    for(;;)
    {
        fgets(str,sizeof(str),in);
        len = strlen(str);
        str[len] = 0;
        for(i = len - 1; i >= 0; i--) // object code에 불필요한 공백 제거
        {
            if(str[i] == ' ' || str[i] == '\t' || str[i] == '\n')str[i] = 0;
            else break;
        }
        len = strlen(str);
        for(i = 0; i <= len - 1; i++) // blank space를 ' '로 통일
        {
            if(str[i] == '\t')str[i] = ' ';
        }
        if(str != NULL && str[0] == 'H')
        {
            for(i = 13; i <= 18; i++)hstr[i - 13] = str[i];
            hstr[6] = 0;
            CSLTH = findvalue(hstr,16);
        }
        else if(str != NULL && str[0] == 'R') // 참조하는 symbol 체크
        {
            len = 1;
            for(;;)
            {
                for(i = len; i <= len + 1; i++)hstr[i - len] = str[i];
                hstr[2] = 0;
                for(i = 0; i <= 6; i++)esym[i] = ' ';
                for(i = len + 2; i <= len + 7; i++)
                {
                    if(str[i] == 0)break;
                    esym[i - len - 2] = str[i];
                }
                esym[6] = 0;
                index = hash_function(esym,30);
                insiesym = esymhead[index];
                for(;;)
                {
                    if(insiesym == NULL) // 참조하는 symbol이 존재하지 않는 경우
                    {
                        eflag = 5;
                        break;
                    }
                    if(strcmp(insiesym->esym,esym) == 0)break;
                    insiesym = insiesym->nextesym;
                }
                if(eflag != 0)break;
                index = findvalue(hstr,10);
                refer[index] = insiesym->locv;
                len = len + 8;
                if(len >= strlen(str))break;
            }
        }
        else if(str != NULL && str[0] == 'T') // object code를 배치
        {
            for(i = 1; i <= 6; i++)hstr[i - 1] = str[i];
            hstr[6] = 0;
            location = findvalue(hstr,16);
            add = location + *CSADDR;
            for(i = 7; i <= 8; i++)hstr[i - 7] = str[i];
            hstr[2] = 0;
            len = findvalue(hstr,16); // load 작업
            for(i = 0; i < len; i++)
            {
                hstr[0] = str[9 + (i * 2)];
                hstr[1] = str[10 + (i * 2)];
                hstr[2] = 0;
                valuei = findvalue(hstr,16);
                memo[add / 16][add % 16] = valuei;
                add = add + 1;
            }
        }
        else if(str != NULL && str[0] == 'M') // 메모리에 있는 값들을 변경시켜준다.
        {
            for(i = 1; i <= 6; i++)hstr[i - 1] = str[i];
            hstr[6] = 0;
            location = findvalue(hstr,16);
            add = location + *CSADDR;
            for(i = 7; i <= 8; i++)hstr[i - 7] = str[i];
            hstr[2] = 0;
            len = findvalue(hstr,16);
            for(i = 10; i <= 11; i++)hstr[i - 10] = str[i];
            hstr[2] = 0;
            index = findvalue(hstr,16);
            if(len == 6)
            {
                valuei = 0;
                for(i = 0; i <= 2; i++)
                {
                    valuei = valuei * 256;
                    valuei = valuei + (int)(memo[(add + i) / 16][(add + i) % 16]);
                }
                if(str[9] == '+')rvaluei = refer[index];
                else rvaluei = (int)(refer[index] * -1) & 0xffffff; // 2의 보수로 변경
                valuei = valuei + rvaluei;
                valuei = valuei & 0xffffff;
                for(i = 2; i >= 0; i--)
                {
                    memo[(add + i) / 16][(add + i) % 16] = (char)(valuei % 256);
                    valuei = valuei / 256;
                }
            }
            else
            {
                valuei = 0;
                valuei = (int)(memo[add / 16][add % 16] % 16);
                for(i = 1; i <= 2; i++)
                {
                    valuei = valuei * 256;
                    valuei = valuei + (int)(memo[(add + i) / 16][(add + i) % 16]);
                }
                if(str[9] == '+')rvaluei = refer[index];
                else rvaluei = (int)(refer[index] * -1) & 0xfffff; // 2의 보수로 변경
                valuei = valuei + rvaluei;
                valuei = valuei & 0xfffff;
                for(i = 2; i >= 1; i--)
                {
                    memo[(add + i) / 16][(add + i) % 16] = (char)(valuei % 256);
                    valuei = valuei / 256;
                }
                memo[add / 16][add % 16] = memo[add / 16][add % 16] + valuei;
            }
        }
        else if(str[0] == 'E') // E가 나올 경우 탐색을 멈춤
        {
            *CSADDR = *CSADDR + CSLTH;
            break;
        }
        if(eflag != 0)break;
    }
    return eflag;
}
// 함수 : printloadmap()
// 목적 : ESTAB에서 symbol들에 정보를 가지고와 주소값을기준으로 오름차순 정렬한뒤 출력한다.
void printloadmap()
{
    int i, totallength = 0, len = 0;
    char hstr[10];
    for(i = 0; i < 10; i++)hstr[i] = 0;
    esymcell *sesymhead = NULL, *insishead = NULL, *phead;
    esymcell *insiesymhead = NULL, *newesym = 0;
    for(i = 0; i < 30; i++)
    {
        insiesymhead = esymhead[i];
        if(insiesymhead != NULL) // hashtable에서 symbol들에 대한 정보를 가지고 온다.
        {
            for(;;)
            {

                if(insiesymhead == NULL)break;
                newesym = (esymcell *)malloc(sizeof(esymcell));
                strcpy(newesym->esym,insiesymhead->esym);
                newesym->typeflag = insiesymhead->typeflag;
                newesym->locv = insiesymhead->locv;
                newesym->nextesym = NULL;
                if(newesym->typeflag == 0)newesym->length = insiesymhead->length;
                if(sesymhead == NULL)sesymhead = newesym; // 위치값을 기준으로 오름차순 정렬
                else
                {
                    if(sesymhead->locv > newesym->locv)
                    {
                        newesym->nextesym = sesymhead;
                        sesymhead = newesym;
                    }
                    else
                    {
                        phead = sesymhead;
                        insishead = sesymhead;
                        for(;;)
                        {
                            if(insishead == NULL)break;
                            if(insishead->locv > newesym->locv)break;
                            phead = insishead;
                            insishead = insishead->nextesym;
                        }
                        phead->nextesym = newesym;
                        newesym->nextesym = insishead;
                    }
                }
                insiesymhead = insiesymhead->nextesym;
            }
        }
    }
    totallength = 0; // 출력
    printf("\tcontrol\tsymbol\taddress\tlength\n");
    printf("\tsection\tname\n");
    printf("\t______________________________\n");
    for(;;) // 탐색해주면서 출력
    {
        if(sesymhead == NULL)break;
        if(sesymhead->typeflag == 0) // control section인 경우
        {
            if(sesymhead->locv > 0xffff)len = 5;
            else len = 4;
            transstring(sesymhead->locv,hstr,len);
            printf("\t%s\t\t%s",sesymhead->esym,hstr);
            transstring(sesymhead->length,hstr,4);
            printf("\t%s\n",hstr);
            totallength = totallength + sesymhead->length; // 총 길이를 구하기 위해
        }
        else // symbol인 경우
        {
            printf("\t\t%s",sesymhead->esym);
            if(sesymhead->locv > 0xffff)len = 5;
            else len = 4;
            transstring(sesymhead->locv,hstr,len);
            printf("\t%s\n",hstr);
        }
        phead = sesymhead;
        sesymhead = sesymhead->nextesym;
        free(phead);
    }
    printf("\t______________________________\n");
    transstring(totallength,hstr,4);
    printf("                  total length  %s\n",hstr);
}
