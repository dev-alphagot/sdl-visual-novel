#define _CRT_SECURE_NO_WARNINGS

#include "texthold.h"
#include "character.h"

#include "../util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static text_bucket_t* hashTable = NULL;

static text_h_t* th_create_node(int key, const char* value) {
    text_h_t* newNode;
    // 메모리 할당
    newNode = (text_h_t*)malloc(sizeof(text_h_t));
    // 사용자가 전해준 값을 대입
    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL; // 생성할 때는 next를 NULL로 초기화

    return newNode;
}

static int hashFunction(int key) {
    return key % TEXTHOLD_BKT_SIZE;
}

// 새로 키 추가하는 함수
static void th_add(int key, const char* value) {
    // 어느 버켓에 추가할지 인덱스를 계산
    int hashIndex = hashFunction(key);
    // 새로 노드 생성
    text_h_t* newNode = th_create_node(key, value);
    // 계산한 인덱스의 버켓에 아무 노드도 없을 경우
    if (hashTable[hashIndex].count == 0) {
        hashTable[hashIndex].count = 1;
        hashTable[hashIndex].head = newNode; // head를 교체
    }
    // 버켓에 다른 노드가 있을 경우 한칸씩 밀고 내가 헤드가 된다(실제로는 포인터만 바꿔줌)
    else {
        newNode->next = hashTable[hashIndex].head;
        hashTable[hashIndex].head = newNode;
        hashTable[hashIndex].count++;
    }
}

// 키를 삭제해주는 함수
static void th_remove_key(int key) {
    int hashIndex = hashFunction(key);
    // 삭제가 되었는지 확인하는 flag 선언
    int flag = 0;
    // 키를 찾아줄 iterator 선언
    text_h_t* node = NULL;
    text_h_t* before = NULL; // 노드가 지나간 바로 전 노드
    // 버켓의 head부터 시작
    node = hashTable[hashIndex].head;
    // 버켓을 순회하면서 key를 찾는다.
    while (node != NULL) // NULL 이 나올때까지 순회
    {
        if (node->key == key) {
            // 포인터 바꿔주기 노드가 1 . 헤드인 경우 2 . 헤드가 아닌경우
            if (node == hashTable[hashIndex].head) {
                hashTable[hashIndex].head = node->next; // 다음 노드가 이제 헤드
            }
            else {
                before->next = node->next; // 전노드가 이제 내 다음 노드를 가르킴
            }
            // 나머지 작업 수행
            hashTable[hashIndex].count--;
            free(node);
            flag = 1;
        }
        before = node; // 노드 바꿔주기 전에 저장
        node = node->next;
    }
    // flag의 값에 따라 출력 다르게 해줌
    if (flag == 1) { // 삭제가 되었다면
        printf("\n [ %d ] 이/가 삭제되었습니다. \n", key);
    }
    else { // 키가 없어서 삭제가 안된 경우
        printf("\n [ %d ] 이/가 존재하지 않아 삭제하지 못했습니다.\n", key);
    }
}

// 키를 주면 value를 알려주는 함수
void th_search(int key, text_h_t* out) {
    int hashIndex = hashFunction(key);
    text_h_t* node = hashTable[hashIndex].head;
    int flag = 0;
    while (node != NULL)
    {
        if (node->key == key) {
            flag = 1;
            break;
        }
        node = node->next;
    }
    if (flag == 1) { // 키를 찾았다면
        memcpy(out, node, sizeof(text_h_t));
        // out = node;
        printf(" 키는 [ %d ], 값은 [ %s ] 입니다. \n", node->key, node->value);
    }
    else {
        memcpy(out, calloc(1, sizeof(text_h_t)), sizeof(text_h_t));
        printf(" 존재하지 않는 키는 찾을 수 없습니다. \n");
    }
}

// 해시테이블 전체를 출력해주는 함수
void th_display(void) {
    // 반복자 선언
    text_h_t* iterator;
    printf("\n========= display start ========= \n");
    for (int i = 0; i < TEXTHOLD_BKT_SIZE; i++) {
        iterator = hashTable[i].head;
        printf("Bucket[%d]: ", i);
        while (iterator && ~(long long)iterator < 0x10000000)
        {
            printf("(key: %d, val: %s)  -> ", iterator->key, iterator->value);
            iterator = iterator->next;
        }
        printf("\n");
    }
    printf("\n========= display complete ========= \n");
}

void th_init(void) {
    hashTable = (text_bucket_t*)malloc(TEXTHOLD_BKT_SIZE * sizeof(text_bucket_t));
}

void th_load(void) {
    char nbuf[64] = "";

    for (int i = 0; i < CHAR_CAPACITY; i++) {
        if (!strlen(characters[i].path)) continue;

        sprintf(nbuf, "def/text/%d.csv", characters[i].id);

        FILE* file = fopen(nbuf, "rt");
        char* fbuf = (char*)malloc(65536);

        if (!fbuf || !file) {
            free(fbuf);
			continue;
        }
        printf("%s START\n", nbuf);

        while (!feof(file)) {
            fgets(fbuf, 65536, file);

            int dd = 0;

            for (int j = 0; j < 2; j++) {
                char* st = strtok(!j ? fbuf : NULL, "	");

                str_trim_lf(strlen(st), st);

                if (!j) dd = atoi(st);
                else th_add(characters[i].id * 100000 + dd, _strdup(st));
            }
        }
        printf("%s COMPLETED\n", nbuf);
        free(fbuf);
    }
}
