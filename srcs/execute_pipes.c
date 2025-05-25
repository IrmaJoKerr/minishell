/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 23:46:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles execution within the left child process of a pipe.
- Closes unused pipe read end.
- Redirects stdout to the pipe write end.
- Closes the pipe write end.
- Executes the command node.
- Exits with the command's status.
*/
// void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: ===== LEFT PIPE EXECUTION DETAILS =====\n");
//     print_node_debug(cmd_node, "CMD", "exec_pipe_left");
    
//     // Print arguments as a flat list for easy verification
//     fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Command arguments list: ");
//     if (cmd_node && cmd_node->args) {
//         int i = 0;
//         while (cmd_node->args[i]) {
//             fprintf(stderr, "[%d]='%s' ", i, cmd_node->args[i]);
//             i++;
//         }
//     } else {
//         fprintf(stderr, "No arguments");
//     }
//     fprintf(stderr, "\n");
    
//     // Print redirections
//     fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Redirections: ");
//     if (cmd_node && cmd_node->redir) {
//         t_node *redir = cmd_node->redir;
//         while (redir) {
//             fprintf(stderr, "%s[%s] ", get_token_str(redir->type),
//                     redir->args ? redir->args[0] : "NULL");
//             redir = redir->redir;
//         }
//     } else {
//         fprintf(stderr, "None");
//     }
//     fprintf(stderr, "\n");
    
//     // Continue with original function...
//     // Close read end of pipe - we're only writing to the pipe
//     close(pipe_fd[0]);
    
//     // Setup pipe redirection for stdout
//     if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: dup2 failed\n");
//         close(pipe_fd[1]);
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Left stdout redirected to pipe write end\n");
//     close(pipe_fd[1]);
    
//     // Process redirections if any
//     int redir_success = 1;
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Processing redirections\n");
//         redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
//         if (!redir_success || vars->error_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Redirection failed, writing empty output to pipe\n");
//             exit(0);  // Exit with success so pipe continues
//         }
//     }
    
//     // Only execute command if redirections succeeded
//     fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Executing command with args: ");
//     if (cmd_node && cmd_node->args) {
//         int i = 0;
//         while (cmd_node->args[i]) {
//             fprintf(stderr, "'%s' ", cmd_node->args[i]);
//             i++;
//         }
//     }
//     fprintf(stderr, "\n");
    
//     int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
//     fprintf(stderr, "DEBUG-PIPE-LEFT-DETAILED: Command completed with result: %d\n", cmd_result);
//     exit(cmd_result);
// }
void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    fprintf(stderr, "DEBUG-PIPE-LEFT: Starting left child (pid=%d) for '%s'\n", 
            getpid(), cmd_node->args ? cmd_node->args[0] : "NULL");
    
    // Print redirections if any
    if (cmd_node->redir)
    {
        t_node *redir = cmd_node->redir;
        while (redir)
        {
            fprintf(stderr, "DEBUG-PIPE-LEFT: %s[%s] ", 
                    get_token_str(redir->type), 
                    redir->args ? redir->args[0] : "NULL");
            redir = redir->redir;
        }
        fprintf(stderr, "\n");
    }
    
    // Close read end of pipe - we're only writing to the pipe
    close(pipe_fd[0]);
    
    // Setup pipe redirection for stdout
    if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-PIPE-LEFT: dup2 failed\n");
        close(pipe_fd[1]);
        exit(1);
    }
    
    fprintf(stderr, "DEBUG-PIPE-LEFT: Left stdout redirected to pipe write end\n");
    close(pipe_fd[1]);
    
    // Process redirections if any
    int redir_success = 1;
    if (cmd_node->redir && is_redirection(cmd_node->redir->type))
    {
        fprintf(stderr, "DEBUG-PIPE-LEFT: Processing redirections for left cmd\n");
        redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
        if (!redir_success || vars->error_code == ERR_REDIRECTION)
        {
            fprintf(stderr, "DEBUG-PIPE-LEFT: Redirection failed, writing empty output to pipe\n");
            exit(0);  // Exit with success so the pipeline continues
        }
    }
    
    // Only execute command if redirections succeeded
    int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    exit(cmd_result);
}

/*
Handles execution within the right child process of a pipe.
- Closes unused pipe write end (already done by parent before fork).
- Redirects stdin to the pipe read end.
- Closes the pipe read end.
- Executes the command node.
- Exits with the command's status.
*/
// void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: ===== RIGHT PIPE EXECUTION DETAILS =====\n");
//     print_node_debug(cmd_node, "CMD", "exec_pipe_right");
    
