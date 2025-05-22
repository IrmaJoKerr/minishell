/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 07:37:47 by bleow            ###   ########.fr       */
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
// void	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
// 	close(pipe_fd[0]);
// 	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
// 	{
// 		perror("dup2 (left child)");
// 		exit(EXIT_FAILURE);
// 	}
// 	close(pipe_fd[1]);
// 	exit(execute_cmd(cmd_node, vars->env, vars));
// }
// void	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
// 	close(pipe_fd[0]);
// 	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
// 	{
// 		perror("dup2 (left child)");
// 		exit(EXIT_FAILURE);
// 	}
// 	close(pipe_fd[1]);
// 	fprintf(stderr, "DEBUG-CRITICAL: Left pipe process (PID %d) executing command\n", getpid());
//     exit(execute_cmd(cmd_node, vars->env, vars));
// }
// void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     // Setup pipe
//     close(pipe_fd[0]);
//     if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//     {
//         perror("dup2 error in left child");
//         exit(1);
//     }
//     close(pipe_fd[1]);
    
//     // Process redirections if they exist in the command's redir field
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-LEFT: Processing redirections for left command\n");
//         // Try to process redirections, but continue even if they fail
//         proc_redir_chain(cmd_node->redir, cmd_node, vars);
//         // Note: No error code reset - let command execution set it
//     }
    
//     fprintf(stderr, "DEBUG-CRITICAL: Left pipe process (PID %d) executing command\n", getpid());
//     exit(execute_cmd(cmd_node, vars->env, vars));
// }
// void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     // Setup pipe
//     close(pipe_fd[0]);
//     if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//     {
//         perror("dup2 error in left child");
//         exit(1);
//     }
//     close(pipe_fd[1]);
    
//     // Process redirections if they exist in the command's redir field
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-LEFT: Processing redirections for left command\n");
//         // Try to process redirections, but continue even if they fail
//         proc_redir_chain(cmd_node->redir, cmd_node, vars);
//         // Note: No error code reset needed - pipe context is detected from pipe_root
//     }
    
//     fprintf(stderr, "DEBUG-CRITICAL: Left pipe process (PID %d) executing command\n", getpid());
//     exit(execute_cmd(cmd_node, vars->env, vars));
// }
void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    // Setup pipe
    close(pipe_fd[0]);
    if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
    {
        perror("dup2 error in left child");
        exit(1);
    }
    close(pipe_fd[1]);
    
    // Track if we had a redirection error
    int had_redir_error = 0;
    
    // Process redirections if they exist in the command's redir field
    if (cmd_node->redir && is_redirection(cmd_node->redir->type))
    {
        fprintf(stderr, "DEBUG-PIPE-LEFT: Processing redirections for left command\n");
        // Try to process redirections, but continue even if they fail
        proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
        // Check if redirection had an error
        if (vars->error_code == ERR_REDIRECTION)
        {
            had_redir_error = 1;
            fprintf(stderr, "DEBUG-PIPE-LEFT: Redirection failed, will exit with error code 1\n");
        }
    }
    
    fprintf(stderr, "DEBUG-CRITICAL: Left pipe process (PID %d) executing command\n", getpid());
    
    // Execute command normally
    int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
    // If there was a redirection error, override the exit code to ensure proper status
    if (had_redir_error)
    {
        fprintf(stderr, "DEBUG-PIPE-LEFT: Exiting with error code 1 due to redirection failure\n");
        exit(1);
    }
    else
    {
        exit(cmd_result);
    }
}

/*
Handles execution within the right child process of a pipe.
- Closes unused pipe write end (already done by parent before fork).
- Redirects stdin to the pipe read end.
- Closes the pipe read end.
- Executes the command node.
- Exits with the command's status.
*/
// void	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
// 	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
// 	{
// 		perror("dup2 (right child)");
// 		exit(EXIT_FAILURE);
// 	}
// 	close(pipe_fd[0]);
// 	exit(execute_cmd(cmd_node, vars->env, vars));
// }
// void	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
// 	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
// 	{
// 		perror("dup2 (right child)");
// 		exit(EXIT_FAILURE);
// 	}
// 	close(pipe_fd[0]);
// 	fprintf(stderr, "DEBUG-CRITICAL: Right pipe process (PID %d) executing command\n", getpid());
//     exit(execute_cmd(cmd_node, vars->env, vars));
// }
// void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
//     {
//         perror("dup2 error in right child");
//         exit(1);
//     }
//     close(pipe_fd[0]);
    
//     // Process redirections if they exist in the command's redir field
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-RIGHT: Processing redirections for right command\n");
//         // Try to process redirections, but continue even if they fail
//         proc_redir_chain(cmd_node->redir, cmd_node, vars);
//         // Note: No error code reset - let command execution set it
//     }
    
