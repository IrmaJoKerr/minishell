/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 09:42:04 by bleow            ###   ########.fr       */
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
	*moves = 1;
	return (0);
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
int tokenizer(char *input, t_vars *vars)
{
    int hd_is_delim;
    int result;

    fprintf(stderr, "DEBUG-TOKENIZE: Starting tokenization of input: '%s'\n", input);
    if (!input || !*input)
        return (0);
    init_tokenizer(vars);
    hd_is_delim = 0;
    while (input && input[vars->pos])
    {
        fprintf(stderr, "DEBUG-TOKENIZE: Processing position %d, char '%c'\n", 
                vars->pos, input[vars->pos]);
        vars->next_flag = 0;
        if (hd_is_delim)
        {
            result = proc_hd_delim(input, vars, &hd_is_delim);
            if (result == 0)
            {
                fprintf(stderr, "DEBUG-TOKENIZE: Heredoc delimiter processing failed\n");
                return (0);
            }
            if (result == 1)
            {
                fprintf(stderr, "DEBUG-TOKENIZE: Heredoc delimiter processed, continuing\n");
                continue;
            }
        }
        if (!vars->next_flag && !hd_is_delim)
        {
            if (!handle_token(input, vars, &hd_is_delim))
            {
                fprintf(stderr, "DEBUG-TOKENIZE: Failed to handle token at position %d\n", vars->pos);
                return (0);
            }
        }
        
        // This is critical - we must continue only if it returns 1, NOT return on 0
        if (chk_move_pos(vars, hd_is_delim))
        {
            fprintf(stderr, "DEBUG-TOKENIZE: Position check triggered continue\n");
            continue;
        }
        fprintf(stderr, "DEBUG-TOKENIZE: Position advanced to %d\n", vars->pos);
    }
    fprintf(stderr, "DEBUG-TOKENIZE: Tokenization complete, finishing\n");
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
int	handle_token(char *input, t_vars *vars, int *hd_is_delim)
{
	t_tokentype	token_type;
	int			moves;
	int			adj_saved[3];

	ft_memcpy(adj_saved, vars->adj_state, sizeof(adj_saved));
	token_type = get_token_at(input, vars->pos, &moves);
	if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
		return (handle_quotes(input, vars, adj_saved));
	if (input[vars->pos] == '$' && !vars->quote_depth)
	{
		ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
		tokenize_expan(input, vars);
		return (1);
	}
	if (is_operator_token(token_type))
		return (proc_opr_token(input, vars, hd_is_delim, token_type));
	if (ft_isspace(input[vars->pos]))
	{
		tokenize_white(input, vars);
		return (1);
	}
	return (1);
}

/*
Extracts quoted content from input.
Sets the quote_type to TYPE_SINGLE_QUOTE or TYPE_DOUBLE_QUOTE.
Returns:
- The quoted string.
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
// char *get_quoted_str(char *input, t_vars *vars, int *quote_type)
// {
// 	int     start;
// 	int     end;
// 	char    quote_char;
// 	char    *content;

// 	start = vars->pos;
// 	quote_char = input[vars->pos];
// 	fprintf(stderr, "DEBUG-QUOTE: Processing quoted string starting at position %d with %c\n", 
// 			start, quote_char);
// 	*quote_type = (quote_char == '\"') ? TYPE_DOUBLE_QUOTE : TYPE_SINGLE_QUOTE;
// 	vars->pos++;
// 	end = vars->pos;
// 	while (input[end] && input[end] != quote_char)
// 		end++;
// 	if (!input[end])
// 	{
// 		fprintf(stderr, "DEBUG-QUOTE: Unclosed quote, initiating completion process\n");
// 		return (handle_quote_completion(input, vars));
// 	}
// 	content = ft_substr(input, vars->pos, end - vars->pos);
// 	fprintf(stderr, "DEBUG-QUOTE: Extracted content: '%s'\n", content ? content : "NULL");
// 	vars->pos = end + 1;
	
// 	// Check if this might be a quoted redirection target
// 	char next_char = 0;
// 	int i = vars->pos;
// 	while (input[i] && (input[i] == ' ' || input[i] == '\t'))
// 		i++;
// 	next_char = input[i];
	
// 	// Fixed version - use a ternary with a string description instead of a character
// 	fprintf(stderr, "DEBUG-QUOTE: Next non-space character after quote: %s\n", 
//         next_char ? (char[]){next_char, '\0'} : "NONE");
	
// 	return (content);
// }
char *get_quoted_str(char *input, t_vars *vars, int *quote_type)
{
    int     start;
    int     end;
    char    quote_char;
    char    *content;

    start = vars->pos;
    quote_char = input[vars->pos];
    fprintf(stderr, "DEBUG-QUOTE: Processing quoted string starting at position %d with %c\n", 
            start, quote_char);
    *quote_type = (quote_char == '\"') ? TYPE_DOUBLE_QUOTE : TYPE_SINGLE_QUOTE;
    vars->pos++;
    end = vars->pos;
    while (input[end] && input[end] != quote_char)
        end++;
    if (!input[end])
    {
        fprintf(stderr, "DEBUG-QUOTE: Unclosed quote, initiating completion process\n");
        return (handle_quote_completion(input, vars));
    }
    content = ft_substr(input, vars->pos, end - vars->pos);
    fprintf(stderr, "DEBUG-QUOTE: Extracted content: '%s'\n", content ? content : "NULL");
    vars->pos = end + 1;
    
    // Check next character after quote
    int i = vars->pos;
    while (input[i] && (input[i] == ' ' || input[i] == '\t'))
        i++;
        
    fprintf(stderr, "DEBUG-QUOTE: Next non-space character after quote: %s\n", 
            input[i] ? (char[]){input[i], '\0'} : "NONE");
    
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
int	process_operator_char(char *input, int *i, t_vars *vars)
{
	int			moves;
	t_tokentype	token_type;

	token_type = get_token_at(input, *i, &moves);
	if (token_type == 0)
	{
		return (0);
	}
	vars->curr_type = token_type;
	if (moves == 2)
	{
		handle_double_operator(input, vars);
	}
	else
	{
		handle_single_operator(input, vars);
	}
	return (1);
}
