# SOPG_TP1
 *
 * SOPG 1er bimestre 2020 
 * TP N° 1
 * 
 * Pablo José Carlo Alonso Castillo
 * 10° Cohorte 
 * 
 * NamedFIFI_reader.c
 *
 * recibe Texto por el stdin y lo procesa:
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
 * NamedFIFI_writer.c
 *
 * recibe señales y texto desde la consola y lo reenvía por el stdout 
 *
 * recibe SIGINT  y la envía como CTRL+C
 * recibe SIGUSR1 y la envía como SIGN:1
 * recibe SIGUSR2 y la envía como SIGN:2
 *  
 * recibe Texto   y lo envia como DATA:Texto
 *
 * Agregué lo de CTRL+C para practicar un poco 
 *
 * 
 
 
 */
