#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

struct Node {
    int data;
    struct Node *next;
};

struct Job {
    int arrival_time;
    int cpu_burst;
    int start;
    int end;
    int turnaround;
    int response;
    int remaining_time;
    bool inQueue;
    int serial;
    int alloted_time;
    int alloted_left;
};

void printList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

void clearQueue(struct Node **head, struct Node **tail, struct Node **head2, struct Node **tail2, int *reset) {
    *reset = 100;
    if (*head2 == NULL) return;

    if (*head == NULL) {
        *head = *head2;
        *tail = *tail2;
    } else {
        (*tail)->next = *head2;
        *tail = *tail2;
    }

    *head2 = *tail2 = NULL;
}


void enqueueReset(struct Job jobs[], struct Node **head, struct Node **tail, int len) {
    for(int i=0; i < len; i++) {
        if (jobs[i].inQueue && jobs[i].remaining_time > 0) {
            struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
            newNode->data = i;
            newNode->next = NULL;

            if(!(*head)) {
                *head = *tail = newNode;
            } else {
                (*tail)->next = newNode;
                *tail = newNode;
            }
        }
    }
}

void enqueueFirst(struct Job jobs[], struct Node **head, struct Node **tail, int len, int current_time) {
    for (int i = 0; i < len; i++) {
        if (!jobs[i].inQueue && jobs[i].remaining_time > 0 && jobs[i].arrival_time <= current_time && jobs[i].response == -1) {
            struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
            newNode->data = i;
            newNode->next = NULL;

            if (!(*head)) {
                *head = *tail = newNode;
            } else {
                (*tail)->next = newNode;
                *tail = newNode;
            }

            jobs[i].inQueue = true;
        }
    }
}


void initStruct(struct Job *j, int s, int a, int c, int r, int aa)
{
    j->arrival_time = a;
    j->cpu_burst = c;
    j->response = r;
    j->serial = s;
    j->remaining_time = c;
    j->alloted_time = aa;
    j->alloted_left = aa;
}

void printStruct(struct Job jobs[], int len)
{
    printf("---------------------------------------------------------------\n");
    printf("| Job | Arrival | Burst | Start | End | Turnaround | Response |\n");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < len; i++)
    {
        printf("| %3d | %7d | %5d | %5d | %3d | %10d | %8d |\n",
               i + 1,
               jobs[i].arrival_time,
               jobs[i].cpu_burst,
               jobs[i].start,
               jobs[i].end,
               jobs[i].turnaround,
               jobs[i].response);
    }
    printf("---------------------------------------------------------------\n");  
}

void degradeQueue(struct Job *j, struct Node **head, struct Node **tail, struct Node **head2, struct Node **tail2) {
    struct Node *temp = *head;
    struct Node *prev = NULL;

    while (temp != NULL) {
        struct Node *nextNode = temp->next;
        struct Job *currentJob = &j[temp->data];

        if (currentJob->alloted_left == 0) {
            if (prev == NULL)
                *head = nextNode;
            else
                prev->next = nextNode;

            if (temp == *tail)
                *tail = prev;

            temp->next = NULL;
            if (*head2 == NULL) {
                *head2 = *tail2 = temp;
            } else {
                (*tail2)->next = temp;
                *tail2 = temp;
            }

            currentJob->alloted_left = currentJob->alloted_time;
        } else {
            prev = temp; 
        }

        temp = nextNode;
    }
}

bool checkEmpty(struct Job jobs[], int len) {
    bool isEmpty = true;
    for (int i = 0; i < len; i++) {
        if(jobs[i].response == -1) isEmpty = false;
    }
    return isEmpty;
}

//######### Multi-level Feedback Queue ########### //

// faulty logic... breaks after first one

