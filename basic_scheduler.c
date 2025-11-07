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

void clearQueue(struct Node **head, struct Node **tail, struct Node **head2, struct Node **tail2) {
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

bool checkEmpty(struct Node *head) {
    if(head == NULL) {
        return true;
    } 
    return false;
}




// ######## First Come First Serve ######## //


// FCFS Simple but inaccurate, Just parses through the array in order
void fcfs(struct Job jobs[], int len)
{
    int current_time = 0;
    for (int i = 0; i < len; i++)
    {
        if (i == 0)
        {
            jobs[i].start = jobs[i].arrival_time;
        }
        else
        {
            jobs[i].start = current_time;
        }
        jobs[i].end = jobs[i].cpu_burst + jobs[i].start;
        jobs[i].turnaround = jobs[i].end - jobs[i].arrival_time;
        jobs[i].response = jobs[i].start - jobs[i].arrival_time;
        current_time = jobs[i].end;
    }
}

// ##!!!!!##Need to implement quickSort ##### for this to sort in nlog(n) complexity##!!!!!## //
void fcfs2(struct Job jobs[], int len)
{
    int current_time = 0;
    for (int i = 0; i < len; i++)
    {
        if (i == 0)
        {
            jobs[i].start = jobs[i].arrival_time;
        }
        else
        {
            jobs[i].start = current_time;
        }
        jobs[i].end = jobs[i].cpu_burst + jobs[i].start;
        jobs[i].turnaround = jobs[i].end - jobs[i].arrival_time;
        jobs[i].response = jobs[i].start - jobs[i].arrival_time;
        current_time = jobs[i].end;
    }
}



// ######## Non-Preamptive Shortest Job First ######## //


// Ugly manual method(NOT CORRECT) and very inefficient
void sjfNP(struct Job jobs2[], int len)
{
    struct Job sjobs[3];
    struct Job sjobs2[3];
    int count = 0;

    for (int i = 0; i < len; i++)
    {
        sjobs[i].arrival_time = jobs2[i].arrival_time;
        sjobs[i].cpu_burst = jobs2[i].cpu_burst;
    }

    int remaining = len;

    while (remaining > 0)
    {
        int smallest_index = 0;
        int smallest = sjobs[0].cpu_burst;

        for (int i = 1; i < remaining; i++)
        {
            if (sjobs[i].cpu_burst < smallest)
            {
                smallest = sjobs[i].cpu_burst;
                smallest_index = i;
            }
        }

        sjobs2[count] = sjobs[smallest_index];
        count++;

        for (int j = smallest_index; j < remaining - 1; j++)
        {
            sjobs[j] = sjobs[j + 1];
        }

        remaining--;
    }

    for (int i = 0; i < len; i++)
    {
        jobs2[i].arrival_time = sjobs2[i].arrival_time;
        jobs2[i].cpu_burst = sjobs2[i].cpu_burst;
    }
    int current_time = 0;
    for (int i = 0; i < len; i++)
    {
        if (i == 0)
        {
            jobs2[i].start = jobs2[i].arrival_time;
        }
        else
        {
            jobs2[i].start = current_time;
        }
        jobs2[i].end = jobs2[i].cpu_burst + jobs2[i].start;
        jobs2[i].turnaround = jobs2[i].end - jobs2[i].arrival_time;
        jobs2[i].response = jobs2[i].start - jobs2[i].arrival_time;
        current_time = jobs2[i].end;
    }
}

// Direct Manipulation, Fast and efficient for small cases
void sjfNP2(struct Job jobs2[], int len)
{
    int current_time = 0;
    int smallest_index = 0;
    int smallest_burst = jobs2[0].cpu_burst;
    int remainingJobs = len;
    while (remainingJobs > 0)
    {
        smallest_burst = INT_MAX;
        for (int i = 0; i < len; i++)
        {
            if (jobs2[i].response == -1 && jobs2[i].arrival_time <= current_time)
            {
                if (jobs2[i].cpu_burst < smallest_burst)
                {
                    smallest_burst = jobs2[i].cpu_burst;
                    smallest_index = i;
                }
            }
            else
                continue;
        }

        if (remainingJobs == len)
            jobs2[smallest_index].start = jobs2[smallest_index].arrival_time;
        else
            jobs2[smallest_index].start = current_time;
        jobs2[smallest_index].end = jobs2[smallest_index].start + jobs2[smallest_index].cpu_burst;
        jobs2[smallest_index].turnaround = jobs2[smallest_index].end - jobs2[smallest_index].arrival_time;
        jobs2[smallest_index].response = jobs2[smallest_index].start - jobs2[smallest_index].arrival_time;
        current_time = jobs2[smallest_index].end;
        remainingJobs--;
    }
}

// Uses a ready queue, inefficient dueto storing entire struct in readyQueue
void sjfNP3(struct Job jobs2[], int len) {
    struct Job *readyQueue = (struct Job *)malloc(len * sizeof(struct Job));
    struct Job helperStruct;
    int current_time = 0;
    int readyCount = 0;
    int smallest_burst = INT_MAX;
    int smallest_index = 0;
    int remaining_jobs = len;
    while (remaining_jobs > 0) {
        smallest_burst = INT_MAX;
        for (int i = 0; i < len; i++) {
            if (jobs2[i].response == -1 && jobs2[i].inQueue == false && jobs2[i].arrival_time <= current_time) {
                readyQueue[readyCount] = jobs2[i];
                jobs2[i].inQueue = true;
                readyCount++;
            }
        }

        for (int i = 0; i < readyCount; i++) {
            if (readyQueue[i].cpu_burst < smallest_burst) {
                smallest_burst = readyQueue[i].cpu_burst;
                smallest_index = i;
            }
        }

        helperStruct = readyQueue[smallest_index];

        for (int i = 0; i < len; i++) {
            if (helperStruct.serial == jobs2[i].serial) {
                if (remaining_jobs == len)
                    jobs2[i].start = jobs2[i].arrival_time;
                else
                    jobs2[i].start = current_time;
                jobs2[i].end = jobs2[i].start + jobs2[i].cpu_burst;
                jobs2[i].turnaround = jobs2[i].end - jobs2[i].arrival_time;
                jobs2[i].response = jobs2[i].start - jobs2[i].arrival_time;
                current_time = jobs2[i].end;
                remaining_jobs--;
            }
            else
                continue;
        }

        for (int i = smallest_index; i < readyCount - 1; i++)
        {
            readyQueue[i] = readyQueue[i + 1];
        }
        readyCount--;
    }
    free(readyQueue);
}

// Much more efficient, uses index(like a pointer)
void sjfNP4(struct Job jobs2[], int len) {
    int *readyQueue = (int *)malloc(len * sizeof(int));
    int current_time = 0;
    int readyCount = 0;
    int smallest_burst = INT_MAX;
    int smallest_index = 0;
    int si = 0;
    int remaining_jobs = len;
    while (remaining_jobs > 0) {
        smallest_burst = INT_MAX;
        for (int i = 0; i < len; i++) {
            if (jobs2[i].response == -1 && jobs2[i].inQueue == false && jobs2[i].arrival_time <= current_time) {
                readyQueue[readyCount] = i;
                jobs2[i].inQueue = true;
                readyCount++;
            }
        }

        for (int i = 0; i < readyCount; i++) {
            if (jobs2[readyQueue[i]].cpu_burst < smallest_burst) {
                smallest_burst = jobs2[readyQueue[i]].cpu_burst;
                smallest_index = readyQueue[i];
                si = i;
            }
        }

        if (remaining_jobs == len)
            jobs2[smallest_index].start = jobs2[smallest_index].arrival_time;
        else
            jobs2[smallest_index].start = current_time;
        jobs2[smallest_index].end = jobs2[smallest_index].start + jobs2[smallest_index].cpu_burst;
        jobs2[smallest_index].turnaround = jobs2[smallest_index].end - jobs2[smallest_index].arrival_time;
        jobs2[smallest_index].response = jobs2[smallest_index].start - jobs2[smallest_index].arrival_time;
        current_time = jobs2[smallest_index].end;
        remaining_jobs--;

        for (int i = si; i < readyCount - 1; i++) {
            readyQueue[i] = readyQueue[i + 1];
        }
        readyCount--;
    }
    free(readyQueue);
}

// Use of proper pointers to point to direct address not index.. very efficient and closest to actual implementation
void sjfNP_pointer_final(struct Job jobs2[], int len) {
    struct Job **readyQueue = malloc(len * sizeof(struct Job *));
    int current_time = 0;
    int readyCount = 0;
    int smallest_burst = INT_MAX;
    int smallest_index = 0;
    int remaining_jobs = len;
    while (remaining_jobs > 0) {
        smallest_burst = INT_MAX;
        for (int i = 0; i < len; i++) {
            if (jobs2[i].response == -1 && jobs2[i].inQueue == false && jobs2[i].arrival_time <= current_time) {
                readyQueue[readyCount] = &jobs2[i];
                jobs2[i].inQueue = true;
                readyCount++;
            }
        }

        if (readyCount == 0) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < len; i++) {
                if (jobs2[i].response == -1 && jobs2[i].arrival_time < next_arrival) {
                    next_arrival = jobs2[i].arrival_time;
                }
            }

            if (next_arrival == INT_MAX) {
                break;
            }

            current_time = next_arrival;
            continue;
        }

        for (int i = 0; i < readyCount; i++) {
            if (readyQueue[i]->cpu_burst < smallest_burst) {
                smallest_burst = readyQueue[i]->cpu_burst;
                smallest_index = i;
            }
        }

        struct Job *job = readyQueue[smallest_index];

        if (remaining_jobs == len)
            job->start = job->arrival_time;
        else
            job->start = current_time;
        job->end = job->start + job->cpu_burst;
        job->turnaround = job->end - job->arrival_time;
        job->response = job->start - job->arrival_time;
        current_time = job->end;
        remaining_jobs--;

        for (int i = smallest_index; i < readyCount - 1; i++) {
            readyQueue[i] = readyQueue[i + 1];
        }
        readyCount--;
    }
    free(readyQueue);
}



