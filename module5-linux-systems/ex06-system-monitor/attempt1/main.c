#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

// signal control
static volatile sig_atomic_t running = 1;

void handler_sigint(int sig)
{
    (void)sig;
    running = 0;
}

// CPU usage
typedef struct {
    long user;
    long nice;
    long system;
    long idle;
} cpu_stat_t;

static int read_cpu_stat(cpu_stat_t *s)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) return -1;
    int ok = fscanf(fp, "cpu %ld %ld %ld %ld",
                    &s->user, &s->nice, &s->system, &s->idle);
    fclose(fp);
    return (ok == 4) ? 0 : -1;
}

static float calc_cpu_usage(cpu_stat_t *a, cpu_stat_t *b)
{
    long total_a = a->user + a->nice + a->system + a->idle;
    long total_b = b->user + b->nice + b->system + b->idle;
    long delta_total = total_b - total_a;
    long delta_idle = b->idle - a->idle;
    if (delta_total == 0) return 0.0f;
    return (float)(delta_total - delta_idle) / (float)delta_total * 100.00f;
}

// memory info
typedef struct {
    long total_kb;
    long free_kb;
    long available_kb;
} mem_info_t;

static int read_mem_info(mem_info_t *m)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) return -1;

    char line[128];
    m->total_kb     = 0;
    m->free_kb      = 0;
    m->available_kb = 0;
    int found       = 0;

    while (fgets(line, sizeof(line), fp) != NULL && found < 3) {
        if (sscanf(line, "MemTotal: %ld kB",     &m->total_kb)     == 1) found++;
        if (sscanf(line, "MemFree: %ld kB",      &m->free_kb)      == 1) found++;
        if (sscanf(line, "MemAvailable: %ld kB", &m->available_kb) == 1) found++;
    }
    fclose(fp);
    return (found == 3) ? 0 : -1;
}

// uptime
static int read_uptime(double *uptime)
{
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) return -1;
    int ok = fscanf(fp, "%lf", uptime);
    fclose(fp);
    return (ok == 1) ? 0 : -1;
}

// shared monitor data
typedef struct {
    float   cpu_pct;
    long    mem_total_kb;
    long    mem_used_kb;
    long    mem_avail_kb;
    double  uptime_sec;
} monitor_data_t;

static monitor_data_t g_data;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

// collector thread
void *thread_collector(void *arg)
{
    (void)arg;
    while (running) {
        cpu_stat_t s1, s2;
        mem_info_t mem;
        double uptime;

        read_cpu_stat(&s1);
        sleep(1);
        read_cpu_stat(&s2);

        float cpu = calc_cpu_usage(&s1, &s2);
        read_mem_info(&mem);
        read_uptime(&uptime);

        pthread_mutex_lock(&g_mutex);
        g_data.cpu_pct  = cpu;
        g_data.mem_total_kb = mem.total_kb;
        g_data.mem_used_kb =  mem.total_kb - mem.available_kb;
        g_data.mem_avail_kb = mem.available_kb;
        g_data.uptime_sec = uptime;
        pthread_mutex_unlock(&g_mutex);
    }
    return NULL;
}

// display
static void print_dashboard(int tick)
{
    pthread_mutex_lock(&g_mutex);
    monitor_data_t d = g_data;
    pthread_mutex_unlock(&g_mutex);

    long uptime_h = (long)(d.uptime_sec / 3600);
    long uptime_m = (long)(d.uptime_sec / 60) % 60;
    long uptime_s = (long)d.uptime_sec % 60;

    printf("\n=== system monitor [tick %d] ===\n", tick);
    printf("uptime  : %ldh %02ldm %02lds\n", uptime_h, uptime_m, uptime_s);
    printf("cpu     : %.1f%%\n", d.cpu_pct);
    printf("memory  : %ld MB total | %ld MB used | %ld MB free\n",
            d.mem_total_kb / 1024,
            d.mem_used_kb  / 1024,
            d.mem_avail_kb / 1024);
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler_sigint;
    sigaction(SIGINT, &sa, NULL);

    printf("system monitor starting - press Ctrl+C to stop\n");

    pthread_t collector;
    pthread_create(&collector, NULL, thread_collector, NULL);

    // wait for first collection cycle
    sleep(2);

    int tick = 0;
    while (running) {
        tick++;
        print_dashboard(tick);
        sleep(2);
    }

    printf("\n=== shutting down ===\n");
    pthread_join(collector, NULL);
    pthread_mutex_destroy(&g_mutex);
    printf("done.\n");

    return 0;
}
