/*
 * instanciaFunctions.h
 *
 *  Created on: 5 may. 2018
 *      Author: utnso
 */

#ifndef INSTANCIAFUNCTIONS_H_
#define INSTANCIAFUNCTIONS_H_
#define INSTANCIA_ALIVE 0
#define INSTANCIA_FALLEN 1

	#include "../../coordinador.h"

	typedef struct Instancia{
		int id;
		int socket;
		int spaceUsed;
		char firstLetter;
		char lastLetter;
		t_list* storedKeys;
		int isFallen;
		char* name;
	}Instancia;

	Instancia* existsInstanciaWithName(char* arrivedInstanciaName, t_list* instancias);
	void instanciaIsBack(Instancia* instancia);
	int instanciaDoOperationDummy();
	int instanciaDoOperation(Instancia* instancia, Operation* operation);
	Instancia* lookForKey(char* key, t_list* instancias);
	void removeKeyFromFallenInstancia(char* key, Instancia* instancia);
	void addKeyToInstanciaStruct(Instancia* instancia, char* key);
	void instanciaHasFallen(Instancia* fallenInstancia);
	int waitForInstanciaResponse(Instancia* chosenInstancia);
	int firstInstanciaBeforeSecond(Instancia* firstInstancia, Instancia* secondInstancia);
	int createNewInstancia(int instanciaSocket, t_list* instancias, int* greatesInstanciaId, char* name);
	Instancia* createInstancia(int id, int socket, int spaceUsed, char firstLetter, char lastLetter, t_list* storedKeys, char* name);
	void destroyInstancia(Instancia* instancia);

	/*-----------------------------------------------------*/

	/*
	 * TEST FUNCTIONS
	 */
	void initializeSomeInstancias(t_list* instancias);
	void showInstancia(Instancia* instancia);
	void showInstancias(t_list* instancias);
	/*
	 * TEST FUNCTIONS
	 */


#endif /* INSTANCIAFUNCTIONS_H_ */
