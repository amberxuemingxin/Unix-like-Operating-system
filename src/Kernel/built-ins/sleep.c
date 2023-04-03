#include <stdio.h>
#include "../user.h"

void my_sleep(char *ticks) {
    printf("ticks in my_sleep = %s\n", ticks);

    int t = atoi(ticks);
    if (t == 0) {
        perror("Fail to atoi");
    }
    
    p_sleep(t * 10);
}