/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/06 01:51:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"


// Helper function to restore standard file descriptors
void reset_std_fd(t_pipe *pipes)
{
    if (!pipes)
        return ;
        
    if (pipes->saved_stdin != -1)
    {
        dup2(pipes->saved_stdin, STDIN_FILENO);
        close(pipes->saved_stdin);
        pipes->saved_stdin = -1;
    }
    
    if (pipes->saved_stdout != -1)
    {
        dup2(pipes->saved_stdout, STDOUT_FILENO);
        close(pipes->saved_stdout);
        pipes->saved_stdout = -1;
    }
    
    // Close heredoc fd if it was opened
    if (pipes->heredoc_fd > 2)
    {
        close(pipes->heredoc_fd);
        pipes->heredoc_fd = -1;
    }
    
    // Close redirection fd if it was opened
    if (pipes->redirection_fd > 2)
    {
        close(pipes->redirection_fd);
        pipes->redirection_fd = -1;
    }
}

/*
Configures pipe redirections for command execution.
- Checks if command is part of a pipe node.
- Closes read end of pipe to avoid descriptor leaks.
- Redirects STDOUT to pipe write end for data flow.
- Ensures proper cleanup of file descriptors.
Returns:
Nothing (void function).
Works with execute_pipe_command() during pipeline execution.

Example: For first command in "ls | grep txt"
- Redirects stdout to pipe
- Output of "ls" flows through pipe to "grep"
- Ensures file descriptors are properly managed
*/
void	init_pipe(t_node *cmd, int *pipe_fd)
{
	if (cmd->type == TYPE_PIPE)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
}

/*
Validates pipe node structure before execution.
- Checks that node is a valid pipe node.
- Ensures both left and right branches exist.
- Verifies node has proper type.
Returns:
1 if valid, 0 if invalid.
Works with execute_pipes() for error checking.

Example: Before executing "cmd1 | cmd2"
- Checks that pipe node has TYPE_PIPE
- Verifies left branch (cmd1) exists
- Verifies right branch (cmd2) exists
*/
int	validate_pipe_node(t_node *pipe_node)
{
	if (!pipe_node)
		return (0);
	if (pipe_node->type != TYPE_PIPE)
		return (0);
	if (!pipe_node->left)
		return (0);
	if (!pipe_node->right)
		return (0);
	return (1);
}

/*
Creates and sets up pipe for command communication.
- Initializes pipe file descriptors.
- Handles pipe creation errors.
Returns:
1 on success, 0 on failure.
Works with execute_pipes() for pipe setup.

Example: Before forking processes
- Creates pipe with read and write ends
- Reports errors if pipe creation fails
- Returns success/failure status
*/
int	setup_pipe(int *pipefd)
{
	if (pipe(pipefd) == -1)
	{
		ft_putendl_fd("pipe: Creation failed", 2);
		return (0);
	}
	return (1);
}

/*
Executes left side of pipe in child process.
- Closes unused pipe ends.
- Redirects stdout to pipe write end.
- Executes left command or sub-pipeline.
Returns:
Never returns (calls exit).
Works with execute_pipes() for left command.

Example: For "ls | grep txt"
- Creates child process for "ls"
- Redirects output to pipe
- Executes "ls" command
- Exits with command status
*/
void	exec_left_cmd(t_node *pipe_node, int *pipefd, t_vars *vars)
{
	int	exit_status;

	close(pipefd[0]);
	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		exit(1);
	close(pipefd[1]);
	exit_status = execute_cmd(pipe_node->left, vars->env, vars);
	exit(exit_status);
}

