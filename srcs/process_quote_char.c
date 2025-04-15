/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/15 18:17:44 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Expands a variable and appends it to the result string
Return :
 - Updated result string.
 - Original string unchanged if variable expansion fails
 - NULL on error.
Works with expand_quoted_str().
*/
char	*expand_quoted_var(char *expanded, char *content, int *pos
				,t_vars *vars)
{
	char	*var_value;
	char	*temp;
	
	var_value = handle_expansion(content, pos, vars);
	if (!var_value)
		return (expanded);
	temp = ft_strjoin(expanded, var_value);
	free(expanded);
	free(var_value);
	if (!temp)
		return (NULL);
	return (temp);
}

/*
Appends non-variable text to the expanded result string
Processes characters until a $ variable marker is found
Return:
 - Updated result string.
 - NULL on error.
Works with expand_quoted_str().
*/
char *append_basic_strs(char *expanded, char *content, int *pos)
{
	int start;

	start = *pos;
	while (content[*pos] && content[*pos] != '$')
		(*pos)++;
	expanded = append_substr(expanded, content, start, *pos - start);
	if (!expanded)
		return (NULL);
	return (expanded);
}

/*
Expands variables in a double-quoted string
Takes a string that may contain $VAR and expands them
Handles:
 - Character-by-character processing.
 - Variable expansion.
Returns:
 - A newly allocated string with variables expanded.
 - NULL on error.
Example: Input "$USER lives in $HOME"
 - Expands to "john lives in /home/john"
Works with process_quote_char().
*/
char *expand_quoted_str(char *content, t_vars *vars)
{
	char	*expanded;
	int		pos;
	
	pos = 0;
	expanded = ft_strdup("");
	if (!expanded)
		return (NULL);
	while (content[pos])
	{
		if (content[pos] == '$')
		{
			expanded = expand_quoted_var(expanded, content, &pos, vars);
			if (!expanded)
				return (NULL);
		}
		else
		{
			expanded = append_basic_strs(expanded, content, &pos);
			if (!expanded)
				return (NULL);
		}
	}
	return (expanded);
}
/*
Appends a substring to an existing string.
Handles:
 - Memory allocation for substring extraction.
 - String joining with proper cleanup.
Returns:
 - New concatenated string (caller must free).
 - NULL on error
Works with append_basic_strs().
*/
char	*append_substr(char *dest, char *src, int start, int len)
{
	char	*chunk;
	char	*result;
	
	if (!dest || !src)
		return (NULL);
	chunk = ft_substr(src, start, len);
	if (!chunk)
	{
		free(dest);
		return (NULL);
	}
	result = ft_strjoin(dest, chunk);
	free(dest);
	free(chunk);
	return (result);
}

/*
Extracts quoted content from input.
Returns the quoted string (or NULL on error).
Sets the quote_type to TYPE_SINGLE_QUOTE or TYPE_DOUBLE_QUOTE.
*/
char	*get_quoted_str(char *input, t_vars *vars, int *quote_type)
{
    char	quote_char;
    char	*content;
	int		start;
    int		len;
    
    quote_char = input[vars->pos];
    if (quote_char == '\'')
        *quote_type = TYPE_SINGLE_QUOTE;
    else
        *quote_type = TYPE_DOUBLE_QUOTE;
    check_token_adj(input, vars);
    start = ++(vars->pos);
    while (input[vars->pos] && input[vars->pos] != quote_char)
        (vars->pos)++;
    if (!input[vars->pos])
    {
        vars->pos = start - 1;
        return (NULL);
    }
    len = vars->pos - start;
    content = ft_substr(input, start, len);
    if (!content)
        return (NULL);
    (vars->pos)++;
    return (content);
}

/*
Process quoted content, including variable expansion in double quotes.
Finds the appropriate command node for the content.
Returns the command node (or NULL if not found/error).
Modifies the content pointer if expansion is needed.
*/
t_node	*process_quoted_str(char **content_ptr, int quote_type, t_vars *vars)
{
    t_node	*cmd_node;
    
    if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(*content_ptr, '$'))
    {
        char *expanded = expand_quoted_str(*content_ptr, vars);
        free(*content_ptr);
        if (!expanded)
            return (NULL);
        *content_ptr = expanded;
    }
    cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
    return (cmd_node);
}

/*
Tries to merge the quoted token with existing tokens based on adjacency.
Returns:
 - 1 if successfully merged.
 - 0 otherwise.
*/
int	merge_quoted_token(char *input, char *content, t_vars *vars)
{
    char	*dummy_token;
	int		join_success;
    
    if (!vars->adj_state[0])
        return (0);
    dummy_token = ft_strdup("");
    if (!dummy_token)
        return (0);
    join_success = handle_tok_join(input, vars, content, dummy_token);
    if (!join_success)
        free(dummy_token);
    if (join_success)
    {
        if (vars->adj_state[1])
            process_right_adj(input, vars);
        process_adj(NULL, vars);
        return (1);
    }
    return (0);
}

/*
Process a character within quotes in the input string
Handles extraction, expansion, and creation of tokens
Return:
 - 1 on success.
 - 0 on failure.
*/
int	process_quote_char(char *input, t_vars *vars)
{
    int		quote_type;
    char	*content;
    t_node	*cmd_node;
    
    content = get_quoted_str(input, vars, &quote_type);
    if (!content)
        return (0);
    cmd_node = process_quoted_str(&content, quote_type, vars);
    if (!cmd_node)
    {
        free(content);
        process_adj(NULL, vars);
        return (1);
    }
    if (!merge_quoted_token(input, content, vars))
    {
        append_arg(cmd_node, content, quote_type);
        free(content);
        if (vars->adj_state[1])
            process_right_adj(input, vars);
        process_adj(NULL, vars);
    }
    return (1);
}
