/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/09 20:33:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"


// Helper function to restore standard file descriptors
// void reset_std_fd(t_pipe *pipes)
// {
//     if (!pipes)
//         return ;
        
//     if (pipes->saved_stdin != -1)
//     {
//         dup2(pipes->saved_stdin, STDIN_FILENO);
//         close(pipes->saved_stdin);
//         pipes->saved_stdin = -1;
//     }
    
//     if (pipes->saved_stdout != -1)
//     {
//         dup2(pipes->saved_stdout, STDOUT_FILENO);
//         close(pipes->saved_stdout);
//         pipes->saved_stdout = -1;
//     }
    
//     // Close heredoc fd if it was opened
//     if (pipes->heredoc_fd > 2)
//     {
//         close(pipes->heredoc_fd);
//         pipes->heredoc_fd = -1;
//     }
    
//     // Close redirection fd if it was opened
//     if (pipes->redirection_fd > 2)
//     {
//         close(pipes->redirection_fd);
//         pipes->redirection_fd = -1;
//     }
// }

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
// void	init_pipe(t_node *cmd, int *pipe_fd)
// {
// 	if (cmd->type == TYPE_PIPE)
// 	{
// 		close(pipe_fd[0]);
// 		dup2(pipe_fd[1], STDOUT_FILENO);
// 		close(pipe_fd[1]);
// 	}
// }

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
// int	setup_pipe(int *pipefd)
// {
// 	if (pipe(pipefd) == -1)
// 	{
// 		ft_putendl_fd("pipe: Creation failed", 2);
// 		return (0);
// 	}
// 	return (1);
// }

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
// void	exec_left_cmd(t_node *pipe_node, int *pipefd, t_vars *vars)
// {
// 	int	exit_status;

// 	close(pipefd[0]);
// 	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
// 		exit(1);
// 	close(pipefd[1]);
// 	exit_status = execute_cmd(pipe_node->left, vars->env, vars);
// 	exit(exit_status);
// }

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
// void	exec_right_cmd(t_node *pipe_node, int *pipefd, t_vars *vars)
// {
// 	t_node	*cmd;
// 	t_node	*redir_node;
// 	int		saved_stdout;
// 	int		append;

// 	close(pipefd[1]);
// 	if (dup2(pipefd[0], STDIN_FILENO) == -1)
// 		exit(1);
// 	close(pipefd[0]);
// 	cmd = pipe_node->right;
// 	redir_node = NULL;
// 	if (cmd && cmd->right)
// 	{
// 		redir_node = cmd->right;
// 		if (is_redirection(redir_node->type))
// 		{
// 			saved_stdout = dup(STDOUT_FILENO);
// 			if (saved_stdout == -1)
// 				exit(1);
// 			append = 0;
// 			if (redir_node->type == TYPE_APPEND_REDIRECT)
// 				append = 1;
// 			if (!output_redirect(redir_node, &saved_stdout, append, vars))
// 				exit(1);
// 		}
// 	}
// 	exit(execute_cmd(pipe_node->right, vars->env, vars));
// }

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
// pid_t	make_child_proc(t_node *pipe_node, int *pipefd,
// 		t_vars *vars, int is_left)
// {
// 	pid_t	pid;

// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		if (is_left)
// 			exec_left_cmd(pipe_node, pipefd, vars);
// 		else
// 			exec_right_cmd(pipe_node, pipefd, vars);
// 	}
// 	else if (pid < 0)
// 		ft_putendl_fd("fork: Creation failed", 2);
// 	return (pid);
// }

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
int execute_pipes(t_node *pipe_node, t_vars *vars)
{
    int     pipe_fd[2];
    pid_t   left_pid;
    pid_t   right_pid;
    int     status;
    int     left_status = 0;
    
    if (pipe(pipe_fd) == -1) {
        ft_putendl_fd("pipe: Creation failed", 2);
        return 1;
    }
    
    // Fork left process
    left_pid = fork();
    if (left_pid == 0) {
        // Child process - left side
        close(pipe_fd[0]);  // Close read end in left process
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
            exit(1);
        close(pipe_fd[1]);
        
        // Execute left command - will exit on its own
        exit(execute_cmd(pipe_node->left, vars->env, vars));
    }
    else if (left_pid > 0) {
        // Parent process
        close(pipe_fd[1]);  // Close write end in parent
        
        // Fork right process
        right_pid = fork();
        if (right_pid == 0) {
            // Child process - right side
            if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
                exit(1);
            close(pipe_fd[0]);
            
            // Execute right command - will exit on its own
            exit(execute_cmd(pipe_node->right, vars->env, vars));
        }
        else if (right_pid > 0) {
            // Parent process - close pipe fully
            close(pipe_fd[0]);
            
            // Wait for both processes independently
            waitpid(left_pid, &left_status, 0);
            waitpid(right_pid, &status, 0);
            
            // Return right command status, or left if right succeeded
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                return left_status;
            else
                return (handle_cmd_status(status, vars));
        }
    }
    
    // Error in fork
    ft_putendl_fd("fork: Creation failed", 2);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    return 1;
}


