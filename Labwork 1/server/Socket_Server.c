#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

int main() {
    int ss, cli, pid;
    struct sockaddr_in ad;
    char buffer[BUFSIZ];
    char s[100];

    socklen_t ad_length = sizeof(ad);

    //read file
    ssize_t read_return;
    ssize_t write_return;

    //open the file to send
    int filefd;
    char *input_file;
    char *output_file = "output.png";

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
                // it's client turn to chat, I wait and read message from client

                // Read the data received from the CLIENT
                read_return = read(cli, buffer, BUFSIZ);
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

                do {
                    printf("server> Enter file path: ");
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

                // Send data to the CLIENT
                write_return = write(cli, buffer, read_return);
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