void MLFQ(struct Job jobs[], int len) {
    struct Node *head = NULL, *tail = NULL;
    struct Node *head2 = NULL, *tail2 = NULL;

    bool isQ1Empty;
    bool isQ2Empty;

    int current_time = 0;
    int reset_timer = 100;
    int remaining_jobs = len;

    while(remaining_jobs > 0) {
        // Enqueue all running jobs to highest priority Queue on timer reset
        if(reset_timer == 0) {
            printf("Resetting\n");
            // clearQueue(&head, &tail, &head2, &tail2);
            printList(head);
            enqueueReset(jobs, &head, &tail, len);
            reset_timer = 100;
        } else {
            enqueueFirst(jobs, &head, &tail, len, current_time);

            // Check alloted time for each in queues and degrade them if time's up 
            degradeQueue(jobs, &head, &tail, &head2, &tail2);

            printf("First List: \n");
            printList(head);
            printf("Second List: \n");
            printList(head2);

            isQ1Empty = checkEmpty(head);
            isQ2Empty = checkEmpty(head2);

            if (isQ1Empty && isQ2Empty) {
                current_time+=1;
                reset_timer-=1;
                continue;
            }

            struct Node **activeHead, **activeTail;
            struct Node *curNode;
            struct Job *curJob;

            if (!isQ1Empty) {
                activeHead = &head;
                activeTail = &tail;
            } else {
                activeHead = &head2;
                activeTail = &tail2;
            }

            curNode = *activeHead;
            curJob = &jobs[curNode->data];

            // Move head forward
            *activeHead = curNode->next;
            curNode->next = NULL;

            if(curJob->response == -1) {
                curJob->start = current_time;
                curJob->response = curJob->start - curJob->arrival_time;
            }

            current_time++;
            curJob->remaining_time--;
            curJob->alloted_left--;
            reset_timer--;


            if (curJob->remaining_time <= 0) {
            // finished
                curJob->end = current_time;
                curJob->turnaround = curJob->end - curJob->arrival_time;
                curJob->inQueue = false;
                remaining_jobs--;
                free(curNode);
            } 
            else if (curJob->alloted_left <= 0) {
                // degrade to next queue
                break;
            } 
            else {
                // put back to end of same queue
                if (*activeTail == NULL) {
                    *activeHead = *activeTail = curNode;
                } else {
                    (*activeTail)->next = curNode;
                    *activeTail = curNode;
                }
            }
            printf("20 units over inside if and else block");
        }
    }
}

