/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/27 03:44:22 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// /*
// Handles execution within the left child process of a pipe.
// - Closes unused pipe read end.
// - Redirects stdout to the pipe write end.
// - Closes the pipe write end.
// - Executes the command node.
// - Exits with the command's status.
// */
// void	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
// 	int	cmd_result;
// 	int	redir_success;
// 	// Close read end of pipe - we're only writing to the pipe
// 	close(pipe_fd[0]);
// 	// Setup pipe redirection for stdout
// 	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
// 	{
// 		close(pipe_fd[1]);
// 		exit(1);
// 	}
// 	close(pipe_fd[1]);
// 	// Process redirections if any
// 	redir_success = 1;
// 	if (cmd_node->redir && is_redirection(cmd_node->redir->type))
// 	{
// 		redir_success = proc_redir_chain(cmd_node->redir, vars);
// 		if (!redir_success || vars->error_code == ERR_REDIRECTION)
// 		{
// 			exit(0);  // Exit with success so the pipeline continues
// 		}
// 	}
// 	// Only execute command if redirections succeeded
// 	cmd_result = execute_cmd(cmd_node, vars->env, vars);
// 	exit(cmd_result);
// }
/*
Handles execution within the left child process of a pipe.
- Closes unused pipe read end.
- Redirects stdout to the pipe write end.
- Closes the pipe write end.
- Executes the command node.
Returns:
- The exit status code that should be used when exiting the process
*/
// int	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     int	cmd_result;
//     int	redir_success;

//     fprintf(stderr, "DEBUG-PIPE-LEFT: Setting up left pipe process for '%s'\n",
//         cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     // Close read end of pipe - we're only writing to the pipe
//     close(pipe_fd[0]);
    
//     // Setup pipe redirection for stdout
//     if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-LEFT: Failed to redirect stdout to pipe\n");
//         close(pipe_fd[1]);
//         return (1);
//     }
//     close(pipe_fd[1]);
    
//     // Process redirections if any
//     redir_success = 1;
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-LEFT: Processing redirections\n");
//         redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
//         if (!redir_success || vars->error_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG-PIPE-LEFT: Redirection failed, exiting with 0\n");
//             return (0);  // Exit with success so the pipeline continues
//         }
//     }
    
//     // Only execute command if redirections succeeded
//     fprintf(stderr, "DEBUG-PIPE-LEFT: Executing command\n");
//     cmd_result = execute_cmd(cmd_node, vars->env, vars);
//     fprintf(stderr, "DEBUG-PIPE-LEFT: Command execution completed with status %d\n", cmd_result);
    