// ######## Preamptive Shortest Job First ######## //


void sjfP(struct Job jobs2[], int len) {
    struct Job **readyQueue = malloc(len * sizeof(struct Job *));
    int current_time = 0;
    int queueCounter = 0;
    int smallest_burst = INT_MAX;
    int smallest_index = 0;
    int remaining_jobs = len;

    while (remaining_jobs > 0) {
        smallest_burst = INT_MAX;
        for (int i = 0; i < len; i++) {
            if (jobs2[i].response == -1 && jobs2[i].inQueue == false && jobs2[i].arrival_time <= current_time) {
                readyQueue[queueCounter] = &jobs2[i];
                printf("Added job[%d] to readyQueue (arrival=%d, burst=%d)\n",
                       i, jobs2[i].arrival_time, jobs2[i].cpu_burst);
                jobs2[i].inQueue = true;
                queueCounter++;
            }
        }

        if (queueCounter == 0) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < len; i++) {
                if (jobs2[i].response == -1 && jobs2[i].arrival_time < next_arrival) {
                    next_arrival = jobs2[i].arrival_time;
                }
            }
            if (next_arrival == INT_MAX) {
                break;
            }
            current_time = next_arrival;
            continue;
        }

        for (int i = 0; i < queueCounter; i++) {
            if (readyQueue[i]->remaining_time < smallest_burst) {
                smallest_burst = readyQueue[i]->remaining_time;
                smallest_index = i;
            }
        }

        struct Job *job = readyQueue[smallest_index];

        if (remaining_jobs == len)
            job->start = job->arrival_time;
        else {
            if (job->remaining_time == job->cpu_burst) {
                job->start = current_time;
            }
        }


        current_time += 1;
        job->remaining_time -= 1;

        if (job->remaining_time <= 0) {
            job->end = current_time;
            job->turnaround = job->end - job->arrival_time;
            job->response = job->start - job->arrival_time;
            job->inQueue = false;
            remaining_jobs--;
            for (int i = smallest_index; i < queueCounter - 1; i++) {
                readyQueue[i] = readyQueue[i + 1];
            }
            queueCounter--;
        }
    }
    free(readyQueue);
}


