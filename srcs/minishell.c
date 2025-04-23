/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 11:29:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void print_tokens(t_node *head) // Debug function
{
	t_node *curr = head;
	int i;
	
	fprintf(stderr, "\n--- TOKEN LIST ---\n");
	while (curr)
	{
		fprintf(stderr, "Token: type=%d (%s), content='%s'\n", 
		      curr->type, get_token_str(curr->type), 
		      curr->args ? curr->args[0] : "NULL");
		
		if (curr->args && curr->args[1])
		{
			fprintf(stderr, "  Arguments:");
			i = 1;
			while (curr->args[i])
			{
				fprintf(stderr, " '%s'", curr->args[i]);
				i++;
			}
			fprintf(stderr, "\n");
		}
		curr = curr->next;
	}
	fprintf(stderr, "----------------\n\n");
}

/*
Reads input line from the user with prompt display.
- Displays the shell prompt and awaits user input.
- Handles Ctrl+D (EOF) by calling builtin_exit.
- Adds non-empty lines to command history.
Returns:
- User input as an allocated string.
- Never returns on EOF (exits program).
Works with main() in command loop.

Example: When user types "ls -la"
- Displays prompt "bleshell$> "
- Returns "ls -la" as an allocated string
- Adds command to history for up-arrow recall
*/
char	*reader(void)
{
	char	*line;

	line = readline(PROMPT);
	if (!line)
		return (NULL);
	if (*line)
		add_history(line);
	return (line);
}

/*
Processes input with quotes that need completion.
- Gets additional input for unclosed quotes.
- Re-tokenizes the completed command.
- Updates token list with completed input.
Returns:
Newly allocated complete command string.
NULL on memory allocation failure.
Works with process_command().

Example: When user types "echo "hello
- Prompts for completion of the double quote
- User types "world" and Enter
- Returns combined string: echo "hello world"
*/
char	*handle_quote_completion(char *cmd, t_vars *vars)
{
	char	*new_cmd;
	
	vars->quote_depth = 0;
	if (validate_quotes(cmd, vars))
		return cmd;
	new_cmd = fix_open_quotes(cmd, vars);
	if (!new_cmd)
		return (NULL);
	if (new_cmd != cmd)
	{
		free(cmd);
		cmd = new_cmd;
	}
	cleanup_token_list(vars);
	improved_tokenize(cmd, vars);
	return (cmd);
}

/*
Master command function that builds and executes the command's
abstract syntax tree.
- Creates AST from tokenized input.
- Executes the command if AST built successfully.
- Provides debug information about the process.
Works with process_command().

Example: For "echo hello | grep h"
- Builds AST with pipe node at root
- Echo command on left branch, grep on right
- Executes the pipeline with proper redirection
*/
void	build_and_execute(t_vars *vars)
{
	fprintf(stderr, "[build_and_execute:%d] Building AST from token list\n", __LINE__);
	if (!validate_redirection_targets(vars))
	{
		fprintf(stderr, "[build_and_execute:%d] Redirection validation failed, skipping execution\n", __LINE__);
		return ;
	}
	vars->astroot = proc_token_list(vars);
	if (vars->astroot)
	{
		DBG_PRINTF(DEBUG_EXEC, "AST built successfully, root type=%d\n", vars->astroot->type);
		execute_cmd(vars->astroot, vars->env, vars);
	}
}

/*
Main entry point for tokenization and expansion. 
Tokenizes input and processes expansions.
Returns 1 on success, 0 on failure.
Example: For input "echo "hello
- Tokenizes the initial content
- Detects unclosed quotes and prompts for completion
- Returns completed command string with proper quotes
*/
int	process_input_tokens(char *command, t_vars *vars)
{
	if (!command || !*command)
		return (0);
	cleanup_token_list(vars);
	if (!improved_tokenize(command, vars))
		return (0);
	return (1);
}

/*
Master control function for completing an incomplete pipe command by
getting additional input.
- Gets continuation input for commands ending with a pipe
- Updates partial_input with the complete command
- Re-tokenizes the completed command
Returns:
- 1 on successful pipe completion.
- 0 on memory allocation or tokenization error.
Works with handle_pipe_syntax().

Example: When user types "ls |"
- Prompts for continuation after the pipe.
- User inputs "grep hello".
- Creates and tokenizes "ls | grep hello".
*/
int	finalize_pipes(t_vars *vars)
{
    char *completed_cmd = complete_pipe_cmd(vars->partial_input, vars);
    if (!completed_cmd)
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return (0);
    }
    free(vars->partial_input);
    vars->partial_input = completed_cmd;
    cleanup_token_list(vars);
    if (!process_input_tokens(vars->partial_input, vars))
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return (0);
    }
    return (1);
}

