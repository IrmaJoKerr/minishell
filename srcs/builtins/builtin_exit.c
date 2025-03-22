/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:50 by lechan            #+#    #+#             */
/*   Updated: 2025/03/23 03:14:12 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <readline/readline.h>

/*
Built-in command: exit. Exits the shell.
- Initializes cmdcode to 0 then updates it with the last command code.
- Prints "exit" to STDOUT.
- Saves history to HISTORY_FILE.
- Clears readline history.
- Calls cleanup_exit() to free all allocated memory.
- Exits the program with the last command code.
Works with execute_builtin().
*/

static int	ft_strisnum(const char *str)
{
	if (!str || (*str == '-' && !*(str + 1)))
		return (0);
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (0);
		str++;
	}
	return (1);
}

static int checking(char **args)
{
	int	cmdcode;

	cmdcode = 0;
	if (args[2])
	{
		cmdcode = 1;
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
	}
	else if (ft_strisnum(args[1]) == 0)
	{
		cmdcode = 2;
		ft_putstr_fd("exit: ", 2);
		ft_putstr_fd(args[1], 2);
		ft_putendl_fd(": numeric argument required\n", 2);
	}
	else
		cmdcode = ft_atoi(args[1]);
	return (cmdcode);
}

int	builtin_exit(char **args, t_vars *vars)
{
	(void)**args;
	int	cmdcode;

	cmdcode = 0;
	if (args[1])
		cmdcode = checking(args);
	ft_putendl_fd("exit", STDOUT_FILENO);
	save_history();
	rl_clear_history();
	if (vars && vars->pipeline)
	{
		vars->pipeline->exec_cmds = NULL;
		vars->pipeline->pipe_fds = NULL;
		vars->pipeline->pids = NULL;
		vars->pipeline->status = NULL;
	}
	if (vars)
		cleanup_token_list(vars);
	exit(cmdcode);
	return (0);
}
