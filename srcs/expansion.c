/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:34:52 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Handles special variables like $? and $0
*/
char	*handle_special_var(char *var_name, t_vars *vars)
{
	if (!ft_strcmp(var_name, "?"))
		return (ft_itoa(vars->error_code));
	if (!ft_strcmp(var_name, "0"))
		return (ft_strdup("minishell"));
	return (NULL);
}

/*
Main function handling expansion of environment variables.
Returns the expanded value.
*/
char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
	int		start;
	char	*var_name;
	char	*value;
	char	*result;

	start = ++(*pos);
	while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
		(*pos)++;
	var_name = ft_substr(input, start, *pos - start);
	if (!var_name)
		return (NULL);
	value = handle_special_var(var_name, vars);
	if (!value)
		value = getenv(var_name);
	free(var_name);
	if (value)
		result = ft_strdup(value);
	else
		result = ft_strdup("");
	return (result);
}
