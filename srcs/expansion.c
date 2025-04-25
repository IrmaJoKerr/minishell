/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 14:19:33 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes special shell variables like $? and $0.
- Handles $?: Returns exit status of last command.
- Handles $0: Returns shell name ("bleshell").
- For empty var_name: Returns an empty string.
Returns:
- Newly allocated string with variable value.
- NULL if not a special variable.
Example: After a command exits with status 1:
$? -> "1"
$0 -> "bleshell"
*/
char	*handle_special_var(const char *var_name, t_vars *vars)
{
	if ((!var_name || !vars) || (var_name[0] == '\0'))
		return (ft_strdup(""));
	if (ft_strcmp(var_name, "?") == 0)
	{
		return (ft_itoa(vars->error_code));
	}
	if (ft_strcmp(var_name, "0") == 0)
	{
		return (ft_strdup("bleshell"));
	}
	return (NULL);
}

/*
Gets the value of a variable by checking special variables first,
then environment.
- Tries special variables like $? and $0 first
- Falls back to environment variables if not special
- Returns empty string for undefined variables
Returns:
- Newly allocated string containing variable value.
- Empty string for undefined or invalid variables.
Example:
get_var_value("HOME", vars) -> "/Users/bleow"
get_var_value("?", vars) -> "0" (assuming last command succeeded)
*/
char	*get_var_value(const char *var_name, t_vars *vars)
{
	char	*result;

	result = handle_special_var(var_name, vars);
	if (!result)
	{
		result = get_env_val(var_name, vars->env);
		if (!result)
			result = ft_strdup("");
	}
	return (result);
}

/*
Handles empty variable names in expansion.
- For non-NULL but empty var_name: Returns "$"
- For NULL var_name: Returns NULL
Returns:
- Newly allocated string with "$" when var_name exists but is empty.
- NULL when var_name is NULL.
Example:
empty_var("") -> "$"
empty_var(NULL) -> NULL
*/
char	*empty_var(char *var_name)
{
	if (var_name)
		return (ft_strdup("$"));
	else
		return (NULL);
}

/*
Master control function for variable expansion.Handles all expansion cases.
Returns:
- Newly allocated string with expansion result.
*/
char	*expand_variable(char *input, int *pos, char *var_name, t_vars *vars)
{
	char	*local_var_name;
	char	*result;
	int		free_var_name;

	local_var_name = NULL;
	result = NULL;
	free_var_name = 0;
	if (!var_name && input && pos)
	{
		(*pos)++;
		if (!input[*pos] || input[*pos] <= ' ' || input[*pos] == '\n')
			return (ft_strdup("$"));
		local_var_name = get_var_name(input, pos);
		var_name = local_var_name;
		free_var_name = 1;
	}
	if (!var_name || !*var_name)
		result = empty_var(var_name);
	else
		result = get_var_value(var_name, vars);
	if (!result)
		result = ft_strdup("");
	if (free_var_name && local_var_name)
		free(local_var_name);
	return (result);
}

/*
Retrieves value of an environment variable from vars->env.
- Searches through environment array for matching variable.
- Extracts value portion after the '=' character.
- Returns empty string for missing variables.
Returns:
- Newly allocated string containing variable value.
- Empty string if variable not found or on errors.
Example: For "HOME=/Users/bleow" in environment:
get_env_val("HOME", env) -> "/Users/bleow"
get_env_val("NONEXISTENT", env) -> ""
*/
char	*get_env_val(const char *var_name, char **env)
{
	int		i;
	size_t	var_len;
	char	*val;

	if (!var_name || !*var_name || !env)
	{
		val = ft_strdup("");
		return (val);
	}
	var_len = ft_strlen(var_name);
	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], var_name, var_len) && env[i][var_len] == '=')
		{
			val = ft_strdup(env[i] + var_len + 1);
			return (val);
		}
		i++;
	}
	val = ft_strdup("");
	return (val);
}

/*
Extracts variable name from input string.
- Takes input string and current position.
- Extracts variable name (alphanumeric + underscore).
- Updates position to point after the variable name.
Returns:
Newly allocated string containing variable name.
NULL on memory allocation failure.
*/
char	*get_var_name(char *input, int *pos)
{
	int		start;
	char	*var_name;

	start = *pos;
	while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
		(*pos)++;
	var_name = ft_substr(input, start, *pos - start);
	return (var_name);
}
