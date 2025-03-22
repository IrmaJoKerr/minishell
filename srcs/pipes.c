/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:41 by bleow             #+#    #+#             */
/*   Updated: 2025/03/23 03:09:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"


// Helper function to restore standard file descriptors
void reset_std_fd(t_pipe *pipeline)
{
    if (!pipeline)
        return ;
        
    if (pipeline->saved_stdin != -1)
    {
        dup2(pipeline->saved_stdin, STDIN_FILENO);
        close(pipeline->saved_stdin);
        pipeline->saved_stdin = -1;
    }
    
    if (pipeline->saved_stdout != -1)
    {
        dup2(pipeline->saved_stdout, STDOUT_FILENO);
        close(pipeline->saved_stdout);
        pipeline->saved_stdout = -1;
    }
    
    // Close heredoc fd if it was opened
    if (pipeline->heredoc_fd > 2)
    {
        close(pipeline->heredoc_fd);
        pipeline->heredoc_fd = -1;
    }
    
    // Close redirection fd if it was opened
    if (pipeline->redirection_fd > 2)
    {
        close(pipeline->redirection_fd);
        pipeline->redirection_fd = -1;
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
Works with execute_pipeline() for error checking.

Example: Before executing "cmd1 | cmd2"
- Checks that pipe node has TYPE_PIPE
- Verifies left branch (cmd1) exists
- Verifies right branch (cmd2) exists
OLD VERSION
int	validate_pipe_node(t_node *pipe_node)
{
    if (!pipe_node || pipe_node->type != TYPE_PIPE)
        return (0);
    if (!pipe_node->left || !pipe_node->right)
        return (0);
    return (1);
}
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
Works with execute_pipeline() for pipe setup.

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
Works with execute_pipeline() for left command.

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
Works with execute_pipeline() for right command.

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
Works with execute_pipeline() for process creation.

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
		ft_safefree((void **)pipe_cmd);
	if ((free_flags & 2) && result && *result)
		ft_safefree((void **)result);
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
		ft_safefree((void **)result);
		return (-1);
	}
	*ast = init_ast_struct();
	if (!*ast)
	{
		ft_safefree((void **)result);
		ft_safefree((void **)pipe_cmd);
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
Works with run_pipeline() during pipeline execution.

Example: For "ls | grep txt" pipeline
- Creates pipe between processes
- Launches child processes for both commands
- Sets process IDs in left_pid and right_pid
*/
int	setup_pipeline_procs(t_node *pipe_node, t_vars *vars,
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
OLDER WORKING VERSION
int	execute_pipeline(t_node *pipe_node, t_vars *vars)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;

	if (!validate_pipe_node(pipe_node))
		return (1);
	if (!setup_pipe(pipefd))
		return (1);
	pid1 = fork();
	if (pid1 < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	if (pid1 == 0)
	{
		exec_left_cmd(pipe_node, pipefd, vars);
		exit(1);
	}
	pid2 = fork();
	if (pid2 < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	if (pid2 == 0)
	{
		exec_right_cmd(pipe_node, pipefd, vars);
		exit(1);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	return (WEXITSTATUS(status2));
}
*/
/*NEWER OLD VERSION
int execute_pipeline(t_node *pipe_node, t_vars *vars)
{
    int		pipefd[2];
    pid_t	left_pid;
    pid_t	right_pid;  // Fixed typo in pid_t declaration
    int		status;
    
    status = 0;
    if (!pipe_node || pipe_node->type != TYPE_PIPE)
        return (1);
    // Create the pipe
    if (pipe(pipefd) == -1)
    {
        print_error("pipe creation failed", vars, 1);
        return (1);
    }
    // Execute left side of pipe
    left_pid = fork();
    if (left_pid == 0)
    {
        // Child process for left command
        close(pipefd[0]);  // Close read end
        // Redirect stdout to pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            exit(1);
        close(pipefd[1]);  // Close original write end
        // Handle any redirections in the left command
        t_node *left_cmd = pipe_node->left;
        setup_cmd_redirects(left_cmd, vars);
        // Execute the left command
        int result = execute_cmd(left_cmd, vars->env, vars);
        exit(result);
    }
    else if (left_pid < 0)
    {
        print_error("fork failed", vars, 1);
        close(pipefd[0]);
        close(pipefd[1]);
        return (1);
    }
    // Execute right side of pipe
    right_pid = fork();
    if (right_pid == 0)
    {
        // Child process for right command
        close(pipefd[1]);  // Close write end
        // Redirect stdin to pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
            exit(1);
        close(pipefd[0]);  // Close original read end
        // Handle any redirections in the right command
        t_node *right_cmd = pipe_node->right;
        setup_cmd_redirects(right_cmd, vars);
        // Execute the right command
        int result = execute_cmd(right_cmd, vars->env, vars);
        exit(result);
    }
    else if (right_pid < 0)
    {
        print_error("fork failed", vars, 1);
        close(pipefd[0]);
        close(pipefd[1]);
        return (1);
    }
    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    // Wait for both processes
    waitpid(left_pid, &status, 0);
    waitpid(right_pid, &status, 0);
    
    return (handle_cmd_status(status, vars));
}
*/
int execute_pipeline(t_node *pipe_node, t_vars *vars)
{
    int     pipefd[2];
    pid_t   left_pid;
    pid_t   right_pid;
	int		left_status;
	int		right_status = 0;
    
	left_status = 0;
	right_status = 0;
    if (!pipe_node || pipe_node->type != TYPE_PIPE)
        return (1);
        
    // Create the pipe
    if (pipe(pipefd) == -1)
    {
        print_error("pipe creation failed", vars, 1);
        return (1);
    }
    
    // Execute left side of pipe
    left_pid = fork();
    if (left_pid == 0)
    {
        // Child process for left command
        close(pipefd[0]);  // Close read end
        
        // Redirect stdout to pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            exit(1);
            
        close(pipefd[1]);  // Close original write end
        
        // Execute the left command
        exit(execute_cmd(pipe_node->left, vars->env, vars));
    }
    else if (left_pid < 0)
    {
        print_error("fork failed", vars, 1);
        close(pipefd[0]);
        close(pipefd[1]);
        return (1);
    }
    
    // Execute right side of pipe
    right_pid = fork();
    if (right_pid == 0)
    {
        // Child process for right command
        close(pipefd[1]);  // Close write end
        
        // Redirect stdin to pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
            exit(1);
            
        close(pipefd[0]);  // Close original read end
        
        // Execute the right command
        exit(execute_cmd(pipe_node->right, vars->env, vars));
    }
    else if (right_pid < 0)
    {
        print_error("fork failed", vars, 1);
        close(pipefd[0]);
        close(pipefd[1]);
        return (1);
    }
    
    // // Parent process
    // close(pipefd[0]);
    // close(pipefd[1]);
    
    // // Wait for left process (but don't use its status)
    // waitpid(left_pid, NULL, 0);
    
    // // Wait for right process and use its status
    // waitpid(right_pid, &status, 0);
    
    // // Return the status of the right (last) command in the pipeline
    // return (handle_cmd_status(status, vars));
	// Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    
    // Wait for left process
 
    waitpid(left_pid, &left_status, 0);
    
    // Wait for right process and use its status
    
    waitpid(right_pid, &right_status, 0);
    
    // Set error code based on right process status
    vars->error_code = handle_cmd_status(right_status, vars);
    
    return (vars->error_code);
}

/*
Sets up all redirections associated with a command node.
- Traverses through command's redirection nodes.
- Sets up each redirection (in, out, append, heredoc).
- Handles error reporting for failed redirections.
Returns:
1 on success, 0 on failure.
Works with execute_pipeline() to handle command redirections.

Example: For "cmd < infile > outfile":
- Sets up input redirection from "infile"
- Sets up output redirection to "outfile"
- Returns 1 if all redirections succeed
*/
/*
Sets up all redirections for a command and executes it.
- Processes all input redirections first.
- Then processes all output redirections.
- Finally executes the command with properly set up streams.
Returns:
Command execution status code.
Works with execute_pipeline() for handling redirections in pipes.
*/
/*
Sets up all redirections for a command and executes it.
- Processes all input redirections first.
- Then processes all output redirections.
- Finally executes the command with properly set up streams.
Returns:
Command execution status code.
Works with execute_pipeline() for handling redirections in pipes.
*/
/*OLD VERSION
int	setup_cmd_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node	*current;
    int		redirection_fd;
    int		result;
    char    *delimiter;
    
    // Initialize pipeline structure if it doesn't exist
    if (!vars->pipeline)
    {
        vars->pipeline = (t_pipe *)malloc(sizeof(t_pipe));
        if (!vars->pipeline)
            return (1);
        vars->pipeline->saved_stdin = -1;
        vars->pipeline->saved_stdout = -1;
        vars->pipeline->heredoc_fd = -1;
    }
    
    // Save original stdin/stdout for restoration in pipeline struct
    vars->pipeline->saved_stdin = dup(STDIN_FILENO);
    vars->pipeline->saved_stdout = dup(STDOUT_FILENO);
    if (vars->pipeline->saved_stdin == -1 || vars->pipeline->saved_stdout == -1)
        return (1);
        
    // Find and process input redirections
    current = find_linked_redirects(cmd_node, vars);
    while (current)
    {
        if (current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC)
        {
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Set up the input redirection
            if (current->type == TYPE_IN_REDIRECT)
            {
                if (!setup_in_redir(current, &redirection_fd))
                    return (1);
            }
            else if (current->type == TYPE_HEREDOC)
            {
                // Extract delimiter from the heredoc node
                delimiter = NULL;
                if (current->args && current->args[0])
                    delimiter = current->args[0];
                else
                    return (1); // Error: missing delimiter
                
                // Call read_heredoc with proper arguments
                if (!read_heredoc(&redirection_fd, delimiter, vars, 1))
                    return (1);
                    
                vars->pipeline->heredoc_fd = redirection_fd;
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
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Store append mode in the pipeline structure
            vars->pipeline->append_mode = (current->type == TYPE_APPEND_REDIRECT);
            vars->pipeline->current_redirect = current;
            
            // Set up output redirection
            if (!setup_out_redir(current, &redirection_fd, vars->pipeline->append_mode))
                return (1);
        }
        current = current->next;
    }
    
    // Execute the command
    result = execute_cmd(cmd_node, vars->env, vars);
    
    // Restore original stdin/stdout
    if (vars->pipeline->saved_stdin != -1)
    {
        dup2(vars->pipeline->saved_stdin, STDIN_FILENO);
        close(vars->pipeline->saved_stdin);
        vars->pipeline->saved_stdin = -1;
    }
    
    if (vars->pipeline->saved_stdout != -1)
    {
        dup2(vars->pipeline->saved_stdout, STDOUT_FILENO);
        close(vars->pipeline->saved_stdout);
        vars->pipeline->saved_stdout = -1;
    }
    
    // Close heredoc fd if it was opened
    if (vars->pipeline->heredoc_fd > 2)
    {
        close(vars->pipeline->heredoc_fd);
        vars->pipeline->heredoc_fd = -1;
    }
    
    return (result);
}
*/
/*NEWER OLD VERSION
int setup_cmd_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node *current;
    int saved_stdin, saved_stdout;
    
    // Save original stdin/stdout
    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
    
    if (saved_stdin == -1 || saved_stdout == -1)
        return (1);
        
    // Find related redirection nodes
    current = find_linked_redirects(cmd_node, vars);
    
    // First handle all input redirections (< and <<)
    while (current)
    {
        if (current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC)
        {
            // Process quotes in filename
            process_quotes_in_redirect(current);
            
            int fd;
            if (current->type == TYPE_IN_REDIRECT)
            {
                if (!setup_in_redir(current, &fd))
                {
                    // Restore stdin/stdout and return error
                    dup2(saved_stdin, STDIN_FILENO);
                    dup2(saved_stdout, STDOUT_FILENO);
                    close(saved_stdin);
                    close(saved_stdout);
                    return (1);
                }
            }
            else // TYPE_HEREDOC
            {
                // Handle heredoc
                if (!read_heredoc(&fd, current->args[0], vars, 1))
                {
                    // Restore stdin/stdout and return error
                    dup2(saved_stdin, STDIN_FILENO);
                    dup2(saved_stdout, STDOUT_FILENO);
                    close(saved_stdin);
                    close(saved_stdout);
                    return (1);
                }
            }
            close(fd); // Close fd after redirecting
        }
        current = current->next;
    }
    
    // Then handle all output redirections (> and >>)
    current = find_linked_redirects(cmd_node, vars);
    while (current)
    {
        if (current->type == TYPE_OUT_REDIRECT || current->type == TYPE_APPEND_REDIRECT)
        {
            // Process quotes in filename
            process_quotes_in_redirect(current);
            
            int fd;
            if (!setup_out_redir(current, &fd, 
                                current->type == TYPE_APPEND_REDIRECT))
            {
                // Restore stdin/stdout and return error
                dup2(saved_stdin, STDIN_FILENO);
                dup2(saved_stdout, STDOUT_FILENO);
                close(saved_stdin);
                close(saved_stdout);
                return (1);
            }
            close(fd); // Close fd after redirecting
        }
        current = current->next;
    }
    
    // Execute command
    int result = execute_cmd(cmd_node, vars->env, vars);
    
    // Restore stdin/stdout
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
    
    return (result);
}
*/
/*NEWEST OLD VERSION
int setup_cmd_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node  *current;
    int     redirection_fd;
    int     result;
    char    *delimiter;
    
    // Initialize pipeline structure if it doesn't exist
    if (!vars->pipeline)
    {
        vars->pipeline = (t_pipe *)malloc(sizeof(t_pipe));
        if (!vars->pipeline)
            return (1);
        vars->pipeline->saved_stdin = -1;
        vars->pipeline->saved_stdout = -1;
        vars->pipeline->heredoc_fd = -1;
    }
    
    // Save original stdin/stdout for restoration in pipeline struct
    vars->pipeline->saved_stdin = dup(STDIN_FILENO);
    vars->pipeline->saved_stdout = dup(STDOUT_FILENO);
    if (vars->pipeline->saved_stdin == -1 || vars->pipeline->saved_stdout == -1)
        return (1);
        
    // Find and process input redirections
    current = find_linked_redirects(cmd_node, vars);
    while (current)
    {
        if (current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC)
        {
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Set up the input redirection
            if (current->type == TYPE_IN_REDIRECT)
            {
                if (!setup_in_redir(current, &redirection_fd))
                    return (1);
            }
            else if (current->type == TYPE_HEREDOC)
            {
                // Extract delimiter from the heredoc node
                delimiter = NULL;
                if (current->args && current->args[0])
                    delimiter = current->args[0];
                else
                    return (1); // Error: missing delimiter
                
                // Call read_heredoc with proper arguments
                if (!read_heredoc(&redirection_fd, delimiter, vars, 1))
                    return (1);
                    
                vars->pipeline->heredoc_fd = redirection_fd;
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
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Store append mode in the pipeline structure
            vars->pipeline->append_mode = (current->type == TYPE_APPEND_REDIRECT);
            vars->pipeline->current_redirect = current;
            
            // Set up output redirection
            if (!setup_out_redir(current, &redirection_fd, vars->pipeline->append_mode))
                return (1);
        }
        current = current->next;
    }
    
    // Execute the command
    result = execute_cmd(cmd_node, vars->env, vars);
    
    // Restore original stdin/stdout
    if (vars->pipeline->saved_stdin != -1)
    {
        dup2(vars->pipeline->saved_stdin, STDIN_FILENO);
        close(vars->pipeline->saved_stdin);
        vars->pipeline->saved_stdin = -1;
    }
    
    if (vars->pipeline->saved_stdout != -1)
    {
        dup2(vars->pipeline->saved_stdout, STDOUT_FILENO);
        close(vars->pipeline->saved_stdout);
        vars->pipeline->saved_stdout = -1;
    }
    
    // Close heredoc fd if it was opened
    if (vars->pipeline->heredoc_fd > 2)
    {
        close(vars->pipeline->heredoc_fd);
        vars->pipeline->heredoc_fd = -1;
    }
    
    return (result);
}
*/
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
    char    *delimiter;
    
    // Initialize pipeline structure if it doesn't exist
    if (!vars->pipeline)
    {
        vars->pipeline = init_pipeline();
        if (!vars->pipeline)
            return (vars->error_code = 1);
    }
    
    // Save original stdin/stdout for restoration in pipeline struct
    vars->pipeline->saved_stdin = dup(STDIN_FILENO);
    vars->pipeline->saved_stdout = dup(STDOUT_FILENO);
    if (vars->pipeline->saved_stdin == -1 || vars->pipeline->saved_stdout == -1)
        return (vars->error_code = 1);
        
    // Find and process input redirections
    current = find_linked_redirects(cmd_node, vars);
    while (current)
    {
        if (current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC)
        {
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Set up the input redirection
            if (current->type == TYPE_IN_REDIRECT)
            {
                if (!setup_in_redir(current, &(vars->pipeline->redirection_fd), vars))
                {
                    // Restore file descriptors
                    reset_std_fd(vars->pipeline);
                    return (vars->error_code = 1);
                }
            }
            else if (current->type == TYPE_HEREDOC)
            {
                // Extract delimiter from the heredoc node
                delimiter = NULL;
                if (current->args && current->args[0])
                    delimiter = current->args[0];
                else
                {
                    // Restore file descriptors
                    reset_std_fd(vars->pipeline);
                    return (vars->error_code = 1);
                }
                
                // Call read_heredoc with proper arguments
                if (!read_heredoc(&(vars->pipeline->redirection_fd), delimiter, vars, 1))
                {
                    // Restore file descriptors
                    reset_std_fd(vars->pipeline);
                    return (vars->error_code = 1);
                }
                    
                vars->pipeline->heredoc_fd = vars->pipeline->redirection_fd;
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
            // Process quotes in the redirection filename
            process_quotes_in_redirect(current);
            
            // Store append mode in the pipeline structure
            vars->pipeline->append_mode = (current->type == TYPE_APPEND_REDIRECT);
            vars->pipeline->current_redirect = current;
            
            // Set up output redirection
            if (!setup_out_redir(current, &(vars->pipeline->redirection_fd), vars->pipeline->append_mode))
            {
                // Restore file descriptors
                reset_std_fd(vars->pipeline);
                return (vars->error_code = 1);
            }
        }
        current = current->next;
    }
    
    // Execute the command
    result = execute_cmd(cmd_node, vars->env, vars);
    
    // Store result in vars->error_code
    vars->error_code = result;
    
    // Restore original stdin/stdout
    reset_std_fd(vars->pipeline);
    
    return vars->error_code;
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
Helper function to process quotes in redirection file names.
- Handles both single and double quotes in filenames.
- Allows filenames with spaces when quoted.
- Preserves content between quotes.
Works with setup_cmd_redirects().
OLD VERSION
void	process_quotes_in_redirect(t_node *redir_node)
{
    char	*file;
    
    if (!redir_node || !redir_node->args || !redir_node->args[0])
        return ;
        
    file = redir_node->args[0];
    
    // Process quotes by removing surrounding quotes but keeping inner content
    if ((file[0] == '"' && file[ft_strlen(file) - 1] == '"') ||
        (file[0] == '\'' && file[ft_strlen(file) - 1] == '\''))
    {
        process_quotes_in_arg(&redir_node->args[0]);
    }
}
*/
/* NEWER OLDER VERSION
void process_quotes_in_redirect(t_node *redir_node)
{
    char *file;
	char *new_file;
    int len;
	
	if (!node || !node->args || !node->args[0])
        return ;
    file = node->args[0];
    len = ft_strlen(file);
    // Simple quote check - remove surrounding quotes if present
    if (len >= 2 && 
        ((file[0] == '"' && file[len-1] == '"') ||
         (file[0] == '\'' && file[len-1] == '\'')))
    {
        new_file = ft_substr(file, 1, len-2);
        if (new_file)
        {
            free(file);
            node->args[0] = new_file;
        }
    }
}
*/
/*
Processes quotes in a redirection node's filename.
- Handles both single and double quotes in filenames.
- Removes surrounding quotes while preserving content.
- Updates the node's args[0] with processed filename.
Returns:
Nothing (void function).
Works with setup_in_redir and setup_out_redir.
NEWER OLD VERSION
void process_quotes_in_redirect(t_node *node)
{
    char *str;
    int len;
    char *new_str;
    
    if (!node || !node->args || !node->args[0])
        return;
    
    str = node->args[0];
    len = ft_strlen(str);
    
    // Check if string has matching quotes at start and end
    if (len >= 2 && 
        ((str[0] == '"' && str[len-1] == '"') ||
         (str[0] == '\'' && str[len-1] == '\'')))
    {
        // Create new string without quotes
        new_str = ft_substr(str, 1, len-2);
        if (new_str)
        {
            // Replace original string with new one
            free(str);
            node->args[0] = new_str;
        }
    }
}
*/
/*
Processes quotes in a redirection node's filename.
- Handles both single and double quotes in filenames.
- Removes surrounding quotes while preserving content.
- Updates the node's args[0] with processed filename.
HOPEFULLY LAST REFACTOR
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
            ft_safefree((void **)&node->args[0]);
            node->args[0] = new_str;
        }
    }
}
*/
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
            ft_safefree((void **)&node->args[0]);
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
