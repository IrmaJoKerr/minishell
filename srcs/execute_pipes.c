/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 07:43:45 by bleow            ###   ########.fr       */
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
//     close(pipe_fd[0]);
//     if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//     {
//         perror("dup2 error in left child");
//         exit(1);
//     }
//     close(pipe_fd[1]);
    
//     int had_redir_error = 0;
    
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
//         if (vars->error_code == ERR_REDIRECTION)
//         {
//             had_redir_error = 1;
//         }
//     }
    
//     int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
//     if (had_redir_error)
//     {
//         exit(1);
//     }
//     else
//     {
//         exit(cmd_result);
//     }
// }
// void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     fprintf(stderr, "DEBUG: exec_pipe_left start for cmd: %s (pid=%d)\n", 
//             cmd_node->args[0], getpid());
    
//     close(pipe_fd[0]);
//     if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG: dup2 failed in left child\n");
//         exit(1);
//     }
//     close(pipe_fd[1]);
    
//     // Save original error code before redirection
//     int redir_success = 1;
    
//     // Process redirections if any
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG: Processing redirections for left cmd: %s\n", 
//                 cmd_node->args[0]);
                
//         redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
//         if (!redir_success || vars->error_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG: Left cmd redirection failed, exiting with code 1\n");
//             exit(1);  // Exit with error if redirection failed
//         }
//     }
    
//     // Only execute command if redirections succeeded
//     fprintf(stderr, "DEBUG: Executing left cmd: %s\n", cmd_node->args[0]);
//     int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
//     fprintf(stderr, "DEBUG: Left cmd completed with result: %d\n", cmd_result);
//     exit(cmd_result);
// }
void exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    fprintf(stderr, "DEBUG: exec_pipe_left start for cmd: %s (pid=%d)\n", 
            cmd_node->args[0], getpid());
    
    // Close read end of pipe - we're only writing to the pipe
    close(pipe_fd[0]);
    
    // Setup pipe redirection for stdout
    if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: dup2 failed in left child\n");
        close(pipe_fd[1]);
        exit(1);
    }
    close(pipe_fd[1]);
    
    // Process redirections if any
    int redir_success = 1;
    if (cmd_node->redir && is_redirection(cmd_node->redir->type))
    {
        fprintf(stderr, "DEBUG: Processing redirections for left cmd: %s\n", 
                cmd_node->args[0]);
                
        redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
        // Critical fix: Skip command execution if redirection failed
        if (!redir_success || vars->error_code == ERR_REDIRECTION)
        {
            fprintf(stderr, "DEBUG: Left cmd redirection failed, exiting with code 1\n");
            exit(1);  // Exit with error if redirection failed
        }
    }
    
    // Only execute command if redirections succeeded
    fprintf(stderr, "DEBUG: Executing left cmd: %s\n", cmd_node->args[0]);
    int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
    fprintf(stderr, "DEBUG: Left cmd completed with result: %d\n", cmd_result);
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
//     if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
//     {
//         perror("dup2 error in right child");
//         exit(1);
//     }
//     close(pipe_fd[0]);
    
//     int had_redir_error = 0;
    
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
//         if (vars->error_code == ERR_REDIRECTION)
//         {
//             had_redir_error = 1;
//         }
//     }
    
//     int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
//     if (had_redir_error)
//     {
//         exit(1);
//     }
//     else
//     {
//         exit(cmd_result);
//     }
// }
// void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     fprintf(stderr, "DEBUG: exec_pipe_right start for cmd: %s (pid=%d)\n", 
//             cmd_node->args[0], getpid());
    
//     if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG: dup2 failed in right child\n");
//         exit(1);
//     }
//     close(pipe_fd[0]);
    
//     // Save original error code before redirection
//     int redir_success = 1;
    
//     // Process redirections if any
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG: Processing redirections for right cmd: %s\n", 
//                 cmd_node->args[0]);
                
//         redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
//         if (!redir_success || vars->error_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG: Right cmd redirection failed, exiting with code 1\n");
//             exit(1);  // Exit with error if redirection failed
//         }
//     }
    
