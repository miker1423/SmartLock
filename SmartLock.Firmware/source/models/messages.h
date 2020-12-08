/*
 * messages.h
 *
 *  Created on: 1 dic. 2020
 *      Author: Miguel
 */

#ifndef MODELS_MESSAGES_H_
#define MODELS_MESSAGES_H_

#define SERVO_OPEN_BIT (1 << 0)

#define AUTH_AUTHENTICATE (1 << 0)
#define AUTH_REGISTER (1 << 1)

#define READY_BIT (1 << 0)

typedef enum {
	AUTH,
	REGISTER,
	RESPONSE
} MessageType;

typedef enum {
	TRUE,
	FALSE
} BoolType;

typedef struct {
	MessageType type;
	char thumbprint[20];
} RequestMessage;

typedef struct {
	MessageType type;
	BoolType result;
} ResponseMessage;


#endif /* MODELS_MESSAGES_H_ */
