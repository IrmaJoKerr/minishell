/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 12:26:38 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Function to read input line from the user using readline.
Displays the prompt "bleshell$ " and waits for user input.
Handles Ctrl+D (EOF) by printing "exit" and terminating the program.
Adds non-empty lines to history with add_history. Recall with arrow keys.
Returns: The input line from the user, or exits on EOF.
*/
char	*reader(void)
{
	char	*line;

	line = readline("bleshell$ ");
	if (!line)
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		exit(0);
	}
	if (*line)
		add_history(line);
	return (line);
}

/*
Initialize the shell environment and settings.
This function:
1) Zeroes out all fields in s_vars struct
2) Loads environment variables using envp into vars->env
3) Loads command history from HISTORY_FILE
4) Loads signal handlers (SIGINT, SIGQUIT)
Called once when shell starts up. Works with main().
*/
void	init_shell(t_vars *vars, char **envp)
{
	ft_memset(vars, 0, sizeof(t_vars));
	vars->env = envp;
	vars->quote_depth = 0;
	load_history();
	load_signals();
}

/*
Main command processing loop.
Process a single command line through the shell pipeline:
1) Validate input is not NULL
2) Perform lexing to break into tokens
3) Build the abstract syntax tree (AST)
4) Execute the command if AST was built successfully
5) Free the command string after processing
Returns 1 if processing was successful, 0 if command was NULL.
Works with main().
*/
int	process_command(char *command, t_vars *vars)
{
	if (!command)
		return (0);
	lexerlist(command, vars);
	vars->astroot = build_ast(vars);
	if (vars->astroot)
		execute_cmd(vars->astroot, vars->env, vars);
	free(command);
	return (1);
}

/*
Start point for the minishell program.
1) Initialize the shell environment
2) Enter infinite loop ( while(1) )to read and process commands
3) Read user input with reader()
4) Process each command with process_command()
5) Repeat until process_command() returns 0 (on EOF or error)
6) Save command history before exiting
Suppresses unused parameter warnings for ac and av (needed by main)
because of readline.
Returns 0 on normal exit.
*/
int	main(int ac, char **av, char **envp)
{
	t_vars	vars;
	char	*command;

	(void)ac;
	(void)av;
	init_shell(&vars, envp);
	while (1)
	{
		command = reader();
		if (!process_command(command, &vars))
			break ;
	}
	save_history();
	return (0);
}
