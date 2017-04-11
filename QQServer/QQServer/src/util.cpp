#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
namespace util {
    void setDaemon()
    {
        pid_t pid, sid;
        pid = fork();
        if (pid < 0)
        {
            printf("fork failed %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (pid > 0)
        {
            exit(EXIT_SUCCESS);
        }

        if ((sid = setsid()) < 0)
        {
            printf("setsid failed %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

#ifdef debug
         if (chdir("/") < 0)
         {
             printf("chdir failed %s\n", strerror(errno));
             exit(EXIT_FAILURE);
         }
         umask(0);
         close(STDIN_FILENO);
         close(STDOUT_FILENO);
         close(STDERR_FILENO);
#endif
    }

    void catch_Signal(int Sign)
    {
        switch (Sign)
        {
        case SIGINT:
            //printf("signal SIGINT\n");
            break;
        case SIGPIPE:
            //printf("signal SIGPIPE\n");
            //signal1(SIGPIPE, catch_Signal);
            break;
        }
    }

    int signal1(int signo, void (*func)(int))
    {
        struct sigaction act, oact;
        act.sa_handler = func;
        if(sigemptyset(&act.sa_mask)==-1){
            perror("sigemptyset error");
            return -1;
        }
        act.sa_flags = 0;
        return sigaction(signo, &act, &oact);
    }




}
