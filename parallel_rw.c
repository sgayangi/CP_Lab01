#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_THREADS 1024
#define UPPER_LIMIT_RANDOM 65535

int n = 0;
int m = 0;
int thread_count = 0;
float m_insert_fraction = 0.0, m_delete_fraction = 0.0, m_member_fraction = 0.0;

// Total number of each operation
int m_insert = 0, m_delete = 0, m_member = 0;

struct list_node_s *head = NULL;
pthread_rwlock_t rwlock;

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
void *parallel_Routine(void *thread_id)
{

    int local_m = 0;
    int local_insert_ops = 0;
    int local_delete_ops = 0;
    int local_member_ops = 0;

    int id = *(int *)thread_id;

    // Divide the total no of insert operations to be done among the threads
    if (m_insert % thread_count <= id)
    {
        local_insert_ops = m_insert / thread_count;
    }
    else
    {
        local_insert_ops = m_insert / thread_count + 1;
    }

    // Divide the total no of delete operations to be done among the threads
    if (m_delete % thread_count <= id)
    {
        local_delete_ops = m_delete / thread_count;
    }
    else
    {
        local_delete_ops = m_delete / thread_count + 1;
    }

    // Divide the total no of member operations to be done among the threads
    if (m_member % thread_count <= id)
    {
        local_member_ops = m_member / thread_count;
    }
    else
    {
        local_member_ops = m_member / thread_count + 1;
    }

    local_m = local_insert_ops + local_delete_ops + local_member_ops;

    int total_operations = 0;
    int member_operations_count = 0;
    int insert_operations_count = 0;
    int delete_operations_count = 0;

    int i = 0;
    while (total_operations < local_m)
    {
        int random_value = rand() % UPPER_LIMIT_RANDOM;
        int operation_type = rand() % 3;

        // selects the operation to be performed: 0 - insert, 1 - delete, 2 - member
        if (operation_type == 0 && insert_operations_count < local_insert_ops)
        {
            pthread_rwlock_wrlock(&rwlock);
            Insert(random_value, &head);
            pthread_rwlock_unlock(&rwlock);
            insert_operations_count++;
        }

        else if (operation_type == 1 && delete_operations_count < local_delete_ops)
        {
            pthread_rwlock_wrlock(&rwlock);
            Delete(random_value, &head);
            pthread_rwlock_unlock(&rwlock);
            delete_operations_count++;
        }

        else if (operation_type == 2 && member_operations_count < local_member_ops)
        {
            pthread_rwlock_rdlock(&rwlock);
            Member(random_value, head);
            pthread_rwlock_unlock(&rwlock);
            member_operations_count++;
        }
        total_operations = insert_operations_count + member_operations_count + delete_operations_count;
        i++;
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
    n = (int)strtol(argv[1], (char **)NULL, 10);
    m = (int)strtol(argv[2], (char **)NULL, 10);
    thread_count = (int)strtol(argv[3], (char **)NULL, 10);

    m_member_fraction = (float)atof(argv[4]);
    m_insert_fraction = (float)atof(argv[5]);
    m_delete_fraction = (float)atof(argv[6]);

    pthread_t *thread_handlers;
    thread_handlers = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);

    // time variables
    struct timeval start, end;

    int *thread_id;
    thread_id = (int *)malloc(sizeof(int) * thread_count);

    m_insert = (int)(m_insert_fraction * m);
    m_delete = (int)(m_delete_fraction * m);
    m_member = (int)(m_member_fraction * m);

    int i = 0;

    // Insert n random numbers from 0 to 65,535 to the linked list
    while (i < n)
    {
        int random_no = rand() % UPPER_LIMIT_RANDOM;
        if (Insert(random_no, &head) == 1)
            i++;
    }

    pthread_rwlock_init(&rwlock, NULL);
    gettimeofday(&start, NULL);

    // Thread Creation
    i = 0;
    while (i < thread_count)
    {
        thread_id[i] = i;
        pthread_create(&thread_handlers[i], NULL, (void *)parallel_Routine, (void *)&thread_id[i]);
        i++;
    }

    // Thread Join
    i = 0;
    while (i < thread_count)
    {
        pthread_join(thread_handlers[i], NULL);
        i++;
    }
    gettimeofday(&end, NULL);
    pthread_rwlock_destroy(&rwlock);

    printf("Linked List with read write locks Time Spent : %.6f\n", getExecutionTime(start, end));

    return 0;
}