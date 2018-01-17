
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    int so;
    char buffer[BUFSIZ];
    struct sockaddr_in ad;
    char s[100];

    //read file
    ssize_t read_return;
    ssize_t write_return;

    socklen_t ad_length = sizeof(ad);
    struct hostent *hep;

    //open the file to send
    int filefd;
    char *input_file;
    char *output_file = "output.txt";

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

        // Read the input file
        read_return = read(filefd, buffer, BUFSIZ);
        if (read_return == -1) {
            perror("read");
        }
        
        // Send data to the SERVER
        write_return = write(serv, buffer, read_return);
        if (write_return == -1) {
            perror("write");
        }

        // Read the data received from the SERVER
        read_return = read(serv, buffer, BUFSIZ);
        if (read_return == -1) {
            perror("read");
        } else {
            printf("Received!\n");
        }

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
}