/*
Executes right side of pipe in child process.
- Closes unused pipe ends.
- Redirects stdin to pipe read end.
- Executes right command or sub-pipeline.
Returns:
Never returns (calls exit).
Works with execute_pipes() for right command.

Example: For "ls | grep txt"
- Creates child process for "grep txt"
- Redirects input from pipe
- Executes "grep" command
- Exits with command status
*/
void	exec_right_cmd(t_node *pipe_node, int *pipefd, t_vars *vars)
{
	t_node	*cmd;
	t_node	*redir_node;
	int		saved_stdout;
	int		append;

	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
		exit(1);
	close(pipefd[0]);
	cmd = pipe_node->right;
	redir_node = NULL;
	if (cmd && cmd->right)
	{
		redir_node = cmd->right;
		if (is_redirection(redir_node->type))
		{
			saved_stdout = dup(STDOUT_FILENO);
			if (saved_stdout == -1)
				exit(1);
			append = 0;
			if (redir_node->type == TYPE_APPEND_REDIRECT)
				append = 1;
			if (!output_redirect(redir_node, &saved_stdout, append, vars))
				exit(1);
		}
	}
	exit(execute_cmd(pipe_node->right, vars->env, vars));
}

/*
Creates child process for a pipeline command.
- Forks new process.
- Handles fork errors.
- Executes specified command in child.
Returns:
Process ID on success, -1 on failure.
Works with execute_pipes() for process creation.

Example: Creating process for command in pipeline
- Forks new process
- Child executes command function
- Parent returns child's PID
- Returns -1 if fork fails
*/
pid_t	make_child_proc(t_node *pipe_node, int *pipefd,
		t_vars *vars, int is_left)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (is_left)
			exec_left_cmd(pipe_node, pipefd, vars);
		else
			exec_right_cmd(pipe_node, pipefd, vars);
	}
	else if (pid < 0)
		ft_putendl_fd("fork: Creation failed", 2);
	return (pid);
}

/*
Cleans up resources allocated during pipe completion processing.
- Frees AST structure if provided.
- Releases pipe command buffer if requested.
- Frees result buffer if requested.
- Centralizes cleanup to avoid code duplication.
Works with handle_pipe_completion().
*/
void	reset_done_pipes(t_ast *ast, char **pipe_cmd, char **result,
				int free_flags)
{
	if (ast)
		cleanup_ast_struct(ast);
	if ((free_flags & 1) && pipe_cmd && *pipe_cmd)
		free(*pipe_cmd);
	if ((free_flags & 2) && result && *result)
		free(*result);
}

/*
Prepares and validates a command for pipe completion processing.
- Creates working copies of command string.
- Checks if command needs completion based on syntax flag.
- Creates AST structure for processing.
Returns:
- 1 on successful preparation.
- 0 if no completion needed (early return case).
- -1 on preparation errors.
Works with handle_pipe_completion().
*/
int	prep_pipe_complete(char *cmd, char **result, char **pipe_cmd, t_ast **ast)
{
	*result = ft_strdup(cmd);
	if (!*result)
		return (-1);
	*pipe_cmd = ft_strdup(*result);
	if (!*pipe_cmd)
	{
		free(*result);
		return (-1);
	}
	*ast = init_ast();
	if (!*ast)
	{
		free(*result);
		free(*pipe_cmd);
		return (-1);
	}
	return (1);
}

/*
Handles pipe completion for commands needing continuation.
- Processes commands with trailing pipes.
- Prompts for additional input as needed.
Returns:
- Complete command string or NULL on error.
Works with process_pipe_syntax().
Example: For "ls |" (incomplete pipe)
- Returns: "ls | grep hello" after user inputs "grep hello"
*/
char	*handle_pipe_completion(char *cmd, t_vars *vars, int syntax_chk)
{
	char	*pipe_cmd;
	char	*result;
	t_ast	*ast;
	int		prep_status;

	if (!syntax_chk || is_input_complete(vars))
		return (ft_strdup(cmd));
	prep_status = prep_pipe_complete(cmd, &result, &pipe_cmd, &ast);
	if (prep_status < 0)
		return (NULL);
	if (prep_status == 0)
		return (ft_strdup(cmd));
	if (handle_unfinished_pipes(&pipe_cmd, vars, ast) < 0)
	{
		reset_done_pipes(ast, &pipe_cmd, &result, 3);
		return (NULL);
	}
	reset_done_pipes(ast, NULL, &result, 2);
	return (pipe_cmd);
}

