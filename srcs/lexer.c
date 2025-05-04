/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 01:10:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Master function to get token type at current position
Checks double tokens first, then single tokens
Returns token type and updates position via moves parameter
*/
t_tokentype	get_token_at(char *input, int pos, int *moves)
{
	t_tokentype	token_type;

	token_type = 0;
	*moves = 0;
	token_type = is_double_token(input, pos, moves);
	if (token_type != 0)
	{
		return (token_type);
	}
	token_type = is_single_token(input, pos, moves);
	if (token_type != 0)
	{
		return (token_type);
	}
	*moves = 1; // Default move is 1 character if no special token found
    return (0); // Not a special token start
}

/*
Master function for lexing analysis.
Handle token processing based on token type.
- Preserve adjacency state across function boundaries
- Route to appropriate specialized handlers
- Process operators, quotes, expansions and whitespace
Returns:
- 1 on success.
- 0 on error.
*/
// int	tokenizer(char *input, t_vars *vars)
// {
// 	int	hd_is_delim;
// 	int	result;

// 	init_tokenizer(vars);
// 	hd_is_delim = 0;
// 	while (input && input[vars->pos])
// 	{
// 		vars->next_flag = 0;
// 		if (hd_is_delim)
// 		{
// 			result = proc_hd_delim(input, vars, &hd_is_delim);
// 			if (result == 0)
// 				return (0);
// 			if (result == 1)
// 				continue ;
// 		}
// 		if (!vars->next_flag && !hd_is_delim)
// 		{
// 			if (!handle_token(input, vars, &hd_is_delim))
// 				return (0);
// 		}
// 		if (chk_move_pos(vars, hd_is_delim))
// 			continue ;
// 	}
// 	return (finish_tokenizing(input, vars, hd_is_delim));
// }
int	tokenizer(char *input, t_vars *vars)
{
    int	hd_is_delim;
    int	result;

    init_tokenizer(vars); // Initialize state for new tokenization
    hd_is_delim = 0; // Flag for expecting heredoc delimiter

    while (input && input[vars->pos])
    {
        vars->next_flag = 0; // Reset flag for each iteration

        // --- ADDED DEBUG PRINT ---
        fprintf(stderr, "[DEBUG-TOKENIZER-LOOP] Before handle_token: pos=%d, prev_type=%s, curr_type=%s\n",
                vars->pos, get_token_str(vars->prev_type), get_token_str(vars->curr_type));
        // --- END ADDED DEBUG PRINT ---

        // Handle heredoc delimiter expectation separately
        if (hd_is_delim)
        {
            result = proc_hd_delim(input, vars, &hd_is_delim);
            if (result == 0) return (0); // Error
            if (result == 2) continue; // Skipped whitespace, continue loop
            // If result == 1, next_flag is set, handled below
        }
        else
        {
            // General token handling
            if (!handle_token(input, vars, &hd_is_delim))
                return (0); // Error in handler
        }

        // --- ADDED DEBUG PRINT ---
        fprintf(stderr, "[DEBUG-TOKENIZER-LOOP] After handle_token: pos=%d, prev_type=%s, curr_type=%s, next_flag=%d\n",
                vars->pos, get_token_str(vars->prev_type), get_token_str(vars->curr_type), vars->next_flag);
        // --- END ADDED DEBUG PRINT ---

        // Check if handler set next_flag (meaning pos was advanced internally)
        if (chk_move_pos(vars, hd_is_delim)) // chk_move_pos returns 1 if next_flag is set
            continue; // Skip manual pos increment if handler advanced it

        // If next_flag was not set, manually advance position (unless it was heredoc delim processing)
        // chk_move_pos already handles the !hd_is_delim check for advancing pos
    }

    // After loop, process any remaining text and check for unterminated heredoc
    return (finish_tokenizing(input, vars, hd_is_delim));
}


/*
Handle token processing based on token type
- Preserve adjacency state across function boundaries
- Route to appropriate specialized handlers
- Process operators, quotes, expansions and whitespace
Returns:
- 1 on success
- 0 on error
*/
// int	handle_token(char *input, t_vars *vars, int *hd_is_delim)
// {
// 	t_tokentype	token_type;
// 	int			moves;
// 	int			adj_saved[3];

// 	ft_memcpy(adj_saved, vars->adj_state, sizeof(adj_saved));
// 	token_type = get_token_at(input, vars->pos, &moves);
// 	if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
// 		return (handle_quotes(input, vars, adj_saved));
// 	if (input[vars->pos] == '$' && !vars->quote_depth)
// 	{
// 		ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
// 		tokenize_expan(input, vars);
// 		return (1);
// 	}
// 	if (is_operator_token(token_type))
// 		return (proc_opr_token(input, vars, hd_is_delim, token_type));
// 	if (ft_isspace(input[vars->pos]))
// 	{
// 		tokenize_white(input, vars);
// 		return (1);
// 	}
// 	return (1);
// }
int	handle_token(char *input, t_vars *vars, int *hd_is_delim)
{
    t_tokentype	token_type;
    int			moves;
    int			adj_saved[3];

    // Save adjacency state before potentially modifying it
    ft_memcpy(adj_saved, vars->adj_state, sizeof(adj_saved));

    // Determine token type at current position
    token_type = get_token_at(input, vars->pos, &moves);

    // Handle quotes: Restore adjacency state before calling tokenize_quote
    if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
    {
        // Restore adjacency state that might have been changed by handle_text
        ft_memcpy(vars->adj_state, adj_saved, sizeof(int) * 3);
        tokenize_quote(input, vars);
        // tokenize_quote sets next_flag on success, error_code on failure
        return (vars->error_code == 0); // Return success if no error
    }

    // Handle unquoted expansion ($)
    if (input[vars->pos] == '$' && !vars->quote_depth) // Check quote_depth if needed
    {
        tokenize_expan(input, vars);
        // tokenize_expan sets next_flag on success
        return (1); // Assume success for now, error handling might be needed in make_exp_token
    }

    // Handle operators (pipe, redirects)
    if (is_operator_token(token_type))
    {
        // proc_opr_token handles text before operator and creates operator token
        if (!proc_opr_token(input, vars, hd_is_delim, token_type))
            return (0); // Error during operator processing
        // proc_opr_token sets next_flag
        return (1);
    }

    // Handle whitespace
    if (ft_isspace(input[vars->pos]))
    {
        tokenize_white(input, vars);
        // tokenize_white sets next_flag
        return (1);
    }

    // If none of the above, it's part of a regular text token.
    // We don't call handle_text here directly. Instead, we just advance the position.
    // handle_text will be called later when a special character or end of input is encountered.
    // vars->pos++; // Move to the next character

    // Let chk_move_pos handle the position increment in the main loop
    return (1); // Indicate success, main loop will advance pos if next_flag isn't set
}

