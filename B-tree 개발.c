//2019253053 김현호 xcode를 이용하여 작성하였음.

//알고리즘 기초 b-tree 데이터 관리 개발
// 비트리 탐색, 삽입, 삭제 프로그램
// 입력: 2 개의 회사명 파일 - Com_names1.txt(29413 레코드),  Com_names2.txt(40040레코드)
//       (주의: 회사명은 중간에 space 글자를 포함할 수 도 있음.) = fgets 함수를 이용함.
// 먼저 두 파일 내의 모든 회사명을 각 레코드로 하여 넣고,
//  그 다음  명령문 실행 루프로 실험함.
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define MAXK 2
#define HALFK MAXK/2 //capacity 이다.
#define MAX 100 // 스택 최대 원소수
#define false 0
#define true 1

typedef struct element
{
    char name[100]; //회사명
    int nleng; // 회사명 길이
}ele;
//레코드 정의, 회사명과 그 길이로 구성됨

int top = -1;

typedef struct node* nodeptr; // nodeptr은 node의 형태를 갖는 포인터이다.

typedef struct node
{
    int fill_cnt;
    ele rec[MAXK];
    nodeptr ptr[MAXK + 1];
}node;  // 일반노드

typedef struct big_node
{
    nodeptr ptr[MAXK + 2];
    ele rec[MAXK + 1];
}big_node;
// 일반노드보다 레코드와 포인터가 하나씩 더 큰 빅노드 (합병 시 사용)

typedef struct two_Bn
{
    nodeptr ptr[2 * MAXK + 1];
    ele rec[2 * MAXK];
}two_Bnode;

nodeptr root = NULL;  //root는 전역 변수로 사용한다.

nodeptr stack[MAX]; //stack의 max값은 100

void push(nodeptr node);

nodeptr pop();

void insert_btree();//file 전체를 삽입 이 함수는 insert_arec을 호출한다.

int insert_arec(ele in_rec);// 레코드를 하나 삽입한다.

nodeptr retrieve(char*, int*); // 키값을 넣어 검색한다. [깊이와 함께 출력]

int seq_scan_btree(nodeptr curr); // 전체 레코드들을 출력한다.

void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char wcase, int j);

int B_tree_deletion(char*); //키를 받고 그 레코드를 삭제하는 함수

int main()
{
    char line[200], * res_gets;
    ele a_insert;
    char name_s[100];
    size_t lleng;
    int i, k, r = 0;
    nodeptr tp;

    insert_btree();

    while (1) //명령문 수행 루프이다.
    {
        fflush(stdin); //buffer clear
        printf("\n명령을 넣으시오\n");
        printf("insert: i 이름 / delete: d 이름 / retrieve: r 이름 / 전체출력: p / 종료: e  >> ");
        res_gets = gets(line); //gets 함수로 입력값을 받는다
        if (res_gets == NULL)
        {
            break; //no input
        }

        lleng = strlen(line);
        if (lleng <= 0)
        {
            continue; //empty line input. try again.
        }

        i = 1;
        if (line[0] == 'E' || line[0] == 'e')
        {      // exit program
            printf("종료명령이 들어왔음!\n\n");
            return 0;
        }

        else if (line[0] == 'I' || line[0] == 'i') //회사명을 먼저 가져옴
        {
            k = 0;
            while (line[i] == ' ')
            {
                i++;
            }
            while (i < lleng)
            {
                name_s[k] = line[i];
                k++;
                i++;
            }
            name_s[k] = '\0';

            if (strlen(name_s) == 0)
            {
                continue;
            }

            strcpy(a_insert.name, name_s); // strcpy로 복사해줌.
            a_insert.nleng = strlen(name_s); //레코드 길이 저장

            top = -1;
            r = insert_arec(a_insert); // 레코드 하나만 삽입하는 함수를 호출
            if (r == 0)
            {
                printf("삽입 실패.\n");
            }
            else
            {
                printf("삽입 성공.\n");
            }
        }
        else if (line[0] == 'D' || line[0] == 'd') // 삭제 명령 수행
        {
            k = 0;
            while (line[i] == ' ')
            {
                i++;
            }
            while (i < lleng)
            {
                name_s[k] = line[i];
                k++;
                i++;
            }
            name_s[k] = '\0'; //입력 받은 값을 name_s에 저장 마지막 값은 NULL 값

            if (strlen(name_s) == 0)
            {
                continue;
            }

            r = B_tree_deletion(name_s); //레코드 값 입력 후 삭제를 진행시킴
            if (r == 0) //return 값이 0이면 레코드가 존재하지 않음
            {
                printf("삭제요청 받은 키의 레코드가 존재하지 않음.\n");
            }
            else
            {
                printf("삭제 성공.\n");
            }
        }
        else if (line[0] == 'R' || line[0] == 'r') //탐색 명령 수행
        {
            k = 0;
            while (line[i] == ' ')
            {
                i++;
            }
            while (i < lleng)
            {
                name_s[k] = line[i]; // 탐색 레코드를 name_s에 저장함
                k++;
                i++;
            }
            name_s[k] = '\0'; // 마지막 값은 NULL 넣어줌

            if (strlen(name_s) == 0)
            {
                continue;
            }
            tp = retrieve(name_s, &i); // 탐색 함수 호출

            if (tp) //return 값이 0이면 레코드가 존재하지 않음.
            {
                printf("탐색성공. Name: %s, 길이: %d\n", tp->rec[i].name, tp->rec[i].nleng);
            }
            else
            {
                printf("탐색요청 받은 키를 가진 레코드가 존재하지 않음.\n");
            }
        }
        else if (line[0] == 'P' || line[0] == 'p') // 프린트 명령 수행. 모든 레코드를 키 순서로 출력.
        {
            seq_scan_btree(root); //전체 레코드 값 출력.
        }
        else
        {
            printf("input error\n");
        } //while
    }
}


