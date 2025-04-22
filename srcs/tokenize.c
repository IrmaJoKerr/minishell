/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 16:24:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets appropriate token type based on position and context
Determines if current token should be a command or argument
Updates vars->curr_type accordingly
*/
void	set_token_type(t_vars *vars, char *input)
{
	int			moves;
	t_tokentype	special_type;
	// Save previous type
	special_type = 0;
	vars->prev_type = vars->curr_type;
	if (input && input[0] == '$')
	{
		vars->curr_type = TYPE_EXPANSION;
		return;
	}
	if (input && *input)
	{
		special_type = get_token_at(input, 0, &moves);
		if (special_type != 0)
		{
			vars->curr_type = special_type;
			return ;
		}
	}
	if (!vars->head || vars->prev_type == TYPE_PIPE)
		vars->curr_type = TYPE_CMD;
	else
		vars->curr_type = TYPE_ARGS;
}

void	maketoken_with_type(char *token, t_tokentype type, t_vars *vars)
{
    t_node	*node;
	t_node	*check;
    int		node_freed;
	int		found_in_list;

	found_in_list = 0;
	check = vars->head;
    node = initnode(type, token);
    if (!node)
        return ;
    node_freed = build_token_linklist(vars, node);
    if (!node_freed) 
    {
        if (node != vars->head && node != vars->current)
        {
            found_in_list = 0;
            check = vars->head;
            while (check && !found_in_list)
            {
                if (check == node)
                    found_in_list = 1;
                check = check->next;
            }
            if (!found_in_list)
            {
                fprintf(stderr, "DEBUG[maketoken_with_type]: Freeing orphaned node not in list. "
                        "node=%p, type=%d, content='%s'\n",
                        (void*)node, node->type, 
                        (node->args && node->args[0]) ? node->args[0] : "NULL");
                free_token_node(node);
            }
        }
    }
}

// int is_adjacent_token(char *input, int pos) // POSSIBLE CANDIDATE FOR DEPRECATION
// {
// 	if (pos <= 0)
// 		return (0);
	
// 	// Don't join if previous char is an operator
// 	if (input[pos - 1] == '<' || input[pos - 1] == '>' || 
// 		input[pos - 1] == '|' || input[pos - 1] == ';')
// 		return (0);
		
// 	// Don't join if current position is an operator
// 	if ((size_t)pos < ft_strlen(input) && 
// 		(input[pos] == '<' || input[pos] == '>' || 
// 		 input[pos] == '|' || input[pos] == ';'))
// 		return (0);
	
// 	// Check if previous character is not whitespace
// 	if (ft_isspace(input[pos - 1]))
// 		return (0);
// 	// It's adjacent if previous char is not whitespace or operator
// 	return (1);
// }

// /*
// Creates an expansion token from input
// - Identifies $ variables and creates TYPE_EXPANSION tokens
// - Handles $? as special case (exit status)
// - Doesn't perform expansion - just identifies tokens
// Returns 1 if expansion token was created, 0 otherwise
// */

// int init_quote_processing(char *input, int *i, int *is_adjacent, char *quote_char)
// {
// 	int	quote_type;
	
// 	fprintf(stderr, "DEBUG: Entering init_quote_processing, quote char: '%c'\n", *quote_char);
//     *quote_char = input[*i];
//     *is_adjacent = is_adjacent_token(input, *i);
//     if (*quote_char == '\'')
//         quote_type = TYPE_SINGLE_QUOTE;
//     else
//         quote_type = TYPE_DOUBLE_QUOTE;
//     return (quote_type);
// }


/*
Processes operator characters in the input string.
- Identifies redirection operators and pipe.
- Creates tokens for these operators.
- Updates position past the operator.
Returns:
1 if operator was processed, 0 otherwise.

Example: For input "cmd > file"
- Processes the '>' operator
- Creates redirect token
- Returns 1 to indicate operator was handled
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
	if (moves == 2) {
		handle_double_operator(input, vars);
	}
	else
	{
		handle_single_operator(input, vars);
	}
	return (1);
}

/*
Process any accumulated text before a special character.
Creates a token from the text between vars->start and vars->pos.
*/
// static void handle_text(char *input, t_vars *vars)
// {
// 	char *token_preview;

