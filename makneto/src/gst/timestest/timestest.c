
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>


clock_t beginning;
clock_t middle;
clock_t end;

struct tms  process_times;

void print_times(const clock_t at, const struct tms *times)
{
    printf("Begin: %ld  userspace: %ld  system: %ld\n",
        at, times->tms_utime, times->tms_stime);
}

#define BUF_SIZE 512
int main(int argc, char *argv[])
{
    long int i;
    FILE *source = NULL;
    long int total_bytes = 0;
    size_t current_bytes = 0;
    char readbuf[BUF_SIZE];

    beginning = times(&process_times);
    print_times(beginning, &process_times);

    for (i=0; i< 0xFFFFFFA; i++) {
        int x = i + i;
    }

    middle = times(&process_times);
    print_times(middle, &process_times);

    source = fopen("/dev/urandom", "rb");
    if (!source) {
        fprintf(stderr, "Could not open /dev/urandom\n");
        return 1;
    }

    while ((current_bytes = fread(readbuf, BUF_SIZE, 1, source)) > 0 
            && total_bytes < 4000) {
        // do not need to do anything with random input.
        total_bytes += current_bytes;
    }
    fclose(source);


    end = times(&process_times);
    print_times(end, &process_times);

   
    clock_t runtime = end - beginning;
    print_times(runtime, &process_times);
    printf("Percentage: user: %4ld%% system: %4ld%%\n",
        process_times.tms_utime * 100/runtime,
        process_times.tms_stime * 100/runtime);

    
    return 0;
}