void insert_btree() //파일전체의 레코드를 삽입한다. insert_arec을 호출
{
    FILE* fp;
    ele data;

    char line[200];
    char* ret_fgets;
    int lineleng;
    int check, count = 0, r = 0;

    fp = fopen("Com_names1.txt", "r"); //첫번째 파일을 불러옴
    if (fp == NULL) //파일이 없을 때
    {
        printf("파일을 불러올 수 없습니다. : Com_names1.txt\n");
        scanf("%d", &check);
    }

    root = NULL;

    while (1)
    {
        ret_fgets = fgets(line, 200, fp);
        if (!ret_fgets)
        {
            break; //파일을 모두 읽음
        }

        lineleng = strlen(line); // line 의 마지막 글자는 newline 글자임.
        if (lineleng - 1 >= 100)
        {
            continue; // 회사명이 너무 길어서 무시
        }
        line[lineleng - 1] = '\0'; //  마지막 newline 글자 제거.

        strcpy(data.name, line);
        data.nleng = strlen(line);

        top = -1;
        r = insert_arec(data);  // 한 레코드를 비트리에 삽입한다.
        if (r != 0)
        {
            count++; // 삽입성공 카운트 증가.
        }
    }// while문

    fp = fopen("Com_names2.txt", "r"); // 두번째 파일을 불러옴
    if (fp == NULL) //파일이 없을때
    {
        printf("파일을 불러올 수 없습니다 : Com_names2.txt\n");
        scanf("%d", &check);
    }

    while (1)
    {
        ret_fgets = fgets(line, 200, fp);
        if (!ret_fgets)
        {
            break; // 파일 모두 읽음.
        }
        lineleng = strlen(line);
        if (lineleng - 1 >= 100)
        {
            continue; // 회사명이 너무 길어서 무시
        }
        line[lineleng - 1] = '\0';

        strcpy(data.name, line); // 삽입할 레코드 준비.
        data.nleng = strlen(line);

        top = -1;
        r = insert_arec(data); // 한 레코드를 비트리에 삽입한다.
        if (r != 0)
        {
            count++;  // 삽입성공 카운트 증가.
        }
    }
    printf("삽입 성공한 레코드 수 = %d", count);
    fclose(fp);
}

// 레코드 하나를 삽입하는 함수이다.
// 반환값: 0: 삽입실패,  1, 2: 삽입성공 (1: 층 증가 없이, 2:한 층 더 늘어 남.)

