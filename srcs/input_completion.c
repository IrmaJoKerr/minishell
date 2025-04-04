/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 01:46:26 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if a command is complete.
Returns 1 if complete, 0 if more input is needed.
Complete means no unclosed quotes and no dangling pipes.
*/
int	is_input_complete(t_vars *vars)
{
	t_node	*current;
	int		expecting_command;

	expecting_command = 0;
	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			expecting_command = 1;
		}
		else if (expecting_command && current->type != TYPE_ARGS
			&& current->type != TYPE_CMD)
		{
			expecting_command = 1;
		}
		else if (expecting_command)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	return (!expecting_command);
}

/*
Check if there's an unfinished pipe at the end of input.
Returns:
- 1 if there's an unfinished pipe needing completion
- 0 if there's no unfinished pipe
Works with handle_unfinished_pipes().
*/
int	check_unfinished_pipe(t_vars *vars, t_ast *ast)
{
	t_node	*last_token;
	t_node	*current;

	last_token = NULL;
	current = vars->head;
	while (current)
	{
		last_token = current;
		current = current->next;
	}
	if (last_token && last_token->type == TYPE_PIPE)
	{
		if (ast)
			ast->pipe_at_end = 1;
		return (1);
	}
	if (ast && ast->pipe_at_end)
		return (1);
	return (0);
}

/*
Check for unfinished pipes in input and handle them.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
*/
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	char	*addon_input;
	char	*tmp;
	char	*combined;

	addon_input = NULL;
	tmp = NULL;
	if (!check_unfinished_pipe(vars, ast))
		return (0);
	addon_input = readline("> ");
	if (!addon_input)
	{
		free(addon_input);
		return (handle_unfinished_pipes(processed_cmd, vars, ast));
	}
	tmp = ft_strtrim(addon_input, " \t\n");
	free(addon_input);
	addon_input = tmp;
	if (!addon_input || addon_input[0] == '\0')
	{
		free(addon_input);
		return (handle_unfinished_pipes(processed_cmd, vars, ast));
	}
	tmp = ft_strjoin(*processed_cmd, " ");
	if (!tmp)
	{
		free(addon_input);
		return (-1);
	}
	combined = ft_strjoin(tmp, addon_input);
	free(tmp);
	free(addon_input);
	if (!combined)
		return (-1);
	free(*processed_cmd);
	*processed_cmd = combined;
	cleanup_token_list(vars);
	improved_tokenize(*processed_cmd, vars);
	return (1);
}

char	*get_quote_input(t_vars *vars)
{
	char	*addon;
	char	*prompt;
	char	quote_char;

	if (!vars)
	{
		ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
		return (NULL);
	}
	if (vars->quote_depth <= 0)
	{
		ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
		return (NULL);
	}
	if (vars->quote_depth > 32 || vars->quote_depth < 1)
	{
		vars->quote_depth = 0;
		ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
		return (NULL);
	}
	prompt = "QUOTE> ";
	quote_char = vars->quote_ctx[vars->quote_depth - 1].type;
	ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
	if (quote_char == '\'')
		prompt = "SQUOTE> ";
	else if (quote_char == '"')
		prompt = "DQUOTE> ";
	addon = readline(prompt);
	if (!addon)
		return (NULL);
	return (addon);
}

/*
Process a single quote completion cycle.
Returns:
- 1 if succeeded and more processing needed
- 0 if succeeded and no more processing needed
- -1 if an error occurred
*/
int	chk_quotes_closed(char **processed_cmd, t_vars *vars)
{
	char	*addon;
	int		result;

	addon = get_quote_input(vars);
	if (!addon)
	{
		free(processed_cmd);
		return (-1);
	}
	*processed_cmd = append_new_input(*processed_cmd, addon);
	free(addon);
	if (!*processed_cmd)
		return (-1);
	if (tokenize_to_test(*processed_cmd, vars) < 0)
		return (-1);
	if (vars->quote_depth > 0)
		result = 1;
	else
		result = 0;
	return (result);
}

/*
Main handler for unclosed quotes in input
Returns:
  1 if quotes were handled and modifications were made
  0 if no unclosed quotes found
 -1 if an error occurred
*/
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
	char *completed_input;
	char *temp;
	 
	if (!processed_cmd || !*processed_cmd)
		return (-1);	 
	// First check if we have unclosed quotes
	if (validate_quotes(*processed_cmd, vars))
		return (0); // All quotes are closed, nothing to do	 
	// We have unclosed quotes, get completion
	completed_input = complete_quoted_input(vars, *processed_cmd);
	if (!completed_input)
		return (-1); 
	// Replace the original command with the completed one
	temp = *processed_cmd;  // Store the old pointer
	*processed_cmd = completed_input;  // Update to the new pointer
	free(temp);  // Free the old memory
	// Re-tokenize with the completed command
	cleanup_token_list(vars);
	improved_tokenize(*processed_cmd, vars); 
	return (1);
}

/*
Combines original input with continuation input
Returns a newly allocated string with both inputs joined
*/
char *append_input(char *original, char *additional)
{
    char *temp;
    char *result;
    
    // First join with a newline
    temp = ft_strjoin(original, "\n");
    if (!temp)
        return (NULL);
    
    // Then add the additional input
    result = ft_strjoin(temp, additional);
    free(temp);
    
    return (result);
}