/*
 * Determines if a redirection node is related to a specific command
 * Takes into account the position of nodes in the token list
 * Returns 1 if related, 0 if not
*/
// int is_related_to_cmd(t_node *redir_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current;
//     t_node *prev_cmd = NULL;
//     t_node *next_cmd = NULL;
    
//     if (!redir_node || !cmd_node || !vars || !vars->head)
//         return (0);
    
//     /* Find the command positions */
//     current = vars->head;
//     while (current)
//     {
//         if (current->type == TYPE_CMD)
//         {
//             if (current == cmd_node)
//             {
//                 /* We found our target command */
//                 break;
//             }
//             prev_cmd = current;
//         }
//         current = current->next;
//     }
    
//     /* Find next command after our target */
//     next_cmd = find_cmd(cmd_node->next, NULL, FIND_NEXT, NULL);
    
//     /* Check if redirection is between our command and the next one */
//     current = cmd_node->next;
//     while (current && current != next_cmd)
//     {
//         if (current == redir_node)
//             return (1);
//         current = current->next;
//     }
    
//     /* Check if redirection is between previous command and our command */
//     if (prev_cmd)
//     {
//         current = prev_cmd->next;
//         while (current && current != cmd_node)
//         {
//             if (current == redir_node)
//                 return (0); /* Belongs to previous command */
//             current = current->next;
//         }
//     }
    
//     /* Redirection before first command belongs to first command */
//     if (!prev_cmd && cmd_node == vars->cmd_nodes[0])
//     {
//         current = vars->head;
//         while (current && current != cmd_node)
//         {
//             if (current == redir_node)
//                 return (1);
//             current = current->next;
//         }
//     }
    
//     /* Check for special cases like consecutive commands without pipes */
//     /* Redirections after a command belong to it until another command is found */
    
//     return (0);
// }

/**
 * Clean up memory after pipe completion processing
 * Mode: 1 = free pipe_cmd only, 2 = free result only, 3 = free both
 */
// void reset_done_pipes(char **pipe_cmd, char **result, int mode)
// {
//     // Clean up result string if needed
//     if ((mode == 2 || mode == 3) && result && *result)
//     {
//         free(*result);
//         *result = NULL;
//     }
//     // Clean up pipe command string if needed
//     if ((mode == 1 || mode == 3) && pipe_cmd && *pipe_cmd)
//     {
//         free(*pipe_cmd);
//         *pipe_cmd = NULL;
//     }
// }

// /*
// Prepares and validates a command for pipe completion processing.
// - Creates working copies of command string.
// - Checks if command needs completion based on syntax flag.
// - Creates AST structure for processing.
// Returns:
// - 1 on successful preparation.
// - 0 if no completion needed (early return case).
// - -1 on preparation errors.
// Works with handle_pipe_completion().
// */
// int	prep_pipe_complete(char *cmd, char **result, char **pipe_cmd)
// {
// 	*result = ft_strdup(cmd);
// 	if (!*result)
// 		return (-1);
// 	*pipe_cmd = ft_strdup(*result);
// 	if (!*pipe_cmd)
// 	{
// 		free(*result);
// 		return (-1);
// 	}
// 	return (1);
// }