/*
Sets up and launches child processes for a pipeline.
- Creates pipe for command communication.
- Launches left and right child processes.
- Handles fork and pipe errors appropriately.
Returns:
0 on success with pids set, 1 on error.
Works with run_pipes() during pipeline execution.

Example: For "ls | grep txt" pipeline
- Creates pipe between processes
- Launches child processes for both commands
- Sets process IDs in left_pid and right_pid
*/
int	setup_pipes_procs(t_node *pipe_node, t_vars *vars,
	pid_t *left_pid, pid_t *right_pid)
{
	int	pipefd[2];

	if (!validate_pipe_node(pipe_node))
		return (1);
	if (!setup_pipe(pipefd))
		return (1);
	*left_pid = make_child_proc(pipe_node, pipefd, vars, 1);
	if (*left_pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	*right_pid = make_child_proc(pipe_node, pipefd, vars, 0);
	if (*right_pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(*left_pid, NULL, 0);
		return (1);
	}
	close(pipefd[0]);
	close(pipefd[1]);
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
//     int     pipefd[2];
//     pid_t   left_pid;
// 	pid_t	right_pid;
//     int     left_status = 0;
//     int     right_status = 0;
    
//     if (!pipe_node || pipe_node->type != TYPE_PIPE)
//         return 1;
    
//     // Initialize pipes structure if not already done
//     if (!vars->pipes)
//     {
//         vars->pipes = init_pipes();
//         if (!vars->pipes)
//             return print_error("failed to initialize pipes", vars, 1);
//     }
// 	// Debugging output
// 	fprintf(stderr, "[DEBUG-PIPE] Starting execute_pipes on node type=%d\n", pipe_node->type);
// 	if (pipe_node->left && pipe_node->left->args)
//     	fprintf(stderr, "[DEBUG-PIPE] Left command: %s\n", pipe_node->left->args[0]);
// 	if (pipe_node->right && pipe_node->right->args)
//     	fprintf(stderr, "[DEBUG-PIPE] Right command/pipe: %s\n", pipe_node->right->args[0]);

//     // Track pipe depth
//     vars->pipes->pipe_count++;
    
//     // Create the pipe
//     if (pipe(pipefd) == -1)
//         return print_error("pipe creation failed", vars, 1);
//     // After pipe creation
// 	fprintf(stderr, "[DEBUG-PIPE] Created pipe: fd[0]=%d, fd[1]=%d\n", pipefd[0], pipefd[1]);
//     // Execute left side of pipe
//     left_pid = fork();
//     if (left_pid == 0)
//     {
//         // Child process for left command
//         close(pipefd[0]);  // Close read end
//         if (dup2(pipefd[1], STDOUT_FILENO) == -1)
//             exit(1);
//         close(pipefd[1]);
// 		// In left child process before execution
// 		fprintf(stderr, "[DEBUG-PIPE] Left child process (pid=%d) redirecting stdout to fd=%d\n", getpid(), pipefd[1]);
//         exit(execute_cmd(pipe_node->left, vars->env, vars));
//     }
//     else if (left_pid < 0)
//     {
//         close(pipefd[0]);
//         close(pipefd[1]);
//         return print_error("fork failed", vars, 1);
//     }
    
//     // Parent process
//     close(pipefd[1]);  // Parent doesn't write to pipe
    
//     // Handle right side - check if it's a pipe or command
//     if (pipe_node->right->type == TYPE_PIPE)
//     {
//         // Save current stdin using the struct field
//         vars->pipes->saved_stdin = dup(STDIN_FILENO);
        
//         // Connect pipe output to stdin
//         if (dup2(pipefd[0], STDIN_FILENO) == -1)
//         {
//             close(pipefd[0]);
//             return print_error("dup2 failed", vars, 1);
//         }
//         close(pipefd[0]);
//         // Before recursive call for right pipe
// 		fprintf(stderr, "[DEBUG-PIPE] About to recurse on right pipe (pid=%d)\n", getpid());
//         // Recursively handle the rest of the pipeline
//         right_status = execute_pipes(pipe_node->right, vars);
        
//         // Restore stdin
//         if (vars->pipes->saved_stdin > 2)
//         {
//             dup2(vars->pipes->saved_stdin, STDIN_FILENO);
//             close(vars->pipes->saved_stdin);
//             vars->pipes->saved_stdin = -1;
//         }
//     }
//     else
//     {
//         // Right side is a command, fork and execute it
//         right_pid = fork();
//         if (right_pid == 0)
//         {
//             // Child process
// 			// When handling right command execution
// 			fprintf(stderr, "[DEBUG-PIPE] Executing right command directly (pid=%d)\n", getpid());
//             if (dup2(pipefd[0], STDIN_FILENO) == -1)
//                 exit(1);
//             close(pipefd[0]);
//             exit(execute_cmd(pipe_node->right, vars->env, vars));
//         }
//         else if (right_pid < 0)
//         {
//             close(pipefd[0]);
//             return print_error("fork failed", vars, 1);
//         }
//         // When waiting for processes
// 		fprintf(stderr, "[DEBUG-PIPE] Parent waiting for child pid=%d\n", left_pid);
//         // Parent closes pipe and waits for right command
//         close(pipefd[0]);
//         waitpid(right_pid, &right_status, 0);
//     }
    
//     // Wait for left command to complete
//     waitpid(left_pid, &left_status, 0);
    
//     // Decrement pipe count as we exit this level
//     vars->pipes->pipe_count--;
    
//     // Return the status of the last command in the pipeline
//     vars->error_code = handle_cmd_status(right_status, vars);
//     return vars->error_code;
// }
/* 
 * Main function to execute a pipeline of commands
 * Returns exit code of last command in pipeline
 */
/* 
 * Simplified pipeline execution using the cmd_nodes array
 * Return: Final command status
 */
int execute_pipes(t_node *pipe_node, t_vars *vars)
{
    int pipe_count;
    int result;
    
    if (!pipe_node || pipe_node->type != TYPE_PIPE)
        return (1);
    
    DBG_PRINTF(DEBUG_EXEC, "=== EXECUTING PIPELINE ===\n");
    
    // Get pipe count from command count
    pipe_count = vars->cmd_count - 1;  // Pipes = commands - 1
    if (pipe_count < 1)
        return (1);
        
    DBG_PRINTF(DEBUG_EXEC, "Pipeline has %d pipes (%d commands)\n", 
              pipe_count, vars->cmd_count);
    
    // Initialize pipe arrays
    if (!init_pipe_arrays(vars->pipes, pipe_count))
        return print_error("Failed to allocate pipe arrays", vars, 1);
    vars->pipes->pipe_count = pipe_count;
    
    // Simply copy commands from cmd_nodes (already in correct order)
    int i = 0;
    while (i < vars->cmd_count) {
        vars->pipes->exec_cmds[i] = vars->cmd_nodes[i];
        DBG_PRINTF(DEBUG_EXEC, "Using command[%d]: %s\n", 
                  i, vars->cmd_nodes[i]->args[0]);
        i++;
    }
    
    // Rest of pipeline execution remains the same...
    if (!make_pipes(vars->pipes, pipe_count))
        return print_error("Failed to create pipes", vars, 1);
    
    if (!fork_processes(vars->pipes, pipe_count, vars))
    {
        close_all_pipe_fds(vars->pipes);
        return print_error("Failed to create processes", vars, 1);
    }
    
    close_all_pipe_fds(vars->pipes);
    
    result = wait_for_processes(vars->pipes, vars);
    DBG_PRINTF(DEBUG_EXEC, "=== PIPELINE EXECUTION COMPLETE ===\n");
    return result;
}

/*
Sets up all redirections for a command and executes it.
- Processes all input redirections first.
- Then processes all output redirections.
- Finally executes the command with properly set up streams.
Returns:
Command execution status code properly stored in vars->error_code.
*/
int setup_cmd_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node  *current;
    int     result;
    // char    *delimiter;
    
    // Initialize pipes structure if it doesn't exist
    if (!vars->pipes)
    {
        vars->pipes = init_pipes();
        if (!vars->pipes)
            return (vars->error_code = 1);
    }
    
    // Save original stdin/stdout for restoration in pipes struct
    vars->pipes->saved_stdin = dup(STDIN_FILENO);
    vars->pipes->saved_stdout = dup(STDOUT_FILENO);
    if (vars->pipes->saved_stdin == -1 || vars->pipes->saved_stdout == -1)
        return (vars->error_code = 1);
        
    // Find and process input redirections
    current = find_linked_redirects(cmd_node, vars);
    while (current)
    {
        if (current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC)
        {
            // Set current redirection node in pipes struct
            vars->pipes->current_redirect = current;
            
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Set up the input redirection
            if (current->type == TYPE_IN_REDIRECT)
            {
                if (!setup_in_redir(current, vars))
                {
                    // Restore file descriptors
                    reset_redirect_fds(vars);
                    return (vars->error_code);
                }
            }
            else if (current->type == TYPE_HEREDOC)
            {
                // Handle heredoc
                if (!proc_heredoc(current, vars))
                {
                    reset_redirect_fds(vars);
                    return (vars->error_code);
                }
            }
        }
        current = current->next;
    }
    
    // Find and process output redirections
    current = find_linked_redirects(cmd_node, vars);
    while (current)
    {
        if (current->type == TYPE_OUT_REDIRECT || current->type == TYPE_APPEND_REDIRECT)
        {
            // Set current redirection node in pipes struct
            vars->pipes->current_redirect = current;
            
            // Set mode based on redirection type without ternary
            if (current->type == TYPE_APPEND_REDIRECT)
                vars->pipes->out_mode = OUT_MODE_APPEND;
            else
                vars->pipes->out_mode = OUT_MODE_TRUNCATE;
            
            // Process quotes and set up output redirection
            if (!setup_out_redir(current, vars))
            {
                reset_redirect_fds(vars);
                return (vars->error_code);
            }
        }
        current = current->next;
    }
    
    // Execute the command
    result = execute_cmd(cmd_node, vars->env, vars);
    
    // Restore original file descriptors
    reset_redirect_fds(vars);
    
    return result;
}

/*
Determines if a redirection node is related to a command node.
- Uses the find_linked_redirects function to get related redirections
- Checks if the given redirection is in the chain of related redirections
Returns:
1 if related, 0 if not.
Works with setup_multi_redirects().
*/
int	is_related_to_cmd(t_node *redir_node, t_node *cmd_node, t_vars *vars)
{
    t_node *linked_redirect;
    
    if (!redir_node || !cmd_node)
        return (0);
    // Get the first linked redirection
    linked_redirect = find_linked_redirects(cmd_node, vars);
    // Traverse the chain of redirections
    while (linked_redirect)
    {
        if (linked_redirect == redir_node)
            return (1);
        // Move to the next redirection node
        linked_redirect = linked_redirect->next;
        // Stop if we hit a command or pipe
        if (!linked_redirect || linked_redirect->type == TYPE_CMD || 
            linked_redirect->type == TYPE_PIPE)
            break ;
        // Skip non-redirection nodes
        if (!is_redirection(linked_redirect->type))
            continue ;
    }
    return (0);
}

/*
Sets up multiple redirections for a command.
- Processes all input redirections first.
- Then processes all output redirections.
- Handles error reporting for failed redirections.
Returns:
1 on success, 0 on failure.
Works with execute_pipeline() for handling multiple redirections.
*/
int setup_multi_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node *current;
    int fd_in;
    int fd_out;
    
    fd_in = STDIN_FILENO;
    fd_out = STDOUT_FILENO;
    
    // First pass: set up all input redirections
    current = vars->head;
    while (current)
    {
        if ((current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC) &&
            is_related_to_cmd(current, cmd_node, vars))
        {
            // Process the input redirection
            if (!setup_redirection(current, vars, &fd_in))
                return (0);
        }
        current = current->next;
    }
    
    // Second pass: set up all output redirections
    current = vars->head;
    while (current)
    {
        if ((current->type == TYPE_OUT_REDIRECT || current->type == TYPE_APPEND_REDIRECT) &&
            is_related_to_cmd(current, cmd_node, vars))
        {
            // Process the output redirection
            if (!setup_redirection(current, vars, &fd_out))
                return (0);
        }
        current = current->next;
    }
    
    return (1);
}