// 	if (vars->pos > vars->start)
// 	{
// 		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
// 		set_token_type(vars, token_preview);
// 		handle_string(input, vars);
// 		free(token_preview);
// 	}
// }
void	handle_text(char *input, t_vars *vars)
{
    char *token_preview;
	
	token_preview = NULL; 
    fprintf(stderr, "[TOK_DBG] handle_text: Called. Start=%d, Pos=%d\n", vars->start, vars->pos); // DEBUG
    if (vars->pos > vars->start)
    {
        token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
        fprintf(stderr, "[TOK_DBG] handle_text: Extracted text = '%s'\n", token_preview ? token_preview : "NULL"); // DEBUG
        set_token_type(vars, token_preview);
        fprintf(stderr, "[TOK_DBG] handle_text: Set token type to %d. Calling handle_string.\n", vars->curr_type); // DEBUG
        handle_string(input, vars); // handle_string uses vars->start and vars->pos
        if (token_preview)
		{ // Check before freeing
            free(token_preview);
        }
        vars->start = vars->pos; // Reset start after processing text
        fprintf(stderr, "[TOK_DBG] handle_text: Reset Start to %d\n", vars->start); // DEBUG
    }
	else
	{
        fprintf(stderr, "[TOK_DBG] handle_text: No text to process (Pos <= Start).\n"); // DEBUG
    }
}

// static	void imp_tok_quote(char *input, t_vars *vars)
// {
// 	int	saved_pos;
	
//     fprintf(stderr, "DEBUG[imp_tok_quote]: ENTER with pos=%d, start=%d, char='%c'\n", 
//         vars->pos, vars->start, input[vars->pos]);
//     handle_text(input, vars);
//     // Save current position to restore if process_quote_char fails
//     saved_pos = vars->pos;
    
//     if (process_quote_char(input, vars))
//     {
//         fprintf(stderr, "DEBUG[imp_tok_quote]: After process_quote - pos=%d, start=%d\n", 
//             vars->pos, vars->start);
//         fprintf(stderr, "DEBUG[imp_tok_quote]: Adjacency state: left=%d, right=%d\n",
//             vars->adj_state[0], vars->adj_state[1]);
//         vars->next_flag = 1;
//         // Right adjacency handling now happens in process_quote_char
//     }
//     else
//     {
//         fprintf(stderr, "DEBUG[imp_tok_quote]: process_quote_char returned 0 (failed)\n");
//         vars->pos = saved_pos; // Restore position on failure
//     }
//     fprintf(stderr, "DEBUG[imp_tok_quote]: EXIT - next_flag=%d\n", vars->next_flag);
// }
void	imp_tok_quote(char *input, t_vars *vars)
{
    fprintf(stderr, "DEBUG[imp_tok_quote]: ENTER with pos=%d, char='%c', prev_type=%d\n", 
        vars->pos, input[vars->pos], vars->prev_type);
    // In imp_tok_quote()
	fprintf(stderr, "DEBUG: imp_tok_quote: Previous token type=%d, pos=%d\n",
        vars->prev_type, vars->pos);
    // Check if this quoted string follows a redirection operator
    int is_redir_target = 0;
    if (vars->prev_type == TYPE_IN_REDIRECT || vars->prev_type == TYPE_OUT_REDIRECT ||
        vars->prev_type == TYPE_APPEND_REDIRECT || vars->prev_type == TYPE_HEREDOC)
	{
        is_redir_target = 1;
        fprintf(stderr, "DEBUG[imp_tok_quote]: Quote follows redirection operator\n");
    }
    
    handle_text(input, vars);
    int saved_pos = vars->pos;
    
    if (process_quote_char(input, vars, is_redir_target))
	{
        fprintf(stderr, "DEBUG[imp_tok_quote]: Quote processed successfully\n");
        vars->next_flag = 1;
    } else
	{
        fprintf(stderr, "DEBUG[imp_tok_quote]: Quote processing failed\n");
        vars->pos = saved_pos;
    }
}

