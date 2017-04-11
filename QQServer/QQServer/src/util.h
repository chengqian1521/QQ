#ifndef __UTILE_H_
#define __UTILE_H_

#define FILEBUFSIZE 262142 //255k redhat6.3 32bit socket最大缓冲区255k


#define ErrPrint(...) std::cout<<__FILE__<<":"<<__LINE__<<": "; \
                      printf(__VA_ARGS__);
#define ErrPrintErrStr ErrPrint("%s\n",strerror(errno))
namespace util {

    /**
     * @brief setDaemon set the application to Daemon state on err will exit the application
     */
    void setDaemon();


    /**
     * @brief catch_Signal callback
     * @param Sign the signno catched
     *
     */
    void catch_Signal(int Sign);


    /**
     * @brief set signal cahandler
     * @param signo want to catch signo
     * @param func the callback when catch the signal
     * @return on successful return 0,on err return -1 and errno will be set
     */
    int signal1(int signo, void (*func)(int));



}


#endif /* __UTILE_H__ */
