/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 14:06:58 by bleow            ###   ########.fr       */
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
char *handle_special_var(const char *var_name, t_vars *vars)
{
	fprintf(stderr, "DEBUG: Entering handle_special_var()\n");
	// If var_name or vars is NULL, return empty string (not NULL)
	if (!var_name || !vars)
		return (ft_strdup(""));
	// Special case for error code ($?)
	if (ft_strcmp(var_name, "?") == 0)
	{
		fprintf(stderr, "DEBUG: handle_special_var().Handling special var $? = %d\n", vars->error_code);
		return (ft_itoa(vars->error_code));
	}
	// Special case for shell name ($0)
	if (ft_strcmp(var_name, "0") == 0)
	{
		fprintf(stderr, "DEBUG: handle_special_var().Handling special var $0 = bleshell\n");
		return (ft_strdup("bleshell"));
	}
	// Empty variable name case
	if (var_name[0] == '\0')
		return (ft_strdup(""));
	return (NULL);
}


/*
Central variable expansion engine that handles all expansion cases
Returns:
- Newly allocated string with expansion result
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
		{
            fprintf(stderr, "DEBUG: expand_variable: Lone $ detected\n");
            result = ft_strdup("$");
            return (result);
        }
        if (input[*pos] == '?')
		{
            (*pos)++;
            result = ft_itoa(vars->error_code);
            fprintf(stderr, "DEBUG: expand_variable: $? expanded to '%s'\n", result);
            return (result);
        }
        local_var_name = get_var_name(input, pos);
        var_name = local_var_name;
        free_var_name = 1;
    }
    if (!var_name || !*var_name)
	{
        result = ft_strdup(var_name ? "$" : "");
        fprintf(stderr, "DEBUG: expand_variable: Empty variable name\n");
    }
	else if (ft_strcmp(var_name, "?") == 0)
	{
        result = ft_itoa(vars->error_code);
        fprintf(stderr, "DEBUG: expand_variable: $? expanded to '%s'\n", result);
    }
	else if (ft_strcmp(var_name, "0") == 0)
	{
        result = ft_strdup("bleshell");
        fprintf(stderr, "DEBUG: expand_variable: $0 expanded to 'bleshell'\n");
    }
	else 
	{
        result = handle_special_var(var_name, vars);
        if (!result)
		{
            result = get_env_val(var_name, vars->env);
            if (!result)
                result = ft_strdup("");
            fprintf(stderr, "DEBUG: expand_variable: Expanded '%s' to '%s'\n", 
                    var_name, result);
        }
    }
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
char *get_env_val(const char *var_name, char **env)
{
    int     i;
    size_t  var_len;
    char    *val;
    
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

/*
DEBUG FUNCTION
Displays command arguments after expansion.
*/
void debug_cmd_args(t_node *node)
{
    int i = 0;
    
    if (!node || !node->args)
        return;
    
    DBG_PRINTF(DEBUG_EXPAND, "=== Command arguments after expansion ===\n");
    while (node->args[i])
    {
        DBG_PRINTF(DEBUG_EXPAND, "args[%d] = '%s' (quote_type: %p)\n", 
                  i, node->args[i], 
                  (void*)(node->arg_quote_type ? node->arg_quote_type[i] : NULL));
        i++;
    }
    DBG_PRINTF(DEBUG_EXPAND, "========================================\n");
}
