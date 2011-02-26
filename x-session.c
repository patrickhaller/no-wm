#include <unistd.h>
#include <sys/wait.h>

int main(void) { for (;;) { wait(NULL); sleep(1); } }

