/*
 * instancia.c
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */

#include "esi.h"

t_log* logger;

int main(int argc, char* argv[]) {


	if (argc != 2) {
		log_error(logger, "ESI cannot execute: you must enter a script file to read");
		return -1;
	}

	char* ipCoordinador;
	char* ipPlanificador;
	int portCoordinador;
	int portPlanificador;

	getConfig(&ipCoordinador, &ipPlanificador, &portCoordinador, &portPlanificador);

	/*
	 * Script handle (with mmap)
	 * */

	int scriptFd;
	char *script;
	struct stat sb;
	char * loggerPath = getLoggerPath(argv);
	logger = log_create(loggerPath, "tpSO", true, LOG_LEVEL_INFO);
	free(loggerPath);
	initSerializationLogger(logger);

	if ((scriptFd = open(argv[1], O_RDONLY)) == -1) {
		log_error(logger, "The script file cannot be opened");
		return -1;
	}

	if (fstat(scriptFd, &sb) == -1) {
		log_error(logger, "It is not possible to determinate the script file size");
		return -1;
	}

	script = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, scriptFd, 0);

	if (script == MAP_FAILED) {
		log_error(logger, "mmap failed");
		return -1;
	}

	close(scriptFd);

	/*
	 * Handshake between esi and planificador
	 * */

	int planificadorSocket = connectToServer(ipPlanificador, portPlanificador, PLANIFICADOR, ESI, logger);
	if (planificadorSocket < 0){
		//reintentar conexion?
		return -1;
	}

	sendMyIdToServer(planificadorSocket, ESIID, ESI, logger);

	/*
	 * Handshake between esi and coordinador
	 * */

	int coordinadorSocket = connectToServer(ipCoordinador, portCoordinador, COORDINADOR, ESI, logger);
	if (coordinadorSocket < 0){
		//reintentar conexion?
		return -1;
	}

	sendMyIdToServer(coordinadorSocket, ESIID, ESI, logger);
	
	free(ipCoordinador);
	free(ipPlanificador);
	
	/*
	 * ESI wait to planificador, who will order to execute
	 * */
	waitPlanificadorOrders(planificadorSocket, script, coordinadorSocket);

	log_info(logger, "That's all folks!");
	log_destroy(logger);

	return 0;
}

char * getLoggerPath(char * argv[]){

	char **splittedPath = string_split(argv[1], "/");
	int esiNamePosition = numberOfElementsOnArray(splittedPath);
	char * esiName = strdup(splittedPath[esiNamePosition - 1]);
	for (int pos = 0; splittedPath[pos] != NULL; pos++) {
		free(splittedPath[pos]);
	}
	free(splittedPath);
	char *logPath = malloc(strlen("../") + strlen(esiName) + strlen(".log") + 1);

	sprintf(logPath, "%s%s%s", "../", esiName, ".log");
	logPath[strlen("../") + strlen(esiName) + strlen(".log")] = '\0';
	free(esiName);

	return logPath;
}
/*
 * Configuration
 * */
void getConfig(char** ipCoordinador, char** ipPlanificador, int* portCoordinador, int* portPlanificador) {

	t_config* config;
	config = config_create(CFG_FILE);
	*ipCoordinador = strdup(config_get_string_value(config, "IP_COORDINADOR"));
	*ipPlanificador = strdup(config_get_string_value(config, "IP_PLANIFICADOR"));
	*portCoordinador = config_get_int_value(config, "PORT_COORDINADOR");
	*portPlanificador = config_get_int_value(config, "PORT_PLANIFICADOR");
	config_destroy(config);
}

/*
 * Interaction with coordinador and planificador
 * */

void waitPlanificadorOrders(int planificadorSocket, char * script, int coordinadorSocket) {
	char * line = NULL;
	char ** scriptsSplitted = string_split(script, "\n");
	size_t len = numberOfElementsOnArray(scriptsSplitted);
	int esiPC = 0; // ESI program counter

	int response;

	log_info(logger, "Number of sentences: %d", len);

	while (esiPC < len && empty_string(line = scriptsSplitted[esiPC]) == 0) {

		/*
		 * Parser tries to understand each line, one by one (when planificador says)
		 * */

		log_info(logger, "Waiting planificador order...");
		if (recieveInt(&response, planificadorSocket) == CUSTOM_FAILURE) {
			log_error(logger, "recv failed on trying to connect with planificador %s", strerror(errno));
			exit(-1);
		}
		log_info(logger, "I recieve the order from planificador and I will try to execute");

		switch (response) {
			case RUN:
				log_info(logger, "recv an order from planificador");
				tryToExecute(planificadorSocket, line, coordinadorSocket, &esiPC, len);
				break;			

			case KILLESI:
				log_info(logger, "Planificador kill me by console");
				exit(-1);
				break;
		}

		log_info(logger, "esiPC: %d", esiPC);
	}

	if (scriptsSplitted) {
		free(scriptsSplitted);
	}

	if (script) {
		munmap(script, strlen(script) + 1);
	}
}

