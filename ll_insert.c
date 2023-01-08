#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
} LinkedList;

void insert(LinkedList* list, int data, int position) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = data;

    // insert at beginning
    if (position == 0) {
        newNode->next = list->head;
        list->head = newNode;
        return;
    }

    Node* current = list->head;
    int i = 0;
    while (i < position - 1 && current->next != NULL) {
        current = current->next;
        i++;
    }

    newNode->next = current->next;
    current->next = newNode;
}

int main() {
    LinkedList list;
    list.head = NULL;

    insert(&list, 10, 0);
    insert(&list, 20, 1);
    insert(&list, 30, 2);
    insert(&list, 40, 3);

    Node* current = list.head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");

    return 0;
}
