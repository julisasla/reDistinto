/*
 * consola.c
 *
 *  Created on: 17 abr. 2018
 *      Author: utnso
 */

#include "console.h"

void openConsole() {
	char* line;
	char** parameters;
	printf("Listado de comandos\n1.Pausar\n2.Continuar\n3.Bloquear <clave> <id>\n4.Desbloquear <clave>\n5.Listar <recurso>\n6.Kill <ID>\n7.Status <clave>\n8.Deadlock\n");

	while(1) {
		line = readline("> ");
		string_to_lower(line);
		parameters = string_split(line," ");

		//Borrar estos printf cuando todo funcione
		printf("Parameter quantity = %d\n", parameterQuantity(parameters));
		for(int i = 0;i < parameterQuantity(parameters); i++) {
			printf("Parametro %d = %s\n", i+1, parameters[i]);
		}

		if(line) {
			add_history(line);
		}

		if(!strncmp(line, "exit", 4)) {
			free(line);
			break;
		}

		if(validCommand(parameters)) {
			execute(parameters);
		}

		free(line);
	}
}

void execute(char** parameters) {
	char* command = parameters[0];
	int commandNumber = getCommandNumber(command);
	// Borrar cuando todo funcione
	printf("Execute command number: %d\n", commandNumber);
}

int parameterQuantity(char** parameters){
	int i=0;
	while(parameters[i]){
		i++;
	}
	return i;
}

int validCommand(char** parameters) {
	char* command = parameters[0];
	int commandNumber = getCommandNumber(command);
	int cantExtraParameters = parameterQuantity(parameters) - 1;

	switch(commandNumber) {
		case PAUSAR:
			return parameterQuantityIsValid(cantExtraParameters, 0);
		break;
		case CONTINUAR:
			return parameterQuantityIsValid(cantExtraParameters, 0);
		break;
		case BLOQUEAR:
			return parameterQuantityIsValid(cantExtraParameters, 2);
		break;
		case DESBLOQUEAR:
			return parameterQuantityIsValid(cantExtraParameters, 1);
		break;
		case LISTAR:
			return parameterQuantityIsValid(cantExtraParameters, 1);
		break;
		case KILL:
			return parameterQuantityIsValid(cantExtraParameters, 1);
		break;
		case STATUS:
			return parameterQuantityIsValid(cantExtraParameters, 1);
		break;
		case DEADLOCK:
			return parameterQuantityIsValid(cantExtraParameters, 0);
		break;

		default:
			printf("%s: command not found\n", command);
			return 0;
		break;
	}
}

int getCommandNumber(char* command) {
	if(!strcmp(command, "pausar")) {
		return PAUSAR;
	} else if(!strcmp(command, "continuar")) {
		return CONTINUAR;
	} else if(!strcmp(command, "bloquear")) {
		return BLOQUEAR;
	} else if(!strcmp(command, "desbloquear")) {
		return DESBLOQUEAR;
	} else if(!strcmp(command, "listar")) {
		return LISTAR;
	} else if(!strcmp(command, "kill")) {
		return KILL;
	} else if(!strcmp(command, "status")) {
		return STATUS;
	} else if(!strcmp(command, "deadlock")) {
		return DEADLOCK;
	} else if(atoi(command) >= 1 && atoi(command) <= 8) {
		return atoi(command);
	} else {
		return INVALID_COMMAND;
	}
}

int parameterQuantityIsValid(int cantExtraParameters, int necessaryParameters) {
	if (cantExtraParameters != necessaryParameters) {
		printf("Invalid parameter quantity\n");
		return 0;
	}
	return 1;
}
