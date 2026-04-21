#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define BUF_SIZE    512

// function declarations
void read_sys_file(const char *path);
void list_sys_dir(const char *path);
void read_cpu_thermal(void);
void read_net_stats(void);

int main(void)
{
    printf("=== /sys/kernel/hostname ===\n");
    read_sys_file("/proc/sys/kernel/hostname");

    printf("\n=== CPU thermal zones ===\n");
    read_cpu_thermal();

    printf("\n=== network interface stats ===\n");
    read_net_stats();

    printf("\n=== /sys/block (block devices) ===\n");
    list_sys_dir("/sys/block");

    printf("\n=== /sys/class/net (network interface) ===\n");
    list_sys_dir("/sys/class/net");

    return 0;
}

void read_sys_file(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("cannot open %s: %s\n", path, strerror(errno));
        return;
    }

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    ssize_t n = read(fd, buf, BUF_SIZE - 1);
    if (n > 0) {
        //strip trailing newline
        if (buf[n-1] == '\n') buf[n-1] = '\0';
        printf("%s: %s\n", path, buf);
    }
    close(fd);
}

void list_sys_dir(const char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("cannot open %s: %s\n", path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // skip . and ..
        if (entry->d_name[0] == '.') continue;
        printf(" %s\n", entry->d_name);
    }
    closedir(dir);
}

void read_cpu_thermal(void)
{
    int found = 0;
    // walk thermal zones
    for (int i = 0; i < 4; i++) {
        char path[BUF_SIZE];

        // read zone type
        snprintf(path, BUF_SIZE,
                "/sys/class/thermal/cooling_device%d/type", i);
        int fd = open(path, O_RDONLY);
        if (fd < 0) break; // no more zones

        char type[64];
        memset(type, 0, sizeof(type));
        ssize_t n = read(fd, type, sizeof(type) - 1);
        close(fd);
        if (n <= 0) break;
        if (type[n-1] == '\n') type[n-1] = '\0';

        // read temperature
        snprintf(path, BUF_SIZE,
                "/sys/class/thermal/cooling_device%d/temp", i);
        fd = open(path, O_RDONLY);
        if (fd < 0) continue;

        char temp_str[32];
        memset(temp_str, 0, sizeof(temp_str));
        n = read(fd, temp_str, sizeof(temp_str) - 1);
        close(fd);
        if (n <= 0) continue;

        int temp_mc = atoi(temp_str);   // millidegree Celsius
        printf(" zone%d [%s]: %d.%03d°C\n",
                i, type, temp_mc / 1000, temp_mc % 1000);
        found = 1;
    }

    if (!found) {
        printf("  no thermal zones found (WSL2 — no direct hardware access)\n");
        printf("  on real embedded Linux: Raspberry Pi, BeagleBone, i.MX6\n");
        printf("  thermal zones would show CPU, GPU, battery temperatures\n");
    }
}

void read_net_stats(void)
{
    // find first non-loopback interface
    DIR *dir = opendir("/sys/class/net");
    if (dir == NULL) { perror("opendir"); return; }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        if (strcmp(entry->d_name, "lo") == 0) continue; // skip loopback
    
        char path[BUF_SIZE];
        char buf[64];

        // rx bytes
        snprintf(path, BUF_SIZE,
                "/sys/class/net/%s/statistics/rx_bytes",
                entry->d_name);
        int fd = open(path, O_RDONLY);
        if (fd < 0) continue;
        memset(buf, 0, sizeof(buf));
        read(fd, buf, sizeof(buf) - 1);
        close(fd);
        long rx = atol(buf);

        // tx bytes
        snprintf(path, BUF_SIZE,
                "/sys/class/net/%s/statistics/tx_bytes",
                entry->d_name);
        fd = open(path, O_RDONLY);
        if (fd < 0) continue;
        memset(buf, 0, sizeof(buf));
        read(fd, buf, sizeof(buf) - 1);
        close(fd);
        long tx = atol(buf);

        printf(" %s: rx=%ld bytes tx=%ld bytes\n",
                entry->d_name, rx, tx);
    }
    closedir(dir);
}
