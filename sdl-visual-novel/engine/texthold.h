#pragma once

#include <stdlib.h>

#define TEXTHOLD_BKT_SIZE 256
#define TEXTHOLD_MAX_CHARS 1024

// 노드 구조체 선언
struct text_h_s {
    int key; // 해시 함수에 사용될 키
    const char* value; // key 가 가지고 있는 데이터
    struct text_h_s* next; // 다음 노드를 가르키는 포인터
};

typedef struct text_h_s text_h_t;

// 버켓 구조체 선언
typedef struct {
    text_h_t* head; // 버켓 가장 앞에 있는 노드의 포인터
    int count; // 버켓에 들어있는 노드의 개수
} text_bucket_t;

void th_search(int key, text_h_t* out);

void th_display(void);

void th_init(void);

void th_load(void);
