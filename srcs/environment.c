/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 16:15:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:34:41 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Expands environment variables. 
Takes in string and checks if it is an environment variable.
Handles "?" as a special case for exit status.
Returns expanded value or empty string.
*/
char	*expand_env_var(const char *var_name, t_vars *vars)
{
	int		i;
	size_t	var_len;
	char	*value;

	if (!var_name)
		return (ft_strdup(""));
	if (!ft_strcmp(var_name, "?"))
		return (ft_itoa(vars->error_code));
	i = 0;
	var_len = ft_strlen(var_name);
	while (vars->env[i])
	{
		if (!ft_strncmp(vars->env[i], var_name, var_len) 
			&& vars->env[i][var_len] == '=')
			return (ft_strdup(vars->env[i] + var_len + 1));
		i++;
	}
	return (ft_strdup(""));
}

/*
Main function handling expansion of environment variables.
*/
char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
	int		start;
	char	*var_name;
	char	*value;

	if (input[*pos] != '$')
		return (NULL);
	start = ++(*pos);
	while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
		(*pos)++;
	var_name = ft_substr(input, start, *pos - start);
	if (!var_name)
		return (NULL);
	value = expand_env_var(var_name, vars);
	free(var_name);
	return (value);
}