int insert_arec(ele in_rec) //하나의 레코드를 삽입  key = 회사명
{
    int i, j;
    nodeptr curr, child, new_ptr, tptr = NULL;
    big_node bnode;

    if (root == NULL) // root가 NULL이면 btree가 비어있음. 맨 첫 노드를 만들어 여기에 넣는다.
    {
        root = (nodeptr)malloc(sizeof(node)); //메모리 할당 해줘야 함.
        root->rec[0] = in_rec;
        root->ptr[0] = NULL;
        root->ptr[1] = NULL;
        root->fill_cnt = 1; // 레코드 수

        return 1; // 삽입 성공
    }

    curr = root; // root를 curr에 대입시킴.

    //아래 빈 곳은 in_rec 이 들어가면 좋을 리프노드를 찾아 curr가 가리키게 하는 부분이 와야 함!!
    do
    {
        for (i = 0; i < curr->fill_cnt; i++)
        {
            // curr 노드를 조사하여 내려갈 포인터를 찾는다. 만약 in_key와 동일한 키의 레코드가 존재하면 함수를 종료한다.
            if (strcmp(in_rec.name, curr->rec[i].name) < 0)
            {
                break; //while문 루프 탈출
            }
            else if (strcmp(in_rec.name, curr->rec[i].name) == 0)
            {
                printf("동일키 이미 존재로 안 넣음: %s\n", in_rec.name); // 동일한 레코드면 함수 바로 종료.
                return 0;
            }
        }
        child = curr->ptr[i]; // child = 내려갈 포인터 ;

        if (child != NULL) //내려갈 포인터가 있다면
        {
            push(curr); //curr를 push 함수에 넣어준다.
            curr = child; // curr의 위치를 바꿔줌.
        }
        else  //내려갈 포인터가 없다면(리프노드에 도달함)
        {
            break; //while문 루프 종류
        }
    } while (1);

    child = NULL; //child 값 초기화 해줘야 함.
    
    do
    {
        if (curr->fill_cnt < MAXK) //curr node가 full인 상태가 아님!
        {
            for (i = 0; i < curr->fill_cnt; i++)
            {
                if (strcmp(in_rec.name, curr->rec[i].name) < 0)
                {
                    break; //입력해야되는 레코드 위치를 찾음.
                }
            }
            for (j = curr->fill_cnt; j > i; j--)
            {
                curr->ptr[j + 1] = curr->ptr[j];
                curr->rec[j] = curr->rec[j - 1];
            } // i 이후의 모든 레코드를 우측으로 한 칸씩 이동한다.

            curr->rec[i] = in_rec;
            curr->ptr[i + 1] = child;  //child! 꼭 적어줘야함!!! bnode로 갈 경우 child값으로 넣어줘야 오류가 안생김.
            curr->fill_cnt++; //레코드 수 증가

            return 1; // 삽입성공 (종류 1: 루트의 추가 없이 가능함).
        }
        
        else // curr node가 full인 상태
        {
            for (i = 0; i < MAXK; i++)
            {
                if (strcmp(in_rec.name, curr->rec[i].name) < 0)
                {
                    break; //입력해야되는 레코드 위치를 찾음.
                }
            }
            
            //curr node가 full이므로 bignode를 준비해야함.
            bnode.ptr[0] = curr->ptr[0]; //curr의 첫번째 노드를 bnode 첫번째 노드를 가르킨다.
            for (j = 0; j < i; j++)
            {
                bnode.rec[j] = curr->rec[j]; //현재 ptr[0]을 bnode.ptr[0]에 넣고
                bnode.ptr[j + 1] = curr->ptr[j + 1]; //bnode를 MAXK까지 레코드와 포인터를 넣음.
            }

            bnode.rec[j] = in_rec;
            bnode.ptr[j + 1] = child; // child 값을 넣어줘야함. 처음에는 NULL값
            j++;

            while (i < MAXK) //MAXK가 i보다 작을때
            {
                bnode.rec[j] = curr->rec[i]; //curr의 레코드를 bnode에 넣음
                bnode.ptr[j + 1] = curr->ptr[i + 1];

                j++;
                i++;
            }
            //아래 빈 곳은 big node 를 3 부분으로 나누어 전반부는 curr 에, 가운데 레코드는 in_rec에,
            //   후반부는 새 노드에 넣고, child가 이 새 노드를 가리키게 하는 부분이 와야 함!!


            for (i = 0; i < HALFK; i++) //빅노드의 좌측 절반을 curr노드에 넣는다.
            {
                curr->ptr[i] = bnode.ptr[i]; //빅노드의 앞쪽의 반을 현재 노드에 넣는다
                curr->rec[i] = bnode.rec[i];
            }
            curr->fill_cnt = HALFK;
            curr->ptr[i] = bnode.ptr[i]; //bnode는 curr[i]를 가르킨다.
            curr->ptr[i + 1] = NULL;

            new_ptr = (nodeptr)malloc(sizeof(node)); //새로운 노드 메모리 할당 시켜줌/

            for (i = 0; i < HALFK; i++) //빅노드의 후반부르르 채워 넣어줌.
            {
                new_ptr->ptr[i] = bnode.ptr[i + 1 + HALFK];
                new_ptr->rec[i] = bnode.rec[i + 1 + HALFK];
            }
            new_ptr->ptr[i] = bnode.ptr[MAXK+1]; //bnode의 끝을 새노드에 이음.
            new_ptr->fill_cnt = HALFK;

            //부모로 올릴 쌍을 준비한다.
            in_rec = bnode.rec[HALFK]; //bnode의 가운데 레코드를 in_rec에 넣음
            child = new_ptr; //tptr을 child에 넣음.

            if (top >= 0)
            {
                curr = pop(); // curr을 하나씩 pop하여 위치를 바꿔줌.
            }
            else
            {// 스택이 empty 임 (즉 curr 는 root 노드임.) 새 root 노드를 만들어 curr 의 부모로 함.
                tptr = (nodeptr)malloc(sizeof(node)); //새로운 메모리 할당해줌.
                tptr->rec[0] = in_rec;
                tptr->ptr[0] = curr; //curr에 연결시킴.
                tptr->ptr[1] = child;
                tptr->fill_cnt = 1; // 레코드값은 1이다.
                root = tptr; //root는 새로 할당해준 node(tptr)로 변경됨.
                return 2; // 삽입 성공 (종류 2: 새 루트가 생김)
            }
        }
    } while (1);

    return 0;
}

