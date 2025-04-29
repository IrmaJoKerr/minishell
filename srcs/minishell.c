/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/29 17:45:47 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Before validate_redir_targets: error_code=%d\n", vars->error_code); //DEBUG PRINT
	if (!validate_redir_targets(vars))
	{
		if (DEBUG_ERROR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] validate_redir_targets failed: error_code=%d\n", vars->error_code); //DEBUG PRINT
		return ;
	}
	if (DEBUG_AST) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Starting AST construction\n"); //DEBUG PRINT
	vars->astroot = proc_token_list(vars);
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] After proc_token_list: error_code=%d, astroot=%p\n", vars->error_code, (void*)vars->astroot); //DEBUG PRINT
	if (vars->astroot)
	{
		if (DEBUG_AST) //DEBUG PRINT
			print_ast(vars->astroot, NULL); //DEBUG PRINT
		if (DEBUG_ERROR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] Before execute_cmd: error_code=%d\n", vars->error_code); //DEBUG PRINT
		execute_cmd(vars->astroot, vars->env, vars);
		if (DEBUG_ERROR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] After execute_cmd: error_code=%d\n", vars->error_code); //DEBUG PRINT
	}
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
	int	pipe_result;

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
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Starting process_command: error_code=%d\n", vars->error_code); //DEBUG PRINT
	// vars->error_code = 0; // I think this is the source of the reset of vars->error_code = 0; // Reset error code
	vars->partial_input = ft_strdup(command);
	if (!vars->partial_input)
		return ;
	fprintf(stderr, "[DEBUG] process_command() - before line calling handle_quote_completion(): pre-tokenization error_code=%d\n", vars->error_code);
	vars->partial_input = handle_quote_completion(vars->partial_input, vars);
	fprintf(stderr, "[DEBUG] process_command - after calling handle_quote_completion(): post-tokenization error_code=%d\n", vars->error_code);
	if (!vars->partial_input)
		return ;
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Before tokenization: error_code=%d\n", vars->error_code); //DEBUG PRINT
	fprintf(stderr, "[DEBUG] process_command() - before line calling process_input_token(): pre-tokenization error_code=%d\n", vars->error_code);
	if (!process_input_tokens(vars->partial_input, vars))
	{
		if (DEBUG_ERROR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] Tokenization failed: error_code=%d\n", vars->error_code); //DEBUG PRINT
		free(vars->partial_input);
		vars->partial_input = NULL;
		return ;
	}
	fprintf(stderr, "[DEBUG] process_command - after line calling process_input_tokens(): post-tokenization error_code=%d\n", vars->error_code);
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Before pipe syntax handling: error_code=%d\n", vars->error_code); //DEBUG PRINT
	if (!handle_pipe_syntax(vars))
	{
		if (DEBUG_ERROR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] Pipe syntax handling failed: error_code=%d\n", vars->error_code); //DEBUG PRINT
		return ;
	}
	fprintf(stderr, "[DEBUG] process_command - after line calling handle_pipe_syntax(): post-tokenization error_code=%d\n", vars->error_code);
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Before build_and_execute: error_code=%d\n", vars->error_code); //DEBUG PRINT
	build_and_execute(vars);
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] After build_and_execute: error_code=%d\n", vars->error_code); //DEBUG PRINT
	if (vars->partial_input)
	{
		free(vars->partial_input);
		vars->partial_input = NULL;
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
	char	*exit_args[2];

	exit_args[0] = NULL;
	exit_args[1] = NULL;
	(void)argc;
	(void)argv;
	ft_memset(&vars, 0, sizeof(t_vars));
	init_shell(&vars, envp);
	setup_debug_flags(); // Initialize debug flags
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
