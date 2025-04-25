/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   improved_tokenize.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 16:42:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 21:06:46 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tokenizes input string. Calls delimiter validation when << is found.
Returns 1 on success, 0 on failure (syntax error or malloc error).
// */
// int	improved_tokenize(char *input, t_vars *vars)
// {
// 	t_tokentype	token_type;
// 	char		*raw_delimiter_str;
// 	int			moves;
// 	int			heredoc_expecting_delim;

// 	heredoc_expecting_delim = 0;
// 	vars->pos = 0;
// 	vars->start = 0;
// 	vars->quote_depth = 0;
// 	if (vars->pipes->heredoc_delim)
// 	{
// 		free(vars->pipes->heredoc_delim);
// 		vars->pipes->heredoc_delim = NULL;
// 	}
// 	vars->pipes->hd_expand = 0;
// 	fprintf(stderr, "[TOK_DBG] improved_tokenize: START, input='%.*s...'\n", 20, input); // DEBUG
// 	while (input && input[vars->pos])
// 	{
// 		vars->next_flag = 0;
// 		token_type = get_token_at(input, vars->pos, &moves);
// 		fprintf(stderr, "[TOK_DBG] Loop Top: Pos=%d, Char='%c', Start=%d, ExpectDelim=%d\n",
// 			vars->pos, input[vars->pos] ? input[vars->pos] : '0', vars->start, heredoc_expecting_delim); // DEBUG
// 		if (heredoc_expecting_delim)
// 		{
// 			fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Current char '%c'\n", input[vars->pos]); // DEBUG
// 			if (ft_isspace(input[vars->pos]))
// 			{
// 				fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Skipping whitespace at pos %d\n", vars->pos); // DEBUG
// 				vars->pos++;
// 				vars->start = vars->pos;
// 				continue ;
// 			}
// 			fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Found non-whitespace '%c' at pos %d. Processing delimiter.\n", input[vars->pos], vars->pos); // DEBUG
// 			vars->start = vars->pos;
// 			fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Start set to %d\n", vars->start);
// 			while (input[vars->pos] && !ft_isspace(input[vars->pos])
// 				&& !is_operator_token(get_token_at(input, vars->pos, &moves)))
// 			{
// 				vars->pos++;
// 			}
// 			fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Loop finished. Pos=%d (Delimiter end)\n", vars->pos); // DEBUG
// 			if (vars->pos == vars->start)
// 			{
// 				tok_syntax_error_msg("newline", vars);
// 				return (0);
// 			}
// 			raw_delimiter_str = ft_substr(input, vars->start,
// 					vars->pos - vars->start);
// 			if (!raw_delimiter_str)
// 			{
// 				vars->error_code = ERR_DEFAULT;
// 				return (0);
// 			}
// 			fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Raw delimiter string = '%s'\n", raw_delimiter_str);
// 			if (!is_valid_delim(raw_delimiter_str, vars))
// 			{
// 				fprintf(stderr, "[TOK_DBG] Delimiter Validation FAILED for '%s'\n", raw_delimiter_str);
// 				free(raw_delimiter_str);
// 				return (0);
// 			}
// 			fprintf(stderr, "[TOK_DBG] Delimiter Validation SUCCEEDED for '%s'. Stored: '%s', Expand=%d\n",
// 				raw_delimiter_str, vars->pipes->heredoc_delim, vars->pipes->hd_expand);
// 			debug_token_creation("improved_tokenize:raw_delimiter", "<<", TYPE_HEREDOC, vars);
// 			maketoken(raw_delimiter_str, TYPE_ARGS, vars);
// 			free(raw_delimiter_str);
// 			heredoc_expecting_delim = 0;
// 			fprintf(stderr, "[TOK_DBG] Delimiter Processed: ExpectDelim reset to 0.\n");
// 			vars->start = vars->pos;
// 			vars->next_flag = 1;
// 		}
// 		if (!vars->next_flag && !heredoc_expecting_delim)
// 		{
// 			fprintf(stderr, "[TOK_DBG] Regular Token Handling: Pos=%d, Char='%c', Type=%d\n", vars->pos, input[vars->pos], token_type); // DEBUG
// 			if (token_type == TYPE_SINGLE_QUOTE
// 				|| token_type == TYPE_DOUBLE_QUOTE)
// 			{
// 				fprintf(stderr, "[TOK_DBG] Calling tokenize_quote\n");
// 				tokenize_quote(input, vars);
// 			}
// 			else if (input[vars->pos] == '$' && !vars->quote_depth)
// 			{
// 				fprintf(stderr, "[TOK_DBG] Calling tokenize_expan\n");
// 				tokenize_expan(input, vars);
// 			}
// 			else if (is_operator_token(token_type))
// 			{
// 				// debug_token_creation("improved_tokenize:heredoc", "<<", TYPE_HEREDOC, vars);
// 				// maketoken("<<", TYPE_HEREDOC, vars);
// 				fprintf(stderr, "[TOK_DBG] Handling Operator Token: Type=%d\n", token_type);
// 				handle_text(input, vars);
// 				if (token_type == TYPE_HEREDOC)
// 				{
// 					fprintf(stderr, "[TOK_DBG] Operator is HEREDOC (<<)\n");
// 					debug_token_creation("improved_tokenize:heredoc", "<<", TYPE_HEREDOC, vars);
// 					maketoken("<<", TYPE_HEREDOC, vars);
// 					vars->pos += 2;
// 					vars->start = vars->pos;
// 					heredoc_expecting_delim = 1;
// 					fprintf(stderr, "[TOK_DBG] Set ExpectDelim=1. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
// 				}
// 				else
// 				{
// 					fprintf(stderr, "[TOK_DBG] Handling other operator (calling process_operator_char)\n"); // DEBUG
// 					if (!process_operator_char(input, &vars->pos, vars))
// 					{
// 						return (0);
// 					}
// 				}
// 				vars->next_flag = 1;
// 			}
// 			else if (ft_isspace(input[vars->pos]))
// 			{
// 				fprintf(stderr, "[TOK_DBG] Calling tokenize_white\n"); // DEBUG
// 				tokenize_white(input, vars);
// 			}
// 			else
// 			{
// 				fprintf(stderr, "[TOK_DBG] Regular char, advancing pos. Pos=%d -> %d\n", vars->pos, vars->pos + 1); // DEBUG
// 			}
// 		}
// 		if (vars->next_flag)
// 		{
// 			fprintf(stderr, "[TOK_DBG] next_flag is set, continuing loop. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
// 			continue ;
// 		}
// 		if (!heredoc_expecting_delim)
// 		{
// 			vars->pos++;
// 		}
// 	}
// 	fprintf(stderr, "[TOK_DBG] Loop finished. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
// 	if (heredoc_expecting_delim)
// 	{
// 		tok_syntax_error_msg("newline", vars);
// 		return (0);
// 	}
// 	fprintf(stderr, "[TOK_DBG] Processing final text chunk if any (Start=%d, Pos=%d)\n", vars->start, vars->pos); // DEBUG
// 	handle_text(input, vars);
// 	fprintf(stderr, "[TOK_DBG] improved_tokenize: END\n");
// 	return (1);
// }

