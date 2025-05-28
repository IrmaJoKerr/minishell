/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:56 by lechan            #+#    #+#             */
/*   Updated: 2025/05/28 02:43:53 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: export.
- Sets or displays environment variables depending on arguments.
Example with arguments: export VAR_NAME="VALUE"
Example without arguments: export
Returns 0 on success, 1 on failure.
*/
int	builtin_export(char **args, t_vars *vars)
{
	int	cmdcode;

	if (!vars || !vars->env)
	{
		cmdcode = 1;
		vars->error_code = cmdcode;
		return (cmdcode);
	}
	if (!args[1])
		return (export_without_args(vars));
	return (export_with_args(args, vars));
}

/*
Handle export WITH NO ARGUMENTS.
- Prints all environment variables.
Returns 0 on success, 1 on failure.
*/
int	export_without_args(t_vars *vars)
{
	int	count;
	int	cmdcode;

	count = 0;
	while (vars->env[count])
		count++;
	cmdcode = sort_env(count, vars);
	vars->error_code = cmdcode;
	return (cmdcode);
}

/*
Handle export WITH ARGUMENTS.
- Checks if arguments are valid.
- Sets environment variables.
Returns 0 on success, 1 on failure.
*/
int	export_with_args(char **args, t_vars *vars)
{
	int	i;
	int	cmdcode;

	i = 1;
	cmdcode = 0;
	while (args[i])
	{
		if (valid_export(args[i]))
			modify_env(&vars->env, 1, args[i]);
		else
		{
			ft_putstr_fd("export: '", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			cmdcode = 1;
		}
		i++;
	}
	return (vars->error_code = cmdcode);
}

/*
Master control function that sorts and prints environment variables.
- Processes each environment variable and prints them in export format.
- Frees all allocated memory.
Returns 0 on success, 1 on failure.
*/
int	sort_env(int count, t_vars *vars)
{
	int		i;
	int		cmdcode;
	char	**sort_env;

	sort_env = make_sorted_env(count, vars);
	if (!sort_env)
		return (1);
	i = 0;
	cmdcode = 0;
	while (sort_env[i])
	{
		cmdcode = process_export_var(sort_env[i]);
		i++;
	}
	i = 0;
	while (i < count)
	{
		free(sort_env[i]);
		i++;
	}
	free(sort_env);
	return (cmdcode);
}
