#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int *add;
int *chunk;
int *process_using;

void swap(int *a, int *b);

void selectionSort(int array[], int size);

void split_mem(int address);

int find_add_to_alloc(int allocate);

void deallocate_and_combine(int index);

void print_memory_map();

void move_array(int index);

unsigned long nextpower2(unsigned long number)
{
    if (!(number&(number-1)))
        return number;
    int i = 1;
    for(; (number = number>>1) ; i++);
    return (1<<i);
}

int main(int argc, char* argv[]){

    //File and space for each argument in a command
    char instruction;
    char PID[10] = {0};
    char memory_to_allocate[5] = {0};
    FILE* commands = fopen("sample.data", "r");

    //Get each argument
    char buffer[15] = {0};
    fgets(buffer, sizeof(buffer), commands);
    int mem_pool = atoi(buffer);

    add = (int *)calloc(mem_pool, sizeof(int));
    chunk = (int *)calloc(mem_pool, sizeof(int));
    process_using = (int *)calloc(mem_pool, sizeof(int));
    //add[0] = 0;
    chunk[0] = mem_pool;

    int to_sort[mem_pool];
    for (int j = 0; j < mem_pool; ++j) {
        to_sort[j] = j;
    }


    while (fgets(buffer, sizeof(buffer), commands)){
        char *temp;
        memset(PID, 0, sizeof(PID));
        memset(memory_to_allocate, 0, sizeof(memory_to_allocate));
        temp = strtok(buffer, " ");
        //printf("%s ", temp);
        instruction = temp[0];
        temp = strtok(NULL, " ");
        strcpy(PID, temp);
        //printf("%s", PID);


        if(instruction == 'M'){
            temp = strtok(NULL, " ");
            //printf(" %s", temp);

            int power2_to_allocate = nextpower2(atoi(temp));
            int address = find_add_to_alloc(power2_to_allocate); // Not actual addresss but index in array
            if (address == -1) {
                char error[40] = {0};
                strcat(error, "Memory unavailable : ");
                strcat(error, PID);
                strcat(error, " ");
                char temp1[20];
                sprintf(temp1, "%d ", atoi(temp));
                strcat(error, temp1);
                sprintf(temp1, "%d ", power2_to_allocate);
                strcat(error, temp1);
                fprintf(stderr, "%s", error);
                continue;
            }
            while(chunk[address] > power2_to_allocate)
                split_mem(address);
            process_using[address] = atoi(PID);

        }
        else
        {
            for (int i = 0; chunk[i] ; ++i) {
                if (process_using[i] == atoi(PID)) {
                    process_using[i] = 0;
                    deallocate_and_combine(i);
                    break;
                }
            }
        }
    }

    print_memory_map();

    return 0;
}

void print_memory_map() {

    int i;
    for (i = 0; chunk[i]; ++i);
    int* sorting = (int *)malloc(i * sizeof(int));
    for (int l = 0; l < i; ++l) {
        sorting[l] = add[l];
    }
    selectionSort(sorting, i);
    for (int j = 0; j < i; ++j) {
        for (int k = 0; k < i; ++k) {
            if (sorting[j] == add[k]) {
                printf("0x%03x %03d ", add[k], chunk[k]);
                if (process_using[k] == 0)
                    printf("FREE\n");
                else
                    printf("%03d\n", process_using[k]);
            }
        }

    }
}

void deallocate_and_combine(int index) {


            int to_try_combine = add[index] ^chunk[index];
            int j;
            for (j = 0;  ; ++j) {
                if (add[j] == to_try_combine)
                {
                    if (process_using[j] == 0 && chunk[j] == chunk[index])
                    {
                        int to_del, not_del;
                        if(add[j] > add[index]) {
                            to_del = j;
                            not_del = index;
                        }
                        else {
                            to_del = index;
                            not_del = j;
                        }
                        chunk[not_del] = 2*chunk[not_del];
                        add[to_del] = 0;
                        chunk[to_del] = 0;
                        process_using[to_del] = 0;
                        move_array(to_del);
                        if(process_using[not_del] == 0)
                            deallocate_and_combine(not_del);
                        break;
                    }
                    break;
                }
            }

}

void move_array(int index) {

    do
    {
        add[index] = add[index+1];
        chunk[index] = chunk[index+1];
        process_using[index] = process_using[index+1];

    }
    while(chunk[++index]);
}

int find_add_to_alloc(int allocate) {

    int least = INT32_MAX, index = 0;
    for (int i = 0; chunk[i] ; ++i) {
        if(process_using[i] == 0) {
            if ((chunk[i] <= least) && (chunk[i] >= allocate)) {
                if(chunk[i] == least)
                    if(add[index] < add[i])
                        continue;
                least = chunk[i];
                index = i;
            }
        }
    }
    if(least == INT32_MAX)
        return -1;

    return index;
}

void split_mem(int address){

    chunk[address] = chunk[address]/2;
    int i = 0;
    for(;chunk[i]; i++);
    chunk[i] = chunk[address];
    add[i] = add[address] + chunk[i];
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void selectionSort(int array[], int size)
{
    for (int step = 0; step < size - 1; step++)
    {
        int min_idx = step;
        for (int i = step + 1; i < size; i++)
        {
            if (array[i] < array[min_idx])
                min_idx = i;
        }

        swap(&array[min_idx], &array[step]);
    }
}