int seq_scan_btree(nodeptr curr) //모든 레코드 값 출력
{
    int i, n;

    if (curr)
    {
        n = curr->fill_cnt; //
        for (i = 0; i < n; i++)
        {
            seq_scan_btree(curr->ptr[i]); //내려갈 노드를 넣어줌.
            printf("Name: %s\n", curr->rec[i].name);  //레코드 값 출력
        }
        seq_scan_btree(curr->ptr[i]); //내려갈 노드를 넣어줌.
    }
    else if (!curr)
    {
        return 0;
    }

    return 0;
} //재귀함수를 이용하여 출력함.

//*********** stack 관련 함수들 ********************

void push(nodeptr node)
{
    if (top > MAX)
    {
        printf("stack is full\n");
        return; //원래 함수로 되돌아감.
    }
    top++; // top값 증가시켜줌
    stack[top] = node;
}

nodeptr pop()
{
    int temp = 0;
    if (top < 0)
    {
        printf("stack is empty.\n");
        return 0;
    }

    temp = top;
    top--;
    return stack[temp]; //이전 스택
}

nodeptr retrieve(char* skey, int* idx_found) //탐색하는 함수
{
    nodeptr curr = root; //root를 curr로 대입해줌
    nodeptr P;
    int i;
    do
    {
        for (i = 0; i < curr->fill_cnt; i++)
        {
            if (strcmp(skey, curr->rec[i].name) < 0)
            {
                break;
            }
            else if (strcmp(skey, curr->rec[i].name) == 0)
            {
                *idx_found = i;
                return curr; //찾은 위치 return 해줌.
            }
            else
            {
                ; // 그대로 건너뜀
            }
        }
        P = curr->ptr[i]; //P는 다음 노드로 내려갈 노드임
        if (P)
        {
            curr = P;
        }
    } while (P); //NULL이 아닐때까지

    return NULL;// 찾는 값이 없음. 탐색 종료
}