/*
Processes quotes in a redirection node's filename.
- Handles both single and double quotes in filenames.
- Removes surrounding quotes while preserving content.
- Updates the node's args[0] with processed filename.
*/
void	process_quotes_in_redirect(t_node *node)
{
    char	*str;
    int		len;
    char	*new_str;
    int		is_quoted;

    if (!node || !node->args || !node->args[0])
        return ;
    str = node->args[0];
    len = ft_strlen(str);
    is_quoted = 0;
    
    // Check if string has matching quotes at start and end
    if (len >= 2)
    {
        if (str[0] == '"' && str[len - 1] == '"')
            is_quoted = 1;
        else if (str[0] == '\'' && str[len - 1] == '\'')
            is_quoted = 1;
    }
    
    if (is_quoted)
    {
        // Create new string without quotes
        new_str = ft_substr(str, 1, len - 2);
        if (new_str)
        {
            // Replace original string with new one
            free(node->args[0]);
            node->args[0] = new_str;
        }
    }
}

/*
Finds all redirection nodes related to a command.
Returns the first redirection node in the chain.
*/
t_node *find_linked_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node *current;
    t_node *cmd_prev;
    
    if (!cmd_node || !vars->head)
        return (NULL);
    // Find the position of this command in the token list
    current = vars->head;
    cmd_prev = NULL;
    while (current && current != cmd_node)
    {
        cmd_prev = current;
        current = current->next;
    }
    if (!current) // Command not found in token list
        return (NULL);
    // Start from command and scan forward for redirections
    // until we hit another command or the end
    current = cmd_node->next;
    while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
    {
        if (is_redirection(current->type))
            return (current);
        current = current->next;
    }
    // Start from before command and scan backward for redirections
    // until we hit another command or the beginning
    current = cmd_prev;
    while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
    {
        if (is_redirection(current->type))
            return (current);
        current = current->prev;
    }
    return (NULL);
}

