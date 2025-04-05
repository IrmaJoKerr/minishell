/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 19:12:53 by bleow            ###   ########.fr       */
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