//     // Print arguments as a flat list for easy verification
//     fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Command arguments list: ");
//     if (cmd_node && cmd_node->args) {
//         int i = 0;
//         while (cmd_node->args[i]) {
//             fprintf(stderr, "[%d]='%s' ", i, cmd_node->args[i]);
//             i++;
//         }
//     } else {
//         fprintf(stderr, "No arguments");
//     }
//     fprintf(stderr, "\n");
    
//     // Print redirections
//     fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Redirections: ");
//     if (cmd_node && cmd_node->redir) {
//         t_node *redir = cmd_node->redir;
//         while (redir) {
//             fprintf(stderr, "%s[%s] ", get_token_str(redir->type),
//                     redir->args ? redir->args[0] : "NULL");
//             redir = redir->redir;
//         }
//     } else {
//         fprintf(stderr, "None");
//     }
//     fprintf(stderr, "\n");
    
//     // Setup pipe redirection for stdin
//     if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: dup2 failed\n");
//         close(pipe_fd[0]);
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Stdin redirected from pipe read end\n");
//     close(pipe_fd[0]);
    
//     // Process redirections if any
//     int redir_success = 1;
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Processing redirections\n");
//         redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
//         if (!redir_success || vars->error_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Redirection failed, exiting\n");
//             exit(1);
//         }
//     }
    
//     // Only execute command if redirections succeeded
//     fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Executing command with args: ");
//     if (cmd_node && cmd_node->args) {
//         int i = 0;
//         while (cmd_node->args[i]) {
//             fprintf(stderr, "'%s' ", cmd_node->args[i]);
//             i++;
//         }
//     }
//     fprintf(stderr, "\n");
    
//     int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
//     fprintf(stderr, "DEBUG-PIPE-RIGHT-DETAILED: Command completed with result: %d\n", cmd_result);
//     exit(cmd_result);
// }
void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    fprintf(stderr, "DEBUG-PIPE-RIGHT: Starting right child (pid=%d) for '%s'\n", 
            getpid(), cmd_node->args ? cmd_node->args[0] : "NULL");
    
    // Setup pipe redirection for stdin
    if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-PIPE-RIGHT: dup2 failed\n");
        close(pipe_fd[0]);
        exit(1);
    }
    
    fprintf(stderr, "DEBUG-PIPE-RIGHT: Stdin redirected from pipe read end\n");
    close(pipe_fd[0]);
    
    // Process redirections if any
    if (cmd_node->redir && is_redirection(cmd_node->redir->type))
    {
        fprintf(stderr, "DEBUG-PIPE-RIGHT: Processing redirections\n");
        int redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
        if (!redir_success || vars->error_code == ERR_REDIRECTION)
        {
            fprintf(stderr, "DEBUG-PIPE-RIGHT: Redirection failed, exiting\n");
            exit(1);
        }
    }
    
    // Execute command
    int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    exit(cmd_result);
}

/*
Creates and sets up the left child process for a pipe.
Returns:
- 0 on successful fork
- 1 on fork failure (with appropriate cleanup)
Note: If successful, closes pipe_fd[1] in the parent process.
*/
int	fork_left_child(t_node *left_cmd, int pipe_fd[2], t_vars *vars
				, pid_t *left_pid_ptr)
{
	*left_pid_ptr = fork();
	if (*left_pid_ptr == -1)
	{
		ft_putendl_fd("fork: Creation failed (left)", 2);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (1);
	}
	if (*left_pid_ptr == 0)
	{
		exec_pipe_left(left_cmd, pipe_fd, vars);
	}
	close(pipe_fd[1]);
	pipe_fd[1] = -1;
	return (0);
}

/*
 * Initializes pipe execution variables and creates the pipe.
 * Parameters:
 * - pipe_fd: Array to store the created pipe file descriptors
 * - r_status_ptr: Pointer to the r_status variable to initialize
 * - l_status_ptr: Pointer to the l_status variable to initialize
 * Returns:
 * - 0 on successful initialization and pipe creation
 * - 1 on pipe creation failure (with appropriate error message)
 */
int	init_pipe_exec(int pipe_fd[2], int *r_status_ptr, int *l_status_ptr)
{
	*r_status_ptr = 0;
	*l_status_ptr = 0;
	if (pipe(pipe_fd) == -1)
	{
		ft_putendl_fd("pipe: Creation failed", 2);
		return (1);
	}
	return (0);
}

/*
Executes commands connected by a pipe.
- Sets up and launches pipeline child processes.
- Waits for both processes to complete.
- Captures status of command execution.
Returns:
- Status of right command.
- 1 on error.
Works with execute_cmd() for pipeline execution.

Example: For "ls -l | grep txt"
- Sets up pipeline processes
- Waits for both commands to complete
- Returns final execution status
*/
// int execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int     pipe_fd[2];
//     pid_t   left_pid;
//     pid_t   right_pid;
//     int     l_status;
//     int     r_status;

