#include <stdio.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
    openlog(NULL,LOG_ODELAY,LOG_USER);  //start syslog connection
    if (argc != 3)  //Check number of inpus
    {
        syslog(LOG_ERR,"Invalid Number of inputs");
        return 1;

    }

    FILE* writefile=fopen(argv[1],"w");

    if (writefile==NULL)    //check if file created
    {
        syslog(LOG_ERR,"Error, could not create file");
        return 1;
    }

    syslog(LOG_DEBUG,"Writing to file");

    if (fprintf(writefile,argv[2]) < 0) //if fprintf returns a negative value, that means write process failed.
    {
        syslog(LOG_ERR,"Error, could not write to file");
    }
    else
    {
        fclose(writefile);
        syslog(LOG_DEBUG,"File write success");
    }
    

    return 0;




}