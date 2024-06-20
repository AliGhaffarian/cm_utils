#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define INVALID_ARGS_MSG "invalid args"
#define BUFFERSIZE 128
#define FD_ARR_SIZE 2
void finish_and_exit(int* fd_to_close, int err_code){
	int i = 0;
	while(i < FD_ARR_SIZE){
			close(fd_to_close[i++]);
	}
	if (err_code){
			printf("err occured during something, err code : %d\n", err_code);
	}
	exit(err_code);
}
int main(int argc, char** argv){

		char* paths[FD_ARR_SIZE];
		int fd_list[FD_ARR_SIZE];

		int src = open(argv[1], O_RDONLY);
		paths[0] = argv[1];
		fd_list[0] = src;

		int dst = open(argv[2], O_WRONLY | O_CREAT);
		paths[1] = argv[2];
		fd_list[1] = dst;

		char buff[BUFFERSIZE];
		int read_bytes = 0;
		int write_bytes = 0;

		while (1){
				read_bytes = read( src, buff, BUFFERSIZE);
				if(read_bytes == -1){
						printf("err during reading from %s\n", argv[1]);
						finish_and_exit(fd_list, 1);
				}

				write_bytes = write( dst, buff, read_bytes);
				if(write_bytes == -1){
						printf("err during writing to %s\n", argv[2]);
						finish_and_exit(fd_list, 1);
				}
				if (read_bytes != BUFFERSIZE){
						finish_and_exit(fd_list, 0);
				}
				printf("transfered %d bytes\n", read_bytes);
		}

}