//     return (cmd_result);
// }
int	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    int cmd_result;
    int redir_success;
    t_node *actual_cmd;

    fprintf(stderr, "PIPE-DBG-LEFT: === LEFT PROCESS START ===\n");
    fprintf(stderr, "PIPE-DBG-LEFT: Node type=%s, args[0]='%s'\n", 
            get_token_str(cmd_node->type),
            cmd_node->args ? cmd_node->args[0] : "NULL");

    // Close read end of pipe - we're only writing to the pipe
    if (close(pipe_fd[0]) == -1)
        fprintf(stderr, "PIPE-DBG-LEFT: Error closing read end: %s\n", strerror(errno));
    else
        fprintf(stderr, "PIPE-DBG-LEFT: Closed read end fd=%d\n", pipe_fd[0]);

    // Setup pipe redirection for stdout
    fprintf(stderr, "PIPE-DBG-LEFT: Redirecting stdout to pipe write end fd=%d\n", pipe_fd[1]);
    if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
        fprintf(stderr, "PIPE-DBG-LEFT: dup2 failed: %s\n", strerror(errno));
        close(pipe_fd[1]);
        return 1;
    }
    
    if (close(pipe_fd[1]) == -1)
        fprintf(stderr, "PIPE-DBG-LEFT: Error closing write end: %s\n", strerror(errno));
    else
        fprintf(stderr, "PIPE-DBG-LEFT: Closed write end fd=%d after dup2\n", pipe_fd[1]);

    // Process redirections based on node type
    if (is_redirection(cmd_node->type)) {
        fprintf(stderr, "PIPE-DBG-LEFT: Node is redirection type=%s\n", 
                get_token_str(cmd_node->type));
        
        // Find actual command in redirection->left
        if (cmd_node->left && cmd_node->left->type == TYPE_CMD) {
            actual_cmd = cmd_node->left;
            fprintf(stderr, "PIPE-DBG-LEFT: Found command in left: '%s'\n",
                    actual_cmd->args ? actual_cmd->args[0] : "NULL");
        } else {
            fprintf(stderr, "PIPE-DBG-LEFT: ERROR: Missing command in redirection->left\n");
            fprintf(stderr, "PIPE-DBG-LEFT: Left node type=%s\n",
                    cmd_node->left ? get_token_str(cmd_node->left->type) : "NULL");
            return 1;
        }
        
        fprintf(stderr, "PIPE-DBG-LEFT: Processing redirection chain\n");
        redir_success = proc_redir_chain(cmd_node, actual_cmd, vars);
        
        if (!redir_success) {
            fprintf(stderr, "PIPE-DBG-LEFT: Redirection failed, error_code=%d\n", 
                    vars->error_code);
            
            // Special case: Test if a file exists but has invalid permissions
            if (vars->error_code == ERR_PERMISSIONS) {
                fprintf(stderr, "PIPE-DBG-LEFT: Permission error detected\n");
                return vars->error_code; // Return actual error
            }
            return 0; // Other redirection errors should still allow pipeline to continue
        }
    } else if (cmd_node->type == TYPE_CMD) {
        fprintf(stderr, "PIPE-DBG-LEFT: Direct command node: '%s'\n", 
                cmd_node->args ? cmd_node->args[0] : "NULL");
        actual_cmd = cmd_node;
    } else {
        fprintf(stderr, "PIPE-DBG-LEFT: Unexpected node type: %s\n",
                get_token_str(cmd_node->type));
        return 1;
    }

    // Check if redirections are attached to this command
    if (actual_cmd->redir) {
        fprintf(stderr, "PIPE-DBG-LEFT: Command has attached redirections\n");
        redir_success = proc_redir_chain(actual_cmd->redir, actual_cmd, vars);
        if (!redir_success) {
            fprintf(stderr, "PIPE-DBG-LEFT: Attached redirection failed, error_code=%d\n", 
                    vars->error_code);
            return vars->error_code;
        }
    }
    
    fprintf(stderr, "PIPE-DBG-LEFT: Executing command '%s'\n",
            actual_cmd->args ? actual_cmd->args[0] : "NULL");
    
    cmd_result = execute_cmd(actual_cmd, vars->env, vars);
    
    fprintf(stderr, "PIPE-DBG-LEFT: Command execution completed with status %d\n", cmd_result);
    return cmd_result;
}

// /*
// Handles execution within the right child process of a pipe.
// - Closes unused pipe write end (already done by parent before fork).
// - Redirects stdin to the pipe read end.
// - Closes the pipe read end.
// - Executes the command node.
// - Exits with the command's status.
// */
// void	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
// 	int	redir_success;
// 	int	cmd_result;
// 	// Setup pipe redirection for stdin
// 	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
// 	{
// 		close(pipe_fd[0]);
// 		exit(1);
// 	}
// 	close(pipe_fd[0]);
// 	// Process redirections if any
// 	if (cmd_node->redir && is_redirection(cmd_node->redir->type))
// 	{
// 		redir_success = proc_redir_chain(cmd_node->redir, vars);
// 		if (!redir_success || vars->error_code == ERR_REDIRECTION)
// 		{
// 			exit(1);
// 		}
// 	}
// 	// Execute command
// 	cmd_result = execute_cmd(cmd_node, vars->env, vars);
// 	exit(cmd_result);
// }
/*
Handles execution within the right child process of a pipe.
- Redirects stdin to the pipe read end.
- Closes the pipe read end.
- Executes the command node.
Returns:
- The exit status code that should be used when exiting the process
*/
// int	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
// {
//     int	redir_success;
//     int	cmd_result;

//     fprintf(stderr, "DEBUG-PIPE-RIGHT: Setting up right pipe process for '%s'\n",
//         cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     // Setup pipe redirection for stdin
//     if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-RIGHT: Failed to redirect stdin from pipe\n");
//         close(pipe_fd[0]);
//         return (1);
//     }
//     close(pipe_fd[0]);
    