/*
Check if there's an unfinished pipe at the end of input.
Returns:
- 1 if there's an unfinished pipe needing completion
- 0 if there's no unfinished pipe
Works with handle_unfinished_pipes().
*/
// int check_unfinished_pipe(t_vars *vars)
// {
//     t_node *last_token;
//     t_node *current;

//     last_token = NULL;
//     current = vars->head;
//     while (current)
//     {
//         last_token = current;
//         current = current->next;
//     }
//     if (last_token && last_token->type == TYPE_PIPE)
//     {
//         if (vars->pipes)
//             vars->pipes->pipe_at_end = 1;
//         return (1);
//     }
//     if (vars->pipes && vars->pipes->pipe_at_end)
//         return (1);
//     return (0);
// }

/*
Check for unfinished pipes in input and handle them.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
*/
// int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars)
// {
// 	char	*addon_input;
// 	char	*tmp;
// 	char	*combined;

// 	addon_input = NULL;
// 	tmp = NULL;
// 	if (!check_unfinished_pipe(vars))
// 		return (0);
// 	addon_input = readline("> ");
// 	if (!addon_input)
// 	{
// 		free(addon_input);
// 		return (handle_unfinished_pipes(processed_cmd, vars));
// 	}
// 	tmp = ft_strtrim(addon_input, " \t\n");
// 	free(addon_input);
// 	addon_input = tmp;
// 	if (!addon_input || addon_input[0] == '\0')
// 	{
// 		free(addon_input);
// 		return (handle_unfinished_pipes(processed_cmd, vars));
// 	}
// 	tmp = ft_strjoin(*processed_cmd, " ");
// 	if (!tmp)
// 	{
// 		free(addon_input);
// 		return (-1);
// 	}
// 	combined = ft_strjoin(tmp, addon_input);
// 	free(tmp);
// 	free(addon_input);
// 	if (!combined)
// 		return (-1);
// 	free(*processed_cmd);
// 	*processed_cmd = combined;
// 	cleanup_token_list(vars);
// 	improved_tokenize(*processed_cmd, vars);
// 	return (1);
// }

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
// char	*handle_pipe_completion(char *cmd, t_vars *vars, int syntax_chk)
// {
// 	char	*pipe_cmd;
// 	char	*result;
// 	int prep_status;

// 	if (!syntax_chk || is_input_complete(vars))
// 		return (ft_strdup(cmd));
// 	prep_status = prep_pipe_complete(cmd, &result, &pipe_cmd);
// 	if (prep_status < 0)
// 		return (NULL);
// 	if (prep_status == 0)
// 		return (ft_strdup(cmd));
// 	if (handle_unfinished_pipes(&pipe_cmd, vars) < 0)
// 	{
// 		reset_done_pipes(&pipe_cmd, &result, 3);
// 		return (NULL);
// 	}
// 	reset_done_pipes(NULL, &result, 2);
// 	return (pipe_cmd);
// }

/*
Sets up multiple redirections for a command.
- Processes all input redirections first.
- Then processes all output redirections.
- Handles error reporting for failed redirections.
Returns:
1 on success, 0 on failure.
Works with execute_pipeline() for handling multiple redirections.
*/

/*
Determines if a redirection node relates to a specific command.
- Checks relative position of redirection and command in token list.
- Considers commands before and after redirection.
Returns:
- 1 if redirection relates to the command.
- 0 otherwise.
Works with setup_multi_redirects().
*/
int is_related_to_cmd(t_node *redir_node, t_node *cmd_node, t_vars *vars)
{
    t_node *current;
    t_node *prev_cmd = NULL;
    t_node *next_cmd = NULL;
    
    if (!redir_node || !cmd_node || !vars || !vars->head)
        return (0);
    
    /* Find the command positions */
    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_CMD)
        {
            if (current == cmd_node)
            {
                /* We found our target command */
                break;
            }
            prev_cmd = current;
        }
        current = current->next;
    }
    
    /* Find next command after our target */
	next_cmd = find_cmd(cmd_node->next, NULL, FIND_NEXT, vars);
    // next_cmd = find_cmd(cmd_node->next, NULL, FIND_NEXT, NULL);
    
    /* Check if redirection is between our command and the next one */
    current = cmd_node->next;
    while (current && current != next_cmd)
    {
        if (current == redir_node)
            return (1);
        current = current->next;
    }
    
    /* Check if redirection is between previous command and our command */
    if (prev_cmd)
    {
        current = prev_cmd->next;
        while (current && current != cmd_node)
        {
            if (current == redir_node)
                return (0); /* Belongs to previous command */
            current = current->next;
        }
    }
    
    /* Redirection before first command belongs to first command */
    if (!prev_cmd && cmd_node == vars->cmd_nodes[0])
    {
        current = vars->head;
        while (current && current != cmd_node)
        {
            if (current == redir_node)
                return (1);
            current = current->next;
        }
    }
    
    /* Check for special cases like consecutive commands without pipes */
    /* Redirections after a command belong to it until another command is found */
    
    return (0);
}

