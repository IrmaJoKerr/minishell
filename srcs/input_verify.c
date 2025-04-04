/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 06:17:10 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Processes expansion tokens in the token list
 * Finds the command each expansion belongs to
 * Expands variables and adds them as arguments
 * Removes the expansion tokens when done
 */
void	process_expansions(t_vars *vars)
{
    t_node *current = vars->head;
    char *var_name;
    char *expanded_value;
    t_node *cmd_node;

    DBG_PRINTF(DEBUG_EXPAND, "Starting process_expansions\n");
    while (current)
    {
        DBG_PRINTF(DEBUG_EXPAND, "process_expansions: Node type=%d, content='%s'\n", current->type, current->args[0]);
        if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
        {
            DBG_PRINTF(DEBUG_EXPAND, "Found expansion token\n");
            // Extract variable name from token (skip the $ prefix)
            var_name = current->args[0] + 1;
            DBG_PRINTF(DEBUG_EXPAND, "Variable name: %s\n", var_name);
            // Special case for $?
            if (ft_strcmp(var_name, "?") == 0)
            {
                expanded_value = ft_itoa(vars->error_code);
                DBG_PRINTF(DEBUG_EXPAND, "Expanding $? to value: %s\n", expanded_value);
            }
            else
            {
                expanded_value = get_env_val(var_name, vars->env);
                DBG_PRINTF(DEBUG_EXPAND, "Expanding $%s to value: %s\n", var_name, expanded_value);
            }
            if (!expanded_value)
            {
                expanded_value = ft_strdup("");
                DBG_PRINTF(DEBUG_EXPAND, "Variable not found, setting to empty string\n");
            } 
            DBG_PRINTF(DEBUG_EXPAND, "Replacing token content with expanded value: %s\n", expanded_value);
            
            // Find the preceding command node
            cmd_node = find_preceding_cmd(vars->head, current);
            if (cmd_node)
            {
                append_arg(cmd_node, expanded_value, 0); // Append to command node
                DBG_PRINTF(DEBUG_EXPAND, "Appended '%s' to command '%s'\n", expanded_value, cmd_node->args[0]);
            }
            else
            {
                DBG_PRINTF(DEBUG_EXPAND, "No command node found for expansion\n");
                free(expanded_value);
            }
            
            // Clean up the expansion node
            free(current->args[0]);
            free(expanded_value);
            current->type = TYPE_ARGS; // Change type to ARGS
            DBG_PRINTF(DEBUG_EXPAND, "Token type changed to TYPE_ARGS\n");
            
        }
        current = current->next;
    }
    DBG_PRINTF(DEBUG_EXPAND, "Ending process_expansions\n");
}

/*
Finds the command node that precedes a given expansion node
Used to determine which command an expansion belongs to
Returns the command node or NULL if none found
*/
t_node	*find_preceding_cmd(t_node *head, t_node *exp_node)
{
	t_node *current = head;
	t_node *last_cmd = NULL;
	 
	// Loop through nodes until we reach the expansion node
	while (current && current != exp_node)
	{
		if (current->type == TYPE_CMD)
			last_cmd = current;
		// If we find a pipe, reset the command context
		if (current->type == TYPE_PIPE)
			last_cmd = NULL;		 
		current = current->next;
	} 
	return last_cmd;
}

/*
Joins two strings with a newline character between them.
Takes ownership of first string and frees it.
Returns the joined string or NULL on failure.
*/
char	*join_with_newline(char *first, char *second)
{
	char	*with_newline;
	char	*result;

	with_newline = ft_strjoin(first, "\n");
	free(first);
	if (!with_newline)
		return (NULL);
	result = ft_strjoin(with_newline, second);
	free(with_newline);
	return (result);
}

/*
Helper function to append input with newline.
Takes ownership of first string and frees it after joining.
Returns a newly allocated string with combined content.
*/
char	*append_new_input(char *first, char *second)
{
	char	*result;

	if (!first)
		return (ft_strdup(second));
	if (!second)
		return (first);
	result = join_with_newline(first, second);
	return (result);
}
