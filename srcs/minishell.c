/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 02:53:56 by bleow            ###   ########.fr       */
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
		return (NULL);
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
	vars->env = dup_env(envp);
	if (!vars->env)
	{
		ft_putstr_fd("bleshell: error: Failed to duplicate environment\n", 2);
		exit(1);
	}
	vars->shell_level = get_shell_level(vars);
	if (vars->shell_level == -1)
	{
		ft_putstr_fd("bleshell: error: Failed to get shell level\n", 2);
		exit(1);
	}
	vars->shell_level = incr_shell_level(vars);
	if (!vars->shell_level)
		ft_putstr_fd("bleshell: warning: Failed to increment SHLVL\n", 2);
	vars->quote_depth = 0;
	load_history();
	load_signals();
}

/*
Process the user command by:
1) Cleaning up previous token list.
2) Lexing the command into tokens.
3) Building the AST from the tokens.
*/
int process_command(char *command, t_vars *vars)
{
	if (vars->head)
	{
		printf("DEBUG: Cleaning up previous token list\n");
		cleanup_token_list(vars);
	}
	if (!command)
		return (0);
	printf("DEBUG: Processing command: '%s'\n", command);
	lexerlist(command, vars);
	print_token_list(vars);
	vars->astroot = build_ast(vars);
	if (vars->astroot)
	{
		printf("DEBUG: Built AST successfully\n");
		if (vars->astroot->args && vars->astroot->args[0])
			printf("DEBUG: Root command: %s\n", vars->astroot->args[0]);
		execute_cmd(vars->astroot, vars->env, vars);
	}
	else
	{
		printf("DEBUG: Failed to build AST\n");
	}
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
		if (!command)  // EOF detected
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;  // Break the loop instead of exiting immediately
		}
		if (!process_command(command, &vars))
			break ;
	}
	// This will now be reached when Ctrl+D is pressed
	printf("DEBUG: Saving history before exit\n");
	save_history();
	if (vars.env)
	{
		ft_free_2d(vars.env, ft_arrlen(vars.env));
		vars.env = NULL;
	}
	cleanup_exit(&vars);
	return (0);
}