//     fprintf(stderr, "DEBUG-PIPE: Executing '%s | %s'\n", 
//             pipe_node->left && pipe_node->left->args ? pipe_node->left->args[0] : "NULL",
//             pipe_node->right && pipe_node->right->args ? pipe_node->right->args[0] : "NULL");
    
//     // Initialize pipe
//     if (pipe(pipe_fd) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE: Pipe creation failed\n");
//         return (1);
//     }
    
//     // Create left child process
//     left_pid = fork();
//     if (left_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE: Left fork failed\n");
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         return (1);
//     }
    
//     if (left_pid == 0)
//     {
//         exec_pipe_left(pipe_node->left, pipe_fd, vars);
//         // Should never reach here
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE: Created left child with pid=%d\n", left_pid);
    
//     // Create right child process
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE: Right fork failed\n");
//         kill(left_pid, SIGTERM);
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         // Close write end of pipe in right child before calling exec_pipe_right
//         close(pipe_fd[1]);
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//         // Should never reach here
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE: Created right child with pid=%d\n", right_pid);
    
//     // Close unused pipe ends in parent
//     fprintf(stderr, "DEBUG-PIPE: Parent closing pipe ends\n");
//     close(pipe_fd[0]);
//     close(pipe_fd[1]);
    
//     // Wait for child processes
//     fprintf(stderr, "DEBUG-PIPE: Waiting for left child (pid=%d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
//     fprintf(stderr, "DEBUG-PIPE: Left child exited with status %d\n", l_status);
    
//     fprintf(stderr, "DEBUG-PIPE: Waiting for right child (pid=%d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
//     fprintf(stderr, "DEBUG-PIPE: Right child exited with status %d\n", r_status);
    
//     // Process status with handler
//     l_status = handle_cmd_status(l_status, NULL);
//     r_status = handle_cmd_status(r_status, vars);
    
//     // KEY FIX: Always return the right side's status for a pipe
//     // This matches bash behavior where the pipeline exit code is the 
//     // exit code of the last command in the pipeline
//     fprintf(stderr, "DEBUG-PIPE: Pipe execution complete, returning right status %d\n", r_status);
//     return (r_status);
// }
// int execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int     pipe_fd[2];
//     pid_t   left_pid;
//     pid_t   right_pid;
//     int     l_status;
//     int     r_status;

//     fprintf(stderr, "DEBUG-PIPE-DETAILED: ===== PIPE EXECUTION DETAILS =====\n");
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Pipe node structure:\n");
//     print_node_debug(pipe_node, "PIPE", "execute_pipes");
    
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Left command:\n");
//     print_node_debug(pipe_node->left, "LEFT", "execute_pipes");
    
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Right command:\n");
//     print_node_debug(pipe_node->right, "RIGHT", "execute_pipes");
    
//     // Initialize pipe
//     if (pipe(pipe_fd) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-DETAILED: Pipe creation failed\n");
//         return (1);
//     }
    
//     // Create left child process
//     left_pid = fork();
//     if (left_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-DETAILED: Left fork failed\n");
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         return (1);
//     }
    
//     if (left_pid == 0)
//     {
//         exec_pipe_left(pipe_node->left, pipe_fd, vars);
//         // Should never reach here
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Created left child with pid=%d\n", left_pid);
    
//     // Create right child process
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-DETAILED: Right fork failed\n");
//         kill(left_pid, SIGTERM);
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         // Close write end of pipe in right child before calling exec_pipe_right
//         close(pipe_fd[1]);
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//         // Should never reach here
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Created right child with pid=%d\n", right_pid);
    
//     // Close unused pipe ends in parent
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Parent closing pipe ends\n");
//     close(pipe_fd[0]);
//     close(pipe_fd[1]);
    