/*
Handles pipe syntax validation and completion.
- Analyzes pipe syntax in the token list.
- Handles incomplete pipes by prompting for continuation.
- Handles invalid pipes by setting error code and cleaning up.
Returns:
1 if pipe syntax is valid (or was successfully completed).
0 if pipe syntax is invalid or completion failed.
*/
int	handle_pipe_syntax(t_vars *vars)
{
    int pipe_result;
    
    pipe_result = analyze_pipe_syntax(vars);
    if (pipe_result == 2)
    {
        if (!finalize_pipes(vars))
            return (0);
    }
    else if (pipe_result == 1)
    {
        cleanup_token_list(vars);
        vars->error_code = 2;
        free(vars->partial_input);
        vars->partial_input = NULL;
        return (0);
    }
    return (1);
}

/*
Process single line user command string through lexing and execution.
- Handles input tokenization, syntax checking, and execution.
Works with handle_input().

Example: When user types a complex command
- Processes tokens and handles unclosed quotes.
- Validates and completes pipe syntax if needed.
- Builds and executes command if valid.
*/
void	process_command(char *command, t_vars *vars)
{
    vars->error_code = 0;
    vars->partial_input = ft_strdup(command);
    if (!vars->partial_input)
        return ;
    vars->partial_input = handle_quote_completion(vars->partial_input, vars);
    if (!vars->partial_input)
        return ;
    if (!process_input_tokens(vars->partial_input, vars))
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return ;
    }
    if (!handle_pipe_syntax(vars))
        return ;
    build_and_execute(vars);
    if (vars->partial_input)
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
    }
}

/*
Restores a file descriptor to the terminal if it's not already connected.
Can handle STDIN, STDOUT, or STDERR.
Works with reset_terminal_after_heredoc().
*/
void	restore_terminal_fd(int target_fd, int source_fd, int mode)
{
    char	*tty_path;
    int		fd;
    
    if (!isatty(target_fd))
    {
        tty_path = ttyname(source_fd);
        if (tty_path)
        {
            fd = open(tty_path, mode);
            if (fd >= 0)
            {
                dup2(fd, target_fd);
                close(fd);
            }
        }
    }
}

/*
Restores terminal settings after heredoc processing.
- Reconnects STDIN and STDOUT to the terminal if redirected.
- Restores canonical mode and echo settings.
- Ensures readline library knows about the restored terminal state.
Works with heredoc processing to maintain proper terminal behavior.

Example: After a heredoc redirects stdin
- Restores STDIN from STDOUT's terminal
- Restores STDOUT from STDERR's terminal
- Resets terminal attributes to interactive mode
*/
void	reset_terminal_after_heredoc(void)
{
    struct termios	term;
    
    restore_terminal_fd(STDIN_FILENO, STDOUT_FILENO, O_RDONLY);
    restore_terminal_fd(STDOUT_FILENO, STDERR_FILENO, O_WRONLY);
    if (isatty(STDIN_FILENO))
    {
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= (ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        rl_on_new_line();
    }
}

/*
Main shell loop that processes user commands and manages execution flow.
- Reads input through reader() function.
- Handles Ctrl+D and empty input cases.
- Processes commands through tokenizing and execution.
- Manages exit status tracking through pipeline.
Works as the start point of the shell.
*/
int	main(int argc, char **argv, char **envp)
{
	t_vars	vars;
	char	*input;
	char	*exit_args[2] = {NULL, NULL};

	(void)argc;
	(void)argv;
	ft_memset(&vars, 0, sizeof(t_vars));
	init_shell(&vars, envp);
	while (1)
	{
		input = reader();
		if (input == NULL)
			builtin_exit(exit_args, &vars);
		if (input[0] == '\0')
		{
			free(input);
			continue ;
		}
		handle_input(input, &vars);
		free(input);
		reset_shell(&vars);
	}
	return (0);
}
