/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:50 by lechan            #+#    #+#             */
/*   Updated: 2025/03/22 17:15:29 by lechan           ###   ########.fr       */
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
int	builtin_exit(t_vars *vars)
{
	int	cmdcode;

	cmdcode = 0;
	cmdcode = vars->error_code;
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