void	imp_tok_expan(char *input, t_vars *vars)
{
    fprintf(stderr, "DEBUG[imp_tok_expan]: ENTER with pos=%d, start=%d, char='%c', text='%.10s...'\n", 
            vars->pos, vars->start, input[vars->pos], input + vars->pos);
    
    // Check if we have any text accumulated from start to pos
    if (vars->pos > vars->start) {
        fprintf(stderr, "DEBUG[imp_tok_expan]: Accumulated text from %d to %d: '%.*s'\n", 
                vars->start, vars->pos, vars->pos - vars->start, 
                input + vars->start);
    }
    
    handle_text(input, vars);
    
    if (make_exp_token(input, vars))
        vars->next_flag = 1;
        
    fprintf(stderr, "DEBUG[imp_tok_expan]: EXIT with pos=%d, start=%d, next_flag=%d\n", 
            vars->pos, vars->start, vars->next_flag);
}

// /*
// Handle operator tokens in the input.
// Processes operators like >, <, |, etc.
// Sets next_flag if processing should continue from the loop.
// */
// static void imp_tok_operat(char *input, t_vars *vars)
// {
// 	handle_text(input, vars);
// 	if (process_operator_char(input, &vars->pos, vars))
// 		vars->next_flag = 1;
// }

/*
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
// static void imp_tok_white(char *input, t_vars *vars)
// {
// 	handle_text(input, vars);
// 	while (input[vars->pos] && input[vars->pos] <= ' ')
// 		vars->pos++;
// 	vars->start = vars->pos;
// 	vars->next_flag = 1;
// }
void	imp_tok_white(char *input, t_vars *vars)
{
    fprintf(stderr, "[TOK_DBG] imp_tok_white: Called. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
    handle_text(input, vars); // Process any text before whitespace
    int initial_pos = vars->pos; // DEBUG
    while (input[vars->pos] && input[vars->pos] <= ' ')
        vars->pos++;
    fprintf(stderr, "[TOK_DBG] imp_tok_white: Skipped whitespace from %d to %d.\n", initial_pos, vars->pos); // DEBUG
    vars->start = vars->pos; // Start next token after whitespace
    vars->next_flag = 1;
    fprintf(stderr, "[TOK_DBG] imp_tok_white: Set Start=%d, next_flag=1.\n", vars->start); // DEBUG
}

void handle_right_adj(char *input, t_vars *vars)
{
    fprintf(stderr, "DEBUG[handle_right_adj]: ENTER - pos=%d, start=%d\n",
            vars->pos, vars->start);
    
    if (vars->pos <= vars->start)
    {
        fprintf(stderr, "DEBUG[handle_right_adj]: No text to extract (pos <= start)\n");
        return;
    }
    
    char *adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!adjacent_text)
    {
        fprintf(stderr, "DEBUG[handle_right_adj]: Failed to extract text\n");
        return;
    }
    
    fprintf(stderr, "DEBUG[handle_right_adj]: Extracted '%s'\n", adjacent_text);
    
    if (vars->current && vars->current->args && vars->current->args[0])
    {
        // Find the last argument
        int arg_idx = 0;
        while (vars->current->args[arg_idx + 1])
            arg_idx++;
        
        fprintf(stderr, "DEBUG[handle_right_adj]: Current token last arg='%s'\n",
                vars->current->args[arg_idx]);
        
        // Join with the last argument, not the first
        char *joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
        if (joined)
        {
            fprintf(stderr, "DEBUG[handle_right_adj]: Joined result='%s'\n", joined);
            free(vars->current->args[arg_idx]);
            vars->current->args[arg_idx] = joined;
        }
        else
        {
            fprintf(stderr, "DEBUG[handle_right_adj]: Join failed\n");
        }
    }
    else
    {
        fprintf(stderr, "DEBUG[handle_right_adj]: No valid current token to join with\n");
    }
    
    free(adjacent_text);
    fprintf(stderr, "DEBUG[handle_right_adj]: EXIT\n");
}

// /* 
// Tokenizes input string with improved quote handling.
// Creates tokens for commands, args, quotes, and operators.
// */
// int improved_tokenize(char *input, t_vars *vars)
// {
// 	int			moves;
// 	t_tokentype	token_type;

