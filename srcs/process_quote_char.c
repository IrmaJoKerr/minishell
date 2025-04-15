/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/15 16:29:54 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// int process_quote_char(char *input, int *i, t_vars *vars)
// {
//     int     start;
//     char    quote_char;
//     int     quote_type;
//     char    *content;
//     t_node  *cmd_node;
//     int     len;
//     int     last_arg_idx;
//     char    *joined;
	
//     // Get complete adjacency information
//     check_token_adj(input, vars);
//     quote_char = input[*i];
//     // Determine quote type based on quote character
// 	if (quote_char == '\'')
// 		quote_type = TYPE_SINGLE_QUOTE;
// 	else
// 		quote_type = TYPE_DOUBLE_QUOTE;
//     // Extract quoted content
//     start = ++(*i);
//     while (input[*i] && input[*i] != quote_char)
//         (*i)++;
//     // Handle unclosed quotes
//     if (!input[*i])
//     {
//         *i = start;
//         return (0);
//     }
//     // Calculate quoted content length
//     len = *i - start;
//     fprintf(stderr, "DEBUG: Quote handling - len=%d, quote_char=%c\n", 
//             len, quote_char); 
//     // Extract quoted content
//     content = ft_substr(input, start, len);
//     if (!content)
//         return (0);
//     // Move past closing quote
//     (*i)++;
//     // Debug messaging
//     if (len == 0)
//         fprintf(stderr, "DEBUG: Empty quote detected\n");
//     fprintf(stderr, "DEBUG: Setting quote_type=%d for content='%s'\n", 
//             quote_type, content);
//     fprintf(stderr, "DEBUG: Adjacency state: left=%d, right=%d\n",
//             vars->adj_state[0], vars->adj_state[1]);
//     // Process variables in double-quoted strings
//     if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(content, '$'))
//     {
//         char *expanded = ft_strdup("");
//         int pos = 0;
//         char *chunk;
//         char *temp;
//         if (!expanded)
//         {
//             free(content);
//             return (0);
//         }
//         fprintf(stderr, "DEBUG: Pre-processing double-quoted content '%s'\n", content);
//         // Process variable expansions in double quotes
//         while (content[pos])
//         {
//             if (content[pos] == '$')
//             {
//                 // Use handle_expansion to process the variable
//                 char *var_value = handle_expansion(content, &pos, vars);
//                 if (var_value)
//                 {
//                     // Concatenate the expanded value
//                     temp = expanded;
//                     expanded = ft_strjoin(expanded, var_value);
//                     free(temp);
//                     free(var_value);
					
