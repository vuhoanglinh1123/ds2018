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

int main() {

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


    int ss, cli, pid;
    struct sockaddr_in ad;

    socklen_t ad_length = sizeof(ad);    

    //open the file to send
    int filefd;
    char *input_file;
    char output_file[100];

    // create the socket
    ss = socket(AF_INET, SOCK_STREAM, 0);

    // bind the socket to port 12345
    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY;
    ad.sin_port = htons(12345);
    bind(ss, (struct sockaddr *)&ad, ad_length);

    // then listen
    listen(ss, 0);

    printf("Server is ready\n");
    printf("Listening...\n");

    while (1) {
        // an incoming connection
        cli = accept(ss, (struct sockaddr *)&ad, &ad_length);

        pid = fork();
        if (pid == 0) {
            // I'm the son, I'll serve this client
            printf("Client connected\n");

            while (1) {
                printf("Wait for title\n");
                //wait for file name
                read(cli, output_file, sizeof(output_file));
                printf("Received title! %s\n", output_file);

                //return comfirmation
                write(cli, RECEIVED_TITLE, strlen(RECEIVED_TITLE) + 1);
                
                //wait for file size
                read(cli, b_size, strlen(b_size) + 1);
                printf("Received buffer size!\n");

                buffer_size = atol(b_size);
                buffer = (char*) malloc(buffer_size + 1);
                //return comfirmation
                printf("%ld\n", buffer_size);
                write(cli, RECEIVED_SIZE, strlen(RECEIVED_SIZE) + 1);
                
                // Read the data received from the CLIENT
                read_return = read(cli, buffer, buffer_size);
                if (read_return == -1) {
                    perror("read");
                } else {
                    printf("Received!\n");
                }

                printf("%ld\n", read_return);

                // Open the output file
                filefd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);     
                if (filefd == -1) {
                    perror("open");
                }

                // Write to the output file
                write_return = write(filefd, buffer, read_return);
                if (write_return == -1) {
                    perror("write");
                }
                close(filefd);

            }

            return 0;
        } else {
            // I'm the father, continue the loop to accept more clients
            continue;
        }
    }
    // disconnect
    close(cli);
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