// 	vars->pos = 0;
// 	vars->start = 0;
// 	vars->quote_depth = 0;
// 	while (input && input[vars->pos])
// 	{
// 		vars->next_flag = 0;
// 		token_type = get_token_at(input, vars->pos, &moves);
// 		fprintf(stderr, "DEBUG[tokenize]: At pos %d, char '%c', token_type=%d, moves=%d\n", 
// 			vars->pos, input[vars->pos], token_type, moves);
// 		if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
// 			imp_tok_quote(input, vars);
// 		if (!vars->next_flag && input[vars->pos] == '$' && !vars->quote_depth)
// 			imp_tok_expan(input, vars);
// 		if (!vars->next_flag && is_operator_token(get_token_at(input
// 			,vars->pos, &moves)))
// 			imp_tok_operat(input, vars);
// 		if (!vars->next_flag && input[vars->pos] && input[vars->pos] <= ' ')
// 			imp_tok_white(input, vars);
// 		if (vars->next_flag)
// 			continue ;
// 		vars->pos++;
// 	}
// 	handle_text(input, vars);
// 	DBG_PRINTF(DEBUG_TOKENIZE, "improved_tokenize: Token type=%d, content='%s'\n", 
// 			  vars->curr_type, (vars->current && vars->current->args) ? 
// 			  vars->current->args[0] : "(null)");
// 	DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
// 	debug_token_list(vars);
// 	return(1);
// }
/*
MODIFIED: Tokenizes input string. Calls delimiter validation when << is found.
Returns 1 on success, 0 on failure (syntax error or malloc error).
*/
// int improved_tokenize(char *input, t_vars *vars)
// {
//     int			moves;
//     t_tokentype	token_type;
//     int			heredoc_expecting_delim = 0; // Local flag for state

//     vars->pos = 0;
//     vars->start = 0;
//     vars->quote_depth = 0;
//     // Clear previous delimiter info at the start of tokenization
//     if (vars->pipes->heredoc_delim) {
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//     }
//     vars->pipes->hd_expand = 0; // Default

//     while (input && input[vars->pos])
//     {
//         vars->next_flag = 0;
//         token_type = get_token_at(input, vars->pos, &moves);
//         fprintf(stderr, "DEBUG[tokenize]: Pos %d, Char '%c', Type %d, Moves %d, ExpectDelim %d\n",
//             vars->pos, input[vars->pos], token_type, moves, heredoc_expecting_delim);

//         // If expecting a delimiter, the next non-whitespace token MUST be it.
//         if (heredoc_expecting_delim && !ft_isspace(input[vars->pos]))
//         {
//             handle_text(input, vars); // Process any preceding text (shouldn't be any if whitespace skipped)
//             vars->start = vars->pos;
//             // Find end of the raw delimiter word
//             while (input[vars->pos] && !ft_isspace(input[vars->pos]) &&
//                    !is_operator_token(get_token_at(input, vars->pos, &moves))) {
//                 vars->pos++;
//             }
//             if (vars->pos == vars->start) { // Found whitespace/operator immediately
//                 fprintf(stderr, "bleshell: syntax error near unexpected token `newline' or operator\n");
//                 vars->error_code = ERR_SYNTAX;
//                 return (0); // Missing delimiter word
//             }
//             char *raw_delimiter_str = ft_substr(input, vars->start, vars->pos - vars->start);
//             if (!raw_delimiter_str) { /* Malloc error */ vars->error_code = ERR_DEFAULT; return (0); }

//             // *** Validate and Store Delimiter ***
//             if (!is_valid_delim(raw_delimiter_str, vars)) {
//                 free(raw_delimiter_str);
//                 // is_valid_delim sets error code and prints message
//                 return (0); // Validation failed
//             }

//             // Make the delimiter token (e.g., TYPE_ARGS or a new TYPE_DELIMITER)
//             // Store the RAW delimiter string in the token for AST/debugging.
//             maketoken_with_type(raw_delimiter_str, TYPE_ARGS, vars); // Using TYPE_ARGS for now
//             free(raw_delimiter_str); // maketoken should copy it

//             heredoc_expecting_delim = 0; // Reset flag
//             vars->start = vars->pos;     // Reset start for next token
//             vars->next_flag = 1;         // Continue loop
//         }

