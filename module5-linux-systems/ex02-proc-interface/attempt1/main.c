#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE    512

// function declarations
void read_proc_uptime(void);
void read_proc_meminfo(void);
void read_proc_cpuinfo(void);
void read_proc_self(void);

int main(void)
{
    printf("=== /proc/uptime ===\n");
    read_proc_uptime();
    
    printf("\n=== /proc/meminfo (first 5 lines) ===\n");
    read_proc_meminfo();

    printf("\n=== /proc/cpuinfo (model name) ===\n");
    read_proc_cpuinfo();

    printf("\n=== /proc/self ===\n");
    read_proc_self();

    return 0;
}

void read_proc_uptime(void)
{
    int fd = open("/proc/uptime", O_RDONLY);
    if (fd < 0) { perror("open /proc/uptime"); return; }

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    ssize_t n = read(fd, buf, BUF_SIZE - 1);
    if (n > 0) {
        // parse uptime seconds
        double uptime, idle;
        sscanf(buf, "%lf %lf", &uptime, &idle);
        printf("uptime: %.2f seconds (%.2f hours)\n",
                uptime, uptime / 3600.0);
        printf("idle:   %.2f seconds\n", idle);
    }
    close(fd);
}

void read_proc_meminfo(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) { perror("fopen /proc/meminfo"); return; }

    char line[BUF_SIZE];
    int count = 0;
    while (fgets(line, BUF_SIZE, fp) != NULL && count < 5) {
        printf("%s", line);
        count++;
    }
    fclose(fp);
}

void read_proc_cpuinfo(void)
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) { perror("fopen /proc/cpuinfo"); return; }

    char line[BUF_SIZE];
    while (fgets(line, BUF_SIZE, fp) != NULL) {
        if (strncmp(line, "model name", 10) == 0) {
            printf("%s", line);
            break;  // print first CPU only
        }
    }
    fclose(fp);
}

void read_proc_self(void)
{
    // /proc/self is a symlink to /proc/PID of current process
    pid_t pid = getpid();
    printf("PID: %d\n", pid);

    // read /proc/self/status for process info
    FILE *fp =  fopen("/proc/self/status", "r");
    if (fp == NULL) { perror("fopen /proc/self/status"); return; }

    char line[BUF_SIZE];
    int count = 0;
    while (fgets(line, BUF_SIZE, fp) != NULL && count < 8) {
        printf("%s", line);
        count++;
    }
    fclose(fp);
}
