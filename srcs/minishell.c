/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:55:54 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Function to read a line from the user.
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
Initialise the shell environment. Works with main.
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
Process the command. Works with main.
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
Suppresses unused parameter warnings for ac and av
because of readline.
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
