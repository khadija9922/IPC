#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>

using clock_type = std::chrono::high_resolution_clock;
using time_point = std::chrono::time_point<clock_type>;

double now_in_seconds() {
    return std::chrono::duration<double>(clock_type::now().time_since_epoch()).count();
}

int main() {
    const int ITERATIONS = 100000;
    int p1[2]; // parent -> child
    int p2[2]; // child -> parent

    pipe(p1);
    pipe(p2);

    if (fork() == 0) {
        // -------- Child process --------
        close(p1[1]); // close write end of p1
        close(p2[0]); // close read end of p2

        char buf;
        for (int i = 0; i < ITERATIONS; i++) {
            if (read(p1[0], &buf, 1) != 1) {
                perror("child read");
                return 1;
            }
            // send ack back
            if (write(p2[1], "a", 1) != 1) {
                perror("child write");
                return 1;
            }
        }
        return 0;
    } else {
        // -------- Parent process --------
        close(p1[0]); // close read end of p1
        close(p2[1]); // close write end of p2

        char buf;
        double start = now_in_seconds();

        for (int i = 0; i < ITERATIONS; i++) {
            // send message
            if (write(p1[1], "x", 1) != 1) {
                perror("parent write");
                return 1;
            }
            // wait for ack
            if (read(p2[0], &buf, 1) != 1) {
                perror("parent read");
                return 1;
            }
        }

        double end = now_in_seconds();
        std::cout << "Completed " << ITERATIONS << " round-trips in "
                  << (end - start) << " seconds\n";
        std::cout << "Average RTT per message: "
                  << ((end - start) / ITERATIONS) * 1e9
                  << " ns\n";
        wait(nullptr); // wait for child
    }
}
