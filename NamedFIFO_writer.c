/*
 * SOPG 1er bimestre 2020 
 * TP N° 1
 * 
 * Pablo José Carlo Alonso Castillo
 * 10° Cohorte 
 * 
 * NamedFIFI_writer.c
 *
 * recibe SIGINT  y la envía como CTRL+C
 * recibe SIGUSR1 y la envía como SIGN:1
 * recibe SIGUSR2 y la envía como SIGN:2
 *  
 * recibe Texto   y lo envia como DATA:Texto
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 300

volatile sig_atomic_t sigNumber = 0;

// declaro lo handlers para las señales
void sigint_handler(int sig)
{
    sigNumber = SIGINT;
    //write(1, "Llegó Ct+C", 14);
}

void sigusr1_handler(int sig)
{
    sigNumber = SIGUSR1;
    //write(1, "Llegó usr1", 13);
}

void sigusr2_handler(int sig)
{
    sigNumber = SIGUSR2;
    //write(1, "Llegó usr2", 13);
}

int main(void)
{
   
    char outputBuffer[BUFFER_SIZE];
	uint32_t bytesWrote;
	int32_t returnCode, fd;

    //Cambio los handlers de SIGINT, SIGUSR1 y SIGUSR2
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0; //SA_RESTART;

    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = 0; //SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigactionSIGUSR1");
        exit(1);
    }

    sa.sa_handler = sigusr2_handler;
    sa.sa_flags = 0; //SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigactionSIGUSR1");
        exit(1);
    }

    //imprimo el pid del proceso para que mandarle señales desde otra terminal sea más sencillo
    printf("el pid del writer es: %d\n", getpid());

    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for readers...\n");
	if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a reader--type some stuff\n");

    /* Loop forever */
	while (1)
	{
        //Verifico que llegaron las señales y las envío
        switch (sigNumber){
            case SIGINT:
                /* Write buffer to named fifo. */
                if ((bytesWrote = write(fd, "CTRL+C", strlen("CTRL+C"))) == -1)
                {
                    perror("write CTRL+C");
                }
                else
                {
                    printf("writer: wrote CTRL+C %d bytes\n", bytesWrote);
                } 
            break;
            case SIGUSR1:
                /* Write buffer to named fifo. */
                if ((bytesWrote = write(fd, "SIGN:1", strlen("SIGN:1"))) == -1)
                {
                    perror("write SIGN:1");
                }
                else
                {
                    printf("writer: wrote SIGN:1 %d bytes\n", bytesWrote);
                }    
            break;
            case SIGUSR2:
                /* Write buffer to named fifo. */
                if ((bytesWrote = write(fd, "SIGN:2", strlen("SIGN:2"))) == -1)
                {
                    perror("write SIGN:2");
                }
                else
                {
                    printf("writer: wrote SIGN:2 %d bytes\n", bytesWrote);
                }    
            break;
            default:
            break;
        } 
        sigNumber = 0; //quedo listo para la siguiente SIGNAL
        
        //Verifico que llegaron Textos señales y los envío:
        /* Get some text from console */
		fgets(outputBuffer, BUFFER_SIZE, stdin);
        //Agrego DATA:
        char auxBuffer[BUFFER_SIZE + strlen("DATA:")];
        strcpy (auxBuffer,"DATA:");
        strcat (auxBuffer, outputBuffer);

        if(strlen(outputBuffer) != 0){  //evito que se vuelva a escribir el buffer cuando se recive un SIGN
            /* Write buffer to named fifo. Strlen - 1 to avoid sending \n char */
            if ((bytesWrote = write(fd, auxBuffer, strlen(auxBuffer)-1)) == -1)
            {
                perror("writing DATA: etc.\n");
            }
            else
            {
                printf("writer: wrote DATA:etc. %d bytes\n", bytesWrote);
                outputBuffer[0] = 0;  //aseguro que el llamado a strlen(outputBuffer) sea 0
                //memset(outputBuffer, 0, BUFFER_SIZE);
            }       
        }

	}//cierre del while
    
	return 0;
}