//     // Only execute command if redirections succeeded
//     fprintf(stderr, "DEBUG: Executing right cmd: %s\n", cmd_node->args[0]);
//     int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
//     fprintf(stderr, "DEBUG: Right cmd completed with result: %d\n", cmd_result);
//     exit(cmd_result);
// }
void exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    fprintf(stderr, "DEBUG: exec_pipe_right start for cmd: %s (pid=%d)\n", 
            cmd_node->args[0], getpid());
    
    // Setup pipe redirection for stdin
    if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: dup2 failed in right child\n");
        close(pipe_fd[0]);
        exit(1);
    }
    close(pipe_fd[0]);
    
    // Process redirections if any
    int redir_success = 1;
    if (cmd_node->redir && is_redirection(cmd_node->redir->type))
    {
        fprintf(stderr, "DEBUG: Processing redirections for right cmd: %s\n", 
                cmd_node->args[0]);
                
        redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
        
        // Critical fix: Skip command execution if redirection failed
        if (!redir_success || vars->error_code == ERR_REDIRECTION)
        {
            fprintf(stderr, "DEBUG: Right cmd redirection failed, exiting with code 1\n");
            exit(1);  // Exit with error if redirection failed
        }
    }
    
    // Only execute command if redirections succeeded
    fprintf(stderr, "DEBUG: Executing right cmd: %s\n", cmd_node->args[0]);
    int cmd_result = execute_cmd(cmd_node, vars->env, vars);
    
    fprintf(stderr, "DEBUG: Right cmd completed with result: %d\n", cmd_result);
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
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
//     int		pipe_fd[2];
//     pid_t	left_pid;
//     pid_t	right_pid;
//     int		r_status;
//     int		l_status;

//     if (init_pipe_exec(pipe_fd, &r_status, &l_status))
//         return (1);
        
//     if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
//         return (1);
        
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         ft_putendl_fd("fork: Creation failed (right)", 2);
//         close(pipe_fd[0]);
//         waitpid(left_pid, &l_status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         exec_pipe_right(pipe_node->right, pipe_fd, vars);
//     }
        
//     close(pipe_fd[0]);
    
//     waitpid(left_pid, &l_status, 0);
//     waitpid(right_pid, &r_status, 0);
    
//     return (handle_cmd_status(r_status, vars));
// }
int	execute_pipes(t_node *pipe_node, t_vars *vars)
{
    int		pipe_fd[2];
    pid_t	left_pid;
    pid_t	right_pid;
    int		r_status;
    int		l_status;

    // Debug print: Entry with command info
    fprintf(stderr, "DEBUG: execute_pipes for '%s | %s'\n", 
            pipe_node->left->args[0], pipe_node->right->args[0]);
    
    // Initialize pipe and status variables
    if (init_pipe_exec(pipe_fd, &r_status, &l_status))
    {
        fprintf(stderr, "DEBUG: pipe creation failed\n");
        return (1);
    }
    
    // Create left child process
    if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
    {
        fprintf(stderr, "DEBUG: left fork failed\n");
        return (1);
    }
    
    // Debug print: Left child created
    fprintf(stderr, "DEBUG: left child created with pid=%d\n", left_pid);
    
    // Create right child process
    right_pid = fork();
    if (right_pid == -1)
    {
        fprintf(stderr, "DEBUG: right fork failed\n");
        ft_putendl_fd("fork: Creation failed (right)", 2);
        close(pipe_fd[0]);
        waitpid(left_pid, &l_status, 0);
        return (1);
    }
    
    // Debug print: Right child created
    fprintf(stderr, "DEBUG: right child created with pid=%d\n", right_pid);
    
    if (right_pid == 0)
    {
        exec_pipe_right(pipe_node->right, pipe_fd, vars);
        // Should never reach here
    }
    
    // Close unused pipe end in parent
    close(pipe_fd[0]);
    fprintf(stderr, "DEBUG: parent closed pipe read end\n");
    
    // Wait for child processes
    fprintf(stderr, "DEBUG: waiting for left child (pid=%d)\n", left_pid);
    waitpid(left_pid, &l_status, 0);
    fprintf(stderr, "DEBUG: left child exited with raw status %d\n", l_status);
    
    fprintf(stderr, "DEBUG: waiting for right child (pid=%d)\n", right_pid);
    waitpid(right_pid, &r_status, 0);
    fprintf(stderr, "DEBUG: right child exited with raw status %d\n", r_status);
    
    // Process status with original handler
    int final_status = handle_cmd_status(r_status, vars);
    fprintf(stderr, "DEBUG: returning final status: %d\n", final_status);
    
    return (final_status);
}
