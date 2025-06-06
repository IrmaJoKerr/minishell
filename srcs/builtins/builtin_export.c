/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:56 by lechan            #+#    #+#             */
/*   Updated: 2025/06/02 13:30:17 by bleow            ###   ########.fr       */
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
	t_envop	*to_proc;
	char	**new_env;
	int		old_len;

	if (!vars || !vars->env)
		return (vars->error_code = 1);
	if (!args[1])
		return (export_without_args(vars));
	to_proc = parse_envop_list(args, 1);
	if (!to_proc)
		return (vars->error_code = 1);
	match_envline_to_env(to_proc, vars->env);
	new_env = proc_envop_list(to_proc, vars->env);
	if (new_env)
	{
		old_len = ft_arrlen(vars->env);
		ft_free_2d(vars->env, old_len);
		vars->env = new_env;
	}
	free_envop_list(to_proc);
	return (vars->error_code = 0);
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

/*
Master control function to process and print a single environment variable.
- Checks if the variable has a value (has equals sign).
- If it has a value, prints the variable with the value.
- If it doesn't have a value, prints the variable only.
Returns 0 on success.
*/
int	process_export_var(char *env_var)
{
	char	*equal_pos;

	equal_pos = ft_strchr(env_var, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		process_var_with_val(env_var, equal_pos + 1);
		*equal_pos = '=';
	}
	else
	{
		printf("declare -x %s\n", env_var);
	}
	return (0);
}

/*
Process and print a variable with a value (has equals sign).
- Prints the variable name and value in export format.
Example: "declare -x VAR_NAME="VALUE"
Returns 0 on success.
*/
int	process_var_with_val(char *name, char *value)
{
	printf("declare -x %s=\"", name);
	while (*value)
	{
		if (*value == '"')
			printf("\\\"");
		else
			printf("%c", *value);
		value++;
	}
	printf("\"\n");
	return (0);
}