/*
Initialize tokenizer state
- Resets position counters
- Clears quote depth
- Resets heredoc state
*/
static void initialize_tokenizer(t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->quote_depth = 0;
    
    if (vars->pipes->heredoc_delim)
    {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    
    vars->pipes->hd_expand = 0;
}

/*
Process heredoc delimiter when expected
- Skips whitespace before delimiter
- Extracts and validates delimiter string
- Creates token for the delimiter
- Updates state after processing
Returns:
- 0 on error
- 1 on success with next_flag set (continue)
- 2 on waiting for more input (whitespace skipping)
*/
static int process_heredoc_delimiter(char *input, t_vars *vars, int *heredoc_expecting_delim)
{
    char *raw_delimiter_str;
    int moves;
    
    fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Current char '%c'\n", input[vars->pos]); // DEBUG
    
    // Skip whitespace before delimiter
    if (ft_isspace(input[vars->pos]))
    {
        fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Skipping whitespace at pos %d\n", vars->pos); // DEBUG
        vars->pos++;
        vars->start = vars->pos;
        return (2); // Continue to next iteration
    }
    
    // Begin delimiter extraction
    fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Found non-whitespace '%c' at pos %d. Processing delimiter.\n", 
        input[vars->pos], vars->pos); // DEBUG
    vars->start = vars->pos;
    fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Start set to %d\n", vars->start);
    
    // Find delimiter end
    while (input[vars->pos] && !ft_isspace(input[vars->pos])
           && !is_operator_token(get_token_at(input, vars->pos, &moves)))
    {
        vars->pos++;
    }
    
    fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Loop finished. Pos=%d (Delimiter end)\n", vars->pos); // DEBUG
    
    // Check for empty delimiter
    if (vars->pos == vars->start)
    {
        tok_syntax_error_msg("newline", vars);
        return (0); // Error
    }
    
    // Extract delimiter string
    raw_delimiter_str = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!raw_delimiter_str)
    {
        vars->error_code = ERR_DEFAULT;
        return (0); // Error
    }
    
    fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Raw delimiter string = '%s'\n", raw_delimiter_str);
    
    // Validate delimiter
    if (!is_valid_delim(raw_delimiter_str, vars))
    {
        fprintf(stderr, "[TOK_DBG] Delimiter Validation FAILED for '%s'\n", raw_delimiter_str);
        free(raw_delimiter_str);
        return (0); // Error
    }
    
    fprintf(stderr, "[TOK_DBG] Delimiter Validation SUCCEEDED for '%s'. Stored: '%s', Expand=%d\n",
            raw_delimiter_str, vars->pipes->heredoc_delim, vars->pipes->hd_expand);
    
    // Create token for delimiter
    debug_token_creation("process_heredoc_delimiter:raw_delimiter", raw_delimiter_str, TYPE_ARGS, vars);
    maketoken(raw_delimiter_str, TYPE_ARGS, vars);
    free(raw_delimiter_str);
    
    // Reset state
    *heredoc_expecting_delim = 0;
    fprintf(stderr, "[TOK_DBG] Delimiter Processed: ExpectDelim reset to 0.\n");
    vars->start = vars->pos;
    vars->next_flag = 1;
    
    return (1); // Success with next_flag set
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
static int handle_token(char *input, t_vars *vars, int *heredoc_expecting_delim)
{
    t_tokentype token_type;
    int moves;
    int adj_saved[3]; // For state preservation
    
    // Save adjacency state
    ft_memcpy(adj_saved, vars->adj_state, sizeof(adj_saved));
    
    token_type = get_token_at(input, vars->pos, &moves);
    fprintf(stderr, "[TOK_DBG] Regular Token Handling: Pos=%d, Char='%c', Type=%d\n", 
            vars->pos, input[vars->pos], token_type); // DEBUG
    
    // Handle quotes with proper state preservation
    if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
    {
        fprintf(stderr, "[TOK_DBG] Calling tokenize_quote\n");
        // Restore adjacency state before quote processing
        ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
        tokenize_quote(input, vars);
        return (1);
    }
    
    // Handle variable expansion with proper state preservation
    if (input[vars->pos] == '$' && !vars->quote_depth)
    {
        fprintf(stderr, "[TOK_DBG] Calling tokenize_expan\n");
        // Restore adjacency state before expansion processing
        ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
        tokenize_expan(input, vars);
        return (1);
    }
    
    // Handle operator tokens
    if (is_operator_token(token_type))
    {
        fprintf(stderr, "[TOK_DBG] Handling Operator Token: Type=%d\n", token_type);
        handle_text(input, vars);
        
        // Special handling for heredoc
        if (token_type == TYPE_HEREDOC)
        {
            fprintf(stderr, "[TOK_DBG] Operator is HEREDOC (<<)\n");
            debug_token_creation("handle_token:heredoc", "<<", TYPE_HEREDOC, vars);
            maketoken("<<", TYPE_HEREDOC, vars);
            vars->pos += 2;
            vars->start = vars->pos;
            *heredoc_expecting_delim = 1;
            fprintf(stderr, "[TOK_DBG] Set ExpectDelim=1. Pos=%d, Start=%d\n", 
                   vars->pos, vars->start); // DEBUG
        }
        else
        {
            fprintf(stderr, "[TOK_DBG] Handling other operator (calling process_operator_char)\n"); // DEBUG
            if (!process_operator_char(input, &vars->pos, vars))
                return (0);
        }
        
        vars->next_flag = 1;
        return (1);
    }
    
    // Handle whitespace
    if (ft_isspace(input[vars->pos]))
    {
        fprintf(stderr, "[TOK_DBG] Calling tokenize_white\n"); // DEBUG
        tokenize_white(input, vars);
        return (1);
    }
    
    // Just a regular character
    fprintf(stderr, "[TOK_DBG] Regular char, advancing pos. Pos=%d -> %d\n", 
           vars->pos, vars->pos + 1); // DEBUG
    return (1);
}