//     // Process redirections if any
//     if (cmd_node->redir && is_redirection(cmd_node->redir->type))
//     {
//         fprintf(stderr, "DEBUG-PIPE-RIGHT: Processing redirections\n");
//         redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
//         if (!redir_success || vars->error_code == ERR_REDIRECTION)
//         {
//             fprintf(stderr, "DEBUG-PIPE-RIGHT: Redirection failed, exiting with 1\n");
//             return (1);
//         }
//     }
    
//     // Execute command
//     fprintf(stderr, "DEBUG-PIPE-RIGHT: Executing command\n");
//     cmd_result = execute_cmd(cmd_node, vars->env, vars);
//     fprintf(stderr, "DEBUG-PIPE-RIGHT: Command execution completed with status %d\n", cmd_result);
    
//     return (cmd_result);
// }
int exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
    int cmd_result;
    int redir_success;
    t_node *actual_cmd;

    fprintf(stderr, "PIPE-DBG-RIGHT: === RIGHT PROCESS START ===\n");
    fprintf(stderr, "PIPE-DBG-RIGHT: Node type=%s, args[0]='%s'\n", 
            get_token_str(cmd_node->type),
            cmd_node->args ? cmd_node->args[0] : "NULL");

    // Setup pipe redirection for stdin
    fprintf(stderr, "PIPE-DBG-RIGHT: Redirecting stdin from pipe read end fd=%d\n", pipe_fd[0]);
    if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
        fprintf(stderr, "PIPE-DBG-RIGHT: dup2 failed: %s\n", strerror(errno));
        close(pipe_fd[0]);
        return 1;
    }

    if (close(pipe_fd[0]) == -1)
        fprintf(stderr, "PIPE-DBG-RIGHT: Error closing read end after dup2: %s\n", strerror(errno));
    else
        fprintf(stderr, "PIPE-DBG-RIGHT: Closed read end fd=%d after dup2\n", pipe_fd[0]);

    // Process redirections based on node type
    if (is_redirection(cmd_node->type)) {
        fprintf(stderr, "PIPE-DBG-RIGHT: Node is redirection type=%s\n", 
                get_token_str(cmd_node->type));
        
        // Find actual command in redirection->left
        if (cmd_node->left && cmd_node->left->type == TYPE_CMD) {
            actual_cmd = cmd_node->left;
            fprintf(stderr, "PIPE-DBG-RIGHT: Found command in left: '%s'\n",
                    actual_cmd->args ? actual_cmd->args[0] : "NULL");
        } else {
            fprintf(stderr, "PIPE-DBG-RIGHT: ERROR: Missing command in redirection->left\n");
            fprintf(stderr, "PIPE-DBG-RIGHT: Left node type=%s\n",
                    cmd_node->left ? get_token_str(cmd_node->left->type) : "NULL");
            return 1;
        }
        
        fprintf(stderr, "PIPE-DBG-RIGHT: Processing redirection chain\n");
        redir_success = proc_redir_chain(cmd_node, actual_cmd, vars);
        
        if (!redir_success) {
            fprintf(stderr, "PIPE-DBG-RIGHT: Redirection failed, error_code=%d\n", 
                    vars->error_code);
            return vars->error_code;
        }
    } else if (cmd_node->type == TYPE_CMD) {
        fprintf(stderr, "PIPE-DBG-RIGHT: Direct command node: '%s'\n", 
                cmd_node->args ? cmd_node->args[0] : "NULL");
        actual_cmd = cmd_node;
    } else if (cmd_node->type == TYPE_PIPE) {
        // CRITICAL FIX: Handle nested pipes by recursively executing them
        fprintf(stderr, "PIPE-DBG-RIGHT: Found nested pipe, executing recursively\n");
        cmd_result = execute_pipes(cmd_node, vars);
        fprintf(stderr, "PIPE-DBG-RIGHT: Nested pipe execution completed with status %d\n", cmd_result);
        return cmd_result;
    } else {
        fprintf(stderr, "PIPE-DBG-RIGHT: Unexpected node type: %s\n",
                get_token_str(cmd_node->type));
        return 1;
    }

    // Check if redirections are attached to this command
    if (actual_cmd->redir) {
        fprintf(stderr, "PIPE-DBG-RIGHT: Command has attached redirections\n");
        redir_success = proc_redir_chain(actual_cmd->redir, actual_cmd, vars);
        if (!redir_success) {
            fprintf(stderr, "PIPE-DBG-RIGHT: Attached redirection failed, error_code=%d\n", 
                    vars->error_code);
            return vars->error_code;
        }
    }

    // Check stdout file descriptor
    int stdout_fileno = fileno(stdout);
    fprintf(stderr, "PIPE-DBG-RIGHT: Current stdout fileno=%d\n", stdout_fileno);
    
    // Check if stdout is pointing to a terminal
    if (isatty(STDOUT_FILENO))
        fprintf(stderr, "PIPE-DBG-RIGHT: stdout is connected to a terminal\n");
    else
        fprintf(stderr, "PIPE-DBG-RIGHT: stdout is NOT connected to a terminal\n");
    
    fprintf(stderr, "PIPE-DBG-RIGHT: Executing command '%s'\n",
            actual_cmd->args ? actual_cmd->args[0] : "NULL");
    
    cmd_result = execute_cmd(actual_cmd, vars->env, vars);
    
    fprintf(stderr, "PIPE-DBG-RIGHT: Command execution completed with status %d\n", cmd_result);
    return cmd_result;
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
// int	execute_pipes(t_node *pipe_node, t_vars *vars)
// {
// 	int		pipe_fd[2];
// 	pid_t	left_pid;
// 	pid_t	right_pid;
// 	int		l_status;
// 	int		r_status;

