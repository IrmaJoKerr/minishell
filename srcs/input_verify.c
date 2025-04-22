/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 11:23:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// /*
// Expands a variable name to its value
// Handles special cases like lone $, $?, and environment variables
// Returns newly allocated string with the expanded value
// */
// char	*expand_value(char *var_name, t_vars *vars)
// {
// 	char	*expanded_value;
// 	fprintf(stderr, "DEBUG[expand_value]: Called with var_name='%s', vars=%p\n", 
// 		var_name ? var_name : "NULL", (void*)vars);
// 	if (!var_name)
// 		return (ft_strdup(""));
// 	if (var_name[0] == '\0')
// 	{
// 		// Lone $ character
// 		expanded_value = ft_strdup("$");
// 		fprintf(stderr, "DEBUG: Handling lone $\n");
// 	}
// 	else if (ft_strcmp(var_name, "?") == 0)
// 	{
// 		// Special case for $?
// 		expanded_value = ft_itoa(vars->error_code);
// 		fprintf(stderr, "DEBUG: Handling $?, expanded to '%s'\n", expanded_value);
// 	}
// 	else if (ft_strcmp(var_name, "0") == 0)
// 	{
// 		// Special case for $0
// 		expanded_value = ft_strdup("bleshell");
// 		fprintf(stderr, "DEBUG: Handling $0, expanded to '%s'\n", expanded_value);
// 	}
// 	else
// 	{
// 		// Regular environment variable
// 		expanded_value = get_env_val(var_name, vars->env);
// 		if (!expanded_value)
// 			expanded_value = ft_strdup("");
// 		fprintf(stderr, "DEBUG: Expanded env var '%s' to '%s'\n", var_name, expanded_value);
// 	}
// 	return (expanded_value);
// }