//         // Handle regular tokens only if not expecting delimiter right now
//         if (!vars->next_flag && !heredoc_expecting_delim)
//         {
//             if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
//                 imp_tok_quote(input, vars);
//             else if (input[vars->pos] == '$' && !vars->quote_depth)
//                 imp_tok_expan(input, vars);
//             else if (is_operator_token(token_type))
//             {
//                 handle_text(input, vars); // Process text before operator
//                 if (token_type == TYPE_HEREDOC) {
//                     maketoken_with_type("<<", TYPE_HEREDOC, vars);
//                     vars->pos += 2; // Move past <<
//                     vars->start = vars->pos;
//                     heredoc_expecting_delim = 1; // Expect delimiter next
//                 } else {
//                     // Handle other operators (<, >, >>, |)
//                     if (!process_operator_char(input, &vars->pos, vars)) {
//                         // Error handling if process_operator_char fails
//                         return (0);
//                     }
//                 }
//                 vars->next_flag = 1;
//             }
//             else if (ft_isspace(input[vars->pos]))
//                 imp_tok_white(input, vars);
//         }

//         // If a handler set next_flag, or we just processed the delimiter
//         if (vars->next_flag)
//             continue ;

//         // If not expecting delimiter and no handler took action, advance position
//         if (!heredoc_expecting_delim)
//             vars->pos++;
//         else // If expecting delimiter, skip whitespace until we find it or EOL
//             while (input[vars->pos] && ft_isspace(input[vars->pos]))
//                 vars->pos++;
//     }

//     // After loop: Check if we were still expecting a delimiter
//     if (heredoc_expecting_delim) {
//         fprintf(stderr, "bleshell: syntax error near unexpected token `newline'\n");
//         vars->error_code = ERR_SYNTAX;
//         return (0); // Missing delimiter at end of input
//     }

//     // Process any remaining text
//     handle_text(input, vars);

