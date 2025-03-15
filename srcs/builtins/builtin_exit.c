/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:50 by lechan            #+#    #+#             */
/*   Updated: 2025/03/14 09:22:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <readline/readline.h>

/*
Built-in command: exit. Exits the shell.
*/
int	builtin_exit(t_vars *vars)
{
	ft_putendl_fd("exit", STDOUT_FILENO);
	save_history();
	rl_clear_history();
	cleanup_exit(vars);
	exit (vars->error_code);
	return (0);
}
