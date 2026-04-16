#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE    128

int main(void)
{
    // open a file
    printf("=== open/write/close ===\n");

    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    printf("opened test.txt, fd=%d\n", fd);

    // write
    const char *msg = "hello from file descriptor\n";
    ssize_t written = write(fd, msg, strlen(msg));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }
    printf("wrote %zd bytes\n", written);

    close(fd);
    printf("closed fd=%d\n", fd);

    // open for reading
    printf("\n=== open/read/close ===\n");

    fd = open("test.txt", O_RDONLY);
    if (fd < 0) {
        perror("read");
        return 1;
    }
    printf("opened test.txt for read fd=%d\n", fd);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    ssize_t bytes_read = read(fd, buf, BUF_SIZE-1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return 1;
    }
    printf("read %zd bytes: %s", bytes_read, buf);
    close(fd);

    // file descriptors 0 1 2
    printf("\n=== standard file descriptors ===\n");
    printf("stdin   = fd %d\n", STDIN_FILENO);
    printf("stdout  = fd %d\n", STDOUT_FILENO);
    printf("stderr  = fd %d\n", STDERR_FILENO);

    // write directly to stdout via fd
    const char *direct = "writing directly to stdout via fd\n";
    write(STDOUT_FILENO, direct, strlen(direct));

    // lseek
    printf("\n=== lseek ===\n");
    fd = open("test.txt", O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    // seek to beginning
    off_t pos = lseek(fd, 0, SEEK_SET);
    printf("seeked to position %ld\n", pos);

    // read first 5 bytes only
    memset(buf, 0, BUF_SIZE);
    read(fd, buf, 5);
    printf("first 5 bytes: %s\n", buf);

    // where are we now
    pos = lseek(fd, 0, SEEK_CUR);
    printf("current position: %ld\n", pos);

    close(fd);

    return 0;
}
