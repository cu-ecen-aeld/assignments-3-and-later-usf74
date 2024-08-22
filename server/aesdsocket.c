
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/stat.h>
#include <signal.h>

char globshutflag = 0;

void sigactfunc(int sigtype)
{
    if (sigtype == SIGTERM || sigtype == SIGINT)
    {
        globshutflag = 1;
    }
}


int main(int argc, char *argv[])
{
    if (argc > 2 || ((argc == 2) && strcmp(argv[1], "-d")))
    {
        printf("Error in arguements\n");
        return -1;
    }
    openlog("AESDSOCKET",0,LOG_USER);

    struct sigaction termact;
    memset(&termact, 0, sizeof(termact));
    termact.sa_handler = sigactfunc;
    if (sigaction(SIGINT, &termact, NULL))
    {
        printf("Error in registering SIGNINT\n");
    }
    if(sigaction(SIGTERM, &termact, NULL))
    {
        printf("Error in registering SIGTERM\n");
    }

    struct stat fst;
    int new_size = 32768;
    int sfd = socket(PF_INET, SOCK_STREAM, 0);
    char opt=1;
    //setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int yes=1;
	if (setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		printf("setsockopt error");
	}

    // int hehe2 = setsockopt(sfd, 6, SOL_SOCKET, &new_size, sizeof(new_size));
    // int hehe = setsockopt(sfd, 6, SO_RCVBUF, &new_size, sizeof(new_size));

    if (sfd < 0)
    {
        printf("Error in socket func");
        return -1;
    }

    // Binding socket

    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    // hints.ai_addrlen;
    // hints.ai_addr;
    // hints.ai_canonname;
    // hints.ai_next;

    int ret = getaddrinfo(NULL, "9000", &hints, &res);
    if (ret != 0)
    {
        printf("Error in getaddrinfo");
        return -1;
    }
    else
    {
        printf("Addrinfo found ! \n");
    }

    do
    {
        ret = bind(sfd, res->ai_addr, res->ai_addrlen); 
        /* code */
    } while (ret != 0); //loop till bind for github action runer
    freeaddrinfo(res);

    if (ret != 0)
    {
        printf("Error in bind %s\n", strerror(errno));
        return -1;
    }
    else
    {
        printf("Bounded ! \n");
    }

    //freeaddrinfo(&hints);

    pid_t forkvar = -1;
    if (argc == 2)
    {
        forkvar = fork();
    }

    if (forkvar > 0)
    {
        exit(0);
    }

    ret = listen(sfd, 10);

    if (ret != 0)
    {
        printf("Error in bind");
        return -1;
    }
    else
    {
        printf("Listening \n");
    }

    struct sockaddr peerinfo;
    socklen_t peerlen = sizeof(peerinfo);

    char buffer[16384];
    int fd = open("/var/tmp/aesdsocketdata", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

    if (fd < 0)
    {
        printf("Error opening file! \n");
    }
    else
    {
        printf("created file ! \n");
    }

    /**************loop **********/
    while (!globshutflag)
    {

        printf("Accepting connections\n");

        int srxfd = accept(sfd, &peerinfo, &peerlen);

        if (globshutflag)
        {
            break;
        }

        if (srxfd <= -1)
        {
            printf("Error on accept");
            continue;
        }
        else
        {
            printf("Accepted connection from %u.%u.%u.%u : %u\n", (unsigned char)peerinfo.sa_data[2], (unsigned char)peerinfo.sa_data[3], (unsigned char)peerinfo.sa_data[4], (unsigned char)peerinfo.sa_data[5], (unsigned short)((unsigned short)(peerinfo.sa_data[0] << 8) | (unsigned short)peerinfo.sa_data[1]));
            syslog(LOG_INFO, "Accepted connection from %u.%u.%u.%u : %u\n", (unsigned char)peerinfo.sa_data[2], (unsigned char)peerinfo.sa_data[3], (unsigned char)peerinfo.sa_data[4], (unsigned char)peerinfo.sa_data[5], (unsigned short)((unsigned short)(peerinfo.sa_data[0] << 8) | (unsigned short)peerinfo.sa_data[1]));
        }

        while (1) // Rx loop
        {
            memset(buffer, 0, sizeof(buffer));
            int nrx = recv(srxfd, buffer, new_size, 0);

            if (globshutflag)
            {
                break;
            }
            if (nrx <= 0)
            {
                printf("Errno is %d\n", errno);
                printf("Error in recieving %s\n", strerror(errno));
                shutdown(srxfd, SHUT_RDWR);
                break;
            }

            else
            {
                printf("Recieved %d bytes ! \n", nrx);
            }

            write(fd, buffer, nrx);
            if (strchr(buffer, '\n') != NULL)
            {
                break;
            }
        }
        if (globshutflag)
        {
            shutdown(srxfd, SHUT_RDWR);

            break;
        }
        //***************************************** */
        lseek(fd, SEEK_SET, 0);
        fstat(fd, &fst);
        char *txbuffer = malloc(sizeof(fst.st_size));
        read(fd, txbuffer, fst.st_size);
        send(srxfd, txbuffer, fst.st_size, 0);
        free(txbuffer);

        printf("Closing connection from %u.%u.%u.%u : %u\n", (unsigned char)peerinfo.sa_data[2], (unsigned char)peerinfo.sa_data[3], (unsigned char)peerinfo.sa_data[4], (unsigned char)peerinfo.sa_data[5], (unsigned short)((unsigned short)(peerinfo.sa_data[0] << 8) | (unsigned short)peerinfo.sa_data[1]));
        syslog(LOG_INFO, "Closing connection from %u.%u.%u.%u : %u\n", (unsigned char)peerinfo.sa_data[2], (unsigned char)peerinfo.sa_data[3], (unsigned char)peerinfo.sa_data[4], (unsigned char)peerinfo.sa_data[5], (unsigned short)((unsigned short)(peerinfo.sa_data[0] << 8) | (unsigned short)peerinfo.sa_data[1]));
        shutdown(srxfd, SHUT_RDWR);
    }
    syslog(LOG_INFO, "Caught signal, exiting\n");
    // printf("Errno is %d\n", errno);
    // printf("Error in syslog %s\n",strerror(errno));
    printf("Caught signal, exiting\n");

    shutdown(sfd, SHUT_RDWR);
    // shutdown(srxfd,SHUT_RDWR);
    close(fd);

    return 0;
}