//                     if (!expanded)
//                     {
//                         free(content);
//                         return (0);
//                     }
//                 }
//             }
//             else
//             {
//                 // Handle regular character
//                 int start = pos;
//                 while (content[pos] && content[pos] != '$')
//                     pos++;
//                 chunk = ft_substr(content, start, pos - start);
//                 if (!chunk)
//                 {
//                     free(expanded);
//                     free(content);
//                     return (0);
//                 }
//                 temp = expanded;
//                 expanded = ft_strjoin(expanded, chunk);
//                 free(temp);
//                 free(chunk);
//                 if (!expanded)
//                 {
//                     free(content);
//                     return (0);
//                 }
//             }
//         }
//         fprintf(stderr, "DEBUG: Expanded double-quoted content from '%s' to '%s'\n", 
//                 content, expanded);
//         free(content);
//         content = expanded;
//     }
//     // Handle left adjacency (joining with previous token)
//     if (vars->adj_state[0])
//     {
//         cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
//         if (cmd_node && cmd_node->args)
//         {
//             // Find last argument
//             last_arg_idx = 0;
//             while (cmd_node->args[last_arg_idx+1])
//                 last_arg_idx++;
//             // Debug quote types
//             if (cmd_node->arg_quote_type)
//                 fprintf(stderr, "DEBUG: Original arg '%s' had quote_type array\n", 
//                     cmd_node->args[last_arg_idx]);
//             // Join adjacent tokens
//             joined = ft_strjoin(cmd_node->args[last_arg_idx], content);
//             fprintf(stderr, "DEBUG: Joining quote '%s' with '%s' = '%s'\n",
//                   cmd_node->args[last_arg_idx], content, joined);
//             if (joined)
//             {
//                 // Get original argument length
//                 int orig_len = ft_strlen(cmd_node->args[last_arg_idx]);
//                 // Get new content length
//                 int new_len = ft_strlen(content);
//                 // Get total length
//                 int total_len = orig_len + new_len;
//                 // Create new character-level quote type array
//                 int *new_quote_types = malloc(sizeof(int) * (total_len + 1));
//                 if (!new_quote_types)
//                 {
//                     free(joined);
//                     free(content);
//                     return (0);
//                 }
//                 // Copy original quote types if they exist
//                 if (cmd_node->arg_quote_type && cmd_node->arg_quote_type[last_arg_idx])
//                 {
//                     int j = 0;
//                     while (j < orig_len)
//                     {
//                         new_quote_types[j] = cmd_node->arg_quote_type[last_arg_idx][j];
//                         j++;
//                     }
//                 }
//                 else
//                 {
//                     // Default to unquoted if original didn't have quote types
//                     int j = 0;
//                     while (j < orig_len)
//                     {
//                         new_quote_types[j] = 0;
//                         j++;
//                     }
//                 }
//                 // Set quote types for new content
//                 int j = 0;
//                 while (j < new_len)
//                 {
//                     new_quote_types[orig_len + j] = quote_type;
//                     j++;
//                 }
//                 // End marker
//                 new_quote_types[total_len] = -1;
//                 // Update the argument
//                 free(cmd_node->args[last_arg_idx]);
//                 cmd_node->args[last_arg_idx] = joined;
//                 // Update the quote type array
//                 if (cmd_node->arg_quote_type && cmd_node->arg_quote_type[last_arg_idx])
//                 {
//                     free(cmd_node->arg_quote_type[last_arg_idx]);
//                 }
//                 else if (!cmd_node->arg_quote_type)
//                 {
//                     // If arg_quote_type doesn't exist, create it
//                     int arg_count = last_arg_idx + 1;
//                     cmd_node->arg_quote_type = malloc(sizeof(int*) * (arg_count + 1));
//                     if (!cmd_node->arg_quote_type)
//                     {
//                         free(new_quote_types);
//                         free(content);
//                         return (0); 
//                     }
//                     // Initialize others to NULL
//                     int k = 0;
//                     while (k < last_arg_idx)
//                     {
//                         cmd_node->arg_quote_type[k] = NULL;
//                         k++;
//                     }
//                     cmd_node->arg_quote_type[arg_count] = NULL;
//                 }
//                 cmd_node->arg_quote_type[last_arg_idx] = new_quote_types;
//                 fprintf(stderr, "DEBUG: After join, created character-level quote types\n");
//                 free(content);
//                 // Let process_adjacency determine if we should update vars->start
//                 process_adj(i, vars);
//                 return (1);
//             }
//         }
//     }
//     // Handle non-adjacent quotes (new arguments)
//     cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
//     if (cmd_node)
//     {
//         // Append as a new argument
//         append_arg(cmd_node, content, quote_type);
//     }
//     else
//     {
//         // Create a new token
//         if (!vars->head)
//             maketoken_with_type(content, TYPE_CMD, vars);
//         else
//             maketoken_with_type(content, TYPE_ARGS, vars);
//     }
//     free(content);
//     // Let process_adjacency determine if we should update vars->start
//     process_adj(i, vars);
//     return (1);
// }

/*
Expands variables in a double-quoted string
Takes a string that may contain $VAR references and expands them
Returns:
 - A newly allocated string with variables expanded.
 - NULL on error.
*/
// char	*expand_quoted_str(char *content, t_vars *vars)
// {
//     char	*expanded;
// 	char	*chunk;
//     char	*temp;
//     int		pos;
	
// 	pos = 0;
// 	expanded = ft_strdup("");
//     if (!expanded)
//         return (NULL);
//     // Process the string character by character
//     while (content[pos])
//     {
//         if (content[pos] == '$')
//         {
//             // Use handle_expansion to process the variable
//             char *var_value = handle_expansion(content, &pos, vars);
//             if (var_value)
//             {
//                 // Concatenate the expanded value
//                 temp = expanded;
//                 expanded = ft_strjoin(expanded, var_value);
//                 free(temp);
//                 free(var_value);
//                 if (!expanded)
//                     return (NULL);
//             }
//         }
//         else
//         {
//             // Handle regular character sequence (until next $ or end)
//             int start = pos;
//             while (content[pos] && content[pos] != '$')
//                 pos++;
				
//             chunk = ft_substr(content, start, pos - start);
//             if (!chunk)
//             {
//                 free(expanded);
//                 return (NULL);
//             }
//             temp = expanded;
//             expanded = ft_strjoin(expanded, chunk);
//             free(temp);
//             free(chunk);
//             if (!expanded)
//                 return (NULL);
//         }
//     }
//     return (expanded);
// }

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
Process a character within quotes in the input string
Handles extraction, expansion, and creation of tokens
Return:
 - 1 on success.
 - 0 on failure.
*/
// int process_quote_char(char *input, t_vars *vars)
// {
// 	char quote_char;
// 	int quote_type;
// 	int start;
// 	int len;
// 	char *content;
// 	t_node *cmd_node;
	
