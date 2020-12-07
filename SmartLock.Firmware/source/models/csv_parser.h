/*
 * csv_parser.h
 *
 *  Created on: 7 dic. 2020
 *      Author: Miguel
 */

#ifndef MODELS_CSV_PARSER_H_
#define MODELS_CSV_PARSER_H_

#include "messages.h"

ResponseMessage* FromCsv(char *buffer);
char* ToCsv(RequestMessage *message);

#endif /* MODELS_CSV_PARSER_H_ */
