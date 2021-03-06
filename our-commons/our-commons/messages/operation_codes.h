/*
 * operation_codes.h
 *
 *  Created on: 27 abr. 2018
 *      Author: utnso
 */

#ifndef OPERATION_CODES_H_
#define OPERATION_CODES_H_

#define CUSTOM_SUCCESS 1
#define CUSTOM_FAILURE 0

#define SUCCESS '1'
#define FINISHED '2'
#define ABORT '3'
#define OURSET '4'
#define OURGET '5'
#define RUN 6
#define KILLESI 10
#define COMPACT '7'
#define OURSTORE '8'
#define LOCK '9'
#define ERROR_CLAVE_SIZE 'q'
#define ERROR_CLAVE_NOT_IDENTIFIED 'w'
#define ERROR_CLAVE_NON_EXISTENT 'e'
#define ERROR_CLAVE_NOT_FOUND 'r'
#define BLOCK 't'
#define NOTBLOCKED 'y'
#define FREE 'u'
#define NOTFINISHED 'i'
#define BLOCKED 'o'
#define LOCKED 'p'
#define INSTANCIA_RESPONSE_FALLEN 'a'
#define INSTANCIA_RESPONSE_SUCCESS 's'
#define INSTANCIA_RESPONSE_FAILED 'b'
#define INSTANCIA_NEED_TO_COMPACT 'd'

#define ESIID 'f'
#define COORDINADORID 'g'
#define PLANIFICADORID 'h'
#define INSTANCIAID 'j'
#define HANDSHAKEESIPLANIFICADOR 'k'

#define KEYSTATUSMESSAGE 'l'
#define ESIINFORMATIONMESSAGE 'z'
#define CORDINADORCONSOLERESPONSEMESSAGE 'x'

#define INSTANCIA_COMPACT_SUCCESS 'c'
#define INSTANCIA_COMPACT_FAILURE 'v'
#define INSTANCIA_DO_COMPACT 'b'
#define INSTANCIA_DO_OPERATION 'n'
#define INSTANCIA_CHECK_KEY_STATUS 'm'

#define INSTANCIA_DID_OPERATION 'Q'
#define INSTANCIA_COMPACT_REQUEST 'W'
#define INSTANCIA_DID_COMPACT 'E'
#define INSTANCIA_DID_CHECK_KEY_STATUS 'R'

#define PLANIFICADOR_STATUS_REQUEST 'T'
#define PLANIFICADOR_KEY_STATUS_RESPONSE 'Y'
#define PLANIFICADOR_ESI_ID_RESPONSE 'U'

#define STATUS_SIMULATED_INSTANCIA 'I'
#define STATUS_NOT_SIMULATED_INSTANCIA 'O'
#define STATUS_NO_INSTANCIAS_AVAILABLE 'P'
#define STATUS_NOT_SIMULATED_INSTANCIA_BUT_FALLEN 'A'

char* getCoordinadorResponseName(char coordinadorResponse);
char* getEsiInformationResponseName(char esiInformation);

#endif /* OPERATION_CODES_H_ */
