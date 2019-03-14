#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <string.h>


#define PARSER_OK                0
#define PARSER_ERROR_NULLPTR     1
#define PARSER_ERROR_COMMAND_BUFFER_FULL 2
#define PARSER_ERROR_COMMAND_KEYWORD_LEN_EXCEEDED 3
#define PARSER_ERROR_NO_END_DELIMITER 4
#define PARSER_ERROR_KEYWORD_NOT_EXISTING 5
#define PARSER_ERROR_INVALID_REQUEST_COMMAND 6
#define PARSER_ERROR_KEYWORD_LEN_EXCEEDED 7


#define PARSER_BUFFER_SIZE      255
#define PARSER_MAX_COMMANDS     25
#define PARSER_KEYWORD_MAX_LEN  20

#define PARSER_END_DELIMITER ';'

#define PARSER_REQUEST_BYTE '?'
#define PARSER_COMMAND_BYTE '='

#define PARSER_TYPE_REQUEST 0
#define PARSER_TYPE_COMMAND 1

#define PARSER_SUCCESS_STRING  "Command succesfully executed!\n"
#define PARSER_ERROR_STRING  "Error executing command!\n"

typedef uint8_t (*fp_send_t)(uint8_t byte);
typedef uint8_t (*fp_callback_t)(uint8_t * data, uint16_t const len,uint8_t const type);
typedef struct {
    uint8_t keyword[PARSER_KEYWORD_MAX_LEN];
    uint8_t  len_keyword;
    fp_callback_t callback;
}command_t;



uint8_t parser_init(fp_send_t send);
uint8_t parser_parse_data(uint8_t * data,uint16_t const len);
uint8_t parser_reset_state_machine();

uint8_t parser_set_command(uint8_t * data, uint8_t data_len,fp_callback_t callback);

#endif // PARSER_H
