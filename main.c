#include <stdio.h>
#include <string.h>
/*
    BORTH
    Stack based RPN calculator -- Made for Data structure class @ 2018
    
    ================================

    commands -
    edit : edit mode
    done : edit done
    print : print saved stack
    clear : 삭제해라 애송이
    run : Y'know the drill

    ================================

    OPCODES : 
    NOOP - [nothing. nada]
    ADD - [+]
    SUB - [-]
    MUL - [*]
    DIV - [/]
    MOD - [%]
    LSH - [<<]
    RSH - [>>]
    AND - [&]
    OR - [|]
    XOR - [^]
    NOT - [~]
    VAL - [Dec or Hex value]
*/

// 상수들
#define MAX_STACK 0xFF
#define MAX_CODE_LEN 5
#define MAX_VAL 0xFF // overflow value to emulate 8bit integer

// 명령어 타입들
typedef enum _OPTYPE
{
    OP_NADA = -1,
    OP_NOOP, // reserved
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_LSH,
    OP_RSH,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_NOT,
    OP_VAR_NUM,
    OP_VAR_STR,
    OP_SZ
}optype;

typedef enum _VTYPE
{
    V_NULL = 0,
    V_STR,
    V_INT
}vartype;

// 변수 구조체
typedef struct _VAR
{
    vartype type;
    union val
    {
        string* str;
        int* num;
    };
}var;

// 명령어 구조체
typedef struct _OP
{
    optype code;
    var val;
}opcode;

// 스트링 구조체
typedef struct _STR
{
    int len;
    char* v;
}string;

// 명령어 -> 읽을수 있는 스트링 테이블
char opcodeLUT[OP_SZ][MAX_CODE_LEN] =
{
    "NOOP",
    "+",
    "-",
    "*",
    "/",
    "%",
    "<<",
    ">>",
    "&",
    "|",
    "^",
    "~",
    "#"
};

// Stack #1 - 프로그램 스택
opcode opstack[MAX_STACK+1];
int ophead = 0;

// Stack #2 - 스택 컴퓨터 메모리
var pmem[MAX_STACK+1];
int pmemhead = 0;

int progstatus = 0;

// 스트링으로 명령어를 찾기 위한 함수
optype findOp (const char* op)
{
    // 스트링 맨 앞이 숫자면 100% 숫자 변수입니다
    int tmp = (op[0] - '0');
    if (tmp >= 0 && tmp <= 9)
        return OP_VAR_NUM;
    else if (op[0] == '"') // string 변수
        return OP_VAR_STR;

    // 아님 말고
    for (int i=0; i<OP_SZ; i++)
    {
        if (strcmp(op, opcodeLUT[i]) == 0) // found matching operator
            return i;
    }

    // 아무것도 못찾았어요 ㅠㅠ
    return OP_NADA;
}

/*
    OPERATOR STACK
    ==============
*/
// 명령어 스택 복붙 (쓰이는일이 없네요 으악)
void copyOp (opcode* src, opcode* dst)
{
    for (int i=0; i<MAX_STACK; i++)
        dst[i] = src[i];
}

// 프로그램 스택 청소
void clearOp (opcode* src)
{
    for (int i=0; i<MAX_STACK; i++)
        src[i] = (opcode){OP_NADA, -1};
}

// 명령어 넣기
void pushOp (optype type, var val, opcode* stk, int* head)
{
    stk[ (*head) ].code = type;
    stk[ (*head) ].val = val;

    (*head)++;
}

// 명령어 넣기 (스트링)
void addOp (const char* op)
{
    // int val = -1;
    optype code = findOp(op);
    var val  = (var){V_INT, val.num = -1};

    if (code == OP_NADA)
        printf("Enter valid code please!");
    else if (code == OP_VAR_NUM)
    {
        sscanf(op, "%i", val.num);
        // printf("VAL : %d\n", val);
    }
    else if (code == OP_VAR_STR)
        

    // add
    pushOp(code, val, opstack, &ophead);

    return;
}

// 명령어들 출력
void printOpcodes ()
{
    int tmpcode;
    for (int i=0; i<ophead; i++)
    {
        tmpcode = opstack[i].code;

        if (tmpcode == OP_VAR_NUM)
        {
            printf("%02d| %d\n", i, opstack[i].val);
        }
        else if (tmpcode == OP_VAR_STR)
        {
            printf("%02d| %s\n", i, opstack[i].val);
        }
        else
        {
            printf("%02d| %s\n", i, opcodeLUT[ tmpcode ]);
        }
    }
}

/*
    COMPUTER MEM STACK
    ==============
*/
// 메모리 청소
void clearMem (int* src)
{
    for (int i=0; i<MAX_STACK; i++)
        src[i] = 0;
}

// 최상위 값 확인
var peekMem (var* src, int head)
{
    return src[head];
}

// 메모리에 값 넣기
void pushMem (var val, var* src, int* head)
{
    if (*head > MAX_STACK)
    {
        printf("ERR] STACK OVERFLOW");
        progstatus = 0xBAD;
    }

    src[(*head)++] = val;
}

// 메모리에 값 빼기
var popMem (var* src, int* head)
{
    if (*head <= 0)
    {
        printf("ERR] STACK UNDERFLOW");
        progstatus = 0xBAD;
    }

    return src[--(*head)];
}