/**
 * Clean up memory after pipe completion processing
 * Mode: 1 = free pipe_cmd only, 2 = free result only, 3 = free both
 */
void reset_done_pipes(char **pipe_cmd, char **result, int mode)
{
    // Clean up result string if needed
    if ((mode == 2 || mode == 3) && result && *result)
    {
        free(*result);
        *result = NULL;
    }
    // Clean up pipe command string if needed
    if ((mode == 1 || mode == 3) && pipe_cmd && *pipe_cmd)
    {
        free(*pipe_cmd);
        *pipe_cmd = NULL;
    }
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
int	prep_pipe_complete(char *cmd, char **result, char **pipe_cmd)
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
	return (1);
}

/*
Check if there's an unfinished pipe at the end of input.
Returns:
- 1 if there's an unfinished pipe needing completion
- 0 if there's no unfinished pipe
Works with handle_unfinished_pipes().
*/
int check_unfinished_pipe(t_vars *vars)
{
    t_node *last_token;
    t_node *current;

    last_token = NULL;
    current = vars->head;
    while (current)
    {
        last_token = current;
        current = current->next;
    }
    if (last_token && last_token->type == TYPE_PIPE)
    {
        if (vars->pipes)
            vars->pipes->pipe_at_end = 1;
        return (1);
    }
    if (vars->pipes && vars->pipes->pipe_at_end)
        return (1);
    return (0);
}