/* 
Extracts all commands from pipe structure 
*/
void get_pipe_cmds(t_node *pipe_node, t_pipe *pipes)
{
    int cmd_idx;
    t_node *current;
    
    cmd_idx = 0;
    current = pipe_node;
    
    // First command is always the left side of the first pipe
    if (pipe_node && pipe_node->left)
    {
        pipes->exec_cmds[cmd_idx] = pipe_node->left;
        cmd_idx++;
        
        // Debug command extraction
        DBG_PRINTF(DEBUG_EXEC, "Extracted CMD[%d]: %s\n", 
                  cmd_idx-1, pipes->exec_cmds[cmd_idx-1]->args[0]);
    }
    
    // Extract all middle commands by traversing pipe chain
    while (current && current->type == TYPE_PIPE)
    {
        if (current->right)
        {
            if (current->right->type == TYPE_PIPE)
            {
                // If right child is pipe, extract its left child as command
                if (current->right->left)
                {
                    pipes->exec_cmds[cmd_idx] = current->right->left;
                    DBG_PRINTF(DEBUG_EXEC, "Extracted CMD[%d]: %s\n", 
                            cmd_idx, pipes->exec_cmds[cmd_idx]->args[0]);
                    cmd_idx++;
                }
                current = current->right; // Move to next pipe node
            }
            else
            {
                pipes->exec_cmds[cmd_idx] = current->right;
                DBG_PRINTF(DEBUG_EXEC, "Extracted last CMD[%d]: %s\n", 
                          cmd_idx, pipes->exec_cmds[cmd_idx]->args[0]);
                cmd_idx++;
                break;
            }
        }
        else
        {
            break;
        }
    }
    
    // Verify all commands are extracted
    for (int i = 0; i < cmd_idx; i++)
    {
        DBG_PRINTF(DEBUG_EXEC, "VERIFY CMD[%d]: %s\n", i, 
                  pipes->exec_cmds[i]->args[0]);
    }
}