//     DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
//     debug_token_list(vars); // Keep debug output
//     return (1); // Success
// }
int improved_tokenize(char *input, t_vars *vars)
{
    int			moves;
    t_tokentype	token_type;
    int			heredoc_expecting_delim = 0; // Local flag for state

    vars->pos = 0;
    vars->start = 0;
    vars->quote_depth = 0;
    // Clear previous delimiter info at the start of tokenization
    if (vars->pipes->heredoc_delim) {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    vars->pipes->hd_expand = 0; // Default
    fprintf(stderr, "[TOK_DBG] improved_tokenize: START, input='%.*s...'\n", 20, input); // DEBUG

    while (input && input[vars->pos])
    {
        vars->next_flag = 0;
        token_type = get_token_at(input, vars->pos, &moves);
        fprintf(stderr, "[TOK_DBG] Loop Top: Pos=%d, Char='%c', Start=%d, ExpectDelim=%d\n",
            vars->pos, input[vars->pos] ? input[vars->pos] : '0', vars->start, heredoc_expecting_delim); // DEBUG

        // If expecting a delimiter, skip leading whitespace first
        if (heredoc_expecting_delim) {
            fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Current char '%c'\n", input[vars->pos]); // DEBUG
            if (ft_isspace(input[vars->pos])) {
                fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Skipping whitespace at pos %d\n", vars->pos); // DEBUG
                vars->pos++;
                vars->start = vars->pos; // Keep start updated while skipping
                continue; // Go back to top of loop to re-evaluate
            }
            // Found non-whitespace, proceed to extract delimiter
            fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Found non-whitespace '%c' at pos %d. Processing delimiter.\n", input[vars->pos], vars->pos); // DEBUG
            // handle_text(input, vars); // Should not be needed here if whitespace was skipped correctly
            vars->start = vars->pos; // Ensure start is at the beginning of the delimiter
            fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Start set to %d\n", vars->start); // DEBUG

            // Find end of the raw delimiter word (quotes are part of it initially)
            // int delim_start_pos = vars->pos; // REMOVED - Unused variable
            while (input[vars->pos] && !ft_isspace(input[vars->pos]) &&
                   !is_operator_token(get_token_at(input, vars->pos, &moves))) { // REMOVED backslash from previous line
                vars->pos++;
            }
             fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Loop finished. Pos=%d (Delimiter end)\n", vars->pos); // DEBUG

            if (vars->pos == vars->start) { // Found whitespace/operator immediately
                tok_syntax_error_msg("newline", vars); // Use "newline" as the most likely case
            	return (0); // Missing delimiter word
            }
            char *raw_delimiter_str = ft_substr(input, vars->start, vars->pos - vars->start);
            if (!raw_delimiter_str) { /* Malloc error */ vars->error_code = ERR_DEFAULT; return (0); }
            fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Raw delimiter string = '%s'\n", raw_delimiter_str); // DEBUG

            // *** Validate and Store Delimiter ***
            if (!is_valid_delim(raw_delimiter_str, vars)) {
                fprintf(stderr, "[TOK_DBG] Delimiter Validation FAILED for '%s'\n", raw_delimiter_str); // DEBUG
                free(raw_delimiter_str);
                return (0); // Validation failed
            }
             fprintf(stderr, "[TOK_DBG] Delimiter Validation SUCCEEDED for '%s'. Stored: '%s', Expand=%d\n",
                     raw_delimiter_str, vars->pipes->heredoc_delim, vars->pipes->hd_expand); // DEBUG

            // Make the delimiter token (store the RAW delimiter string)
            maketoken_with_type(raw_delimiter_str, TYPE_ARGS, vars);
            free(raw_delimiter_str); // maketoken should copy it

            heredoc_expecting_delim = 0; // Reset flag
            fprintf(stderr, "[TOK_DBG] Delimiter Processed: ExpectDelim reset to 0.\n"); // DEBUG
            vars->start = vars->pos;     // Reset start for next token
            vars->next_flag = 1;         // Continue loop
        }

        // Handle regular tokens only if not expecting delimiter right now
        if (!vars->next_flag && !heredoc_expecting_delim)
        {
             fprintf(stderr, "[TOK_DBG] Regular Token Handling: Pos=%d, Char='%c', Type=%d\n", vars->pos, input[vars->pos], token_type); // DEBUG
            if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE) {
                 fprintf(stderr, "[TOK_DBG] Calling imp_tok_quote\n"); // DEBUG
                imp_tok_quote(input, vars);
            }
            else if (input[vars->pos] == '$' && !vars->quote_depth) {
                 fprintf(stderr, "[TOK_DBG] Calling imp_tok_expan\n"); // DEBUG
                imp_tok_expan(input, vars);
            }
            else if (is_operator_token(token_type))
            {
                 fprintf(stderr, "[TOK_DBG] Handling Operator Token: Type=%d\n", token_type); // DEBUG
                handle_text(input, vars); // Process text before operator
                if (token_type == TYPE_HEREDOC) {
                    fprintf(stderr, "[TOK_DBG] Operator is HEREDOC (<<)\n"); // DEBUG
                    maketoken_with_type("<<", TYPE_HEREDOC, vars);
                    vars->pos += 2; // Move past <<
                    vars->start = vars->pos;
                    heredoc_expecting_delim = 1; // Expect delimiter next
                    fprintf(stderr, "[TOK_DBG] Set ExpectDelim=1. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
                } else {
                    fprintf(stderr, "[TOK_DBG] Handling other operator (calling process_operator_char)\n"); // DEBUG
                    if (!process_operator_char(input, &vars->pos, vars)) {
                        return (0);
                    }
                }
                vars->next_flag = 1;
            }
            else if (ft_isspace(input[vars->pos])) {
                 fprintf(stderr, "[TOK_DBG] Calling imp_tok_white\n"); // DEBUG
                imp_tok_white(input, vars);
            }
             else {
                 fprintf(stderr, "[TOK_DBG] Regular char, advancing pos. Pos=%d -> %d\n", vars->pos, vars->pos + 1); // DEBUG
                 // This is where regular characters accumulate before handle_text is called
             }
        }

        // If a handler set next_flag, or we just processed the delimiter
        if (vars->next_flag) {
             fprintf(stderr, "[TOK_DBG] next_flag is set, continuing loop. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
            continue ;
        }

        // If not expecting delimiter and no handler took action, advance position
        if (!heredoc_expecting_delim) {
            // This case handles accumulating regular characters
            vars->pos++;
        }
        // REMOVED the else block for skipping whitespace here, moved logic to the top of the loop

    } // End while loop

    fprintf(stderr, "[TOK_DBG] Loop finished. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
    // After loop: Check if we were still expecting a delimiter
    if (heredoc_expecting_delim)
	{
        tok_syntax_error_msg("newline", vars);
    	return (0); // Missing delimiter at end of input
    }

    // Process any remaining text
    fprintf(stderr, "[TOK_DBG] Processing final text chunk if any (Start=%d, Pos=%d)\n", vars->start, vars->pos); // DEBUG
    handle_text(input, vars);

    DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
    debug_token_list(vars); // Keep debug output
    fprintf(stderr, "[TOK_DBG] improved_tokenize: END\n"); // DEBUG
    return (1); // Success
}

/*
Helper function to link a new token node to the current node.
- Sets the next pointer of the current node to the new node.
- Sets the prev pointer of the new node to the current node.
- Updates the current pointer to the new node.
- Works with build_token_linklist().
*/
void	token_link(t_node *node, t_vars *vars)
{
	vars->current->next = node;
	node->prev = vars->current;
	vars->current = node;
}

/*
Updates the token list with a new node.
- Handles first token as head.
- Otherwise adds to end of list.
- Updates current pointer.

Example: When adding command node
- If first token, sets as head
- Otherwise links to previous token
- Updates current pointer
*/
/*
MODIFIED: Updates the token list, removing heredoc_active logic.
*/
// int	build_token_linklist(t_vars *vars, t_node *node)
// {
//     t_node *cmd_node;

//     if (!vars || !node)
//         return (0);

//     DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Adding node type=%d (%s), content='%s'\n",
//         node->type, get_token_str(node->type),
//         node->args ? node->args[0] : "NULL");

//     // Case 1: First node becomes the head
//     if (!vars->head)
//     {
//         vars->head = node;
//         vars->current = node;
//         DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Set as head %p\n", (void*)node);
//         return (0); // Node added, not freed
//     }

//     // REMOVED: Heredoc specific logic (Case 2 & 3) - Delimiter is now validated
//     // and stored during tokenization loop, not in this linking function.
//     // The << token and its delimiter (as TYPE_ARGS) are linked like normal tokens.

//     // Existing functionality for merging args and handling pipes
//     if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS)
//     {
//         node->type = TYPE_CMD; // Argument after pipe becomes a command
//     }

//     if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD)
//     {
//         // Merge ARG into preceding CMD
//         cmd_node = vars->current;
//         t_node *next_node = node->next; // Store the next node before modifying links

//         DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Merging ARG '%s' into CMD '%s'\n",
//                   node->args[0], cmd_node->args[0]);

//         append_arg(cmd_node, node->args[0], 0); // Assuming quote_type 0 for now

//         // Free the merged ARG node's content (args array)
//         if (node->args)
//         {
//             free(node->args[0]);
//             free(node->args);
//         }
//         node->args = NULL;
//         if (node->arg_quote_type) // Free quote types if they exist
//         {
//             // Assuming arg_quote_type is array of pointers, need to free sub-arrays if allocated
//             // Or just free(node->arg_quote_type) if it's a flat array
//             // Adjust based on your append_arg implementation
//             free(node->arg_quote_type);
//             node->arg_quote_type = NULL;
//         }


//         // Update links carefully before freeing the node itself
//         if (next_node)
//         {
//             cmd_node->next = next_node;
//             next_node->prev = cmd_node;
//         } else {
//             cmd_node->next = NULL; // Merged node was the last one
//             vars->current = cmd_node; // Update current if merged node was last
//         }

//         // Now free the node structure itself
//         free(node); // Use free, not free_token_node if args are already handled

//         DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: After merge, CMD '%s', next=%p\n",
//             cmd_node->args[0], (void*)cmd_node->next);
//         return (1);  // Indicate that the node was freed
//     }
//     else
//     {
//         // Link normally if not merging
//         DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Linking node %p to current %p\n",
//             (void*)node, (void*)vars->current);
//         token_link(node, vars);
//         return (0);  // Node was linked, not freed
//     }
// }
int	build_token_linklist(t_vars *vars, t_node *node)
{
    t_node *cmd_node;

    if (!vars || !node)
        return (0);

    DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Adding node type=%d (%s), content='%s'\n",
        node->type, get_token_str(node->type),
        node->args ? node->args[0] : "NULL");

    // Case 1: First node becomes the head
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Set as head %p\n", (void*)node);
        return (0); // Node added, not freed
    }

    // REMOVED: Heredoc specific logic (Case 2 & 3) - Delimiter is now validated
    // and stored during tokenization loop, not in this linking function.
    // The << token and its delimiter (as TYPE_ARGS) are linked like normal tokens.

    // Existing functionality for merging args and handling pipes
    if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS)
    {
        node->type = TYPE_CMD; // Argument after pipe becomes a command
    }

    if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD)
    {
        // Merge ARG into preceding CMD
        cmd_node = vars->current;
        t_node *next_node = node->next; // Store the next node before modifying links

        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Merging ARG '%s' into CMD '%s'\n",
                  node->args[0], cmd_node->args[0]);

        append_arg(cmd_node, node->args[0], 0); // Assuming quote_type 0 for now

        // Free the merged ARG node's content (args array)
        // NOTE: append_arg copies the string, so we need to free the original node's args.
        // However, free_token_node will handle this. We just need to ensure
        // the pointers are NULLed out here so free_token_node doesn't double-free
        // if append_arg failed or had issues (though append_arg should handle its own failures).
        // The safest approach is to let free_token_node handle the freeing entirely.
        // We remove the manual frees here.

        // if (node->args)
        // {
        //     free(node->args[0]); // This string was duplicated by setup_args
        //     free(node->args);     // This outer array was allocated by setup_args
        // }
        // node->args = NULL; // Prevent double free if free_token_node is called later? No, free_token_node is called now.
        // if (node->arg_quote_type) // Free quote types if they exist
        // {
        //     // This needs ft_free_int_2d, not just free()
        //     // free(node->arg_quote_type); // INCORRECT - Leaks inner arrays
        //     // ft_free_int_2d(node->arg_quote_type, 1); // Assuming only 1 arg in the merged node
        // }
        // node->arg_quote_type = NULL; // Prevent double free? No, free_token_node is called now.


        // Update links carefully before freeing the node itself
        if (next_node)
        {
            cmd_node->next = next_node;
            next_node->prev = cmd_node;
        } else {
            cmd_node->next = NULL; // Merged node was the last one
            vars->current = cmd_node; // Update current if merged node was last
        }

        // Now free the node structure itself AND ITS CONTENTS
        // free(node); // INCORRECT - Leaks args and arg_quote_type
        free_token_node(node); // CORRECT - Frees node and its allocated members

        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: After merge, CMD '%s', next=%p\n",
            cmd_node->args[0], (void*)cmd_node->next);
        return (1);  // Indicate that the node was freed
    }
    else
    {
        // Link normally if not merging
        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Linking node %p to current %p\n",
            (void*)node, (void*)vars->current);
        token_link(node, vars);
        return (0);  // Node was linked, not freed
    }
}

/* Debug function */
void debug_token_list(t_vars *vars)
{
    t_node *current = vars->head;
    int count = 0;
    
    DBG_PRINTF(DEBUG_TOKENIZE, "=== COMPLETE TOKEN LIST WITH POINTERS ===\n");
    while (current)
    {
        DBG_PRINTF(DEBUG_TOKENIZE, "Token %d: type=%d (%s), content='%s'\n", 
                  count, current->type, get_token_str(current->type), 
                  current->args ? current->args[0] : "NULL");
        DBG_PRINTF(DEBUG_TOKENIZE, "       Address: %p, Prev: %p, Next: %p\n", 
                  (void*)current, (void*)current->prev, (void*)current->next);
        
        if (current->args && current->args[1])
        {
            int arg_idx = 1;
            while (current->args[arg_idx])
            {
                DBG_PRINTF(DEBUG_TOKENIZE, "       Arg[%d]: '%s'\n", 
                          arg_idx, current->args[arg_idx]);
                arg_idx++;
            }
        }
        
        current = current->next;
        count++;
    }
    DBG_PRINTF(DEBUG_TOKENIZE, "======================================\n\n");
}