//     // Wait for child processes
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Waiting for left child (pid=%d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Left child exited with status %d\n", l_status);
    
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Waiting for right child (pid=%d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Right child exited with status %d\n", r_status);
    
//     // Process status with handler
//     l_status = handle_cmd_status(l_status, NULL);
//     r_status = handle_cmd_status(r_status, vars);
    
//     fprintf(stderr, "DEBUG-PIPE-DETAILED: Pipe execution complete, returning status %d\n", r_status);
//     return (r_status);
// }
// int execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int     pipe_fd[2];
//     pid_t   left_pid;
//     pid_t   right_pid;
//     int     l_status;
//     int     r_status;

//     fprintf(stderr, "DEBUG-PIPE: Executing pipe with commands: '%s | %s'\n",
//             pipe_node->left && pipe_node->left->args ? pipe_node->left->args[0] : "NULL",
//             pipe_node->right && pipe_node->right->args ? pipe_node->right->args[0] : "NULL");
    
//     if (pipe(pipe_fd) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE: pipe creation failed\n");
//         return (1);
//     }
    
//     // Create left child process
//     left_pid = fork();
//     if (left_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE: left fork failed\n");
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         return (1);
//     }
    
//     if (left_pid == 0)
//     {
//         exec_pipe_left(pipe_node->left, pipe_fd, vars);
//         // Should never reach here
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE: Created left child with pid=%d\n", left_pid);
    
//     // Create right child process
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE: right fork failed\n");
//         kill(left_pid, SIGTERM);
//         close(pipe_fd[0]);
//         close(pipe_fd[1]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         // Close write end of pipe in right child before calling exec_pipe_right
//         close(pipe_fd[1]);
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//         // Should never reach here
//         exit(1);
//     }
    
//     fprintf(stderr, "DEBUG-PIPE: Created right child with pid=%d\n", right_pid);
    
//     // Close unused pipe ends in parent
//     fprintf(stderr, "DEBUG-PIPE: Parent closing pipe ends\n");
//     close(pipe_fd[0]);
//     close(pipe_fd[1]);
    
//     // Wait for child processes and get their status
//     fprintf(stderr, "DEBUG-PIPE: Waiting for left child (pid=%d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
//     fprintf(stderr, "DEBUG-PIPE: Left child exited with status %d\n", l_status);
//     l_status = handle_cmd_status(l_status, NULL);
    
//     fprintf(stderr, "DEBUG-PIPE: Waiting for right child (pid=%d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
//     fprintf(stderr, "DEBUG-PIPE: Right child exited with status %d\n", r_status);
//     r_status = handle_cmd_status(r_status, vars);  // This updates vars->error_code
    
//     // IMPORTANT: According to POSIX, the exit status of a pipeline is 
//     // the exit status of the last command in the pipeline
//     fprintf(stderr, "DEBUG-PIPE: Pipe execution complete, returning right status %d\n", r_status);
//     return (r_status);
// }
int execute_pipes(t_node *pipe_node, t_vars *vars)
{
    int     pipe_fd[2];
    pid_t   left_pid;
    pid_t   right_pid;
    int     l_status;
    int     r_status;

    fprintf(stderr, "DEBUG-PIPE: Executing pipe with commands: '%s | %s'\n",
            pipe_node->left && pipe_node->left->args ? pipe_node->left->args[0] : "NULL",
            pipe_node->right && pipe_node->right->args ? pipe_node->right->args[0] : "NULL");
    
    if (pipe(pipe_fd) == -1)
    {
        fprintf(stderr, "DEBUG-PIPE: pipe creation failed\n");
        return (1);
    }
    
    // Create left child process
    left_pid = fork();
    if (left_pid == -1)
    {
        fprintf(stderr, "DEBUG-PIPE: left fork failed\n");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return (1);
    }
    
    if (left_pid == 0)
    {
        exec_pipe_left(pipe_node->left, pipe_fd, vars);
        // Should never reach here
        exit(1);
    }
    
    fprintf(stderr, "DEBUG-PIPE: Created left child with pid=%d\n", left_pid);
    
    // Create right child process
    right_pid = fork();
    if (right_pid == -1)
    {
        fprintf(stderr, "DEBUG-PIPE: right fork failed\n");
        kill(left_pid, SIGTERM);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(left_pid, &l_status, 0);
        return (1);
    }
    
    if (right_pid == 0)
    {
        close(pipe_fd[1]); // Close write end in right child
        exec_pipe_right(pipe_node->right, pipe_fd, vars);
        // Should never reach here
        exit(1);
    }
    
    fprintf(stderr, "DEBUG-PIPE: Created right child with pid=%d\n", right_pid);
    
    // Close unused pipe ends in parent
    fprintf(stderr, "DEBUG-PIPE: Parent closing pipe ends\n");
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    
    // Wait for child processes and get their status
    fprintf(stderr, "DEBUG-PIPE: Waiting for left child (pid=%d)\n", left_pid);
    waitpid(left_pid, &l_status, 0);
    fprintf(stderr, "DEBUG-PIPE: Left child exited with status %d\n", l_status);
    l_status = handle_cmd_status(l_status, NULL);
    
    fprintf(stderr, "DEBUG-PIPE: Waiting for right child (pid=%d)\n", right_pid);
    waitpid(right_pid, &r_status, 0);
    fprintf(stderr, "DEBUG-PIPE: Right child exited with status %d\n", r_status);
    
    // CRITICAL FIX: According to POSIX, the exit status of a pipeline is 
    // the exit status of the last command in the pipeline
    r_status = handle_cmd_status(r_status, vars);
    
    fprintf(stderr, "DEBUG-PIPE: Pipe execution complete, returning right status %d\n", r_status);
    return (r_status);
}