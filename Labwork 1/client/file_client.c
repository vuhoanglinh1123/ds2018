
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

const char* RECEIVED_TITLE= "0";
const char* RECEIVED_SIZE= "1";
const char* RECEIVED_FILE= "2";

int file_size(char * fName);

int main(int argc, char* argv[]) {

    //buffer to store content
    char * buffer;
    long int buffer_size;
    //
    char s[100];
    char msg[100];
    char b_size[100];
    //read file
    ssize_t read_return;
    ssize_t write_return;

    //open the file to send
    int filefd;
    char *input_file;
    char output_file[100];

    int so;
    struct sockaddr_in ad;
 
    socklen_t ad_length = sizeof(ad);
    struct hostent *hep;

    // create socket
    int serv = socket(AF_INET, SOCK_STREAM, 0);

    // init address
    hep = gethostbyname(argv[1]);
    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;
    ad.sin_addr = *(struct in_addr *)hep->h_addr_list[0];
    ad.sin_port = htons(12345);

    // connect to server
    connect(serv, (struct sockaddr *)&ad, ad_length);

    while (1) {
        
        //snippet start
        // after connected, it's client turn to chat
        do {
            printf("client> Enter file path: ");
            scanf("%s", s);
            input_file = s;

            // Open the input file
            filefd = open(input_file, O_RDONLY);
            if (filefd == -1) {
                perror("open");
            }

        } while (filefd == -1);
        close(filefd);
        //send file name
        write(serv, input_file, strlen(input_file) + 1);
        printf("Send title...\n");
        //wait for response msg
        read(serv, msg, sizeof(msg));

        //if wrong msg
        if(strcmp(msg,RECEIVED_TITLE) != 0){
           continue;
        }

        printf("Received comfirmation!\n");
         //find file size
        buffer_size = file_size(input_file);
        buffer = (char*) malloc(buffer_size + 1);
        
        //send buffer size
        
        sprintf(b_size, "%ld", buffer_size  + 1);

        write(serv, b_size, strlen(b_size));

        printf("Send file size...\n");
        //wait for response
        read(serv, msg, sizeof(msg));
        
        //if wrong msg
        if(strcmp(msg,RECEIVED_SIZE) != 0){
            continue;
        }
        printf("Received comfirmation!\n");

        //sending file
        filefd = open(input_file, O_RDONLY);
        if (filefd == -1) {
            perror("open");
        }
        // Read the input file
        read_return = read(filefd, buffer, buffer_size);
        if (read_return == -1) {
            perror("read");
        }
        // Send data to the SERVER
        write_return = write(serv, buffer, read_return);
        if (write_return == -1) {
            perror("write");
        }
        printf("Send file...\n");
        
        close(filefd);

    }
    return 0;
}


int file_size(char * fName){
    long int size;
    FILE * pFile;

    pFile = fopen(fName,"r");
    
    if(pFile == NULL){
        printf("Can't find file\n");
        fclose(pFile);
        return -1;
    }

    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);

    fclose(pFile);
    return size;
}