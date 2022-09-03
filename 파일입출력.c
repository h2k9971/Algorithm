#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>

#define m 12
#define p 12
#define Max_dir 8 // 총 가능한 이동방향의 수
#define Timestep 1000 // 시간 단위는 ms 이므로 1000이면 1초를 멈추게 됨.
#define TRUE 1
#define FALSE 0

typedef struct Aoff
{
    short int vert;
    short int horiz;
}offsets;

typedef struct St_element
{
    short int row;
    short int col;
    short int dir;
}ty_element;// 스택에 저장할 데이터

typedef struct ListNode* listPointer;

typedef struct ListNode //연결스택의 노드
{
    ty_element data;
    listPointer link;
}listNode;

ty_element pop(listPointer *top);

void push(listPointer* top, ty_element item);

void reverse_stack(listPointer* top);

offsets move1[Max_dir] = {{-1,0}, {-1,1}, {0,1}, {1,1},{1,0}, {1,-1}, {0,-1}, {-1,-1}};

int maze[m+2][p+2];
int mark[m+2][p+2];// 방문여부


int path(int sy, int sx, int dy, int dx)
{
    int i, j, ocount = 0 , row = 0 , col = 0 , nextRow = 0, nextCol = 0, dir;
    int basex, basey; //추가 기능을 위한 변수 선언
    
    int found= FALSE, num_brtrack =0; // brtrack 일어난 수,
    int EXIT_ROW = dy, EXIT_COL =dx;
    
    ty_element position;
    
    listPointer top = NULL;
    
    if (maze[sy][sx] == 1 || maze[dy][dx] == 1) {
        printf("입력오류: 출발점이나 목표점이 막힌 셀입니다.\n");
        return 0;
    }
    
    position.row = sy;
    position.col = sx;
    position.dir =0;
    push(&top, position);
    
    for(i=0; i<m; i++)
    {
        for(j=0; j<p; j++)
        {
            mark[1+i][1+j]=0;
        }
    }
    
    mark[sy][sx] = 1;

    
    while(top != NULL && !found)
    {
        position = pop(&top);
        num_brtrack++;
        row = position.row;
        col = position.col;
        dir = position.dir;
        
        while(dir < Max_dir && !found)
        {
            nextRow = row + move1[dir].vert;
            nextCol = col + move1[dir].horiz;
            
            if(nextRow == EXIT_ROW && nextCol == EXIT_COL)
            {
                found = TRUE;
                break;
            }
            else if(!maze[nextRow][nextCol] && !mark[nextRow][nextCol])
            {
                mark[nextRow][nextCol] = TRUE;
                position.row = row;
                position.col = col;
                position.dir = ++dir;
                push(&top, position);
                row = nextRow;
                col = nextCol;
                dir =0;
            }
            else ++dir;
        }
        if(found)
            break;
    
    }

    printf("\n");
    
    position.row = row;
    position.col = col;
    push(&top, position);
    
    position.row = nextRow;
    position.col = nextCol;
    push(&top, position);
    reverse_stack(&top);
    
    if(found)
    {
        listPointer curr = top;
        printf("찾은 경로(row, col)\n");
        ocount=0;
        while(curr != NULL)
        {
            printf("[ %d, %d]", curr->data.row, curr->data.col);
            ocount++;
            curr = curr->link;
            
            if(ocount % 6 == 0)
                printf("\n");
        }
    }
    else printf("경로가 존재하지 않습니다!!\n");
    
    printf("\n경로길이 = %d,  ", ocount);
    printf("백트래킹 수: %d\n", num_brtrack-1); // 처음에 1증가 했으므로 -1해줘야함
    
    return 1;
}

void push(listPointer* top, ty_element item)
{
    
    listPointer temp;
    temp = (listPointer)malloc(sizeof(listNode));
    
    temp->data = item;
    temp->link = *top;
    *top = temp;

}

ty_element pop(listPointer* top)
{
    listPointer temp;
    ty_element item;
    temp = *top;
    
    item = (*top)->data;
    *top = temp->link;
    free(temp);
    
    return item;
}

void reverse_stack(listPointer* ptr_top)
{
    listPointer curr;
    listPointer temp;
    listPointer next;
    curr = *ptr_top;
    
    if(curr == NULL || curr->link == NULL)
        return;

    temp = curr;
    curr= curr->link;
    temp->link = NULL;

    while(curr != NULL)
    {
        next = curr->link;
        curr->link = *ptr_top; //
        *ptr_top = curr; // 다시 맨앞을 가리킴 다음노드를 첫번째로 하기 위해
        curr = next;
    }
}
//리스트를 역순으로 출력

int main()
{
    int starty = 0, startx = 0, endy = 0, endx = 0;
    int res;
    FILE* fp = fopen("maze_14_2.txt", "r");
   
    if(fp == NULL)
    {
        printf("파일을 읽을 수 없습니다.");
        return 0;
    }
    for(int i=0; i<m+2; i++)
    {
        for(int j=0; j<m+2; j++)
        {
            fscanf(fp, "%1d" , &maze[i][j]);
        }
    } // 미로 파일 입출력 받기
    
    fclose(fp);
    
    for(int i=0; i<m+2; i++)
    {
        for(int j=0; j<m+2; j++)
        {
            printf("%2d", maze[i][j]);
        }
        printf("\n");
    } // 미로 출력
    
    printf("\n");

    while(1)
    {
        printf("출발점, 목표점은?(4개의 정수;  종료는 Control + D Enter): ");
        res = scanf("%d %d %d %d", &starty, & startx, &endy, &endx);
        if(res != 4)
            break;
        path(starty, startx , endy, endx);// 시작점과 도착점 경로입력
    }
    printf("프로그램이 종료 되었습니다.\n");
    return 0;
}