/* 
 * Creates all pipes needed for execution 
 * Returns 1 on success, 0 on failure
 */
int	make_pipes(t_pipe *pipes, int pipe_count)
{
    int i;
    int j;
    
    i = 0;
    DBG_PRINTF(DEBUG_EXEC, "Creating %d pipes\n", pipe_count);
    while (i < pipe_count)
    {
        /* Reuse existing setup_pipe function */
        if (!setup_pipe(pipes->pipe_fds + (i * 2)))
        {
			DBG_PRINTF(DEBUG_EXEC, "Failed to create pipe %d\n", i);
            /* Clean up previously created pipes */
            /* Clean up previously created pipes */
            j = 0;
            while (j < i)
            {
                close(pipes->pipe_fds[j * 2]);
                close(pipes->pipe_fds[j * 2 + 1]);
                j++;
            }
            return (0);
        }
		DBG_PRINTF(DEBUG_EXEC, "Pipe %d created: read_fd=%d, write_fd=%d\n", 
			i, pipes->pipe_fds[i * 2], pipes->pipe_fds[i * 2 + 1]);
        i++;
    }
    return (1);
}

/* 
 * Sets up child process pipe redirections
 */
void setup_child_pipes(t_pipe *pipes, int cmd_idx, int pipe_count)
{
    int i;
    
	DBG_PRINTF(DEBUG_EXEC, "Setting up pipes for command %d of %d\n", 
		cmd_idx, pipe_count);
    /* Set up stdin from previous pipe (if not first command) */
    if (cmd_idx > 0)
    {
		DBG_PRINTF(DEBUG_EXEC, "Command %d: Redirecting stdin from pipe %d (fd=%d)\n", 
                  cmd_idx, cmd_idx-1, pipes->pipe_fds[(cmd_idx - 1) * 2]);
        dup2(pipes->pipe_fds[(cmd_idx - 1) * 2], STDIN_FILENO);
    }
	else
    {
        DBG_PRINTF(DEBUG_EXEC, "Command %d: Using original stdin\n", cmd_idx);
    }
    
    /* Set up stdout to next pipe (if not last command) */
    if (cmd_idx < pipe_count)
    {
        DBG_PRINTF(DEBUG_EXEC, "Command %d: Redirecting stdout to pipe %d (fd=%d)\n", 
                  cmd_idx, cmd_idx, pipes->pipe_fds[cmd_idx * 2 + 1]);
        dup2(pipes->pipe_fds[cmd_idx * 2 + 1], STDOUT_FILENO);
    }
    else
    {
        DBG_PRINTF(DEBUG_EXEC, "Command %d: Using original stdout (last command)\n", cmd_idx);
    }
    /* Close all pipe fds in child */
    DBG_PRINTF(DEBUG_EXEC, "Closing all pipe fds in child %d\n", cmd_idx);
    i = 0;
    while (i < pipe_count * 2)
    {
        close(pipes->pipe_fds[i]);
        i++;
    }
}