// 	// Get quote character and determine quote type
// 	quote_char = input[vars->pos];
// 	if (quote_char == '\'')
// 		quote_type = TYPE_SINGLE_QUOTE;
// 	else
// 		quote_type = TYPE_DOUBLE_QUOTE;
// 	// Get complete adjacency information
// 	check_token_adj(input, vars);
// 	// Extract quoted content
// 	start = ++(vars->pos);
// 	while (input[vars->pos] && input[vars->pos] != quote_char)
// 		(vars->pos)++;
// 	// Handle unclosed quotes
// 	if (!input[vars->pos])
// 	{
// 		vars->pos = start - 1; // Go back to the quote character
// 		return (0);
// 	}
// 	len = vars->pos - start;
// 	content = ft_substr(input, start, len);
// 	if (!content)
// 		return (0);
// 	// Move past closing quote
// 	(vars->pos)++;
// 	// Handle variable expansion in double quotes
// 	if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(content, '$'))
// 	{
// 		char *expanded = expand_quoted_str(content, vars);
// 		free(content);
// 		if (!expanded)
// 			return (0);
// 		content = expanded;
// 	}
// 	// Find appropriate command node
// 	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
// 	if (!cmd_node)
// 	{
// 		free(content);
// 		process_adj(NULL, vars);
// 		return (1);
// 	}
// 	// Handle left adjacency (joining with previous token)
// 	if (vars->adj_state[0] && cmd_node->args)
// 	{
// 		int arg_idx = proc_join_args(vars, content);
// 		if (arg_idx >= 0)
// 		{
// 			if (cmd_node->arg_quote_type && cmd_node->arg_quote_type[arg_idx])
// 				update_quote_types(vars, arg_idx, content);
// 			free(content);
// 			// Handle right adjacency
// 			if (vars->adj_state[1])
// 				process_right_adj(input, vars);
// 			process_adj(NULL, vars);
// 			return (1);
// 		}
// 	}
// 	// Handle as new argument
// 	append_arg(cmd_node, content, quote_type);
// 	free(content);
// 	// Handle right adjacency if needed
// 	if (vars->adj_state[1])
// 		process_right_adj(input, vars);
// 	process_adj(NULL, vars);
// 	return (1);
// }
/*
Process a character within quotes in the input string
Handles extraction, expansion, and creation of tokens
Return:
 - 1 on success.
 - 0 on failure.
*/
// int process_quote_char(char *input, t_vars *vars)
// {
//     char quote_char;
//     int quote_type;
//     int start;
//     int len;
//     char *content;
//     t_node *cmd_node;
    
//     // Get quote character and determine quote type
//     quote_char = input[vars->pos];
//     if (quote_char == '\'')
//         quote_type = TYPE_SINGLE_QUOTE;
//     else
//         quote_type = TYPE_DOUBLE_QUOTE;
//     // Get complete adjacency information
//     check_token_adj(input, vars);
//     // Extract quoted content
//     start = ++(vars->pos);
//     while (input[vars->pos] && input[vars->pos] != quote_char)
//         (vars->pos)++;
//     // Handle unclosed quotes
//     if (!input[vars->pos])
//     {
//         vars->pos = start - 1; // Go back to the quote character
//         return (0);
//     }
//     len = vars->pos - start;
//     content = ft_substr(input, start, len);
//     if (!content)
//         return (0);
//     // Move past closing quote
//     (vars->pos)++;
//     // Handle variable expansion in double quotes
//     if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(content, '$'))
//     {
//         char *expanded = expand_quoted_str(content, vars);
//         free(content);
//         if (!expanded)
//             return (0);
//         content = expanded;
//     }
//     // Find appropriate command node
//     cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
//     if (!cmd_node)
//     {
//         free(content);
//         process_adj(NULL, vars);
//         return (1);
//     }
//     // Handle left adjacency using handle_tok_join
//     if (vars->adj_state[0])
//     {
//         // Create an empty dummy token for quote type tracking
//         char *dummy_token = ft_strdup("");
//         if (!dummy_token)
//         {
//             free(content);
//             return (0);
//         }
//         // Call handle_tok_join to process adjacency
//         int join_success = handle_tok_join(input, vars, content, dummy_token);
//         // free(dummy_token);
//         if (join_success)
//         {
//             // Handle right adjacency if needed
//             if (vars->adj_state[1])
//                 process_right_adj(input, vars);
//             process_adj(NULL, vars);
//             return (1);
//         }
//     }
//     // If we get here, left adjacency handling didn't apply or failed
//     // Handle as new argument
//     append_arg(cmd_node, content, quote_type);
//     free(content);
//     // Handle right adjacency if needed
//     if (vars->adj_state[1])
//         process_right_adj(input, vars);
//     process_adj(NULL, vars);
//     return (1);
// }

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
