/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 20:29:40 by bleow            ###   ########.fr       */
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
		else if (expecting_command && current->type != TYPE_STRING
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
OLD VERSION
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	char *additional;

	if (!check_unfinished_pipe(vars, ast))
		return (0);

	print_error("Pipe at end of input", NULL, 0);
	additional = readline("pipe> ");

	if (!additional)
	{
		// User pressed Ctrl+D during pipe input
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	*processed_cmd = append_new_input(*processed_cmd, additional);
	ft_safefree((void **)&additional);

	if (!*processed_cmd)
		return (-1);
	return (1);
}
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
		return (-1);
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
	tokenize(*processed_cmd, vars);
	lexerlist(*processed_cmd, vars);
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
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	*processed_cmd = append_new_input(*processed_cmd, addon);
	ft_safefree((void **)&addon);
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
Checks if quotes in a string are properly balanced.
- Handles both double and single quotes
- Properly tracks quote context
- Returns 1 if balanced, 0 if unbalanced
*/
int	quotes_are_closed(const char *str)
{
	int in_double_quote;
	int in_single_quote;
	int i;

	in_double_quote = 0;
	in_single_quote = 0;
	i = 0;
	if (!str)
		return (1);
	while (str[i])
	{
		if (in_single_quote && str[i] == '\'')
			in_single_quote = 0;
		else if (in_double_quote && str[i] == '"')
			in_double_quote = 0;
		else if (!in_single_quote && !in_double_quote)
		{
			if (str[i] == '\'')
				in_single_quote = 1;
			else if (str[i] == '"')
				in_double_quote = 1;
		}
		i++;
	}
	return (!in_single_quote && !in_double_quote);
}

int	handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
	char	*addon;
	char	*new_cmd;
	int		quote_handled;

	if (!processed_cmd || !*processed_cmd)
		return (-1);
	if (quotes_are_closed(*processed_cmd))
	{
		vars->quote_depth = 0;
		return (0);
	}
	cleanup_token_list(vars);
	vars->head = NULL;
	vars->current = NULL;
	quote_handled = 0;
	while (vars->quote_depth > 0)
	{
		addon = get_quote_input(vars);
		if (!addon)
			return (ft_safefree((void **)processed_cmd), -1);
		new_cmd = append_input(*processed_cmd, addon);
		ft_safefree((void **)&addon);
		if (!new_cmd)
			return (-1);
		ft_safefree((void **)processed_cmd);
		*processed_cmd = new_cmd;
		cleanup_token_list(vars);
		vars->head = NULL;
		vars->current = NULL;
		if (tokenize_to_test(*processed_cmd, vars) < 0)
			return (-1);
		quote_handled = 1;
		if (vars->quote_depth == 0)
			break ;
	}
	return quote_handled ? 1 : 0;
}

char	*append_input(const char *first, const char *second)
{
	size_t	len1;
	size_t	len2;
	char	*result;

	if (!first)
		return (ft_strdup(second));
	if (!second)
		return (ft_strdup(first));
	len1 = ft_strlen(first);
	len2 = ft_strlen(second);
	result = malloc(len1 + len2 + 2);
	if (!result)
		return (NULL);
	ft_memcpy(result, first, len1);
	result[len1] = '\n';
	ft_memcpy(result + len1 + 1, second, len2);
	result[len1 + len2 + 1] = '\0';
	return (result);
}