void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char wcase, int j)
{
    int i, k ,m, n, h;
    two_Bnode twoB;   // bnode의 2배의 공간
    
    if(wcase == 'L')
    {
        j = j - 1;  //l_sibing 과 curr(=r_sibling) 사이의 레코드의 index.
    }
    else if(wcase == 'R')
    {
        j = j;      // curr(l_sibling)과 r_sibling 사이의 레코드의 index.
    }
    
    for(i = 0; i < l_sibling->fill_cnt; i++) //l_sibling의 내용을 twoB로 복사한다.
    {
        twoB.ptr[i] = l_sibling->ptr[i];
        twoB.rec[i] = l_sibling->rec[i];
    }
    twoB.ptr[i] = l_sibling->ptr[i];
    
    twoB.rec[i] = father->rec[j]; // 부모에서의 중간키를 가져옴.
    i++;        //다음 레코드 번호가 되게 함.
    
    for(k = 0; k <r_sibling->fill_cnt; k++, i++)
    {
        twoB.ptr[i] = r_sibling->ptr[k];
        twoB.rec[i] = r_sibling->rec[k];
    }
    twoB.ptr[i] = r_sibling->ptr[k]; //i는 twoB 노드의 레코드 수와 같음.
    
    h = i / 2; //h 는 중간 레코드의 인덱스이다.
    
    for(n = 0; n < h; n++) //절반을 l_sibling에 가져온다.
    {
        l_sibling->ptr[n] = twoB.ptr[n];
        l_sibling->rec[n] = twoB.rec[n];
    }
    
    l_sibling->ptr[n] = twoB.ptr[n];
    l_sibling->fill_cnt = h;
    n++;
    
    for(m = 0; m <(i - h - 1); m++, n++) // 후반부를 r_sibling에 가져옴
    {
        r_sibling->ptr[m] = twoB.ptr[n];
        r_sibling->rec[m] = twoB.rec[n];
    }
    
    r_sibling->ptr[m] = twoB.ptr[n];
    r_sibling->fill_cnt = i - h - 1;
    
    //가운데 값은 아버지 노드가 된다.
    father->rec[j] = twoB.rec[h];
    
}

