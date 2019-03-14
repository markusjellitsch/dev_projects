#include <QCoreApplication>
#include "parser.h"

#include <string>
#include <iostream>

using namespace  std;

uint8_t callback_scan(uint8_t * data,uint16_t len, uint8_t type){

    cout << "callback scan called" << endl;
    cout << "data:";

    if (strncmp((char const *)data,"HalloWelt",9)!=0)return 1;
    for (uint16_t i = 0; i< len;i++){

        cout << data[i];
    }
    cout << endl;
    cout << "type:" << dec  << (int)type << endl;

    return 0;
}

uint8_t send_callback(uint8_t byte){
cout << byte;
return 0;
}

int main(int argc, char *argv[])
{

    char const * text = "Scan=8212;";

    parser_init(send_callback);
    parser_set_command((uint8_t *)"Scan",4,callback_scan);
    parser_set_command((uint8_t *)"Discovery",9,0);
    parser_set_command((uint8_t *)"Test",4,0);

    int success = 0;

    cout << "Parsing starts" << endl;
    if (argc > 1){
        success = parser_parse_data((uint8_t *)argv[1],strlen(argv[1]));
    }
    else  success = parser_parse_data((uint8_t *)text,strlen(text));

    if (success == PARSER_OK){
        cout << "Parsing Success!" << endl;
    }
    else cout << "Parsing Error!" << endl;

    cout << "Result:" << dec <<  success << endl;
    return 0;

}