// 메모리 스택 출력
void printMem ()
{
    var tmp;
    for (int i=0; i<pmemhead; i++)
    {
        tmp = pmem[i];

        if (tmp.type == V_INT)
            printf("%02X %d\n", i, *(pmem[ i ].num));
        else if (tmp.type == V_STR)
            printf("%02X %s\n", i, *(pmem[ i ].str));
    }
    printf("HEAD : %d\n", pmemhead);
}

// 프로그램 실행
void runMachine ()
{
    clearMem(pmem);

    var vx, vy; // 명령어의 x y 변수
    int x, y;
    progstatus = 0; // 프로그램의 상태 초기화
    opcode* current = NULL; // 현재 opcode를 담을 포인터 변수
    for (int pc=0; pc<ophead; pc++)
    {
        current = &opstack[pc];

        // 명령어 기능들 실행
        switch (current->code)
        {
            case OP_VAR_NUM: // 숫자 / 변수
            case OP_VAR_STR:
                pushMem(current->val, pmem, &pmemhead);
                break;

            // 사칙연산
            case OP_ADD:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                if (vx.type != V_INT || vy.type != V_INT)
                {
                    printf("ERR] TYPE MISMATCH AT PC=%d!\n", pc);
                    progstatus = 1;
                    break;
                }

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x + y}, pmem, &pmemhead);
                break;
            
            case OP_SUB:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                if (vx.type != V_INT || vy.type != V_INT)
                {
                    printf("ERR] TYPE MISMATCH AT PC=%d!\n", pc);
                    progstatus = 1;
                    break;
                }

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x - y}, pmem, &pmemhead);
                break;

            case OP_MUL:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x * y}, pmem, &pmemhead);
                break;

            case OP_DIV:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x / y}, pmem, &pmemhead);
                break;

            case OP_MOD:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x % y}, pmem, &pmemhead);
                break;

            // 비트 연산자
            case OP_LSH:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x << y}, pmem, &pmemhead);
                break;

            case OP_RSH:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x >> y}, pmem, &pmemhead);
                break;

            case OP_AND:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x & y}, pmem, &pmemhead);
                break;

            case OP_OR:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x | y}, pmem, &pmemhead);
                break;

            case OP_XOR:
                vy = popMem(pmem, &pmemhead);
                vx = popMem(pmem, &pmemhead);

                x = *(vx.num);
                y = *(vy.num);

                pushMem((var){V_INT, x ^ y}, pmem, &pmemhead);
                break;

            case OP_NOT:
                vx = popMem(pmem, &pmemhead);
                x = *(vx.num);

                pushMem((var){V_INT, ~x}, pmem, &pmemhead);
                break;
        }

        // 스택 오버플로우 / 언더플로우가 일어나면 progStatus에 0xBAD 플래그가 담깁니다.
        if (progstatus == 0xBAD)
        {
            printf("PROGRAM EXIT WITH ERROR\n");
            break;
        }
    }

    // 최상위 값 출력 & 끝내기
    printf("TOP VALUE : %d\n", peekMem(pmem, pmemhead - 1));
    printf("Program ended\n");
}

int main (int argc, char** argv)
{
    // 환영 인삿말 & 버전 표시
    printf("BORTH version 0.42 -- bin @ 2018\nType 'help' for help.\n\n");

    int quit = 0, editing = 0;
    char command[8];
    char stat[8] = "READY";
    optype token = OP_NADA;
    while (!quit)
    {
        printf("%s > ", stat);

        // get command
        scanf("%s", command);

        token = findOp(command);

        // exec
        if (strcmp("help", command) == 0)
            printf("-=[HELP]=-\n'help' - Displays this menu\n'edit' - Enter edit mode\n'done' - Quit edit mode\n'run' - Run the program\n'clear' - Clears the whole program\n'dump' - Prints the program\n'quit' - Quit BORTH. Bye!\n\
in 'EDIT >' mode, you can enter the instructions for the stack machine.\nInstructions are following : \n=========================\n\
[+, -, *, /, %] - Normal arthimetic operation.\n\
[<<, >>, &, |, ^, ~] - Bitwise operation.\n\
[Decimals (123) and Hexadecimals (0x123)] - Adds a Constant value\n\
\n=========================\n");
        else if (strcmp("edit", command) == 0)
        {
            editing = 1;
            strcpy(stat, "EDIT");
        }
        else if (strcmp("done", command) == 0)
        {
            editing = 0;
            strcpy(stat, "READY");
        }
        else if (strcmp("clear", command) == 0)
        {
            ophead = 0;
            clearOp(opstack);
        }
        else if (strcmp("dump", command) == 0)
        {
            printOpcodes();
        }
        else if (strcmp("dumpmem", command) == 0)
        {
            printMem();
        }
        else if (strcmp("run", command) == 0)
        {
            runMachine();
        }
        else if (strcmp("quit", command) == 0)
        {
            quit = 1;
        }
        else if (token != OP_NADA)
        {
            if (editing == 1)
                addOp(command);
            else
                printf("ERR] You must enter the EDIT mode in order to edit the program!\n");
        }
        else
            printf("what (type 'help' if you're lost)\n");
    }

    return 0;
}