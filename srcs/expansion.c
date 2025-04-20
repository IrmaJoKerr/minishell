/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/04/20 19:31:36 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes special shell variables like $? and $0.
- Handles $?: Returns exit status of last command.
- Handles $0: Returns shell name ("bleshell").
- For empty var_name: Returns an empty string.
Returns:
Newly allocated string with variable value.
NULL if not a special variable.
Works with handle_expansion().

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
Retrieves value of an environment variable from vars->env.
- Searches through environment array for matching variable.
- Extracts value portion after the '=' character.
- Returns empty string for missing variables.
Returns:
Newly allocated string containing variable value.
Empty string if variable not found or on errors.
Works with handle_expansion().

Example: For "HOME=/Users/bleow" in environment:
get_env_val("HOME", env) -> "/Users/bleow"
get_env_val("NONEXISTENT", env) -> ""
*/
// char	*get_env_val(const char *var_name, char **env)
// {
// 	int		i;
// 	size_t	var_len;

// 	if (!var_name || !*var_name || !env)
// 		return (ft_strdup(""));
// 	var_len = ft_strlen(var_name);
// 	i = 0;
// 	while (env[i])
// 	{
// 		if (!ft_strncmp(env[i], var_name, var_len)
// 			&& env[i][var_len] == '=')
// 			return (ft_strdup(env[i] + var_len + 1));
// 		i++;
// 	}
// 	return (ft_strdup(""));
// }
char *get_env_val(const char *var_name, char **env)
{
    int     i;
    size_t  var_len;
    char    *val;
    
    if (!var_name || !*var_name || !env)
    {
        val = ft_strdup("");
        if (val)
            register_allocation(val, "ENV_EMPTY", __func__);
        return val;
    }
    
    var_len = ft_strlen(var_name);
    i = 0;
    while (env[i])
    {
        if (!ft_strncmp(env[i], var_name, var_len) && env[i][var_len] == '=')
        {
            val = ft_strdup(env[i] + var_len + 1);
            if (val)
                register_allocation(val, "ENV_VALUE", __func__);
            fprintf(stderr, "[MEM_TRACK] Allocated env value for '%s' at %p\n", 
                    var_name, (void*)val);
            return val;
        }
        i++;
    }
    
    val = ft_strdup("");
    if (val)
        register_allocation(val, "ENV_NOTFOUND", __func__);
    return val;
}

/**
 * Central variable expansion engine that handles all expansion cases
 * 
 * @param input       Input string (NULL if var_name is provided)
 * @param pos         Position in input string (updated if not NULL)
 * @param var_name    Pre-extracted variable name (NULL if input/pos provided)
 * @param vars        Shell variables structure
 * 
 * @return Newly allocated string with expansion result
 */
char *expand_variable(char *input, int *pos, char *var_name, t_vars *vars)
{
    char *local_var_name = NULL;
    char *result = NULL;
    int free_var_name = 0;
    
    fprintf(stderr, "DEBUG: expand_variable: Called with %s\n", 
            var_name ? var_name : (input ? "input string" : "NULL"));
    
    // Extract variable name if not provided
    if (!var_name && input && pos) {
        // Skip the $ character
        (*pos)++;
        
        // Handle special case - end of string or whitespace
        if (!input[*pos] || input[*pos] <= ' ' || input[*pos] == '\n') {
            fprintf(stderr, "DEBUG: expand_variable: Lone $ detected\n");
            result = ft_strdup("$");
            #ifdef DEBUG_MEMORY
            register_allocation(result, "LITERAL_$", __func__);
            #endif
            return result;
        }
        
        // Handle special case - exit status
        if (input[*pos] == '?') {
            (*pos)++;
            result = ft_itoa(vars->error_code);
            fprintf(stderr, "DEBUG: expand_variable: $? expanded to '%s'\n", result);
            #ifdef DEBUG_MEMORY
            register_allocation(result, "EXIT_STATUS", __func__);
            #endif
            return result;
        }
        
        // Extract regular variable name
        local_var_name = get_var_name(input, pos);
        var_name = local_var_name;
        free_var_name = 1;
    }
    
    // Handle the variable expansion
    if (!var_name || !*var_name) {
        result = ft_strdup(var_name ? "$" : "");
        fprintf(stderr, "DEBUG: expand_variable: Empty variable name\n");
    } else if (ft_strcmp(var_name, "?") == 0) {
        result = ft_itoa(vars->error_code);
        fprintf(stderr, "DEBUG: expand_variable: $? expanded to '%s'\n", result);
    } else if (ft_strcmp(var_name, "0") == 0) {
        result = ft_strdup("bleshell");
        fprintf(stderr, "DEBUG: expand_variable: $0 expanded to 'bleshell'\n");
    } else {
        // Try special variables first
        result = handle_special_var(var_name, vars);
        
        // If not a special var, try environment variable
        if (!result) {
            result = get_env_val(var_name, vars->env);
            if (!result)
                result = ft_strdup("");
            fprintf(stderr, "DEBUG: expand_variable: Expanded '%s' to '%s'\n", 
                    var_name, result);
        }
    }
    
    // Register the allocation
    #ifdef DEBUG_MEMORY
    if (result)
        register_allocation(result, var_name ? var_name : "EMPTY", __func__);
    #endif
    
    // Clean up if we created a local variable name
    if (free_var_name && local_var_name)
        free(local_var_name);
        
    return result;
}

/*
Extracts variable name from input string.
- Takes input string and current position.
- Extracts variable name (alphanumeric + underscore).
- Updates position to point after the variable name.
Returns:
Newly allocated string containing variable name.
NULL on memory allocation failure.
Works with handle_expansion().
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
Processes environment variable expansion.
- Checks for $ character at current position.
- Extracts variable name following the $.
- Handles special vars or environment vars.
- Updates position to after the expanded variable.
Returns:
Newly allocated string with expanded value.
Empty string if variable not found.
*/
char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
	fprintf(stderr, "DEBUG: Entering handle_expansion()\n");
    char	*var_name;
    char	*var_value;

    // Skip the $ symbol
    (*pos)++;
    // Check if we're at the end of input or at whitespace (lonely $)
    if (!input[*pos] || input[*pos] <= ' ' || input[*pos] == '\n')
    {
        fprintf(stderr, "DEBUG: Lone $ detected, returning literal $\n");
        return (ft_strdup("$"));
    }
    // Check for special case $?
    if (input[*pos] == '?')
    {
        (*pos)++;
        var_value = ft_itoa(vars->error_code);
        fprintf(stderr, "DEBUG: Expanded $? to '%s'\n", var_value);
        return (var_value);
    }
    // Extract variable name
    var_name = get_var_name(input, pos);
    // Handle empty variable name
    if (!var_name || !*var_name)
    {
        fprintf(stderr, "DEBUG: Empty var name, returning literal $\n");
        if (var_name)
            free(var_name);
        return (ft_strdup("$"));
    }
    // Try to handle as special variable first
    var_value = handle_special_var(var_name, vars);
    if (var_value)
    {
        fprintf(stderr, "DEBUG: Expanded special var $%s to '%s'\n", 
                var_name, var_value);
        free(var_name);
        return (var_value);
    }
    // Try to handle as environment variable
    var_value = get_env_val(var_name, vars->env);
    fprintf(stderr, "DEBUG: handle_expansion() Expanded env var $%s to '%s'\n", 
            var_name, var_value ? var_value : "");
    free(var_name);
    // Return the environment variable value or empty string if not found
    if (var_value)
        return (var_value);
    else
        return (ft_strdup(""));
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
