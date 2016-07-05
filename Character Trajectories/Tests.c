#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(){

    struct utsname system;
    printf("SystemName: %s\n", system.sysname);
    printf("Release: %s\n", system.release);
    printf("Machine: %s\n", system.machine);

	
}