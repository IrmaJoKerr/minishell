/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_module.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 12:02:18 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 12:03:14 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_module.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:00:00 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 16:00:00 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Core variable expansion functions
 */

/*
 * Extracts variable name from input string
 * Returns newly allocated string with variable name, NULL on failure
 */
char *extract_var_name(const char *input, int *pos)
{
    int start;
    char *var_name;
    
    if (!input || !pos || input[*pos] != '$')
        return (NULL);
        
    (*pos)++; // Skip past the $ character
    start = *pos;
    
    // Variable names can include alphanumeric and underscore characters
    while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
        (*pos)++;
        
    var_name = ft_substr(input, start, *pos - start);
    
    DBG_PRINTF(DEBUG_EXPAND, "extract_var_name: Extracted '%s'\n", 
              var_name ? var_name : "NULL");
              
    return (var_name);
}

/*
 * Special case handler for $? (exit status) and $0 (shell name)
 * Returns allocated string with value, NULL if not a special variable
 */
char *expand_special_var(const char *var_name, t_vars *vars)
{
    if (!var_name || !*var_name)
        return (ft_strdup(""));
        
    if (!ft_strcmp(var_name, "?"))
        return (ft_itoa(vars ? vars->error_code : 0));
        
    if (!ft_strcmp(var_name, "0"))
        return (ft_strdup("bleshell"));
        
    return (NULL);
}

/*
 * Retrieves environment variable value
 * Returns allocated string with value, empty string if not found
 */
char *get_env_value(const char *var_name, char **env)
{
    int i;
    size_t var_len;
    
    if (!var_name || !*var_name || !env)
        return (ft_strdup(""));
        
    var_len = ft_strlen(var_name);
    i = 0;
    
    while (env[i])
    {
        if (!ft_strncmp(env[i], var_name, var_len) && env[i][var_len] == '=')
            return (ft_strdup(env[i] + var_len + 1));
        i++;
    }
    
    return (ft_strdup(""));
}

/*
 * Main expansion function - expands a variable at current position
 * Updates position to after the expanded variable
 * Returns allocated string with expanded value
 */
char *expand_variable(const char *input, int *pos, t_vars *vars)
{
    char *var_name;
    char *var_value = NULL;
    int start_pos;
    
    DBG_PRINTF(DEBUG_EXPAND, "expand_variable: At pos %d: '%s'\n", 
              *pos, input + *pos);
    
    start_pos = *pos;
    
    // Skip the $ character
    (*pos)++;
    
    // Special case for $?
    if (input[*pos] == '?')
    {
        (*pos)++;
        return (ft_itoa(vars ? vars->error_code : 0));
    }
    
    // Get variable name
    var_name = extract_var_name(input, pos);
    if (!var_name)
    {
        // If extraction failed, reset position and return $ character
        *pos = start_pos + 1;
        return (ft_strdup("$"));
    }
    
    DBG_PRINTF(DEBUG_EXPAND, "Variable name: '%s'\n", var_name);
    
    // First check for special variables
    var_value = expand_special_var(var_name, vars);
    if (var_value)
    {
        DBG_PRINTF(DEBUG_EXPAND, "Special variable '%s' expanded to '%s'\n", 
                  var_name, var_value);
        free(var_name);
        return (var_value);
    }
    
    // Then check environment
    var_value = get_env_value(var_name, vars ? vars->env : NULL);
    DBG_PRINTF(DEBUG_EXPAND, "Environment variable '%s' expanded to '%s'\n", 
              var_name, var_value ? var_value : "");
              
    free(var_name);
    return (var_value);
}

/*
 * Node-level expansion functions
 */

/*
 * Expands a single argument string
 * Returns a newly allocated string with all variables expanded
 */
char *expand_argument(char *arg, t_vars *vars, int quote_type)
{
    char *result = NULL;
    char *segment = NULL;
    char *temp = NULL;
    int pos = 0;
    int start = 0;
    
    // Skip expansion in single quotes
    if (quote_type == 1)
        return ft_strdup(arg);
        
    result = ft_strdup("");
    if (!result)
        return (NULL);
    
    while (arg[pos])
    {
        // Look for $ character
        if (arg[pos] == '$')
        {
            // Add text before the $ if any
            if (pos > start)
            {
                segment = ft_substr(arg, start, pos - start);
                temp = ft_strjoin(result, segment);
                free(segment);
                free(result);
                result = temp;
            }
            
            // Expand the variable
            segment = expand_variable(arg, &pos, vars);
            temp = ft_strjoin(result, segment);
            free(segment);
            free(result);
            result = temp;
            
            // Set new start position
            start = pos;
            continue;
        }
        pos++;
    }
    
    // Add remaining text
    if (pos > start)
    {
        segment = ft_substr(arg, start, pos - start);
        temp = ft_strjoin(result, segment);
        free(segment);
        free(result);
        result = temp;
    }
    
    return result;
}

/*
 * Expands all arguments in a command node
 */
