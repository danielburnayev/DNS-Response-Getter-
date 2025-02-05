#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define UNSIGN_SHORT_MAX 65535
#define PORT 53

//12 bytes of size
typedef struct Header {
    unsigned short id;

    unsigned char qdCount;
    unsigned char anCount; 
    unsigned char nsCount; 
    unsigned char arCount; 

    unsigned char opCode : 4;
    unsigned char rCode : 4;
    unsigned char z : 3;
    unsigned char qr : 1; //0 for query and 1 for response
    unsigned char aa : 1;
    unsigned char tc : 1;
    unsigned char rd : 1;
    unsigned char ra : 1;
} Header;

typedef struct Question {
    unsigned char* qName; 
    unsigned short qType; //set to 1 for this project. Only observing type A records
    unsigned short qClass; //set to 1 since only IN is the one that really used in anything
} Question;

typedef struct Answer {
} Answer;

typedef struct Authority {
} Authority;

typedef struct Additional {
} Additional;

typedef struct DNSMessage {
    Header* header;
    Question* question;
    Answer* answer;
    Authority* authority;
    Additional* additional;
} DNSMessage;

unsigned char* convertHostName(char* hostName);
unsigned char* dnsClientIntoBinaryByteArray(DNSMessage client);
void printResponseContent(unsigned char* response);
int* getFlagsFromResponse(int* index, unsigned char* response);
unsigned int getIntegerFromResponse(int* index, int hexDigits, unsigned char* response);
short hexDigitToDecimalDigit(char givenValue);
char* getNameFromResponse(int* index, unsigned char* response);
char* binaryByteArrayIntoHexString(unsigned char response[], unsigned long responseLength);
void charDigitIntoHexDigit(char* charDigit);
unsigned char* ipAddressFromHexString(int* index, unsigned char* response);
unsigned char getLeadingBitsFrom16BitNum(short num);
unsigned char getEndingBitsFrom16BitNum(short num);

