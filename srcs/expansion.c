/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:39:17 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles special shell variables like $? and $0
Returns a newly allocated string with the variable value or
NULL if not a special var.
- $?: Returns the exit status of the last command (from vars->error_code)
- $0: Returns the shell name ("minishell")
- For empty var_name: Returns an empty string
- For non-special vars: Returns NULL
Example: After a command exits with status 1:
		 $? -> "1"
		 $0 -> "minishell"
*/
char	*handle_special_var(const char *var_name, t_vars *vars)
{
	if (!var_name || !*var_name)
		return (ft_strdup(""));
	if (!ft_strcmp(var_name, "?"))
		return (ft_itoa(vars->error_code));
	if (!ft_strcmp(var_name, "0"))
		return (ft_strdup("minishell"));
	return (NULL);
}

/*
Gets the value of an environment variable from vars->env.
Works with handle_expansion().
Returns a new string with the value or empty string
to handle_expansion() if not found.
Example: HOME -> /Users/bleow
Function will find HOME in vars->env and return /Users/bleow
*/
char	*get_env_value(const char *var_name, char **env)
{
	int		i;
	size_t	var_len;

	if (!var_name || !*var_name || !env)
		return (ft_strdup(""));
	var_len = ft_strlen(var_name);
	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], var_name, var_len)
			&& env[i][var_len] == '=')
			return (ft_strdup(env[i] + var_len + 1));
		i++;
	}
	return (ft_strdup(""));
}

/*
Main function handling expansion of environment variables.
Returns a new string with the expanded value.
Returns NULL if input is not an environment variable (doesn't start with $)
or if memory allocation fails.
Example: Input: $HOME
		 Output: /Users/bleow
		 Input: $NONSENSE
		 Output: ""
*/
char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
	int		start;
	char	*var_name;
	char	*value;

	if (!input || input[*pos] != '$')
		return (NULL);
	start = ++(*pos);
	while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
		(*pos)++;
	var_name = ft_substr(input, start, *pos - start);
	if (!var_name)
		return (NULL);
	value = handle_special_var(var_name, vars);
	if (!value)
	{
		value = get_env_value(var_name, vars->env);
	}
	ft_safefree((void **)&var_name);
	return (value);
}