// 	if (pipe(pipe_fd) == -1)
// 		return (1);
// 	// Create left child process
// 	left_pid = fork();
// 	if (left_pid == -1)
// 	{
// 		close(pipe_fd[0]);
// 		close(pipe_fd[1]);
// 		return (1);
// 	}
// 	if (left_pid == 0)
// 	{
// 		exec_pipe_left(pipe_node->left, pipe_fd, vars);
// 		// Should never reach here
// 		exit(1);
// 	}
// 	// Create right child process
// 	right_pid = fork();
// 	if (right_pid == -1)
// 	{
// 		kill(left_pid, SIGTERM);
// 		close(pipe_fd[0]);
// 		close(pipe_fd[1]);
// 		waitpid(left_pid, &l_status, 0);
// 		return (1);
// 	}
// 	if (right_pid == 0)
// 	{
// 		close(pipe_fd[1]); // Close write end in right child
// 		exec_pipe_right(pipe_node->right, pipe_fd, vars);
// 		// Should never reach here
// 		exit(1);
// 	}
// 	// Close unused pipe ends in parent
// 	close(pipe_fd[0]);
// 	close(pipe_fd[1]);
// 	// Wait for child processes and get their status
// 	waitpid(left_pid, &l_status, 0);
// 	l_status = handle_cmd_status(l_status, NULL);
// 	waitpid(right_pid, &r_status, 0);
// 	// CRITICAL FIX: According to POSIX, the exit status of a pipeline is 
// 	// the exit status of the last command in the pipeline
// 	r_status = handle_cmd_status(r_status, vars);
// 	return (r_status);
// }
// int execute_pipes(t_node *node, t_vars *vars)
// {
//     int pipefd[2];
//     pid_t left_pid, right_pid;
//     int status = 0;
    
//     if (pipe(pipefd) == -1)
//         return (1);
    
//     // Execute left side of pipe
//     left_pid = fork();
//     if (left_pid == 0)
//     {
//         // Child process for left side
//         close(pipefd[0]);
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);
        
//         exit(exec_pipe_left(node->left, vars));
//     }
    
//     // Execute right side of pipe
//     right_pid = fork();
//     if (right_pid == 0)
//     {
//         // Child process for right side
//         close(pipefd[1]);
//         dup2(pipefd[0], STDIN_FILENO);
//         close(pipefd[0]);
        
//         exit(exec_pipe_right(node->right, vars));
//     }
    
//     // Parent process
//     close(pipefd[0]);
//     close(pipefd[1]);
    
