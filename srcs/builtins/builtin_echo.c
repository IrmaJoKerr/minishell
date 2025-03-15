/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/03/14 01:49:15 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: echo. Writes arguments to standard output.
1. Added a newline variable to flag if the -n flag is present.
   Flag is set to 1 if the -n flag is not present and 0 if it is present.
   (Normally a newline is printed at the end of the echo command)
2. Added a space after each argument if there is more than one argument.
*/
int	builtin_echo(char **args)
{
	int	i;
	int	j;
	int	newline;

	if (!args || !args[0])
		return (1);
	i = 1;
	newline = 1;
	if (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		j = 0;
		while (args[i][j])
			write(1, &args[i][j++], 1);
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}