void expand_cmd_args(t_node *node, t_vars *vars)
{
    int i;
    char *expanded;
    char *old_arg;
    
    if (!node || !node->args)
        return;
    
    DBG_PRINTF(DEBUG_EXPAND, "expand_cmd_args: Expanding node type %d (%s)\n",
              node->type, get_token_str(node->type));
    
    // Start from args[0] for all nodes
    i = 0;
    
    // For command nodes, skip the command name (args[0])
    if (node->type == TYPE_CMD)
        i = 1;
    
    while (node->args[i])
    {
        // Get quote type - single quotes (1) prevent expansion
        int quote_type = (node->arg_quote_type) ? node->arg_quote_type[i] : 0;
        
        if (quote_type == 1 || !ft_strchr(node->args[i], '$'))
        {
            i++;
            continue;
        }
        
        // Expand the argument
        DBG_PRINTF(DEBUG_EXPAND, "Processing arg '%s', quote_type=%d\n",
                 node->args[i], quote_type);
                 
        old_arg = node->args[i];
        expanded = expand_argument(old_arg, vars, quote_type);
        
        if (expanded)
        {
            DBG_PRINTF(DEBUG_EXPAND, "Expanded '%s' to '%s'\n", 
                     old_arg, expanded);
            
            free(old_arg);
            node->args[i] = expanded;
        }
        i++;
    }
    
    DBG_PRINTF(DEBUG_EXPAND, "==== Command arguments after expansion ====\n");
    i = 0;
    while (node->args[i])
    {
        DBG_PRINTF(DEBUG_EXPAND, "args[%d] = '%s' (quote_type: %d)\n",
                 i, node->args[i],
                 node->arg_quote_type ? node->arg_quote_type[i] : -1);
        i++;
    }
}

/*
 * Public expansion interface
 */

/*
 * Processes all expansions in the token list
 * Handles commands, arguments, and other token types
 */
void process_expansions(t_vars *vars)
{
    t_node *current;
    
    if (!vars || !vars->head)
        return;
    
    DBG_PRINTF(DEBUG_EXPAND, "process_expansions: Starting\n");
    
    // Process command and arg nodes directly
    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_CMD || current->type == TYPE_ARGS)
            expand_cmd_args(current, vars);
            
        current = current->next;
    }
    
    // Process expansion tokens and convert them to arguments
    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
        {
            process_expansion_token(current, vars);
        }
        current = current->next;
    }
    
    DBG_PRINTF(DEBUG_EXPAND, "process_expansions: Complete\n");
}

/*
 * Processes an expansion token by appending it to its command
 */
void process_expansion_token(t_node *token, t_vars *vars)
{
    char *var_name;
    char *expanded_value;
    t_node *cmd_node;
    
    if (!token || !token->args || !token->args[0])
        return;
    
    DBG_PRINTF(DEBUG_EXPAND, "process_expansion_token: Token '%s'\n", 
              token->args[0]);
    
    // Extract variable name (skip the $ prefix)
    var_name = token->args[0] + 1;
    DBG_PRINTF(DEBUG_EXPAND, "Variable name: %s\n", var_name);
    
    // Special case for $?
    if (ft_strcmp(var_name, "?") == 0)
    {
        expanded_value = ft_itoa(vars->error_code);
        DBG_PRINTF(DEBUG_EXPAND, "Expanding $? to: %s\n", expanded_value);
    }
    else
    {
        expanded_value = get_env_value(var_name, vars->env);
        DBG_PRINTF(DEBUG_EXPAND, "Expanding $%s to: %s\n", 
                  var_name, expanded_value);
    }
    
    // Find the preceding command node
    cmd_node = find_cmd(vars->head, token, FIND_PREV, NULL);
    if (cmd_node)
    {
        append_arg(cmd_node, expanded_value, 0);
        DBG_PRINTF(DEBUG_EXPAND, "Appended '%s' to command '%s'\n", 
                  expanded_value, cmd_node->args[0]);
    }
    
    // Clean up
    free(expanded_value);
    
    // Convert token to args type (it's now been processed)
    token->type = TYPE_ARGS;
    DBG_PRINTF(DEBUG_EXPAND, "Token type changed to TYPE_ARGS\n");
}

/*
 * Heredoc-specific expansion
 */

/*
 * Expands variables in heredoc line
 * Returns newly allocated string with all variables expanded
 */
char *expand_heredoc_line(char *line, t_vars *vars)
{
    char *result = NULL;
    int pos = 0;
    int start_pos = 0;
    char *segment;
    char *temp;
    
    if (!line || !vars)
        return ft_strdup("");
    
    result = ft_strdup("");
    if (!result)
        return ft_strdup("");
    
    while (line[pos])
    {
        if (line[pos] == '$')
        {
            // Add text before variable
            if (pos > start_pos)
            {
                segment = ft_substr(line, start_pos, pos - start_pos);
                temp = ft_strjoin(result, segment);
                free(segment);
                free(result);
                result = temp;
            }
            
            // Get variable value
            segment = expand_variable(line, &pos, vars);
            temp = ft_strjoin(result, segment);
            free(segment);
            free(result);
            result = temp;
            
            start_pos = pos;
            continue;
        }
        pos++;
    }
    
    // Add remaining text
    if (pos > start_pos)
    {
        segment = ft_substr(line, start_pos, pos - start_pos);
        temp = ft_strjoin(result, segment);
        free(segment);
        free(result);
        result = temp;
    }
    
    return result;
}
