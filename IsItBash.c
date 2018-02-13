/*
 * Author: Daniel Liscinsky, 02/12/2018
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <sysexits.h>
#include <string.h>



static void waitpid_wexitstatus(pid_t pid, int *exit_code);



int main(int argc, char **argv) {

	char * file_path = "/bin/bash";

	


	
	int pipe_fd_input[2];// For input to the child process
	int pipe_fd_output[2];// For output from the child process
	pid_t pid;

	/* Create the pipe and fork to share it. */
	if (pipe(pipe_fd_input) < 0 || pipe(pipe_fd_output) < 0) {
		perror("pipe");
	}

	if ((pid = fork()) < 0) {
		perror("fork");
		exit(EX_OSERR);
	}
	// If child process
	else if (pid == 0) {

		// Set up pipe for child's STDIN
		close(pipe_fd_input[1]);

		if (dup2(pipe_fd_input[0], STDIN_FILENO) < 0) {
			perror("dup2");
		}

		close(pipe_fd_input[0]);

		// Set up pipe for child's STDOUT
		close(pipe_fd_output[0]);

		if (dup2(pipe_fd_output[1], STDOUT_FILENO) < 0) {
			perror("dup2");
		}

		if (dup2(pipe_fd_output[1], STDERR_FILENO) < 0) {
			perror("dup2");
		}

		close(pipe_fd_output[1]);

		execl(file_path, file_path, NULL);
		perror("exec");
		exit(EX_OSERR);//TODO not necessarily an error... (-x perms)
	}

	// Close unneeded pipes to child
	close(pipe_fd_input[0]);
	close(pipe_fd_output[1]);
	


	char buf[1024] = { 0 };
	write(pipe_fd_input[1], "cd XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", 39);
	read(pipe_fd_output[0], buf, 1024);

	printf("%s\n", buf);
	if ( !strcmp(buf, "bash: cd: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX: No such file or directory")) {

		printf("true!\n");
	}

	
	int exit_code;
	waitpid_wexitstatus(pid, &exit_code);
	while (wait(NULL) > 0); // Reap all child processes


	return 0;
}

/*
* Performs waitpid(pid, exit_code, 0).
*
* Stores the exit status of the process in exit_code if the process
* terminated normally, otherwise exit_code retains the status value
* set by waitpid().
*/
void waitpid_wexitstatus(pid_t pid, int *exit_code) {

	waitpid(pid, exit_code, 0);

	/* If process terminated normally, get exit code. */
	if (WIFEXITED(*exit_code)) {
		*exit_code = WEXITSTATUS(*exit_code);
	}
}