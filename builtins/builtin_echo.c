/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/03/22 17:20:59 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: echo. Writes arguments to standard output.
-  Newline flag is set to 1 if the -n flag is not present and 0 if it is
   present.
   (Normally a newline is printed at the end of the echo command)
Returns command status (0 for success, 1 for failure).
*/
int	builtin_echo(char **args, t_vars *vars)
{
	int	i;
	int	newline;
	int	cmdcode;

	cmdcode = 0;
	if (!args || !args[0])
	{
		cmdcode = 1;
		vars->error_code = cmdcode;
		return (cmdcode);
	}
	i = 1;
	newline = 1;
	if (print_exit_status(args, vars, &i, &newline))
	{
		vars->error_code = cmdcode;
		return (cmdcode);
	}
	if (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	process_echo_args(args, i, newline);
	vars->error_code = cmdcode;
	return (cmdcode);
}

/*
Process and print echo command arguments.
Adds a space after each argument if there is more than one argument.
Adds newline if needed.
Returns 0 on success.
*/
int	process_echo_args(char **args, int start, int nl_flag)
{
	int	i;
	int	j;

	i = start;
	while (args[i])
	{
		j = 0;
		while (args[i][j])
		{
			write(1, &args[i][j], 1);
			j++;
		}
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (nl_flag)
		write(1, "\n", 1);
	return (0);
}

/*
Special handler for when echo is called with $? as argument.
Directly prints the error code from vars struct.
- Respects -n flag for newline control
- Uses write() for output to be consistent with process_echo_args
Returns 1 if $? was handled, 0 otherwise.
*/
int	print_exit_status(char **args, t_vars *vars, int *i, int *newline)
{
	char	*error_str;
	int		j;

	if (args[*i] && ft_strcmp(args[*i], "-n") == 0)
	{
		*newline = 0;
		(*i)++;
	}
	if (args[*i] && ft_strcmp(args[*i], "$?") == 0 && !args[*i + 1])
	{
		error_str = ft_itoa(vars->error_code);
		if (!error_str)
			return (0);
		j = 0;
		while (error_str[j])
		{
			write(1, &error_str[j], 1);
			j++;
		}
		if (*newline)
			write(1, "\n", 1);
		ft_safefree((void **)&error_str);
		return (1);
	}
	return (0);
}