/* 
 * Creates all child processes for the pipeline
 * Returns 1 on success, 0 on failure
 */
int fork_processes(t_pipe *pipes, int pipe_count, t_vars *vars)
{
    int		i;
    int		j;
    pid_t	pid;
    
    i = 0;
	DBG_PRINTF(DEBUG_EXEC, "Creating %d processes for pipeline\n", pipe_count + 1);
    
    while (i <= pipe_count)
    {
		if (pipes->exec_cmds[i] && pipes->exec_cmds[i]->args)
            DBG_PRINTF(DEBUG_EXEC, "Forking process for command %d: %s\n", 
                       i, pipes->exec_cmds[i]->args[0]);
        else
            DBG_PRINTF(DEBUG_EXEC, "Forking process for command %d: <null>\n", i);
            
        pid = fork();
        if (pid < 0)
        {
            DBG_PRINTF(DEBUG_EXEC, "Fork failed for command %d\n", i);
            /* Handle fork error */
            j = 0;
            while (j < i)
            {
                kill(pipes->pids[j], SIGTERM);
                j++;
            }
            return (0);
        }
        else if (pid == 0)
        {
            /* Child process - set up pipes and execute command */
			DBG_PRINTF(DEBUG_EXEC, "Child process %d (pid=%d) setting up pipes\n", i, getpid());
            setup_child_pipes(pipes, i, pipe_count);
            DBG_PRINTF(DEBUG_EXEC, "Child process %d executing command\n", i);
            exit(execute_cmd(pipes->exec_cmds[i], vars->env, vars));
        }
         /* Parent process */
		 DBG_PRINTF(DEBUG_EXEC, "Parent saved child pid %d for command %d\n", pid, i);
        pipes->pids[i] = pid;
        i++;
    }
    return (1);
}