// ######## Round Robin ######## //


void rr(struct Job jobs[], int len) {
    int current_time = 0;
    int count=0;
    int remaining_jobs = len;
    struct Node *head = NULL;
    struct Node *tail = NULL;

    while (remaining_jobs > 0) {
        for (int i = 0; i < len; i++) {
            if (!jobs[i].inQueue && jobs[i].arrival_time <= current_time && jobs[i].remaining_time > 0) {
                struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
                newNode->data = i;
                newNode->next = NULL;
                jobs[i].inQueue = true;
                if (!head) {
                    head = tail = newNode;
                } else {
                    tail->next = newNode;
                    tail = newNode;
                }
            }
        }

        if (!head) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < len; i++) {
                if (jobs[i].remaining_time > 0 && jobs[i].arrival_time < next_arrival) {
                    next_arrival = jobs[i].arrival_time;
                }
            }
            if (next_arrival == INT_MAX) break;
            current_time = next_arrival;
            continue;
        }

        struct Node *currentNode = head;
        head = head->next;
        if (!head) tail = NULL;

        struct Job *job = &jobs[currentNode->data];
        if (job->remaining_time == job->cpu_burst) job->start = current_time;

        int exec_time = (job->remaining_time < 20) ? job->remaining_time : 20;

        job->remaining_time-=exec_time;
        current_time+=exec_time;

        for (int i = 0; i < len; i++) {
            if (!jobs[i].inQueue && jobs[i].arrival_time <= current_time && jobs[i].remaining_time > 0) {
                struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
                newNode->data = i;
                newNode->next = NULL;
                jobs[i].inQueue = true;
                if (!head) {
                    head = tail = newNode;
                } else {
                    tail->next = newNode;
                    tail = newNode;
                }
            }
        }

        if (job->remaining_time <= 0) {
            job->end = current_time;
            job->turnaround = job->end - job->arrival_time;
            job->response = job->start - job->arrival_time;
            job->inQueue = false;
            remaining_jobs--;
            free(currentNode);
        } else {
            currentNode->next = NULL;
            if (!head) {
                head = tail = currentNode;
            } else {
                tail->next = currentNode;
                tail = currentNode;
            }
        }
    }
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
            clearQueue(&head, &tail, &head2, &tail2);
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

