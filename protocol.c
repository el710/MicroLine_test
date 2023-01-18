// protofile for protocol exchange

#include <stdio.h>
#include <conio.h>
#include <time.h>
//#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define _PROT_HEADER_1  0x01
#define _PROT_HEADER_2  0x02
#define _PROT_HEADER_3  0x03
#define _PROT_HEADER_4  0x04
#define _HEADER_CRC (_PROT_HEADER_1 ^ _PROT_HEADER_2 ^ _PROT_HEADER_3 ^ _PROT_HEADER_4)
#define _MESSAGE_TITLE_SIZE 7


#define _I_ADDRESS  0xA0
#define _H_ADDRESS  0xB0


#define _DATA_SIZE  10 // bytes

#define _RECEIVE_TIMEOUT    1000 //ms


typedef enum {
    RES_OK,
    RES_TIMEOUT,
    RES_ERROR
}T_result;

typedef struct{
    unsigned char header[4];
    unsigned char length;
    unsigned char sender;
    unsigned char recipient;
    unsigned char * data;
    unsigned char crc;
} T_Message;

typedef struct{
    unsigned char fromAddress;
    unsigned char toAddress;
} T_LinkDescription;


void InitMessage(T_Message* mess, char* data){
    
    mess->header[0] = _PROT_HEADER_1;
    mess->header[1] = _PROT_HEADER_2;
    mess->header[2] = _PROT_HEADER_3;
    mess->header[3] = _PROT_HEADER_4;
    mess->length = 0;
    mess->sender = 0;
    mess->recipient = 0;
    mess->data = data;
    mess->crc = 0x0;
}

void SetUpMessage(T_LinkDescription* link, T_Message* mess, unsigned char* data, unsigned char size){

    mess->length = size;
    mess->sender = link->fromAddress;
    mess->recipient = link->toAddress;
    
    mess->crc = _HEADER_CRC ^ mess->length ^ mess->sender ^ mess->recipient;
           
    for(int i = 0; i < size; i++){
        data[i] = rand() & 0xFF;
        mess->data[i] = data[i];
        mess->crc ^= data[i];
    }    
    
}

void ShowMessage(T_Message* mess){
      
    printf("Mesage: From %x to %x\n",mess->sender, mess->recipient);    

    for(int i = 0; i < mess->length; i++){
        printf("Mesage: data %i = %x\n",i, mess->data[i]);    
    }
}

void CopyMessage(T_Message* source, T_Message* dest){

}

T_result ValidateMess(T_Message* mess){
    unsigned char* pointer = (unsigned char*) mess;
    unsigned char crc = pointer[0];

    for(int i = 1; i < _MESSAGE_TITLE_SIZE; i++) crc ^= pointer[i];
    
    for(int i = 0; i < mess->length; i++) crc ^= mess->data[i];

    if(crc == mess->crc) return RES_OK;

    return RES_ERROR;
}

T_result TryReceive(T_LinkDescription desc, T_Message* mess, unsigned char* size, int* timeout){

    char res = rand() % 10;

    if(res < 5) return RES_OK;
    if(res < 8) return RES_TIMEOUT;
        
    return RES_ERROR;  
}

T_result Transmit(T_LinkDescription desc, T_Message* mess, unsigned char size){

    return RES_OK;
}


int main(){
    
    T_LinkDescription socket;
    T_result ioResult;

    socket.fromAddress = _I_ADDRESS;
    socket.toAddress = _H_ADDRESS;
    // socket = OpenChannel();
    //...

    T_Message message;
    T_Message kvitance;

    unsigned char size; // size of data
    int timeout = _RECEIVE_TIMEOUT;

    unsigned char inData[_DATA_SIZE];
    unsigned char outData[_DATA_SIZE];

    InitMessage(&message, inData);
    InitMessage(&kvitance, outData);

     for(;;){

        // random some...
        SetUpMessage(&socket, &message, outData, _DATA_SIZE);
 
        printf("Check out for message... \n");

        ioResult = TryReceive(socket, &message, &size, &timeout);
 
        if(ioResult == RES_OK){

            if(ValidateMess(&message) == RES_OK){
                CopyMessage(&message, &kvitance);
                Transmit(socket, &kvitance, size);

                ShowMessage(&message);

                switch(message.data[0]){
                    case 0xA: // some processing...
                    case 0xB: // some processing...
                    case 0xC: // some processing...
                    case 0xD:{// some processing...
                    }
                }

            }
            else printf("Wrong CRC of message\n");
        }
        else if(ioResult == RES_TIMEOUT) { printf("No message: time out... \n"); }
        else if(ioResult == RES_ERROR) { printf(" IO Error... \n"); }
        
    
    //...

        sleep(2);
        
    }

    return 0;
}

