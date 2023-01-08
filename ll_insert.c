#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
} LinkedList;

void add(LinkedList* list, int data) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        return;
    }

    Node* current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

int main() {
    LinkedList list;
    list.head = NULL;

    add(&list, 10);
    add(&list, 20);
    add(&list, 30);
    add(&list, 40);

    Node* current = list.head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");

    return 0;
}