// Failed Attempt to better algorithm... too complex dueto multiple loops
void MLFQ2(struct Job jobs[], int len) {
    struct Node *head = NULL, *tail = NULL;
    struct Node *head2 = NULL, *tail2 = NULL;

    int current_time = 0;
    int quantum_time = 20;
    int reset_timer = 100;
    int remaining_jobs = len;
    int *reset;
    bool isBreak = false;

    while (remaining_jobs > 0) {
        if (reset_timer <= 0) {
            reset = &reset_timer;
            printf("Resetting\n");
            clearQueue(&head, &tail, &head2, &tail2, reset);
            enqueueReset(jobs, &head, &tail, len);
        } else {
            enqueueFirst(jobs, &head, &tail, len, current_time);

            if (head == NULL && head2 == NULL) {
                current_time++;
                reset_timer--;
            }

            struct Node *current_node;
            struct Job *current_job;
            bool isBreak = false;


            while (head != NULL) {
                printf("\n[DEBUG] --- Starting top of outer while (head queue) ---\n");
                printList(head);

                enqueueFirst(jobs, &head, &tail, len, current_time);

                if(head != NULL && head->next != NULL && jobs[tail->data].response == -1) {
                    struct Node *temp;
                    temp = head;
                    head = head->next;
                    temp->next = NULL;
                    tail->next = temp;
                    tail = temp;
                }

                current_node = head; 
                current_job = &jobs[current_node->data];

                if (current_job->response == -1) {
                    current_job->start = current_time;
                    current_job->response = current_job->start - current_job->arrival_time;
                }

                quantum_time = 20;

                printf("[DEBUG] Entering inner while for Job ID %d with quantum_time = %d\n",
                       current_node->data, quantum_time);

                while (quantum_time > 0) {
                    current_time++;
                    reset_timer--;
                    quantum_time--;
                    current_job->alloted_left--;
                    current_job->remaining_time--;

                    if (current_job->alloted_left == 0) {
                        isBreak = true;
                        printf("\n[DEBUG] Job %d used up its allotted time slice. Moving to lower queue.\n",
                               current_node->data);
                        printList(head);
                        printf("[DEBUG] Current lower queue (before move):\n");
                        printList(head2);
                        printf("\n");

                        current_job->alloted_left = current_job->alloted_time;

                        if (head->next != NULL)
                            head = head->next;
                        else
                            head = NULL, tail = NULL;

                        if (tail2 != NULL) {
                            tail2->next = current_node;
                            tail2 = current_node;
                        } else {
                            head2 = tail2 = current_node;
                        }

                        current_node->next = NULL;

                        printf("[DEBUG] Job %d moved successfully to lower queue.\n", current_node->data);
                        printf("[DEBUG] Updated high-priority queue:\n");
                        printList(head);
                        printf("[DEBUG] Updated lower-priority queue:\n");
                        printList(head2);
                        printf("\n");
                        break;
                    } else if (current_job->remaining_time == 0) {
                        isBreak = true;
                        if(head->next != NULL) head = head->next;
                        else head = NULL, tail = NULL;
                        current_job->end = current_time;
                        current_job->turnaround = current_job->end - current_job->arrival_time;
                        current_job->inQueue = false;
                        remaining_jobs--;
                        current_node->next = NULL;
                        free(current_node);
                        break;
                    } else {
                        printf("[DEBUG] Job %d still running... quantum_time left: %d\n",
                               current_node->data, quantum_time);
                    }
                }
                if(isBreak) break;
                if (current_node == head || head == NULL) break;

                printf("[DEBUG] End of inner while for Job ID %d\n", current_node->data);
                printList(head);

                current_node->next = NULL;
                tail->next = current_node;
                tail = current_node;

                printf("[DEBUG] Job %d moved to end of same queue (Round Robin rotation)\n",
                       current_node->data);
                printf("[DEBUG] Updated queue after rotation:\n");
                printList(head);
            }

            printf("[DEBUG] End of head queue processing.\n");

            while (head == NULL && head2 != NULL) {
                printf("\n[DEBUG] --- Starting top of outer while (head2 queue) ---\n");
                printList(head2);

                enqueueFirst(jobs, &head, &tail, len, current_time);

                if(head2 != NULL && head2->next != NULL && jobs[tail2->data].response == -1) {
                    struct Node *temp;
                    temp = head2;
                    head2 = head2->next;
                    temp->next = NULL;
                    tail2->next = temp;
                    tail2 = temp;
                }
                current_node = head2;
                current_job = &jobs[current_node->data];

                if (current_job->response == -1) {
                    current_job->start = current_time;
                    current_job->response = current_job->start - current_job->arrival_time;
                }

                quantum_time = 20;

                printf("[DEBUG] Entering inner while for Job ID %d with quantum_time = %d in lower priority queue\n",
                       current_node->data, quantum_time);

                while (quantum_time > 0) {
                    current_time++; 
                    reset_timer--;
                    quantum_time--;
                    current_job->alloted_left--;
                    current_job->remaining_time--;

                    if (reset_timer <= 0) {
                        isBreak = true;
                        reset = &reset_timer;
                        printf("Resetting\n");
                        clearQueue(&head, &tail, &head2, &tail2, reset);
                        enqueueReset(jobs, &head, &tail, len);
                        break;
                    }

                    if (current_job->alloted_left <= 0) {
                        isBreak = true;
                        printf("\n[DEBUG] Job %d used up its allotted time slice. Moving to lower queue.\n",
                               current_node->data);
                        printList(head);
                        printf("[DEBUG] Current lower queue (before move):\n");
                        printList(head2);
                        printf("\n");

                        current_job->alloted_left = current_job->alloted_time;

                        if (head2->next != NULL)
                            head2 = head2->next;
                        else
                            head2 = NULL, tail2 = NULL;

                        if (tail2 != NULL) {
                            tail2->next = current_node;
                            tail2 = current_node;
                        } else {            
                            head2 = tail2 = current_node;
                        }

                        current_node->next = NULL;

                        printf("[DEBUG] Job %d moved successfully to lower queue.\n", current_node->data);
                        printf("[DEBUG] Updated high-priority queue:\n");
                        printList(head);
                        printf("[DEBUG] Updated lower-priority queue:\n");
                        printList(head2);
                        printf("\n");
                        break;
                    } else if (current_job->remaining_time <= 0) {
                        isBreak = true;
                        if(head2->next != NULL) head2 = head2->next;
                        else head2 = NULL, tail2 = NULL;
                        current_job->end = current_time;
                        current_job->turnaround = current_job->end - current_job->arrival_time;
                        current_job->inQueue = false;
                        remaining_jobs--;
                        current_node->next = NULL;
                        free(current_node);
                        break;
                    } else {
                        printf("[DEBUG] Job %d still running... quantum_time left: %d with %d time left and %d reset\n",
                               current_node->data, quantum_time, current_job->remaining_time, reset_timer);
                        printf("end");
                    }
                }
                printList(head2);
                if(isBreak) break;
                if (current_node == head2 || head2 == NULL) break;

                printf("[DEBUG] End of inner while for Job ID %d\n", current_node->data);
                printList(head2);

                current_node->next = NULL;
                tail2->next = current_node;
                tail2 = current_node;

                printf("[DEBUG] Job %d moved to end of same queue (Round Robin rotation)\n",
                       current_node->data);
                printf("[DEBUG] Updated queue after rotation:\n");
                printList(head2);
            }
        }
    }
    printf("completed run");
}