/* 
 * Counts pipe nodes in a command chain
 */
int count_pipes(t_node *node)
{
    int count;
    t_node *current;
    
    count = 0;
    current = node;
    DBG_PRINTF(DEBUG_EXEC, "Starting pipe count for AST node %p\n", (void*)node);
    
    while (current && current->type == TYPE_PIPE)
    {
        count++;
		DBG_PRINTF(DEBUG_EXEC, "Found pipe %d\n", count);
        current = current->right;
        if (current && current->type != TYPE_PIPE)
            break;
    }
    DBG_PRINTF(DEBUG_EXEC, "Total pipes counted: %d\n", count);
    return count;
}

/*
 * Closes all pipe file descriptors in parent
 */
void close_all_pipe_fds(t_pipe *pipes)
{
    int i;
    int count;
    
    i = 0;
    count = pipes->pipe_count * 2;
    
    while (i < count)
    {
        close(pipes->pipe_fds[i]);
        i++;
    }
}

/*
 * Waits for all child processes
 */
int wait_for_processes(t_pipe *pipes, t_vars *vars)
{
    int i;
    int last_status;
    i = 0;
	DBG_PRINTF(DEBUG_EXEC, "Parent waiting for %d processes\n", pipes->pipe_count + 1);
    
    while (i <= pipes->pipe_count)
    {
        DBG_PRINTF(DEBUG_EXEC, "Waiting for process %d (pid=%d)\n", i, pipes->pids[i]);
        waitpid(pipes->pids[i], &(pipes->status[i]), 0);
        DBG_PRINTF(DEBUG_EXEC, "Process %d (pid=%d) exited with status %d\n", 
                  i, pipes->pids[i], WEXITSTATUS(pipes->status[i]));
        i++;
    }
    
    /* Return status of last command */
    last_status = handle_cmd_status(pipes->status[pipes->pipe_count], vars);
    DBG_PRINTF(DEBUG_EXEC, "Pipeline completed with final status %d\n", last_status);
    return (last_status);
}

/* 
 * Initializes arrays in pipe structure based on pipe count
 * Returns 1 on success, 0 on failure
 */
int init_pipe_arrays(t_pipe *pipes, int pipe_count)
{
    /* Free previous arrays if they exist */
    if (pipes->pipe_fds)
        free(pipes->pipe_fds);
    if (pipes->pids)
        free(pipes->pids);
    if (pipes->status)
        free(pipes->status);
    if (pipes->exec_cmds)
        free(pipes->exec_cmds);
    
    /* Allocate pipe file descriptors array (2 fds per pipe) */
    pipes->pipe_fds = (int *)malloc(sizeof(int) * pipe_count * 2);
    if (!pipes->pipe_fds)
        return (0);
        
    /* Allocate process IDs array (cmd_count = pipe_count + 1) */
    pipes->pids = (pid_t *)malloc(sizeof(pid_t) * (pipe_count + 1));
    if (!pipes->pids)
    {
        free(pipes->pipe_fds);
        pipes->pipe_fds = NULL;
        return (0);
    }
        
    /* Allocate status array and command node array */
    pipes->status = (int *)malloc(sizeof(int) * (pipe_count + 1));
    pipes->exec_cmds = (t_node **)malloc(sizeof(t_node *) * (pipe_count + 1));
    
    if (!pipes->status || !pipes->exec_cmds)
    {
        free(pipes->pipe_fds);
        free(pipes->pids);
        free(pipes->status);
        free(pipes->exec_cmds);
        pipes->pipe_fds = NULL;
        pipes->pids = NULL;
        pipes->status = NULL;
        pipes->exec_cmds = NULL;
        return (0);
    }
    
    return (1);
}