//     fprintf(stderr, "DEBUG-CRITICAL: Right pipe process (PID %d) executing command\n", getpid());
//     exit(execute_cmd(cmd_node, vars->env, vars));
// }
// void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
//     {
//         perror("dup2 error in right child");
//         exit(1);
//     }
//     close(pipe_fd[0]);
    
//     // Process redirections if they exist in the command's redir field
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-RIGHT: Processing redirections for right command\n");
//         // Try to process redirections, but continue even if they fail
//         proc_redir_chain(cmd_node->redir, cmd_node, vars);
//         // Note: No error code reset needed - pipe context is detected from pipe_root
//     }
    
//     fprintf(stderr, "DEBUG-CRITICAL: Right pipe process (PID %d) executing command\n", getpid());
//     exit(execute_cmd(cmd_node, vars->env, vars));
// }
void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
    {
        perror("dup2 error in right child");
        exit(1);
    }
    close(pipe_fd[0]);
    
    // Track if we had a redirection error
    int had_redir_error = 0;
    
    // Process redirections if they exist in the command's redir field
    if (cmd_node->redir && is_redirection(cmd_node->redir->type))
    {
        fprintf(stderr, "DEBUG-PIPE-RIGHT: Processing redirections for right command\n");
        // Try to process redirections, but continue even if they fail
        proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
        // Check if redirection had an error
        if (vars->error_code == ERR_REDIRECTION)
        {
            had_redir_error = 1;
            fprintf(stderr, "DEBUG-PIPE-RIGHT: Redirection failed, will exit with error code 1\n");
        }
    }
    
    fprintf(stderr, "DEBUG-CRITICAL: Right pipe process (PID %d) executing command\n", getpid());
    
    // Execute command normally
    int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
    // If there was a redirection error, override the exit code to ensure proper status
    if (had_redir_error)
    {
        fprintf(stderr, "DEBUG-PIPE-RIGHT: Exiting with error code 1 due to redirection failure\n");
        exit(1);
    }
    else
    {
        exit(cmd_result);
    }
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
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
// 	int		pipe_fd[2];
// 	pid_t	left_pid;
// 	pid_t	right_pid;
// 	int		r_status;
// 	int		l_status;

// 	if (init_pipe_exec(pipe_fd, &r_status, &l_status))
// 		return (1);
// 	if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
// 		return (1);
// 	right_pid = fork();
// 	if (right_pid == -1)
// 	{
// 		ft_putendl_fd("fork: Creation failed (right)", 2);
// 		close(pipe_fd[0]);
// 		waitpid(left_pid, &l_status, 0);
// 		return (1);
// 	}
// 	if (right_pid == 0)
// 		exec_pipe_right(pipe_node->right, pipe_fd, vars);
// 	close(pipe_fd[0]);
// 	waitpid(left_pid, &l_status, 0);
// 	waitpid(right_pid, &r_status, 0);
// 	return (handle_cmd_status(r_status, vars));
// }
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int		pipe_fd[2];
//     pid_t	left_pid;
//     pid_t	right_pid;
//     int		r_status;
//     int		l_status;

//     fprintf(stderr, "DEBUG-EXEC-PIPE: Starting pipe execution\n");
//     if (init_pipe_exec(pipe_fd, &r_status, &l_status))
//         return (1);
        
//     fprintf(stderr, "DEBUG-EXEC-PIPE: Forking left child\n");
//     if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
//         return (1);
        
//     fprintf(stderr, "DEBUG-EXEC-PIPE: Forking right child\n");
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         ft_putendl_fd("fork: Creation failed (right)", 2);
//         close(pipe_fd[0]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
        
//     close(pipe_fd[0]);
    
//     fprintf(stderr, "DEBUG-EXEC-PIPE: Waiting for left child (PID: %d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
    
//     // If the left command fails with a redirection error, still execute the right command
//     if (WIFEXITED(l_status))
//     {
//         int exit_code = WEXITSTATUS(l_status);
//         fprintf(stderr, "DEBUG-EXEC-PIPE: Left command exited with code %d\n", exit_code);
//         if (exit_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG-EXEC-PIPE: Left command had redirection error, continuing with right command\n");
//         }
//     }
    
//     fprintf(stderr, "DEBUG-EXEC-PIPE: Waiting for right child (PID: %d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
    
//     if (WIFEXITED(r_status))
//     {
//         fprintf(stderr, "DEBUG-EXEC-PIPE: Right command exited with code %d\n", WEXITSTATUS(r_status));
//     }
    
//     return (handle_cmd_status(r_status, vars));
// }
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int		pipe_fd[2];
//     pid_t	left_pid;
//     pid_t	right_pid;
//     int		r_status;
//     int		l_status;

