#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#define INVALID_ARGS_MSG "invalid args"
#define FD_ARR_SIZE 2
#define OPEN_FILE_ERR_MSG "error during openning file"

#define DEBUG_PREFIX "DEBUG : "
#define VERBOSE_PREFIX "VERBOSE : "

int do_create = 0;
int block_size = 128;
int do_debug = 0;
int do_verbose = 0;
int overwrite = 0;
int creat_chmod_mask = S_IRUSR | S_IRGRP | S_IROTH;
char* src_path = NULL;
char* dst_path = NULL;

/*
	options to do:
	force ( overwrite )
	check for overwrite and prompt
*/

void finish_and_exit(int* fd_to_close){
	int i = 0;
	while(i < FD_ARR_SIZE){
			close(fd_to_close[i++]);
	}
	exit(errno);
}


void init_args(int argc, char** argv){
		int opt;
		while((opt = getopt(argc, argv, "cdvfb:")) != -1){
				switch(opt){
						case 'd':{
										 do_debug = 1;
										 break;
								 }
						case 'v':{
										 do_debug = 1;
										 break;
								 }
						case 'b':{
										 block_size = atoi(optarg);
										 break;
								 }
						case 'f':{
										 overwrite = 1;
										 break;
								 }

						case 'm':{
										 //will get to this later
										 creat_chmod_mask = atoi(optarg);
										 break;

								 }
						case 'c':{
										 do_create = 1;
										 break;
								 }
						default:break;
				}
		}

		src_path = argv[argc - 2];
		dst_path = argv[argc - 1];

		if (do_debug){
				printf("%sdebug flag : %d\n", DEBUG_PREFIX ,do_debug);
				printf("%sverbose flag : %d\n", DEBUG_PREFIX, do_verbose);
				printf("%soverwrite flag : %d\n", DEBUG_PREFIX, overwrite);
				printf("%sblock size : %d\n", DEBUG_PREFIX, block_size);
				printf("%ssource path : %s\n", DEBUG_PREFIX, src_path);
				printf("%sdestination path : %s\n", DEBUG_PREFIX, dst_path);
				printf("%schmod perms for created files : %d\n", DEBUG_PREFIX, creat_chmod_mask);
				printf("%screate destination file if doesnt exist : %d\n", DEBUG_PREFIX, do_create);
		}
} 

int main(int argc, char** argv){
		init_args(argc, argv);
		char* paths[FD_ARR_SIZE];
		int fd_list[FD_ARR_SIZE];

		int src = open(src_path, O_RDONLY);
		if(src == -1){
				printf("%s", OPEN_FILE_ERR_MSG);
				printf(": %s\n", src_path);
				finish_and_exit(fd_list);
		}

		fd_list[0] = src;

		int dst = open(dst_path, O_WRONLY);
		if (dst == -1 && do_create){
			dst = open(dst_path, O_WRONLY | O_CREAT);
			chmod(dst_path, creat_chmod_mask);
		}
		if(dst == -1){
				printf("%s", OPEN_FILE_ERR_MSG);
				printf(": %s\n", dst_path);
				finish_and_exit(fd_list);
		}


		fd_list[1] = dst;

		char buff[block_size];
		int read_bytes = 0;
		int write_bytes = 0;

		while (1){
				read_bytes = read( src, buff, block_size );
				if(read_bytes == -1){
						printf("err during reading from %s\n", src_path);
						finish_and_exit(fd_list);
				}

				write_bytes = write( dst, buff, read_bytes);
				if(write_bytes == -1){
						printf("err during writing to %s\n", dst_path);
						finish_and_exit(fd_list);
				}
				if (read_bytes != block_size || read_bytes == 0){
						finish_and_exit(fd_list);
				}
				if ( do_debug ) printf("transfered %d bytes\n", read_bytes);
		}

}
