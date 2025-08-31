#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>

#define ITERATIONS 1000000

double now_in_seconds() {
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main() {
    int p[2];
    if (pipe(p) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child: reader
        close(p[1]);
        char buf;
        for (int i = 0; i < ITERATIONS; i++) {
            if (read(p[0], &buf, 1) != 1) {
                perror("read");
                return 1;
            }
        }
        close(p[0]);
        return 0;
    } else {
        // Parent: writer
        close(p[0]);
        double start = now_in_seconds();

        for (int i = 0; i < ITERATIONS; i++) {
            if (write(p[1], "x", 1) != 1) {
                perror("write");
                return 1;
            }
        }

        double end = now_in_seconds();
        close(p[1]);

        std::cout << "Sent " << ITERATIONS << " messages in "
                  << (end - start) << " seconds\n";
        std::cout << "Average time per message: "
                  << ((end - start) / ITERATIONS) * 1e9
                  << " ns\n";
    }
    return 0;
}