//     fprintf(stderr, "DEBUG-PIPE-EXEC: Starting pipe execution for node %p\n", (void*)pipe_node);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left command: %s, right command: %s\n", 
//         (pipe_node->left && pipe_node->left->args) ? pipe_node->left->args[0] : "NULL",
//         (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
    
//     if (init_pipe_exec(pipe_fd, &r_status, &l_status))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Failed to initialize pipe\n");
//         return (1);
//     }
        
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Forking left child\n");
//     if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left child fork failed\n");
//         return (1);
//     }
        
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Forking right child\n");
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         ft_putendl_fd("fork: Creation failed (right)", 2);
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right child fork failed\n");
//         close(pipe_fd[0]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: In right child process\n");
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//     }
        
//     close(pipe_fd[0]);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for left child (PID: %d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
    
//     if (WIFEXITED(l_status))
//     {
//         int exit_code = WEXITSTATUS(l_status);
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left command exited with code %d\n", exit_code);
//         if (exit_code != 0)
//         {
//             fprintf(stderr, "DEBUG-PIPE-EXEC: Left command failed, continuing with right command\n");
//         }
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for right child (PID: %d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
    
//     if (WIFEXITED(r_status))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command exited with code %d\n", WEXITSTATUS(r_status));
//     }
    
//     return (handle_cmd_status(r_status, vars));
// }
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int		pipe_fd[2];
//     pid_t	left_pid;
//     pid_t	right_pid;
//     int		r_status;
//     int		l_status;

//     fprintf(stderr, "DEBUG-PIPE-EXEC: Starting pipe execution for node %p\n", (void*)pipe_node);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left command: %s, right command: %s\n", 
//         (pipe_node->left && pipe_node->left->args) ? pipe_node->left->args[0] : "NULL",
//         (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
    
//     if (init_pipe_exec(pipe_fd, &r_status, &l_status))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Failed to initialize pipe\n");
//         return (1);
//     }
        
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Forking left child\n");
//     if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left child fork failed\n");
//         return (1);
//     }
        
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Forking right child\n");
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         ft_putendl_fd("fork: Creation failed (right)", 2);
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right child fork failed\n");
//         close(pipe_fd[0]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: In right child process (PID: %d)\n", getpid());
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command will execute: %s\n", 
//             (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//     }
        
//     close(pipe_fd[0]);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for left child (PID: %d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left child (PID: %d) status: %d (exited: %d, signal: %d)\n", 
//         left_pid, l_status, WIFEXITED(l_status), WIFSIGNALED(l_status));
    
//     if (WIFSIGNALED(l_status)) {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left command terminated by signal: %d\n", WTERMSIG(l_status));
//     }
    
//     if (WIFEXITED(l_status))
//     {
//         int exit_code = WEXITSTATUS(l_status);
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left command exited with code %d\n", exit_code);
//         if (exit_code != 0)
//         {
//             fprintf(stderr, "DEBUG-PIPE-EXEC: Left command failed, continuing with right command\n");
//         }
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for right child (PID: %d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Right child (PID: %d) status: %d (exited: %d, signal: %d)\n", 
//         right_pid, r_status, WIFEXITED(r_status), WIFSIGNALED(r_status));
    
//     if (WIFSIGNALED(r_status)) {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command terminated by signal: %d\n", WTERMSIG(r_status));
//     }
    
//     if (WIFEXITED(r_status))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command exited with code %d\n", WEXITSTATUS(r_status));
//     }
    
//     return (handle_cmd_status(r_status, vars));
// }
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int		pipe_fd[2];
//     pid_t	left_pid;
//     pid_t	right_pid;
//     int		r_status;
//     int		l_status;

//     fprintf(stderr, "DEBUG-PIPE-EXEC: Starting pipe execution for node %p\n", (void*)pipe_node);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left command: %s, right command: %s\n", 
//         (pipe_node->left && pipe_node->left->args) ? pipe_node->left->args[0] : "NULL",
//         (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
    
//     if (init_pipe_exec(pipe_fd, &r_status, &l_status))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Failed to initialize pipe\n");
//         return (1);
//     }
        
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Forking left child\n");
//     if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left child fork failed\n");
//         return (1);
//     }
        
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Forking right child\n");
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         ft_putendl_fd("fork: Creation failed (right)", 2);
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right child fork failed\n");
//         close(pipe_fd[0]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: In right child process (PID: %d)\n", getpid());
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command will execute: %s\n", 
//             (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//     }
        
//     close(pipe_fd[0]);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for left child (PID: %d)\n", left_pid);
//     waitpid(left_pid, &l_status, 0);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left child (PID: %d) status: %d (exited: %d, signal: %d)\n", 
//         left_pid, l_status, WIFEXITED(l_status), WIFSIGNALED(l_status));
    
//     if (WIFSIGNALED(l_status)) {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left command terminated by signal: %d\n", WTERMSIG(l_status));
//     }
    
//     if (WIFEXITED(l_status))
//     {
//         int exit_code = WEXITSTATUS(l_status);
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left command exited with code %d\n", exit_code);
//         if (exit_code != 0)
//         {
//             fprintf(stderr, "DEBUG-PIPE-EXEC: Left command failed, continuing with right command\n");
//         }
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for right child (PID: %d)\n", right_pid);
//     waitpid(right_pid, &r_status, 0);
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Right child (PID: %d) status: %d (exited: %d, signal: %d)\n", 
//         right_pid, r_status, WIFEXITED(r_status), WIFSIGNALED(r_status));
    
//     if (WIFSIGNALED(r_status)) {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command terminated by signal: %d\n", WTERMSIG(r_status));
//     }
    
//     if (WIFEXITED(r_status))
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right command exited with code %d\n", WEXITSTATUS(r_status));
//     }
    
//     return (handle_cmd_status(r_status, vars));
// }
int	execute_pipes(t_node *pipe_node, t_vars *vars)
{
    int		pipe_fd[2];
    pid_t	left_pid;
    pid_t	right_pid;
    int		r_status;
    int		l_status;

    fprintf(stderr, "DEBUG-PIPE-EXEC: Starting pipe execution for node %p\n", (void*)pipe_node);
    fprintf(stderr, "DEBUG-PIPE-EXEC: Left command: %s, right command: %s\n", 
        (pipe_node->left && pipe_node->left->args) ? pipe_node->left->args[0] : "NULL",
        (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
    
    if (init_pipe_exec(pipe_fd, &r_status, &l_status))
    {
        fprintf(stderr, "DEBUG-PIPE-EXEC: Failed to initialize pipe\n");
        return (1);
    }
        
    fprintf(stderr, "DEBUG-PIPE-EXEC: Forking left child\n");
    if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
    {
        fprintf(stderr, "DEBUG-PIPE-EXEC: Left child fork failed\n");
        return (1);
    }
        
    fprintf(stderr, "DEBUG-PIPE-EXEC: Forking right child\n");
    right_pid = fork();
    if (right_pid == -1)
    {
        ft_putendl_fd("fork: Creation failed (right)", 2);
        fprintf(stderr, "DEBUG-PIPE-EXEC: Right child fork failed\n");
        close(pipe_fd[0]);
        waitpid(left_pid, &l_status, 0);
        return (1);
    }
    
    if (right_pid == 0)
    {
        fprintf(stderr, "DEBUG-PIPE-EXEC: In right child process (PID: %d)\n", getpid());
        fprintf(stderr, "DEBUG-PIPE-EXEC: Right command will execute: %s\n", 
            (pipe_node->right && pipe_node->right->args) ? pipe_node->right->args[0] : "NULL");
        exec_pipe_right(pipe_node->right, pipe_fd, vars);
    }
        
    close(pipe_fd[0]);
    
    fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for left child (PID: %d)\n", left_pid);
    waitpid(left_pid, &l_status, 0);
    
    fprintf(stderr, "DEBUG-PIPE-EXEC: Left child (PID: %d) status: %d (exited: %d, signal: %d)\n", 
        left_pid, l_status, WIFEXITED(l_status), WIFSIGNALED(l_status));
    
    if (WIFSIGNALED(l_status)) {
        fprintf(stderr, "DEBUG-PIPE-EXEC: Left command terminated by signal: %d\n", WTERMSIG(l_status));
    }
    
    if (WIFEXITED(l_status))
    {
        int exit_code = WEXITSTATUS(l_status);
        fprintf(stderr, "DEBUG-PIPE-EXEC: Left command exited with code %d\n", exit_code);
        if (exit_code != 0)
        {
            fprintf(stderr, "DEBUG-PIPE-EXEC: Left command failed, continuing with right command\n");
        }
    }
    
    fprintf(stderr, "DEBUG-PIPE-EXEC: Waiting for right child (PID: %d)\n", right_pid);
    waitpid(right_pid, &r_status, 0);
    
    fprintf(stderr, "DEBUG-PIPE-EXEC: Right child (PID: %d) status: %d (exited: %d, signal: %d)\n", 
        right_pid, r_status, WIFEXITED(r_status), WIFSIGNALED(r_status));
    
    if (WIFSIGNALED(r_status)) {
        fprintf(stderr, "DEBUG-PIPE-EXEC: Right command terminated by signal: %d\n", WTERMSIG(r_status));
    }
    
    if (WIFEXITED(r_status))
    {
        fprintf(stderr, "DEBUG-PIPE-EXEC: Right command exited with code %d\n", WEXITSTATUS(r_status));
    }
    
    fprintf(stderr, "DEBUG-CRITICAL: Left status: %d, right status: %d\n", l_status, r_status);
    
    return (handle_cmd_status(r_status, vars));
}
