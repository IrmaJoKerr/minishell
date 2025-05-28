/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 17:22:18 by bleow            ###   ########.fr       */
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
	if (!validate_redir_targets(vars))
	{
		return ;
	}
	/*
	 * Debug example: Uncomment any of these lines to see debug output
	 * These functions can now be called directly without DEBUG flags
	 */
	// debug_print_tokens(vars->head);          // Print token list to stdout
	// debug_analyze_list(vars->head);          // Detailed token analysis to stderr
	// debug_save_tokens(vars->head, "tokens.txt"); // Save tokens to file
	vars->astroot = proc_token_list(vars);
	if (vars->astroot)
	{
		// debug_print_ast(vars->astroot);       // Print AST to stdout  
		// debug_analyze_ast(vars->astroot);     // Detailed AST analysis to stderr
		// debug_save_ast(vars->astroot, "ast.txt"); // Save AST to file
		execute_cmd(vars->astroot, vars->env, vars);
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
	{
		return ;
	}
	build_and_execute(vars);
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