/*
Check for unfinished pipes in input and handle them.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
*/
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars)
{
	char	*addon_input;
	char	*tmp;
	char	*combined;

	addon_input = NULL;
	tmp = NULL;
	if (!check_unfinished_pipe(vars))
		return (0);
	addon_input = readline("> ");
	if (!addon_input)
	{
		free(addon_input);
		return (handle_unfinished_pipes(processed_cmd, vars));
	}
	tmp = ft_strtrim(addon_input, " \t\n");
	free(addon_input);
	addon_input = tmp;
	if (!addon_input || addon_input[0] == '\0')
	{
		free(addon_input);
		return (handle_unfinished_pipes(processed_cmd, vars));
	}
	tmp = ft_strjoin(*processed_cmd, " ");
	if (!tmp)
	{
		free(addon_input);
		return (-1);
	}
	combined = ft_strjoin(tmp, addon_input);
	free(tmp);
	free(addon_input);
	if (!combined)
		return (-1);
	free(*processed_cmd);
	*processed_cmd = combined;
	cleanup_token_list(vars);
	improved_tokenize(*processed_cmd, vars);
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
	int prep_status;

	if (!syntax_chk || is_input_complete(vars))
		return (ft_strdup(cmd));
	prep_status = prep_pipe_complete(cmd, &result, &pipe_cmd);
	if (prep_status < 0)
		return (NULL);
	if (prep_status == 0)
		return (ft_strdup(cmd));
	if (handle_unfinished_pipes(&pipe_cmd, vars) < 0)
	{
		reset_done_pipes(&pipe_cmd, &result, 3);
		return (NULL);
	}
	reset_done_pipes(NULL, &result, 2);
	return (pipe_cmd);
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

/*
Process quotes in redirection node's filename
Remove enclosing quotes from the filename
*/
void process_quotes_in_redirect(t_node *redir_node)
{
    if (!redir_node || !redir_node->args || !redir_node->args[0])
        return;
    
    process_arg_quotes(&redir_node->args[0]);
}

/* 
 * Creates all pipes needed for execution 
 * Returns 1 on success, 0 on failure
 */
// int	make_pipes(t_pipe *pipes, int pipe_count)
// {
//     int i;
//     int j;
    
//     i = 0;
//     while (i < pipe_count)
//     {
//         /* Reuse existing setup_pipe function */
//         if (!setup_pipe(pipes->pipe_fds + (i * 2)))
//         {
//             /* Clean up previously created pipes */
//             j = 0;
//             while (j < i)
//             {
//                 close(pipes->pipe_fds[j * 2]);
//                 close(pipes->pipe_fds[j * 2 + 1]);
//                 j++;
//             }
//             return (0);
//         }
//         i++;
//     }
//     return (1);
// }
/* 
 * Creates all pipes needed for execution 
 * Returns 1 on success, 0 on failure
 */
// int make_pipes(t_pipe *pipes, int pipe_count)
// {
//     int i;
//     int j;
    
//     i = 0;
//     while (i < pipe_count)
//     {
//         /* Create pipe directly instead of using setup_pipe function */
//         if (pipe(pipes->pipe_fds + (i * 2)) == -1)
//         {
//             /* Clean up previously created pipes */
//             j = 0;
//             while (j < i)
//             {
//                 close(pipes->pipe_fds[j * 2]);
//                 close(pipes->pipe_fds[j * 2 + 1]);
//                 j++;
//             }
//             return (0);
//         }
//         i++;
//     }
//     return (1);
// }

/* 
 * Sets up child process pipe redirections
 */
void setup_child_pipes(t_pipe *pipes, int cmd_idx, int pipe_count)
{
    int i;
    
    /* Set up stdin from previous pipe (if not first command) */
    if (cmd_idx > 0)
    {
        dup2(pipes->pipe_fds[(cmd_idx - 1) * 2], STDIN_FILENO);
    }
    
    /* Set up stdout to next pipe (if not last command) */
    if (cmd_idx < pipe_count)
    {
        dup2(pipes->pipe_fds[cmd_idx * 2 + 1], STDOUT_FILENO);
    }
    /* Close all pipe fds in child */
    i = 0;
    while (i < pipe_count * 2)
    {
        close(pipes->pipe_fds[i]);
        i++;
    }
}

/* 
Creates all child processes for the pipeline
Returns 1 on success, 0 on failure
*/
int fork_processes(t_pipe *pipes, t_vars *vars)
{
    int     i;
    int     j;
    pid_t   pid;
    
    i = 0;
    while (i <= pipes->pipe_count)
    {
        pid = fork();
        if (pid < 0)
        {
            j = 0;
            while (j < i)
            {
                // Kill existing child processes
                if (pipes->pids[j] > 0)
                    kill(pipes->pids[j], SIGTERM);
                j++;
            }
            return (0);
        }
        else if (pid == 0)
        {
            // Child process - set up pipes and execute command
            setup_child_pipes(pipes, i, pipes->pipe_count);
            // Execute the command
            if (vars->cmd_nodes[i])
                execute_cmd(vars->cmd_nodes[i], vars->env, vars);
            exit(vars->error_code);
        }
        /* Parent process */
        pipes->pids[i] = pid;
        i++;
    }
    return (1);
}

/* 
 * Counts pipe nodes in a command chain
 */
int count_pipes(t_vars *vars)
{
    int     count;
    t_node  *current;
    
    count = 0;
    if (!vars || !vars->head)
        return (0);
        
    current = vars->head;
    
    while (current)
    {
        if (current->type == TYPE_PIPE)
            count++;
        current = current->next;
    }
    
    return (count);
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
    
    while (i <= pipes->pipe_count)
    {
        waitpid(pipes->pids[i], &(pipes->status[i]), 0);
        i++;
    }
    
    /* Return status of last command */
    last_status = handle_cmd_status(pipes->status[pipes->pipe_count], vars);
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
    if (!pipes->status)
    {
        free(pipes->pipe_fds);
        free(pipes->pids);
        free(pipes->status);
        pipes->pipe_fds = NULL;
        pipes->pids = NULL;
        pipes->status = NULL;
        return (0);
    }
    
    return (1);
}