/*
Finalize tokenization process
- Check for unterminated heredoc
- Process any remaining text
- Clean up and return status
Returns:
- 1 on success
- 0 on error
*/
static int finalize_tokenization(char *input, t_vars *vars, int heredoc_expecting_delim)
{
    fprintf(stderr, "[TOK_DBG] Loop finished. Pos=%d, Start=%d\n", 
           vars->pos, vars->start); // DEBUG
    
    // Check for unterminated heredoc
    if (heredoc_expecting_delim)
    {
        tok_syntax_error_msg("newline", vars);
        return (0);
    }
    
    // Process any remaining text
    fprintf(stderr, "[TOK_DBG] Processing final text chunk if any (Start=%d, Pos=%d)\n", 
           vars->start, vars->pos); // DEBUG
    handle_text(input, vars);
    
    fprintf(stderr, "[TOK_DBG] improved_tokenize: END\n");
    return (1);
}

/*
Tokenizes input string. Calls delimiter validation when << is found.
Returns 1 on success, 0 on failure (syntax error or malloc error).
*/
int improved_tokenize(char *input, t_vars *vars)
{
    int heredoc_expecting_delim;

    initialize_tokenizer(vars);
    heredoc_expecting_delim = 0;
    
    fprintf(stderr, "[TOK_DBG] improved_tokenize: START, input='%.*s...'\n", 20, input); // DEBUG
    
    while (input && input[vars->pos])
    {
        vars->next_flag = 0;
        
        fprintf(stderr, "[TOK_DBG] Loop Top: Pos=%d, Char='%c', Start=%d, ExpectDelim=%d\n",
            vars->pos, input[vars->pos] ? input[vars->pos] : '0', vars->start, heredoc_expecting_delim); // DEBUG
        
        if (heredoc_expecting_delim)
        {
            int result = process_heredoc_delimiter(input, vars, &heredoc_expecting_delim);
            if (result == 0)
                return (0);  // Error occurred
            if (result == 1)
                continue;    // Success with next_flag set
            // result == 2 means continue processing normally
        }
        
        if (!vars->next_flag && !heredoc_expecting_delim)
        {
            if (!handle_token(input, vars, &heredoc_expecting_delim))
                return (0);
        }
        
        if (vars->next_flag)
        {
            fprintf(stderr, "[TOK_DBG] next_flag is set, continuing loop. Pos=%d, Start=%d\n", 
                vars->pos, vars->start); // DEBUG
            continue;
        }
        
        if (!heredoc_expecting_delim)
            vars->pos++;
    }
    
    return finalize_tokenization(input, vars, heredoc_expecting_delim);
}