//     // Wait for both sides to complete independently
//     waitpid(left_pid, &status, 0);
//     int left_status = WEXITSTATUS(status);
    
//     waitpid(right_pid, &status, 0);
//     int right_status = WEXITSTATUS(status);
    
//     // Return the right side's status as that's the convention for pipes
//     vars->error_code = right_status;
//     return (right_status);
// }
// int execute_pipes(t_node *node, t_vars *vars)
// {
//     int pipefd[2];
//     pid_t left_pid, right_pid;
//     int status = 0;
    
//     if (pipe(pipefd) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Pipe creation failed\n");
//         return (1);
//     }
    
//     // Execute left side of pipe
//     left_pid = fork();
//     if (left_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left fork failed\n");
//         close(pipefd[0]);
//         close(pipefd[1]);
//         return (1);
//     }
    
//     if (left_pid == 0)
//     {
//         // Child process for left side
//         exit(exec_pipe_left(node->left, pipefd, vars));
//         // This point is never reached
//     }
    
//     // Execute right side of pipe
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right fork failed\n");
//         kill(left_pid, SIGTERM);
//         close(pipefd[0]);
//         close(pipefd[1]);
//         waitpid(left_pid, &status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         // Child process for right side
//         close(pipefd[1]);  // Close write end in right child
//         exit(exec_pipe_right(node->right, pipefd, vars));
//         // This point is never reached
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left process: %d, Right process: %d\n",
//             left_pid, right_pid);
    
//     // Parent process
//     close(pipefd[0]);
//     close(pipefd[1]);
    
//     // Wait for both sides to complete independently
//     waitpid(left_pid, &status, 0);
//     int left_status = WEXITSTATUS(status);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left process exited with status %d\n", 
//             left_status);
    
//     waitpid(right_pid, &status, 0);
//     int right_status = WEXITSTATUS(status);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Right process exited with status %d\n", 
//             right_status);
    
//     // Return the right side's status as that's the convention for pipes
//     vars->error_code = right_status;
//     return (right_status);
// }
// int execute_pipes(t_node *node, t_vars *vars)
// {
//     int pipefd[2];
//     pid_t left_pid, right_pid;
//     int status = 0;
    
//     // NEW: Add comprehensive pipeline structure debugging
//     fprintf(stderr, "DEBUG-PIPE-STRUCTURE: === PIPELINE EXECUTION START ===\n");
//     fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Pipe node: %p\n", (void*)node);
//     fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Left side type: %s\n", 
//             node->left ? get_token_str(node->left->type) : "NULL");
//     fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Right side type: %s\n", 
//             node->right ? get_token_str(node->right->type) : "NULL");
    
//     if (node->left) {
//         if (is_redirection(node->left->type)) {
//             fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Left is redirection '%s' -> cmd '%s'\n",
//                     node->left->args ? node->left->args[0] : "NULL",
//                     node->left->left && node->left->left->args ? node->left->left->args[0] : "NULL");
//         } else {
//             fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Left is direct command '%s'\n",
//                     node->left->args ? node->left->args[0] : "NULL");
//         }
//     }
    
//     if (node->right) {
//         if (is_redirection(node->right->type)) {
//             fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Right is redirection '%s' -> cmd '%s'\n",
//                     node->right->args ? node->right->args[0] : "NULL",
//                     node->right->left && node->right->left->args ? node->right->left->args[0] : "NULL");
//         } else {
//             fprintf(stderr, "DEBUG-PIPE-STRUCTURE: Right is direct command '%s'\n",
//                     node->right->args ? node->right->args[0] : "NULL");
//         }
//     }
    
//     if (pipe(pipefd) == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Pipe creation failed\n");
//         return (1);
//     }
    
//     // Execute left side of pipe
//     left_pid = fork();
//     if (left_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Left fork failed\n");
//         close(pipefd[0]);
//         close(pipefd[1]);
//         return (1);
//     }
    
//     if (left_pid == 0)
//     {
//         // Child process for left side
//         exit(exec_pipe_left(node->left, pipefd, vars));
//         // This point is never reached
//     }
    
