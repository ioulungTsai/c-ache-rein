#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

// shared state between main and signal handler
static volatile int running     = 1;    // main loop control
static volatile int reload      = 0;    // config reload flag
static volatile int sig_count   = 0;    // signal counter

// signal handlers
void handler_sigint(int sig)
{
    (void)sig;
    running = 0;    // signal main loop to stop
}

void handler_sighup(int sig)
{
    (void)sig;
    reload = 1;     // signal main loop to reload config
}

void handler_sigusr1(int sig)
{
    (void)sig;
    sig_count++;    // count custom signals
}

int main(void)
{
    // register signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handler_sigint;
    sigaction(SIGINT, &sa, NULL);   // Ctrl+C

    sa.sa_handler = handler_sighup;
    sigaction(SIGHUP, &sa, NULL);   // hangup - reload config

    sa.sa_handler = handler_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);  // user defined signal 1

    printf("=== signal demo ===\n");
    printf("PID = %d\n", getpid());
    printf("send signals with:\n");
    printf("  kill -SIGINT  %d  (or Ctrl+C)\n", getpid());
    printf("  kill -SIGHUP  %d\n", getpid());
    printf("  kill -SIGUSR1 %d\n", getpid());
    printf("\nrunning - waiting for signals...\n");

    // main loop
    int count = 0;
    while (running) {
        sleep(1);
        count++;
        printf("tick %d | sigusr1_count=%d | reload=%d\n",
              count, sig_count, reload);

        if (reload) {
            printf("→ reloading configuration\n");
            reload = 0;
        }

        // auto-stop after 10 ticks for exercise
        if (count >= 50) {
            printf("→ 50 ticks reached - stopping\n");
            running = 0;
        }
    }

    printf("\n=== cleanup and exit ===\n");
    printf("total ticks:    %d\n", count);
    printf("SIGUSR1 received: %d\n", sig_count);

    return 0;
}