/*
Extracts quoted content from input.
Sets the quote_type to TYPE_SINGLE_QUOTE or TYPE_DOUBLE_QUOTE.
Returns:
- The quoted string.(caller must free).
- NULL on error.
*/
// char	*get_quoted_str(char *input, t_vars *vars, int *quote_type)
// {
// 	char	quote_char;
// 	char	*content;
// 	int		start;
// 	int		len;

// 	quote_char = input[vars->pos];
// 	if (quote_char == '\'')
// 		*quote_type = TYPE_SINGLE_QUOTE;
// 	else
// 		*quote_type = TYPE_DOUBLE_QUOTE;
// 	check_token_adj(input, vars);
// 	start = ++(vars->pos);
// 	while (input[vars->pos] && input[vars->pos] != quote_char)
// 		(vars->pos)++;
// 	if (!input[vars->pos])
// 	{
// 		vars->pos = start - 1;
// 		return (NULL);
// 	}
// 	len = vars->pos - start;
// 	content = ft_substr(input, start, len);
// 	if (!content)
// 		return (NULL);
// 	(vars->pos)++;
// 	return (content);
// }
char	*get_quoted_str(char *input, t_vars *vars, int *quote_type)
{
    char	quote_char;
    char	*content;
    int		start;
    int		len;

    quote_char = input[vars->pos];
    if (quote_char == '\'')
        *quote_type = TYPE_SINGLE_QUOTE;
    else if (quote_char == '"')
        *quote_type = TYPE_DOUBLE_QUOTE;
    else
        return (NULL); // Should not happen if called correctly

    // Check adjacency relative to the opening quote character
    check_token_adj(input, vars);

    start = vars->pos + 1; // Content starts after the opening quote
    vars->pos++;		   // Move past the opening quote

    // Find the closing quote
    while (input[vars->pos] && input[vars->pos] != quote_char)
        (vars->pos)++;

    // Check if closing quote was found
    if (!input[vars->pos])
    {
        // Unclosed quote error
        tok_syntax_error_msg(&quote_char, vars); // Report syntax error
        vars->error_code = ERR_SYNTAX;
        return (NULL);
    }

    // Extract content
    len = vars->pos - start;
    content = ft_substr(input, start, len);
    if (!content)
    {
        vars->error_code = ERR_DEFAULT; // Malloc error
        return (NULL);
    }

    (vars->pos)++; // Move past the closing quote
    return (content);
}

/*
Processes operator characters in the input string.
- Identifies redirection operators and pipe.
- Creates tokens for these operators.
- Updates position past the operator.
Returns:
- 1 if operator was processed.
- 0 if otherwise.

Example: For input "cmd > file"
- Processes the '>' operator.
- Creates redirect token.
- Returns 1 to indicate operator was handled.
*/
// int	process_operator_char(char *input, int *i, t_vars *vars)
// {
// 	int			moves;
// 	t_tokentype	token_type;

// 	token_type = get_token_at(input, *i, &moves);
// 	if (token_type == 0)
// 	{
// 		return (0);
// 	}
// 	vars->curr_type = token_type;
// 	if (moves == 2)
// 	{
// 		handle_double_operator(input, vars);
// 	}
// 	else
// 	{
// 		handle_single_operator(input, vars);
// 	}
// 	return (1);
// }
int	process_operator_char(char *input, int *i, t_vars *vars)
{
    int			moves;
    t_tokentype	token_type;
    char		*token_str;

    // Get token type and number of characters (moves)
    token_type = get_token_at(input, *i, &moves);

    // If it's not a recognized operator start, return 0
    if (!is_operator_token(token_type))
    {
        return (0); // Should not happen if called correctly, but safety check
    }

    // Extract the operator string
    token_str = ft_substr(input, *i, moves);
    if (!token_str)
    {
        vars->error_code = ERR_DEFAULT;
        return (0); // Malloc failure
    }

    // Update types before creating token
    vars->prev_type = vars->curr_type;
    vars->curr_type = token_type;

    // Create the token node
    maketoken(token_str, token_type, vars); // maketoken handles linking

    free(token_str); // Free the temporary string

    // Update position and start for next token
    *i += moves;
    vars->start = *i; // Start of next potential token is after the operator

    return (1); // Success
}