//runner for the client side
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("The command line must have the following format: \nmy-dns-client <host-name>\n");
        return -1;
    }

    srand(time(0));

    // read host name
    char* hostName = argv[1];
    DNSMessage clientMessage;

    printf("Preparing DNS query...\n");

    clientMessage.header = (Header*)malloc(sizeof(Header));
    clientMessage.header->id = rand() % (UNSIGN_SHORT_MAX + 1); //randomly sets the id
    clientMessage.header->qr = 0; //0 because it's a query
    clientMessage.header->opCode = 0; //0 because it's a standard query
    clientMessage.header->aa = 0; //0 because it doesn't matter for queries
    clientMessage.header->tc = 0; //0 because this isn't long enough to get truncated 
    clientMessage.header->rd = 1; //1 because we want that recursive query support
    clientMessage.header->ra = 0; //0 because recursive query support isn't available at name server
    clientMessage.header->z = 0; //0 because it has to be
    clientMessage.header->rCode = 0; //0 because there are no errors (so far)
    clientMessage.header->qdCount = 1; //1 because we have one question (to gmu.edu)
    clientMessage.header->anCount = 0; //0 because this query has no answer RRs
    clientMessage.header->nsCount = 0; //0 because this query has no authority RRs
    clientMessage.header->arCount = 0; //0 because this query has no additional RRs

    clientMessage.question = (Question*)malloc(sizeof(Question));
    clientMessage.question->qName = convertHostName(hostName);
    clientMessage.question->qType = 1;
    clientMessage.question->qClass = 1;

    clientMessage.answer = (Answer*)malloc(sizeof(Answer));
    clientMessage.answer = NULL;

    clientMessage.authority = (Authority*)malloc(sizeof(Authority));
    clientMessage.authority = NULL;

    clientMessage.additional = (Additional*)malloc(sizeof(Additional));
    clientMessage.additional = NULL;

    // Socket stuff to send to the server
    int clientSocket;
    struct sockaddr_in serverAddress;
    unsigned char* message = dnsClientIntoBinaryByteArray(clientMessage);
    unsigned char* response = (unsigned char*)malloc(sizeof(char) * (UNSIGN_SHORT_MAX + 1));

    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Socket couldn't be created!\n");
        return -2;
    }

    struct timeval timeoutStruct;
    timeoutStruct.tv_sec = 5;
    timeoutStruct.tv_usec = 0;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutStruct, sizeof(timeoutStruct)) < 0) {
        printf("Socket couldn't be set\n");
        return -3;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("8.8.8.8"); 

    printf("Contacting DNS query...\n");
    printf("Sending DNS query...\n");

    int serverAddressSize;

    if (sendto(clientSocket, (char*)message, 12 + (strlen((char*)clientMessage.question->qName) + 1) + 4, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Message was not able to be sent.\n");
        return -4;
    }

    serverAddressSize = sizeof(serverAddress);
    char attemptTracker;

    for (attemptTracker = 1; attemptTracker <= 3; attemptTracker++) {
        if (recvfrom(clientSocket, (char*)response, UNSIGN_SHORT_MAX + 1, 0, (struct sockaddr*)&serverAddress, (socklen_t*)&serverAddressSize) < 0) {
            printf("Message could not be received.\n");
            return -5;
        }

        if (response[2] > 0) {break;} //check the id to see if it's changed (it has to since the qr bit is set to 1 for responses)
        printf("DNS response not received (attempt %d out of 3)\n", attemptTracker);
        printf("Sending DNS query again...\n");
    }
    if (response[2] == 0) {
        printf("Timeout Error. Couldn't receive any messages in 15 seconds.\n");
        return -6;
    } 
    printf("DNS response received! (Attempt %d out of 3)\n", attemptTracker);

    printf("Processing DNS response...\n");

    char* responseString = (char*)malloc(sizeof(char) * (UNSIGN_SHORT_MAX * 2) + 1);
    strcpy(responseString, binaryByteArrayIntoHexString(response, UNSIGN_SHORT_MAX)); 
    printResponseContent((unsigned char*)responseString);
}

unsigned char* convertHostName(char* hostName) {
    unsigned char* binaryByteArray = (unsigned char*)malloc(sizeof(char) * strlen(hostName) + 2);
    int len = 0;
    int i;
    int index = 1;

    for (i = 0; i < strlen(hostName) + 1; i++) {
        if (hostName[i] == '.' || hostName[i] == '\0') {
            binaryByteArray[i - len] = len;
            len = 0;
        }
        else {
            binaryByteArray[i + 1] = hostName[i];
            len++;
        }
    }
    binaryByteArray[i] = '\0';

    return binaryByteArray;
}

//0 values end the 
unsigned char* dnsClientIntoBinaryByteArray(DNSMessage client) {
    Header* clientHeader = client.header;
    Question* clientQuestion = client.question;
    unsigned char* message = (unsigned char*)malloc(sizeof(char) * (12 + (strlen((char*)clientQuestion->qName) + 1) + 4));
    int qIndex;

    //id
    message[0] = getLeadingBitsFrom16BitNum(clientHeader->id);
    message[1] = getEndingBitsFrom16BitNum(clientHeader->id);

    //Flags
    message[2] = (clientHeader->qr << 7) + (clientHeader->opCode << 3) + (clientHeader->aa << 2) + (clientHeader->tc << 1) + clientHeader->rd;
    message[3] = (clientHeader->ra << 7) + (clientHeader->z << 4) + (clientHeader->rCode);

    //QDCOUNT
    message[4] = getLeadingBitsFrom16BitNum(clientHeader->qdCount);
    message[5] = getEndingBitsFrom16BitNum(clientHeader->qdCount);

    //ANCOUNT
    message[6] = getLeadingBitsFrom16BitNum(clientHeader->anCount);
    message[7] = getEndingBitsFrom16BitNum(clientHeader->anCount);

    //NSCOUNT
    message[8] = getLeadingBitsFrom16BitNum(clientHeader->nsCount);
    message[9] = getEndingBitsFrom16BitNum(clientHeader->nsCount);

    //ARCOUNT
    message[10] = getLeadingBitsFrom16BitNum(clientHeader->arCount);
    message[11] = getEndingBitsFrom16BitNum(clientHeader->arCount);

    //qName
    for (qIndex = 12; qIndex < 12 + strlen((char*)clientQuestion->qName) + 1; qIndex++) {
        message[qIndex] = clientQuestion->qName[qIndex - 12];
    }

    //qType
    message[qIndex++] = getLeadingBitsFrom16BitNum(clientQuestion->qType);
    message[qIndex++] = getEndingBitsFrom16BitNum(clientQuestion->qType);

    //qClass
    message[qIndex++] = getLeadingBitsFrom16BitNum(clientQuestion->qClass);
    message[qIndex++] = getEndingBitsFrom16BitNum(clientQuestion->qClass);

    return message;
}

void printResponseContent(unsigned char* response) {
    int index = 0;
    int questionCount = 0;
    int answerCount = 0;
    int authorityCount = 0;
    int additionalCount = 0;
    int* flags;
    char* name;
    
    printf("--------------------------------------------------------------------------------\n");
    printf("                             DNS Response's Values:                             \n");

    printf("Header:\n");
    printf("\theader.ID = %d\n", getIntegerFromResponse(&index, 4, response));
    
    flags = getFlagsFromResponse(&index, response);
    printf("\theader.QR = %d\n", flags[0]);
    printf("\theader.OPCODE = %d\n", flags[1]);
    printf("\theader.AA = %d\n", flags[2]);
    printf("\theader.TC = %d\n", flags[3]);
    printf("\theader.RD = %d\n", flags[4]);
    printf("\theader.RA = %d\n", flags[5]);
    printf("\theader.Z = %d\n", flags[6]);
    printf("\theader.RCODE = %d\n", flags[7]);

    questionCount = getIntegerFromResponse(&index, 4, response);
    answerCount = getIntegerFromResponse(&index, 4, response);
    authorityCount = getIntegerFromResponse(&index, 4, response);
    additionalCount = getIntegerFromResponse(&index, 4, response);

    printf("\theader.QDCOUNT = %d\n", questionCount);
    printf("\theader.ANCOUNT = %d\n", answerCount);
    printf("\theader.NSCOUNT = %d\n", authorityCount);
    printf("\theader.ARCOUNT = %d\n", additionalCount);
    printf("\n");

    int ques;
    printf("Questions:\n");
    for (ques = 1; ques <= questionCount; ques++) {
        printf("\tQuestion %d:\n", ques);

        name = getNameFromResponse(&index, response);
        printf("\t\tquestion.QNAME = %s\n", name); 
        printf("\t\tquestion.QTYPE = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tquestion.QCLASS = %d\n", getIntegerFromResponse(&index, 4, response));
    }
    printf("\n");

    int answer;
    printf("Answers:\n");
    for (answer = 1; answer <= answerCount; answer++) {
        printf("\tAnswer %d:\n", answer);

        printf("\t\tanswer.NAME = %s\n", name);
        getIntegerFromResponse(&index, 4, response); //need to make sure the index is at the right position
        printf("\t\tanswer.TYPE = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tanswer.CLASS = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tanswer.TTL = %u\n", getIntegerFromResponse(&index, 8, response));
        printf("\t\tanswer.RDLENGTH = %u\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tanswer.RDATA = %s\n", ipAddressFromHexString(&index, response));
    }
    printf("\n");

    int auth;
    printf("Authoritative:\n");
    for (auth = 1; auth <= authorityCount; auth++) {
        printf("\tAuthority %d:\n", auth);

        printf("\t\tauthority.NAME = %s\n", name); 
        getIntegerFromResponse(&index, 4, response);
        printf("\t\tauthority.TYPE = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tauthority.CLASS = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tauthority.TTL = %u\n", getIntegerFromResponse(&index, 8, response));
        printf("\t\tauthority.RDLENGTH = %u\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\tauthority.RDATA = %s%s\n", getNameFromResponse(&index, response), name);
    }
    printf("\n");

    int add;
    printf("Additional:\n");
    for (add = 1; add <= additionalCount; add++) {
        printf("\tAddition %d:\n", add);
        
        printf("\t\taddition.NAME = %d\n", getIntegerFromResponse(&index, 4, response)); 
        printf("\t\taddition.TYPE = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\taddition.CLASS = %d\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\taddition.TTL = %u\n", getIntegerFromResponse(&index, 8, response));
        printf("\t\taddition.RDLENGTH = %u\n", getIntegerFromResponse(&index, 4, response));
        printf("\t\taddition.RDATA = %s\n", ipAddressFromHexString(&index, response));
    }
    printf("\n");

    printf("--------------------------------------------------------------------------------\n");

}

int* getFlagsFromResponse(int* index, unsigned char* response) {
    int* flags = (int*)malloc(sizeof(int) * 8);
    int flagsInt = getIntegerFromResponse(index, 4, response);
    
    flags[7] = flagsInt % 16;
    flagsInt >>= 4;

    flags[6] = flagsInt % 8;
    flagsInt >>= 3;

    int i;
    for (i = 5; i >= 2; i--) {
        flags[i] = flagsInt % 2;
        flagsInt >>= 1;
    }

    flags[1] = flagsInt % 16;
    flagsInt >>= 4;

    flags[0] = flagsInt % 2;
    
    return flags;
}

char* getNameFromResponse(int* index, unsigned char* response) {
    int num = 0;
    int length = 0;
    int labelCharCounter = 0;
    char* domainName = (char*)malloc((sizeof(char) * 255)); //Has a max size of 255 bytes
 
    do {
        if (!num || labelCharCounter == num) {
            num = getIntegerFromResponse(index, 2, response);
            if (num) {domainName[length - 1] = '.';}
            labelCharCounter = 0;
        }
        else {
            labelCharCounter++;
            domainName[length - 1] = getIntegerFromResponse(index, 2, response);
        }
        length++;
    }
    while (num != 0 && num != 0xc0 && num != 0x0c);

    return domainName;
}

unsigned int getIntegerFromResponse(int* index, int hexDigits, unsigned char* response) {
    unsigned int num = 0;
    int start = *index;
    int multiple = 1 << (4 * (hexDigits - 1));
    short value;

    for ((*index) = start; (*index) < start + hexDigits; (*index)++) {
        value = hexDigitToDecimalDigit(response[(*index)]);
        num += value * multiple;
        multiple >>= 4;
    }

    return num;
}

short hexDigitToDecimalDigit(char givenValue) {
    short value = givenValue;

    if (value >= 48 && value <= 57) {value -= 48;} //for 0 to 9
    else if (value >= 65 && value <= 70) {value -= 55;} //for A to E
    else {value -= 87;} //for a to e
    
    return value;
}

char* binaryByteArrayIntoHexString(unsigned char response[], unsigned long responseLength) {
    char* string = (char*)malloc(sizeof(char) * responseLength * 2 + 1);
    int i;
    char front;
    char back;

    for (i = 0; i < responseLength; i++) {
        front = (response[i] >> 4) % 16;
        back = response[i] % 16;
        
        charDigitIntoHexDigit(&front);
        charDigitIntoHexDigit(&back);
        
        string[i * 2] = front;
        string[i * 2 + 1] = back;
    }

    return string;
}

void charDigitIntoHexDigit(char* charDigit) {
  (*charDigit) += ((*charDigit) < 10) ? 48 : 87;
}

unsigned char* ipAddressFromHexString(int* index, unsigned char* response) {
    unsigned int selectNum;
    int count;
    char front;
    char middle;
    int i = 0;
    unsigned char* ip = (unsigned char*)malloc(sizeof(char) * 16); 

    for (count = 0; count < 4; count++) {
        selectNum = getIntegerFromResponse(index, 2, response);
        
        front = selectNum / 100 % 10;
        middle = selectNum / 10 % 10;

        if (front > 0) {
            ip[i++] = front + 48;
            ip[i++] = middle + 48;
        }
        else if (middle > 0) {ip[i++] = middle + 48;}
        ip[i++] = (selectNum % 10) + 48;
        ip[i++] = '.';
    }
    ip[--i] = '\0';

    return ip;
}

unsigned char getLeadingBitsFrom16BitNum(short num) {
    return (num >> 8) % 256;
}

unsigned char getEndingBitsFrom16BitNum(short num) {
    return num % 256;
}