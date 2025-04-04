/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 04:14:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tokenize input for verification.
Cleans up any previous token list first.
Returns:
- 0 if tokenization was successful
- -1 if an error occurred
*/
int	tokenize_to_test(char *input, t_vars *vars)
{
	if (!input || !vars)
		return (-1);
	cleanup_token_list(vars);
	vars->head = NULL;
	vars->current = NULL;
	init_quote_context(vars);
	improved_tokenize(input, vars);
	return (0);
}

/*
Check if input starts with a pipe token (syntax error).
Returns:
- 1 if pipe found at beginning of input
- 0 if no pipe at beginning or no tokens
Works with chk_syntax_errors().
*/
int	chk_pipe_before_cmd(t_vars *vars, t_ast *ast)
{
	if (!vars || !vars->head)
		return (0);
	if (vars->head->type == TYPE_PIPE)
	{
		if (ast)
			ast->pipe_at_front = 1;
		return (1);
	}
	return (0);
}

/*
Check for consecutive pipes in token list (syntax error).
Code 258 is the standard code used for syntax errors.
Returns:
0 - no consecutive pipes found
1 - consecutive pipes found (sets error code)
*/
int	chk_serial_pipes(t_vars *vars, t_ast *ast)
{
	t_node	*current;

	if (!vars || !vars->head || !ast)
		return (0);
	ast->serial_pipes = 0;
	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			if (ast->serial_pipes > 0)
			{
				vars->error_code = 258;
				ast->syntax_error = 1;
				return (1);
			}
			ast->serial_pipes++;
		}
		else
			ast->serial_pipes = 0;
		current = current->next;
	}
	return (0);
}

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

int	chk_input_valid(t_vars *vars, char **input)
{
	int		modified;
	char	*result;
	t_ast *ast;

	modified = 0;
	if (vars->quote_depth > 0)
	{
		result = fix_open_quotes(*input, vars);
		if (!result)
			return (0);
		if (result != *input)
		{
			*input = result;
			modified = 1;
		}
	}
	ast = init_ast_struct();
	if (ast && !is_input_complete(vars))
	{
		result = handle_pipe_valid(*input, vars, 0);
		if (result && result != *input)
		{
			free(*input);  // Free the memory pointed to by *input
			*input = result;  // Now update the pointer
			modified = 1;
		}
	}
	cleanup_ast_struct(ast);
	return (modified);
}

/*
Process the input to ensure it represents a complete command.
Prompts for additional input when needed.
Returns the complete input string or NULL on error.
*/
char	*verify_input(char *input, t_vars *vars)
{
	char	*complete_input;
	int		modified;

	complete_input = ft_strdup(input);
	if (!complete_input)
		return (NULL);
	modified = 0;
	cleanup_token_list(vars);
	improved_tokenize(complete_input, vars);
	while (vars->quote_depth > 0 || !is_input_complete(vars))
	{
		modified = chk_input_valid(vars, &complete_input);
		if (!complete_input)
			return (NULL);
		cleanup_token_list(vars);
		improved_tokenize(complete_input, vars);
	}
	if (!modified)
	{
		free(complete_input);
		return (input);
	}
	return (complete_input);
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
