/*
 * SOPG 1er bimestre 2020 
 * TP N° 1
 * 
 * Pablo José Carlo Alonso Castillo
 * 10° Cohorte 
 * 
 * NamedFIFO_reader.c
 *
 * recibe Texto y lo procesa:
 * 
 * Todo lo que recibe   lo guarda en: ALL_FL_NAME
 * 
 * Si recibe SIGN:1     lo guarda en: SIGN_FL_NAME 
 * Si recibe SIGN:2     lo guarda en: SIGN_FL_NAME 
 * 
 * Si recibe DATA:Texto lo guarda en: DATA_FL_NAME 
 *
 * Si recibe CTRL+C     lo guarda en: ALL_FL_NAME 
 * 
 * Agregué lo de CTRL+C para practicar un poco 
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

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 300
#define SIGN_FL_NAME "Sign.txt"
#define DATA_FL_NAME "Log.txt"
#define ALL_FL_NAME  "Log+Sign+Resto.txt"

int main(void)
{
	uint8_t inputBuffer[BUFFER_SIZE];
	int32_t bytesRead, returnCode, fd;

    //variables para manejo de archivos
    FILE *fpDATA, *fpSIGN, *fpALL;

    //variables auxiliares para manejo de strings
    char *ptr;
    char str[BUFFER_SIZE]; 

    // limito el número de comunicaciones para salir con seguridad cerrando los archivos
    int i=0, nComunicaciones=8; // solo recibo hasta esta cantidad de comnunicaciones
    
    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1  )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }else{
        printf("Already Exists fifo: %d\n", returnCode);
    }
    
    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for writers...\n");
	if ( (fd = open(FIFO_NAME, O_RDONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a writer\n");

    //archivo para la DATA
    fpDATA = fopen ( DATA_FL_NAME, "a" );
    if (fpDATA == NULL) 
    { 
        printf("Could not open Log.txt" ); 
        return 0; 
    } 
    //archivo para las SIGNALs
    fpSIGN = fopen ( SIGN_FL_NAME, "a");
    if (fpSIGN == NULL) 
    { 
        printf("Could not open Sign.txt" ); 
        return 0; 
    } 
    //archivo para todo lo recibido en crudo antes de formatearlo y guardarlo en Log.txt y Sign.txt
    //este archivo guarda además todo lo recobido que no cumple el formateo de una DATA o unaSIGN
    fpALL = fopen( ALL_FL_NAME, "a" ); 
    if (fpALL == NULL) 
    { 
        printf("Could not open Log+Sign+Resto.txt"); 
        return 0; 
    } 
  
    // Loop until read syscall returns a value <= 0 o que el número  
    // de las comunicaciones recibidas alcanzan a nComunicaciones
	do
	{
        /* read data into local buffer */
		if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1)
        {
			perror("read loop error");
        }
        else
		{
			inputBuffer[bytesRead] = '\0'; //agrego el terminador
			printf("reader: read %d bytes: \"%s\"\n", bytesRead, inputBuffer);
            
            i++;  //contador de comunicaciones

            //detecto el 0 de que se cerró el writter
            if (inputBuffer[0] == 0){
                //writter cerrado
                printf("Writter cerrado\n");
            }else{

                //fpALL guardo todo lo recibido crudo en Log+Sign+Resto.txt
                fprintf(fpALL,"%d.-%s\n", i, inputBuffer);
                    
                //fpDATA identifico la DATA: y la guardo
                ptr = strstr(inputBuffer, "DATA:");
                if (ptr != NULL) 
                {
                    fprintf(fpDATA,"%d.-%s\n", i, inputBuffer);
                }

                //fpSIGN identifico a SIGN:1 y lo guardo
                ptr = strstr(inputBuffer, "SIGN:1");
                if (ptr != NULL) 
                {
                    fprintf(fpSIGN,"%d.-%s\n", i, inputBuffer);
                }

                //fpSIGN identifico a SIGN:2 y lo guardo
                ptr = strstr(inputBuffer, "SIGN:2");
                if (ptr != NULL) 
                {
                    fprintf(fpSIGN,"%d.-%s\n", i, inputBuffer);
                }
            }
		}
	}
	while (bytesRead > 0 );

    fclose(fpALL); 
    fclose(fpDATA); 
    fclose(fpSIGN); 

    printf ("Saliendo, recibidas %d comunicaciones y señales\n", i );

	return 0;
}