void tryToExecute(int planificadorSocket, char * line, int coordinadorSocket, int * esiPC, size_t len) {

	char coordinadorResponse, status = NOTFINISHED;

	/*
	 *  Try to read the line that correspond to esiCP
	 *  Send serialized operation to coordinador
	 *  Wait (recv) operation response from coordinador
	 *  	Success response: iterate esiCP
	 *  	Error response: try to execute same line when planificador says
	 *  Send my response to planificador (ending, success, failure)
	 */

	Operation * operation = malloc(sizeof(Operation));
	interpretateOperation(operation, line);

	if (sendOperation(operation, coordinadorSocket) == CUSTOM_FAILURE) {
		log_error(logger, "ESI cannot send the serialized operation to coordinador", line);
		exit(-1);
	}

	destroy_operation(operation);

	log_info(logger, "Waiting for coordinador response...");
	if (recv_all(coordinadorSocket, &coordinadorResponse, sizeof(char)) == CUSTOM_FAILURE) {
		log_error(logger, "recv failed on try to get the coordinador operation response", line);
		exit(-1);
	}
	log_info(logger, "Recieved coordindador response: %s", getCoordinadorResponseName(coordinadorResponse));

	*esiPC += (coordinadorResponse == SUCCESS || coordinadorResponse == LOCK || coordinadorResponse == FREE ? 1 : 0);

	status = (*esiPC == len ? FINISHED : NOTFINISHED);

	// Create and send the operationResponse
	OperationResponse operationResponse;
	initializeOperationResponse(&operationResponse, coordinadorResponse, status);

	log_info(logger, "I will send the coordinador response to planificador %s", getCoordinadorResponseName(coordinadorResponse));
	char message = ESIINFORMATIONMESSAGE;

	void* package = NULL;
	int offset = 0;

	addToPackageGeneric(&package, &message, sizeof(char), &offset);
	addToPackageGeneric(&package, &operationResponse, sizeof(OperationResponse), &offset);

	if (send_all(planificadorSocket, package, offset) == CUSTOM_FAILURE) {
		log_error(logger, "ESI cannot send the operation response to planificador", line);
		exit(-1);
	}

	log_info(logger, "I could send the response to planificador");
	free(package);

	if (coordinadorResponse == ABORT) {
		log_error(logger, "I cannot keep running", line);
		exit(-1);
	}

	if ((coordinadorResponse == SUCCESS || coordinadorResponse == LOCK || coordinadorResponse == FREE) && line) {
		free(line);
	}
}

void interpretateOperation(Operation * operation, char * line) {
	t_esi_operacion parsedLine = parse(line);

	if (!parsedLine.valido) {
		log_error(logger, "Parsi cannot understand the line %s", line);
		exit(-1);
	}

	switch (parsedLine.keyword) {
		case GET:
			initializeOperation(operation, OURGET, parsedLine.argumentos.GET.clave, NULL);
			break;

		case SET:
			if (empty_string(parsedLine.argumentos.SET.valor) == 1) {
				log_error(logger, "ESI cannot parse a SET without value");
				exit(-1);
			}
			initializeOperation(operation, OURSET, parsedLine.argumentos.SET.clave, parsedLine.argumentos.SET.valor);
			break;

		case STORE:
			initializeOperation(operation, OURSTORE, parsedLine.argumentos.STORE.clave, NULL);
			break;

		default:
			log_error(logger, "Parsi could not understand the keyword %s", line);
			exit(-1);
	}

	showOperation(operation, logger);

	destruir_operacion(parsedLine);
}

void initializeOperation(Operation * operation, char operationCode, char * key, char * value) {
	operation->operationCode = operationCode;

	operation->key = malloc(strlen(key) + 1);
	strcpy(operation->key, key);

	if (value != NULL) {
		operation->value = malloc(strlen(value) + 1);
		strcpy(operation->value, value);
	} else {
		operation->value = NULL;
	}
}

void initializeOperationResponse(OperationResponse * operationResponse, char coordinadorResponse, char status) {
	operationResponse->coordinadorResponse = coordinadorResponse;
	operationResponse->esiStatus = status;
}

void destroy_operation(Operation * operation) {
	if (empty_string(operation->key) == 0)
		free(operation->key);

	if (empty_string(operation->value) == 0)
		free(operation->value);

	free(operation);
}