void MLFQ3(struct Job jobs[], int len) {
    struct Node *head = NULL, *tail = NULL;
    struct Node *head2 = NULL, *tail2 = NULL;

    int current_time = 0;
    int quantum_time = 20;
    int reset_timer = 100;
    int remaining_jobs = len;
    int *reset;
    bool isBreak = false;

    enqueueFirst(jobs, &head, &tail, len, current_time);

    while(remaining_jobs > 0) {
        if(head == NULL && head2 == NULL) {
            current_time++;
        }

         

        while(quantum_time > 0) {
            current_time++;
            reset_timer--;
            quantum_time--;

            enqueueFirst(jobs, &head, &tail, len, current_time);

            if(reset_timer == 0) {
                printf("Resetting\n");
                clearQueue(&head, &tail, &head2, &tail2, reset);
                printList(head);
                enqueueReset(jobs, &head, &tail, len);
                reset_timer = 100;
            }
        }
    }
}



int main() {
    struct Job jobs[3];
    initStruct(&jobs[0], 1, 0, 300, -1, 100);
    initStruct(&jobs[1], 2, 10, 100, -1, 20);
    initStruct(&jobs[2], 3, 2, 200, -1, 45);

    struct Job jobs2[3];
    initStruct(&jobs2[0], 1, 0, 50, -1, 20);   // Early starter, long burst
    initStruct(&jobs2[1], 2, 5, 25, -1, 15);   // Mid-arrival, medium burst
    initStruct(&jobs2[2], 3, 12, 30, -1, 10);  // Late arrival, small quantum


    // MLFQ2 (jobs, 3);
    MLFQ3(jobs2, 3);

    printf("This is after jobs done!");
    printStruct(jobs2, 3);

    return 0;
}
