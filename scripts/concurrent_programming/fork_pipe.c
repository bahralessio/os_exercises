#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 

#define STDIN 0
#define STDOUT 1
#define PIPE_RD 0 
#define PIPE_WR 1

int main(int argc, char** argv) {

	pid_t cat_pid, sort_pid;    // processi figli (non ancora creati)
	int fd[2];            

	pipe(fd);
	
	cat_pid = fork();           // fork processo corrente

	if ( cat_pid == 0 ) {
		close(fd[PIPE_RD]);     // non serve leggere la pipe, la deve scrivere
		close (STDOUT);         // non serve leggere dallo STDOUT
		dup(fd[PIPE_WR]);       // vogliamo che cat finisca in input alla pipe
		execl("/bin/cat", "cat", "names.txt", NULL);
	}

	sort_pid = fork();          // ulteriore fork del processo padre

	if ( sort_pid == 0 ) {
		close(fd[PIPE_WR]);     // vogliamo leggere la pipe, e non scriverla, quindi WR si chiude
		close (STDIN);
		dup(fd[PIPE_RD]);       // prendiamo il RD della pipe
		execl("/usr/bin/sort", "sort", NULL);
	}

	// qui è il processo padre che chiude la pipe
	close(fd[PIPE_RD]); 
	close(fd[PIPE_WR]);
	
	/* wait for children to finish */ 
	waitpid(cat_pid, NULL, 0); 
	waitpid(sort_pid, NULL, 0);
			
	return 0; 
}
