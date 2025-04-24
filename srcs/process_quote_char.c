/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/24 05:47:40 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Expands a variable and appends it to the result string
Return :
 - Updated result string.
 - Original string unchanged if variable expansion fails.
 - NULL on error.
Works with expand_quoted_str().
*/
char	*expand_quoted_var(char *expanded, char *content, int *pos
                ,t_vars *vars)
{
    char	*var_value;
    char	*temp;
    
    var_value = expand_variable(content, pos, NULL, vars);
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
char	*append_basic_strs(char *expanded, char *content, int *pos)
{
	int	start;

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
char	*expand_quoted_str(char *content, t_vars *vars)
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
Modifies the content pointer if expansion is needed.
Returns:
- The command node.
- NULL if not found/error.
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
    {
        free(dummy_token);
    }
    else
    {
        if (vars->adj_state[1])
            process_right_adj(input, vars);
        process_adj(NULL, vars);
    }
    return (join_success);
}

/*
Process a character within quotes in the input string
Handles extraction, expansion, and creation of tokens
Return:
 - 1 on success.
 - 0 on failure.
*/
int process_quote_char(char *input, t_vars *vars, int is_redir_target)
{
    int     quote_type;
    char    *content;
    t_node  *cmd_node;
    t_node  *redir_node;

    fprintf(stderr, "DEBUG[process_quote_char]: Processing quote%s\n",
        is_redir_target ? " as redirection target" : "");
    content = get_quoted_str(input, vars, &quote_type);
    if (!content)
        return (0);
    fprintf(stderr, "DEBUG: process_quote_char: Quote content='%s', redir_target=%d\n",
        content, is_redir_target);
    if (is_redir_target)
    {
        redir_node = find_last_redir(vars);
        if (redir_node && is_redirection(redir_node->type))
        {
            if (redir_node->type == TYPE_HEREDOC && vars && vars->pipes)
            {
                fprintf(stderr, "[DEBUG] Heredoc delimiter is quoted. Expansion flag was set to %d during tokenization.\n", vars->pipes->hd_expand);
            }
            fprintf(stderr, "DEBUG: Associating quoted filename '%s' with redirection type %d\n",
                content, redir_node->type);
            t_node *file_node = initnode(TYPE_ARGS, content);
            if (!file_node)
            {
                free(content);
                return (0);
            }
            redir_node->right = file_node;
            if (redir_node->next)
            {
                file_node->next = redir_node->next;
                redir_node->next->prev = file_node;
            }
            redir_node->next = file_node;
            file_node->prev = redir_node;
            vars->current = file_node;
            fprintf(stderr, "DEBUG: Added file node to token list after redirection node\n");
            free(content);
            return (1);
        }
        fprintf(stderr, "[ERROR] process_quote_char: Quoted string marked as redir target, but no preceding redirection found.\n");
        free(content);
        // Maybe set vars->error_code = ERR_SYNTAX; ?
        return (0); // Indicate error
    }
    cmd_node = process_quoted_str(&content, quote_type, vars);
    if (!cmd_node && vars->adj_state[0] == 0)
    {
        cmd_node = initnode(TYPE_CMD, content);
        if (!cmd_node)
		{
            free(content);
            process_adj(NULL, vars);
            return (0);
        }
        if (build_token_linklist(vars, cmd_node))
		{
             // If build_token_linklist freed the node (e.g., merged), content is already handled.
             // This path might need review depending on build_token_linklist behavior.
        } else
		{
             // Node was linked, content is now owned by the node.
        }
        free(content); // Content is now owned/copied by the node or was freed by merge
        if (vars->adj_state[1])
            process_right_adj(input, vars);
        process_adj(NULL, vars);
        return (1);
    }
    else if (!cmd_node)
    {
        if (!merge_quoted_token(input, content, vars))
        {
             fprintf(stderr, "[ERROR] process_quote_char: Failed to merge quoted token and no command node found.\n");
             free(content);
             process_adj(NULL, vars);
             return (0);
        }
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

/*
Finds the most relevant redirection node in the token list.
Checks in the following order of priority:
 - Current node if it's a redirection
 - Recent previous nodes (up to 3 back)
 - Last redirection in the entire token list
Returns:
 - Pointer to the located redirection node
 - NULL if no redirection node exists
Works with process_quote_char() for redirection target handling.
*/
t_node	*find_last_redir(t_vars *vars)
{
    t_node	*current;
	t_node	*last_redir;
    int		i;

	last_redir = NULL;
    if (vars->current && is_redirection(vars->current->type))
        return (vars->current);
    current = vars->current;
    i = 0;
    while (current && current->prev && i < 3)
	{
        current = current->prev;
        i++;
        if (is_redirection(current->type))
            return (current);
    }
    current = vars->head;
    while (current)
	{
        if (is_redirection(current->type))
            last_redir = current;
        current = current->next;
    }
    return (last_redir);
}

/*
Validates that a single redirection token has a valid target.
- Checks that the redirection has a next token
- Ensures the next token isn't another operator
- Reports appropriate syntax errors
Returns:
- 1 if the redirection has a valid target
- 0 otherwise (with error message shown)
*/
int	validate_single_redir(t_node *redir_node, t_vars *vars)
{
    t_node *next;
    
    next = redir_node->next;
    if (!next)
    {
        tok_syntax_error_msg("newline", vars);
        return (0);
    }
    else if (is_redirection(next->type) || next->type == TYPE_PIPE)
    {
        if (next->args[0])
            tok_syntax_error_msg(next->args[0], vars);
        else
            tok_syntax_error_msg("operator", vars);
        return (0);
    }
    return (1);
}

/*
Validates that all redirection operators in the token list have valid targets.
- Ensures no redirection is at the end of input
- Ensures no redirection is followed by another operator
- Reports appropriate syntax errors
Returns:
- 1 if all redirections have valid targets
- 0 otherwise (with error_code set)
*/
int	validate_redir_targets(t_vars *vars)
{
    t_node	*current;
    
    current = vars->head;
    while (current)
    {
        if (is_redirection(current->type))
        {
            if (!validate_single_redir(current, vars))
                return (0);
        }
        current = current->next;
    }
    return (1);
}