// Attempt to better algorithm... under progress
void MLFQ2(struct Job jobs[], int len) {
    struct Node *head = NULL, *tail = NULL;
    struct Node *head2 = NULL, *tail2 = NULL;

    int current_time = 0;
    int quantum_time = 20;
    int reset_timer = 100;
    int remaining_jobs = len;
    bool isBreak = false;

    while (remaining_jobs > 0) {
        if (reset_timer <= 0) {
            clearQueue(&head, &tail, &head2, &tail2);
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
                printf("\n[DEBUG] --- Starting top of outer while (head queue) ---\n");
                printList(head);

                enqueueFirst(jobs, &head, &tail, len, current_time);
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
        }
    }
}


int main() {
    struct Job jobs[3];
    initStruct(&jobs[0], 1, 0, 300, -1, 100);
    initStruct(&jobs[1], 2, 10, 100, -1, 20);
    initStruct(&jobs[2], 3, 2, 200, -1, 45);

    struct Job jobs2[3];
    initStruct(&jobs2[0], 1, 0, 80, -1, 22);  // The Grunt
    initStruct(&jobs2[1], 2, 10, 40, -1, 100); // The Priority Holder
    initStruct(&jobs2[2], 3, 25, 15, -1, 100); // The Sniper

    rr(jobs, 3);
    MLFQ2 (jobs2, 3);

    printStruct(jobs, 3);
    printStruct(jobs2, 3);

    return 0;
}
