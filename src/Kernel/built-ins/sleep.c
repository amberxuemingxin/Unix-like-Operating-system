#include <stdio.h>
#include "../user.h"

void my_sleep(void *arg) {
    char *ticks = (char *)arg;
    printf("ticks in my_sleep = %s\n", ticks);

    int t = atoi(ticks);
    if (t == 0) {
        perror("Fail to atoi");
    }
    
    p_sleep(t * 10);
}