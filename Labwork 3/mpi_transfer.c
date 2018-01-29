
#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define send_tag 100
#define recv_tag 101

int file_size(char * fName){
    long int size;
    FILE * pFile;

    pFile = fopen(fName,"r");
    
    if(pFile == NULL){
        printf("File not found!\n");
        fclose(pFile);
        return -1;
    }

    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);

    fclose(pFile);
    return size;
}

int main(int argc, char **argv) {
	int filefd;

	// Root variables
	int num_procs, root_process, self_rank, to_rank;
	long int send_length;
	char *send_mess;
	ssize_t read_return;

	// Child variables
	long int recv_length;
	char *recv_mess;
	ssize_t write_return;

	MPI_Status status;

	MPI_Init(&argc, &argv);

	root_process = 0;

	/* find out MY process ID, and how many processes were started. */

	MPI_Comm_rank(MPI_COMM_WORLD, &self_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	char titles[num_procs-1][100];

	while(1) {

		if (self_rank == root_process) {
			to_rank = root_process + 1;

			do {
				printf("\nroot> Enter file name for child [%i]: ", to_rank);
				fflush(stdout);
				do {
					scanf("%s", titles[to_rank-1]);
				} while(getchar() != '\n');

				// Check if the file is valid.
            	filefd = open(titles[to_rank-1], O_RDONLY);
            	if (filefd == -1) {
                	perror("open");
                	to_rank--;
            	}

            	to_rank++;
            	close(filefd);

            } while (filefd == -1 || to_rank < num_procs);
			
			printf("Sending titles and files...\n");
			for(to_rank = 1; to_rank < num_procs; to_rank++) {
				char *title = titles[to_rank-1];

				// Sending titles...
				send_mess = title;
				send_length = strlen(send_mess) + 1;
				MPI_Send(&send_length, 1, MPI_LONG, to_rank, send_tag, MPI_COMM_WORLD);
				MPI_Send(send_mess, send_length, MPI_CHAR, to_rank, send_tag, MPI_COMM_WORLD);

				// Sending files...
				send_length = file_size(title);
				send_mess = (char*) malloc(send_length + 1);
				filefd = open(title, O_RDONLY);
				read_return = read(filefd, send_mess, send_length);

				MPI_Send(&send_length, 1, MPI_LONG, to_rank, send_tag, MPI_COMM_WORLD);
				MPI_Send(send_mess, send_length, MPI_CHAR, to_rank, send_tag, MPI_COMM_WORLD);
				
				close(filefd);
			}
			
		} else {
			
			// Receiving titles...
			MPI_Recv(&recv_length, 1, MPI_LONG, root_process, send_tag, MPI_COMM_WORLD, &status);
			recv_mess = (char *) malloc(recv_length * sizeof(char));
			MPI_Recv(recv_mess, recv_length, MPI_CHAR, root_process, send_tag, MPI_COMM_WORLD, &status);

			printf("\n---Child [%i] out of [%i]---\n", self_rank, num_procs);
			char index[] = "";

			sprintf(index, "%d", self_rank);
			strcat(index, "_");

			printf("Title received!");
			recv_mess = strcat(index, recv_mess);
			printf("New title: %s\n", recv_mess);

			filefd = open(recv_mess, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if (filefd == -1) {
                perror("open");
            }

			// Receiving files...
			MPI_Recv(&recv_length, 1, MPI_LONG, root_process, send_tag, MPI_COMM_WORLD, &status);
			recv_mess = (char *) malloc(recv_length);
			MPI_Recv(recv_mess, recv_length, MPI_CHAR, root_process, send_tag, MPI_COMM_WORLD, &status);

			printf("File received!\n");

			write_return = write(filefd, recv_mess, recv_length);
			if (write_return == -1) {
                perror("write");
            }

            close(filefd);
		}
	}

	MPI_Finalize();
}