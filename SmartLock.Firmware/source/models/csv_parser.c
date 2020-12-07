/*
 * csv_parser.c
 *
 *  Created on: 7 dic. 2020
 *      Author: Miguel
 */

#include "stdint.h"
#include <string.h>
#include "messages.h"

#define MAX_PACKAGE_SIZE 43

char GetMessageType(MessageType type){
	switch(type){
		case AUTH:
			return '0';
		case REGISTER:
			return '1';
		case RESPONSE:
			return '2';
		default:
			return '9';
	}
}

MessageType ToMessageType(char type){
	return (MessageType)(type - '0');
}

void SetMessageField(ResponseMessage* message, char* buffer, int k) {
    switch (k) {
        case 0:
            message->type = ToMessageType(buffer[0]);
            break;
        case 1:
        	message->result = TRUE;
            break;
        default:
            break;
    }
    memset(buffer, 0, 20);
}

ResponseMessage* FromCsv(char *csv) {
	ResponseMessage *result = (ResponseMessage *)malloc(sizeof(ResponseMessage));
	if(NULL == result) return NULL;
	char current;
	char buffer[20];
	uint32_t i = 0, j = 0, k = 0;
	while((current = csv[i++]) != '\0'){
		if(current == ','){
			buffer[j + 1] = '\0';
			k++;
			j = 0;
		} else {
			memcpy(buffer + j++, &current, sizeof(char));
		}
	}


	return result;
}

char* ToCsv(RequestMessage *message){
	char *payload = (char *)malloc(MAX_PACKAGE_SIZE); // max size for package
	if(NULL == payload) return NULL;

	payload[0] = GetMessageType(message->type);
	payload[1] = ',';
	uint32_t i = 0;
	for(i = 0; i < 20; i++){
		snprintf(payload + 2 + (i * 2), MAX_PACKAGE_SIZE, "%0*x", 2, message->thumbprint[i]);
	}
	payload[42] = '\0';

	return payload;
}