int B_tree_deletion(char *out_key)
{
    nodeptr curr, r_sibling, l_sibling, father, Pt, leftptr, rightptr;
    
    int i, j ,k, r_OK, l_OK, found = 0;
    int finished = 0;
    
    curr = root;
    
    do //삭제할 레코드 위치 찾기.
    {
        for(i = 0; i<curr->fill_cnt; i++)
        {
            if(strcmp(out_key, curr->rec[i].name) < 0)
            {
                break; //for문 루프 종료
            }
            else if(strcmp(out_key, curr->rec[i].name) == 0)
            {
                found = 1;
                break; //찾으면 for문 루프를 종료함. 또한 found 변수에 1을 저장함.
            }
        }
        if(found == 1)
            break; //주의: 변수 i에 찾은 위치가 들어 있음.
        
        else //curr에 없다. child로 내려가야 함.
        {
            Pt = curr->ptr[i]; //Pt는 내려갈 노드
            if(Pt)
            {
                push(curr); //curr를 push함수를 이용하여 stack에 넣어줌.
                curr = Pt;
            }
            else //더이상 내려갈 노드가 없음.
            {
                break;
            }
        }
    }while(!found);
    
    
    if(!found)
    {
        return 0; //없으면 함수를 종료한다.
    }
    // Comes here when the key is found. It is in curr's node. i has index of rec to delete.
    // Step (1):  find successor of d_rec.
    
    if(curr->ptr[0]) //nonleaf노드를 가리킨다면 수행하야 함.
    {
        Pt = curr->ptr[i+1];
        push(curr);
        
        while(Pt->ptr[0])
        {
            push(Pt); // push함수에 Pt를 넣어준다.
            Pt = Pt->ptr[0]; //가장 왼쪽 포인터를 따라내려 간다.
        }
        
        curr->rec[i] = Pt->rec[0]; //successor는 rec[0]에 있음.
        curr = Pt; //curr가 successor를 가진 리프노드를 가리키게 한다.
        i = 0; //지울 레코드의 위치를 변수 i가 가리키게 한다.
    }
    
    finished = false;
    
    do// Step (2):
        //Remove record of index i and a pointer to its right from curr's node;
    {
        for(j = i + 1; j < curr->fill_cnt; j++) // i보다 우측의 것들을 한 칸씩 좌측으로 이동
        {
            curr->rec[j-1] = curr->rec[j];
            curr->ptr[j] = curr->ptr[j+1];
        }
        curr->fill_cnt = curr->fill_cnt - 1;
        
        if(curr == root)
        {
            if(curr->fill_cnt == 0)
            {
                root = root->ptr[0];
                free(curr);
            }
            return 1; //삭제 성공으로 알고리즘을 종료함
        }
        
        // Step (4):
        // curr is not the root.
        if(curr->fill_cnt >= HALFK) // 반 이상 채워져 있다면 바로 종료한다.
        {
            return 2; //2값을 리턴한다.
        }
        
        // 여기에 오면 재분배가 아니면 합병을 해야한다.
        //이를 위한 준비작업이 먼저 오고 재분배 아니면 합병을 한다.
        
        father = pop(); //curr의 부모노드를 가져온다.
        
        for(j = 0; j <= father->fill_cnt; j++)
        {
            if(father->ptr[j] == curr)
            {
                break;
            }
        }
        
        if(j >= 1) // 레코드 수가 반 이상 있을 때 왼쪽 형제를 지정해야 함.
        {
            l_sibling = father->ptr[j - 1]; //왼쪽 형제 지정
        }
        else
        {
            l_sibling = NULL;
        }
        
        if(j < father->fill_cnt)
        {
            r_sibling = father->ptr[j + 1]; //오른쪽 형제 지정
        }
        else
        {
            r_sibling = NULL;
        }
        
        r_OK = 0;
        l_OK = 0;
        
        // r_sibling or l_sibling 중 하나가 d 보다 많은 레코드를 가지면 재분배 가능
        if(r_sibling && r_sibling->fill_cnt > HALFK) //오른쪽 형제가 존재하고 오른쪽 형재의 fill_cnt가 HALFK보다 커야함.
        {
            r_OK = 1;
        }
        else if(l_sibling && l_sibling->fill_cnt > HALFK) //왼쪽 형제가 존재하고 왼쪽 형재의 fill_cnt가 HALFK보다 커야함.
        {
            l_OK = 1;
        }
        
        if(r_OK || l_OK) // 재분배가 가능함
        {
            if(r_OK) // 우측 형제로 부터 재분배를 받는다
            {
                redistribution(father, curr, r_sibling, 'R', j);
            }
            else if(l_OK) // 좌측 형제로부터 재분배를 받는다.
            {
                redistribution(father, l_sibling, curr, 'L',j);
            }
            printf("Redistribution has been done.\n");
            return 3; //노드 제거가 성공함.
        }
        else //  Step 6: merging(합병)
        {
            if(r_sibling)
            {
                leftptr = curr;
                rightptr = r_sibling; // r_sibling을 rightptr로 바꿔준다
            }
            else
            {
                leftptr = l_sibling;
                rightptr = curr; // curr을 rightptr로 바꿔줌.
            }
            
            for (i = 0; i < father->fill_cnt; i++)
            {
               if (father->ptr[i] == leftptr)
               {
                   break;
               }
            }
            
            j = leftptr->fill_cnt;
            leftptr->rec[j] = father->rec[i];
            
            j++;
            // 오른쪽 ptr 노드를 왼쪽 ptr노드로 가져온다.
            for(k = 0; k < rightptr->fill_cnt; k++, j++)
            {
                leftptr->ptr[j] = rightptr->ptr[k];
                leftptr->rec[j] = rightptr->rec[k];
            }
            leftptr->ptr[j] = rightptr->ptr[k];
            leftptr->fill_cnt += 1 + rightptr->fill_cnt;
            
            free(rightptr); //free the right sibling node.
            printf("Merging has been done.\n");
            
            curr = father; //curr를 부모로 올림. i는 부모 즉 curr에서 삭제할 레코드의 인덱스를 가짐.
        }
        
    }while(1);
}
