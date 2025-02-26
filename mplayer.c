#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


int main(){
	char *myfifo = "/tmp/myfifo";
	int fd;
	
	if (mkfifo(myfifo, 0666) == -1 && errno != EEXIST) {
		printf("Blad w tworzeniu fifo");
		return 1;
	}


	fd = open(myfifo, O_WRONLY);
	if (fd == -1){
		printf("Blad w otworzeniu fifo do zapisu");
		return 1;
	}

	char command[100];

	while (1) {
		printf("Podaj polecenie dla mplayera:");
		if (fgets(command, sizeof(command), stdin) == NULL){
			printf("Blad wczytywania polecenia");
			break;
		}
		

		if (write(fd, command, strlen(command)) == -1){
			printf("Blad z wpisywaniem w fifo");
			break;
		}
		if (strcmp(command, "quit") == 0) {
			unlink(myfifo);
			printf("Usunieto fifo");
			break;
		}
	}
	close(fd);
	return 0;
}
