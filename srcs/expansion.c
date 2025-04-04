/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 04:50:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Retrieves the exit status from the pipes or vars.
- Checks if pipes exists in vars structure.
- Returns the last command code as a string.
- Falls back to "0" if information not available.
Returns:
Dynamically allocated string containing the exit status.
Works with handle_special_var().
*/
char	*chk_exitstatus(t_vars *vars)
{
	if (vars)
		return (ft_itoa(vars->error_code));
	else
		return (ft_strdup("0"));
}

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
char	*handle_special_var(const char *var_name, t_vars *vars)
{
	if (!var_name || !*var_name)
		return (ft_strdup(""));
	if (!ft_strcmp(var_name, "?"))
		return (chk_exitstatus(vars));
	if (!ft_strcmp(var_name, "0"))
		return (ft_strdup("bleshell"));
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
char	*get_env_val(const char *var_name, char **env)
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
NULL if not a variable or on allocation failure.
Works with lexerlist() and expand_cmd_args().

Example: Input: "$HOME/file" at position 0
- Recognizes $ character
- Extracts "HOME" as variable name
- Returns value (e.g., "/Users/username")
*/
char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
	char	*var_name;
	char	*var_value;

	DBG_PRINTF(DEBUG_EXPAND, "Expansion at pos %d: '%s'\n", *pos, input + *pos);
    
	(*pos)++;
	var_name = get_var_name(input, pos);
	if (!var_name)
	{
        DBG_PRINTF(DEBUG_EXPAND, "Empty variable name\n");
        return (ft_strdup("$"));
	}
	DBG_PRINTF(DEBUG_EXPAND, "Variable name: '%s'\n", var_name);
	var_value = handle_special_var(var_name, vars);
	if (var_value)
	{
        DBG_PRINTF(DEBUG_EXPAND, "Special variable '%s' expanded to '%s'\n", 
                  var_name, var_value);
        free(var_name);
        return var_value;
    }
	var_value = get_env_val(var_name, vars->env);
	DBG_PRINTF(DEBUG_EXPAND, "Environment variable '%s' expanded to '%s'\n", 
		var_name, var_value ? var_value : "");
	free(var_name);
	if (var_value)
		return (var_value);
	return (ft_strdup(""));
}

/*
Expands command arguments, properly handling quoted and unquoted variables
*/
void expand_cmd_args(t_node *node, t_vars *vars)
{
    int i;
    int j;
    int k;
    char *result;
    char *var_name;
    char *var_value;
    char *old_arg;
    char temp_str[2];
    
       if (!node || !node->args)
        return;
    
    DBG_PRINTF(DEBUG_EXPAND, "Expanding args for node type %d (%s)\n", 
              node->type, get_token_str(node->type));
    
    // Start from args[0] for all nodes (we'll handle commands separately)
    i = 0;
    
    // For command nodes, skip the command name (args[0])
    if (node->type == TYPE_CMD)
        i = 1;
        
    while (node->args[i])
    {
        // Skip expansion in single quotes (quote_type == 1)
        if (node->arg_quote_type && node->arg_quote_type[i] == 1)
        {
            DBG_PRINTF(DEBUG_EXPAND, "Skipping expansion for '%s' (in single quotes)\n", 
                      node->args[i]);
            i++;
            continue;
        }
		
        DBG_PRINTF(DEBUG_EXPAND, "Processing arg '%s', quote_type=%d\n", 
			node->args[i], node->arg_quote_type ? node->arg_quote_type[i] : 0);
			
        // Process expansion in double quotes (quote_type == 2) or unquoted
        if (ft_strchr(node->args[i], '$'))
        {
            old_arg = node->args[i];
            result = ft_strdup("");
            if (!result)
                return;
            
            j = 0;
            while (old_arg[j])
            {
                // If we find a $ that's not escaped
                if (old_arg[j] == '$')
                {
					DBG_PRINTF(DEBUG_EXPAND, "Found $ at position %d in '%s'\n", j, old_arg);
                    
                    // Handle $? special case
                    if (old_arg[j+1] == '?')
                    {
                        char *exit_status = ft_itoa(vars->error_code);
                        if (!exit_status)
                        {
                            ft_safefree((void **)&result);
                            return;
                        }
						
                        DBG_PRINTF(DEBUG_EXPAND, "Expanding $? to '%s'\n", exit_status);
                        
                        char *temp = ft_strjoin(result, exit_status);
                        ft_safefree((void **)&result);
                        ft_safefree((void **)&exit_status);
                        
                        if (!temp)
                            return;
                        result = temp;
                        j += 2; // Skip past $?
                        continue;
                    }
                    
                    // Regular variable name
                    k = j + 1;
                    while (old_arg[k] && (ft_isalnum(old_arg[k]) || old_arg[k] == '_'))
                        k++;
                    
                    if (k > j + 1) // We have a variable name
                    {
                        var_name = ft_substr(old_arg, j + 1, k - j - 1);
                        if (!var_name)
                        {
                            ft_safefree((void **)&result);
                            return;
                        }
						
                        DBG_PRINTF(DEBUG_EXPAND, "Found var name: '%s'\n", var_name);
                        
                        var_value = get_env_val(var_name, vars->env);
                        ft_safefree((void **)&var_name);
                        
                        if (var_value)
                        {
							DBG_PRINTF(DEBUG_EXPAND, "Expanding to: '%s'\n", var_value);
                            
                            // Critical change: Join without adding space
                            char *temp = ft_strjoin(result, var_value);
                            ft_safefree((void **)&result);
                            ft_safefree((void **)&var_value);
                            
                            if (!temp)
                                return;
                            result = temp;
                        }
                        
                        j = k; // Skip past the variable name
                        continue;
                    }
                    else // Just a lone $ with no valid variable name
                    {
						DBG_PRINTF(DEBUG_EXPAND, "Lone $ with no variable name\n");
                        
                        char *temp = ft_strjoin(result, "$");
                        ft_safefree((void **)&result);
                        
                        if (!temp)
                            return;
                        result = temp;
                        j++; // Skip past the $
                        continue;
                    }
                }
                
                // For regular characters, append to result
                temp_str[0] = old_arg[j];
                temp_str[1] = '\0';
                char *temp = ft_strjoin(result, temp_str);
                ft_safefree((void **)&result);
                
                if (!temp)
                    return;
                result = temp;
                j++;
            }
            DBG_PRINTF(DEBUG_EXPAND, "Final expansion: '%s' to '%s'\n", old_arg, result);
            
            // Replace old argument with expanded version
            ft_safefree((void **)&node->args[i]);
            node->args[i] = result;
            
            DBG_PRINTF(DEBUG_EXPAND, "Expanded '%s' to '%s'\n", old_arg, result);
        }
        
        i++;
    }
	debug_cmd_args(node);
}

/*
Process expansions and quote handling in the token list.
- Expands environment variables based on quote context
- Handles special cases like $?
- Only expands variables in double quotes or outside quotes
*/
void process_quotes_and_expansions(t_vars *vars)
{
    t_node *current;
    
    if (!vars || !vars->head)
        return;
    
    // First validate quote structure
    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_CMD || current->type == TYPE_ARGS)
        {
            // Expand variables in arguments based on quote type
            expand_cmd_args(current, vars);
        }
        current = current->next;
    }
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
        DBG_PRINTF(DEBUG_EXPAND, "args[%d] = '%s' (quote_type: %d)\n", 
                  i, node->args[i], 
                  node->arg_quote_type ? node->arg_quote_type[i] : -1);
        i++;
    }
    DBG_PRINTF(DEBUG_EXPAND, "========================================\n");
}
