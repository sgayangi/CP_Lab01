#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_THREADS 1024
#define UPPER_LIMIT_RANDOM 65535

// Global variables used to store how many times each operation was done
int member_operations_count = 0;
int insert_operations_count = 0;
int delete_operations_count = 0;

int n = 0;
int m = 0;
int thread_count = 0;
float m_insert_fraction = 0.0, m_delete_fraction = 0.0, m_member_fraction = 0.0;

// Total number of each operation
float m_insert = 0.0, m_delete = 0.0, m_member = 0.0;

struct list_node_s *head = NULL;
pthread_mutex_t mutex;

// Node definition
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
    {
        return 1;
    }
}

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

// parallel routine for the threads
void *parallel_Routine()
{

    int total_operations = 0;

    // acts as boolean values for whether each operation has been done the specified number of times.
    int member_ops_done = 0;
    int insert_ops_done = 0;
    int delete_ops_done = 0;

    while (total_operations < m)
    {
        // Random number selected for the operation
        int random_num = rand() % UPPER_LIMIT_RANDOM;

        // selects the operation to be performed: 0 - insert, 1 - delete, 2 - member
        int operation_type = rand() % 3;

        if (operation_type == 0 && insert_ops_done == 0)
        {

            pthread_mutex_lock(&mutex);
            if (insert_operations_count < m_insert)
            {
                Insert(random_num, &head);
                insert_operations_count++;
            }
            else
                insert_ops_done = 1;
            pthread_mutex_unlock(&mutex);
        }

        // Delete Operation
        else if (operation_type == 1 && delete_ops_done == 0)
        {

            pthread_mutex_lock(&mutex);
            if (delete_operations_count < m_delete)
            {
                Delete(random_num, &head);
                delete_operations_count++;
            }
            else
                delete_ops_done = 1;
            pthread_mutex_unlock(&mutex);
        }

        // Member operation
        if (operation_type == 2 && member_ops_done == 0)
        {
            pthread_mutex_lock(&mutex);
            if (member_operations_count < m_member)
            {
                Member(random_num, head);
                member_operations_count++;
            }
            else
                member_ops_done = 1;
            pthread_mutex_unlock(&mutex);
        }

        // Updating the count
        total_operations = insert_operations_count + member_operations_count + delete_operations_count;
    }
    return NULL;
}

// Get the execution time to perform the m random operations
double getExecutionTime(struct timeval start, struct timeval end)
{
    return (double)(end.tv_sec - start.tv_sec) * 1000 + (double)(end.tv_usec - start.tv_usec) / 1000;
}

int main(int argc, char *argv[])
{

    // extracting inputs from args
    n = (int)strtol(argv[1], (char **)NULL, 10);
    m = (int)strtol(argv[2], (char **)NULL, 10);
    thread_count = (int)strtol(argv[3], (char **)NULL, 10);

    m_member_fraction = (float)atof(argv[4]);
    m_insert_fraction = (float)atof(argv[5]);
    m_delete_fraction = (float)atof(argv[6]);

    pthread_t *thread_handlers;
    thread_handlers = malloc(sizeof(pthread_t) * thread_count);

    struct timeval start, end;

    m_insert = m_insert_fraction * m;
    m_delete = m_delete_fraction * m;
    m_member = m_member_fraction * m;

    int i = 0;

    // Insert n random numbers from 0 to 65,535 to the linked list
    while (i < n)
    {
        int random_no = rand() % UPPER_LIMIT_RANDOM;
        if (Insert(random_no, &head) == 1)
            i++;
    }

    pthread_mutex_init(&mutex, NULL);
    gettimeofday(&start, NULL);

    i = 0;
    while (i < thread_count)
    {
        pthread_create(&thread_handlers[i], NULL, (void *)parallel_Routine, NULL);
        i++;
    }

    i = 0;
    while (i < thread_count)
    {
        pthread_join(thread_handlers[i], NULL);
        i++;
    }

    gettimeofday(&end, NULL);

    pthread_mutex_destroy(&mutex);

    printf("Time taken for linked list with single mutex: %.4f milliseconds\n", getExecutionTime(start, end));

    return 0;
}