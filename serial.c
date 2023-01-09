#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// 2**16 -1 - upper limit of random numbers to be generated
#define UPPER_LIMIT_RANDOM 65535

int n = 0;
int m = 0;
float m_insert_fraction, m_delete_fraction, m_member_fraction;

struct list_node_s
{
    int data;
    struct list_node_s *next;
};

// returns 1 if value is a member of the linked list, 0 otherwise.
int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *current_p = head_p;

    while (current_p != NULL && current_p->data < value)
        current_p = current_p->next;

    if (current_p == NULL || current_p->data > value)
    {
        return 0;
    }
    else
        return 1;
}

//Linked List Insertion function
int Insert(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;

        return 1;
    }
    else
        return 0;
}

//Linked List Deletion function
int Delete(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value)
    {
        if (pred_p == NULL)
        {
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    }
    else
        return 0;
}

// Get the execution time to perform the m random operations
double getExecutionTime(struct timeval start, struct timeval end)
{
    return (double)(end.tv_sec - start.tv_sec) * 1000 + (double)(end.tv_usec - start.tv_usec) / 1000;
}

int main(int argc, char *argv[])
{
    struct list_node_s *head = NULL;
    struct timeval start, end;

    // extracting inputs from args
    n = (int)strtol(argv[1], (char **)NULL, 10);
    m = (int)strtol(argv[2], (char **)NULL, 10);

    m_member_fraction = (float)atof(argv[3]);
    m_insert_fraction = (float)atof(argv[4]);
    m_delete_fraction = (float)atof(argv[5]);

    // Insert n random numbers from 0 to 65,535 to the linked list
    int i = 0;
    while (i < n)
    {
        int random_no = rand() % UPPER_LIMIT_RANDOM;
        if (Insert(random_no, &head) == 1)
            i++;
    }

    int total_operations = 0;
    int member_operations_count = 0;
    int insert_operations_count = 0;
    int delete_operations_count = 0;

    float m_insert = m_insert_fraction * m;
    float m_delete = m_delete_fraction  * m;
    float m_member = m_member_fraction * m;

    gettimeofday(&start, NULL);

    // total operations goes from 0 to m - 1
    while (total_operations < m)
    {

        int random_num = rand() % UPPER_LIMIT_RANDOM;

        // selects the operation to be performed: 0 - insert, 1 - delete, 2 - member
        int operation_type = rand() % 3;

        if (operation_type == 0 && insert_operations_count < m_insert)
        {
            Insert(random_num, &head);
            insert_operations_count++;
        }

        else if (operation_type == 1 && delete_operations_count < m_delete)
        {
            Delete(random_num, &head);
            delete_operations_count++;
        }

        else if(operation_type == 2 && member_operations_count < m_member)
        {
            Member(random_num, head);
            member_operations_count++;
        }

        total_operations = insert_operations_count + member_operations_count + delete_operations_count;
    }
    gettimeofday(&end, NULL);

    printf("Time taken for serial linked list: %.4f milliseconds\n", getExecutionTime(start, end));

    return 0;
}
