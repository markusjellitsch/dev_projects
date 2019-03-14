#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <parser.h>

static fp_send_t m_fp_send= 0;

static command_t m_command_table[PARSER_MAX_COMMANDS];
static uint8_t num_commands = 0;

int8_t is_keyword_existing(uint8_t * data, uint8_t data_len);
uint8_t is_buffer_full();
uint8_t send_back_data(uint8_t * data, uint16_t data_len);


uint8_t parser_init(fp_send_t send){

    m_fp_send = send;

    // set to zero
    memset(m_command_table,0,sizeof(command_t)*PARSER_MAX_COMMANDS);

    return PARSER_OK;

}

uint8_t parser_set_command(uint8_t * data, uint8_t data_len,fp_callback_t callback){

    if (data == 0 || data_len == 0) return PARSER_ERROR_NULLPTR;

    if (is_buffer_full())return PARSER_ERROR_COMMAND_BUFFER_FULL;

    if (data_len > PARSER_KEYWORD_MAX_LEN) return PARSER_ERROR_KEYWORD_LEN_EXCEEDED;

    memcpy(m_command_table[num_commands].keyword, data,data_len);
    m_command_table[num_commands].callback = callback;
    m_command_table[num_commands].len_keyword = data_len;

    num_commands++;

    return PARSER_OK;
}


uint8_t parser_parse_data(uint8_t * data,uint16_t const len){

    if (data == 0 || len == 0) return PARSER_ERROR_NULLPTR;

    // check delimiter (';');
    if (data[len-1] != PARSER_END_DELIMITER){
        send_back_data((uint8_t*)PARSER_ERROR_STRING,strlen(PARSER_ERROR_STRING));
        return PARSER_ERROR_NO_END_DELIMITER;
    }
    // check type
    uint8_t type;
    uint8_t end_index = 0;
    if (data[len-2] == PARSER_REQUEST_BYTE){
        end_index = len -2;
        type = PARSER_TYPE_REQUEST;
    }
    else{
        type = PARSER_TYPE_COMMAND;
        char * tmp = strchr((const char *)data,PARSER_COMMAND_BYTE);
        if (tmp != NULL) end_index = (uint8_t)(tmp-(char*)data);
        else {
            send_back_data((uint8_t*)PARSER_ERROR_STRING,strlen(PARSER_ERROR_STRING));
            return PARSER_ERROR_INVALID_REQUEST_COMMAND;
        }
    }

    // check if extracted command exists
    int8_t index = is_keyword_existing(data,end_index);
    if (index == PARSER_ERROR_KEYWORD_NOT_EXISTING) {
        send_back_data((uint8_t*)PARSER_ERROR_STRING,strlen(PARSER_ERROR_STRING));
        return PARSER_ERROR_KEYWORD_NOT_EXISTING;
      }


    fp_callback_t tmp_callback = m_command_table[index].callback;


    // return here when no callback exists
    if (tmp_callback == 0){
       send_back_data((uint8_t*)PARSER_SUCCESS_STRING,strlen(PARSER_SUCCESS_STRING));
       return PARSER_OK;
     }

    // callback send
    uint8_t success = 0;
    if (type == PARSER_TYPE_REQUEST){
        success =  tmp_callback(0,0,PARSER_TYPE_REQUEST);
    }

   else {

        uint8_t  value_start = end_index +1;
        success =  tmp_callback((uint8_t*)(data+value_start),len-value_start-1,PARSER_TYPE_COMMAND);
    }

    // send back success/error
    if (success == PARSER_OK){
          send_back_data((uint8_t*)PARSER_SUCCESS_STRING,strlen(PARSER_SUCCESS_STRING));
    }
    else send_back_data((uint8_t*)PARSER_ERROR_STRING,strlen(PARSER_ERROR_STRING));


    return success;
}


uint8_t is_buffer_full(){
    return num_commands >= PARSER_KEYWORD_MAX_LEN;
}

int8_t is_keyword_existing(uint8_t * data, uint8_t data_len){

 if (data_len > PARSER_KEYWORD_MAX_LEN) return PARSER_ERROR_KEYWORD_LEN_EXCEEDED;

  for (uint8_t i = 0;i<num_commands;i++){

      if (data_len == m_command_table[i].len_keyword){
          if (memcmp(data,m_command_table[i].keyword,data_len)==0){
              return i;
          }
      }
  }

  return PARSER_ERROR_KEYWORD_NOT_EXISTING;

}

uint8_t send_back_data(uint8_t * data, uint16_t data_len){

    if (data == 0 || data_len == 0)return PARSER_ERROR_NULLPTR;

    if (m_fp_send != 0){

        for (uint16_t i = 0; i<data_len;i++){
            m_fp_send(data[i]);
        }
    }
    else return PARSER_ERROR_NULLPTR;

    return PARSER_OK;
}