//     // Execute right side of pipe
//     right_pid = fork();
//     if (right_pid == -1)
//     {
//         fprintf(stderr, "DEBUG-PIPE-EXEC: Right fork failed\n");
//         kill(left_pid, SIGTERM);
//         close(pipefd[0]);
//         close(pipefd[1]);
//         waitpid(left_pid, &status, 0);
//         return (1);
//     }
    
//     if (right_pid == 0)
//     {
//         // Child process for right side
//         close(pipefd[1]);  // Close write end in right child
//         exit(exec_pipe_right(node->right, pipefd, vars));
//         // This point is never reached
//     }
    
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left process: %d, Right process: %d\n",
//             left_pid, right_pid);
    
//     // Parent process
//     close(pipefd[0]);
//     close(pipefd[1]);
    
//     // Wait for both sides to complete independently
//     waitpid(left_pid, &status, 0);
//     int left_status = WEXITSTATUS(status);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Left process exited with status %d\n", 
//             left_status);
    
//     waitpid(right_pid, &status, 0);
//     int right_status = WEXITSTATUS(status);
//     fprintf(stderr, "DEBUG-PIPE-EXEC: Right process exited with status %d\n", 
//             right_status);
    
//     // Return the right side's status as that's the convention for pipes
//     vars->error_code = right_status;
//     return (right_status);
// }
int execute_pipes(t_node *pipe_node, t_vars *vars)
{
    int pipe_fd[2];
    pid_t left_pid, right_pid;
    int left_status, right_status;
    int left_result, right_result;

    fprintf(stderr, "PIPE-DBG-MAIN: === PIPELINE EXECUTION START ===\n");
    fprintf(stderr, "PIPE-DBG-MAIN: %p\n", (void*)pipe_node);
    
    if (!pipe_node || !pipe_node->left || !pipe_node->right) {
        fprintf(stderr, "PIPE-DBG-MAIN: Invalid pipe structure\n");
        return 1;
    }

    // Log pipe structure
    fprintf(stderr, "PIPE-DBG-MAIN: Left type=%s ", 
            get_token_str(pipe_node->left->type));
    fprintf(stderr, "Right type=%s\n", 
            get_token_str(pipe_node->right->type));

    // Determine the actual command nodes
    if (pipe_node->left->type == TYPE_CMD)
        fprintf(stderr, "PIPE-DBG-MAIN: Left is direct command '%s'\n",
                pipe_node->left->args ? pipe_node->left->args[0] : "NULL");
    else
        fprintf(stderr, "PIPE-DBG-MAIN: Left is %s node\n",
                get_token_str(pipe_node->left->type));

    if (pipe_node->right->type == TYPE_CMD)
        fprintf(stderr, "PIPE-DBG-MAIN: Right is direct command '%s'\n",
                pipe_node->right->args ? pipe_node->right->args[0] : "NULL");
    else if (pipe_node->right->type == TYPE_PIPE)
        fprintf(stderr, "PIPE-DBG-MAIN: Right is another pipe node\n");
    else
        fprintf(stderr, "PIPE-DBG-MAIN: Right is %s node\n",
                get_token_str(pipe_node->right->type));

    // Create pipe
    if (pipe(pipe_fd) == -1) {
        fprintf(stderr, "PIPE-DBG-MAIN: Failed to create pipe: %s\n", strerror(errno));
        return 1;
    }
    fprintf(stderr, "PIPE-DBG-MAIN: Created pipe: read_fd=%d, write_fd=%d\n", 
            pipe_fd[0], pipe_fd[1]);

    // Fork for left command
    fprintf(stderr, "PIPE-DBG-MAIN: Setting up left pipe process for '%s'\n",
            pipe_node->left->args ? pipe_node->left->args[0] : 
            (pipe_node->left->type == TYPE_CMD ? "CMD" : get_token_str(pipe_node->left->type)));

    left_pid = fork();
    if (left_pid == -1) {
        fprintf(stderr, "PIPE-DBG-MAIN: Left fork failed: %s\n", strerror(errno));
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return 1;
    }

    if (left_pid == 0) {
        // Child process for left command
        fprintf(stderr, "PIPE-DBG-MAIN: %d\n", getpid());
        left_result = exec_pipe_left(pipe_node->left, pipe_fd, vars);
        fprintf(stderr, "PIPE-DBG-LEFT: Exiting with status %d\n", left_result);
        exit(left_result);
    }

    // Parent continues here
    // Close write end of pipe for parent
    if (close(pipe_fd[1]) == -1)
        fprintf(stderr, "PIPE-DBG-MAIN: Error closing write end in parent: %s\n", strerror(errno));
    else
        fprintf(stderr, "PIPE-DBG-MAIN: Parent closed write end fd=%d\n", pipe_fd[1]);

    // Fork for right command
    fprintf(stderr, "PIPE-DBG-MAIN: Setting up right pipe process for '%s'\n",
            pipe_node->right->args ? pipe_node->right->args[0] : 
            (pipe_node->right->type == TYPE_CMD ? "CMD" : get_token_str(pipe_node->right->type)));

    right_pid = fork();
    if (right_pid == -1) {
        fprintf(stderr, "PIPE-DBG-MAIN: Right fork failed: %s\n", strerror(errno));
        close(pipe_fd[0]);
        kill(left_pid, SIGTERM);
        waitpid(left_pid, NULL, 0);
        return 1;
    }

    if (right_pid == 0) {
        // Child process for right command
        right_result = exec_pipe_right(pipe_node->right, pipe_fd, vars);
        fprintf(stderr, "PIPE-DBG-RIGHT: Exiting with status %d\n", right_result);
        exit(right_result);
    }

    // Parent continues here
    // Close read end of pipe in parent
    if (close(pipe_fd[0]) == -1)
        fprintf(stderr, "PIPE-DBG-MAIN: Error closing read end in parent: %s\n", strerror(errno));
    else
        fprintf(stderr, "PIPE-DBG-MAIN: Parent closed read end fd=%d\n", pipe_fd[0]);

    // Wait for left process
    fprintf(stderr, "PIPE-DBG-MAIN: Waiting for left process (pid=%d)\n", left_pid);
    if (waitpid(left_pid, &left_status, 0) == -1) {
        fprintf(stderr, "PIPE-DBG-MAIN: Error waiting for left process: %s\n", strerror(errno));
    }
    
    // Check left process status
    if (WIFEXITED(left_status)) {
        left_result = WEXITSTATUS(left_status);
        fprintf(stderr, "PIPE-DBG-MAIN: Left process exited with status %d\n", left_result);
    } else if (WIFSIGNALED(left_status)) {
        left_result = 128 + WTERMSIG(left_status);
        fprintf(stderr, "PIPE-DBG-MAIN: Left process terminated by signal %d\n", 
                WTERMSIG(left_status));
    } else {
        left_result = 1;
        fprintf(stderr, "PIPE-DBG-MAIN: Left process exited abnormally\n");
    }

    // Wait for right process
    fprintf(stderr, "PIPE-DBG-MAIN: Waiting for right process (pid=%d)\n", right_pid);
    if (waitpid(right_pid, &right_status, 0) == -1) {
        fprintf(stderr, "PIPE-DBG-MAIN: Error waiting for right process: %s\n", strerror(errno));
    }
    
    // Check right process status
    if (WIFEXITED(right_status)) {
        right_result = WEXITSTATUS(right_status);
        fprintf(stderr, "PIPE-DBG-MAIN: Right process exited with status %d\n", right_result);
    } else if (WIFSIGNALED(right_status)) {
        right_result = 128 + WTERMSIG(right_status);
        fprintf(stderr, "PIPE-DBG-MAIN: Right process terminated by signal %d\n", 
                WTERMSIG(right_status));
    } else {
        right_result = 1;
        fprintf(stderr, "PIPE-DBG-MAIN: Right process exited abnormally\n");
    }

    // Check for special case - if left failed with permission error but right succeeded
    if (left_result == ERR_PERMISSIONS && right_result == 0) {
        fprintf(stderr, "PIPE-DBG-MAIN: Left had permission error but right succeeded\n");
        fprintf(stderr, "PIPE-DBG-MAIN: CORRECTING final status to right result (0)\n");
        return 0;
    }

    // Return the exit status of the last command in the pipeline
    fprintf(stderr, "PIPE-DBG-MAIN: Pipeline finished, returning right status: %d\n", right_result);
    return right_result;
}
