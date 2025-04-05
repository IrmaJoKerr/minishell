/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 15:10:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes operators and surrounding text in command input.
- Handles text before operators by creating string/command tokens.
- Detects and processes both single and double operators.
- Updates token start/end positions for proper tokenization.
Returns:
- New position index after operator processing.
*/
int	operators(char *input, t_vars *vars)
{
    int			advance;
    t_tokentype	token_type;
    // At the beginning of operators()
	DBG_PRINTF(DEBUG_TOKENIZE, "operators: Initial curr_type=%d, entering with pos=%d\n", 
	vars->curr_type, vars->pos);
    // Handle text before operator
    if (vars->pos > vars->start)
        handle_string(input, vars);
    
    // Get token type and advance value
    token_type = get_token_at(input, vars->pos, &advance);
    
    // Store previous type before updating current
    vars->prev_type = vars->curr_type;
    vars->curr_type = token_type;  // Update current type
    // After setting vars->curr_type
	DBG_PRINTF(DEBUG_TOKENIZE, "operators: Updated curr_type from %d to %d\n", 
	vars->prev_type, vars->curr_type);
    // Process token based on its type
    if (token_type != 0)
    {
        if (advance == 2)
            return handle_double_operator(input, vars);
        else if (advance == 1)
            return handle_single_operator(input, vars);
    }
    
    return vars->pos;
}

/* 
Checks if a token is an operator (pipe or redirection).
Returns 1 if token is an operator, 0 otherwise.
*/
int is_operator_token(t_tokentype type)
{
    if (type == TYPE_PIPE)
        return (1);
    if (type == TYPE_IN_REDIRECT)
        return (1);
    if (type == TYPE_OUT_REDIRECT)
        return (1);
    if (type == TYPE_APPEND_REDIRECT)
        return (1);
    if (type == TYPE_HEREDOC)
        return (1);
    
    return (0);
}

/*
Creates string token for text preceding an operator.
- Checks if current position contains operator character.
- Creates a string token from token_start to current position.
- Updates token tracking in vars structure.
Returns:
- Current position (unchanged).
Works with operators() for text segment tokenization.
*/
void handle_string(char *input, t_vars *vars)
{
    char *token;
    int advance;
    t_tokentype token_type;
    
    if (vars->pos > vars->start)
    {
        token = ft_substr(input, vars->start, vars->pos - vars->start);
        if (!token)
            return;
        
        // Determine token type based on the input
        token_type = get_token_at(token, 0, &advance);
        if (token_type == 0)
        {
            // Pass the token content to help determine type
            set_token_type(vars, token);
        }
        else
        {
            vars->curr_type = token_type;
        }
        
        DBG_PRINTF(DEBUG_TOKENIZE, "Creating token from '%s', type=%d (%s)\n", 
                token, vars->curr_type, get_token_str(vars->curr_type));
        
        maketoken_with_type(token, vars->curr_type, vars);
        free(token);
        
        vars->start = vars->pos;
    }
}

/* 
Checks if character at position is a single-character token
Returns the token type enum value, 0 if not a special token
Handles: ', ", <, >, $, |
*/
int	is_single_token(char *input, int pos, int *advance)
{
	t_tokentype	token_type;

	token_type = 0;
	*advance = 0;
	DBG_PRINTF(DEBUG_TOKENIZE, "is_single_token: Checking char '%c' at pos %d\n", 
		input[pos], pos);
	if (!input || !input[pos])
		return (token_type);
	if (input[pos] == '\'')
		token_type = TYPE_SINGLE_QUOTE;
	else if (input[pos] == '\"')
		token_type = TYPE_DOUBLE_QUOTE;
	else if (input[pos] == '<')
		token_type = TYPE_IN_REDIRECT;
	else if (input[pos] == '>')
		token_type = TYPE_OUT_REDIRECT;
	else if (input[pos] == '$')
		token_type = TYPE_EXPANSION;
	else if (input[pos] == '|')
		token_type = TYPE_PIPE;
	if (token_type != 0)
	{
		*advance = 1;
		DBG_PRINTF(DEBUG_TOKENIZE, "is_single_token: Found token type %d (%s) for '%c'\n", 
			token_type, get_token_str(token_type), input[pos]);
	}
	return (token_type);
}

/* 
Checks if characters at position form a double-character token
Returns the token type enum value, 0 if not a double token
Handles: >>, <<, $?
*/
int	is_double_token(char *input, int pos, int *advance)
{
	t_tokentype	token_type;

	token_type = 0;
	*advance = 0;
	DBG_PRINTF(DEBUG_TOKENIZE, "is_double_token: Checking chars '%c%c' at pos %d\n", 
		input[pos], input[pos+1], pos);
	if (!input || !input[pos] || !input[pos + 1])
		return (token_type);
	if (input[pos] == '>' && input[pos + 1] == '>')
		token_type = TYPE_APPEND_REDIRECT;
	else if (input[pos] == '<' && input[pos + 1] == '<')
		token_type = TYPE_HEREDOC;
	else if (input[pos] == '$' && input[pos + 1] == '?')
		token_type = TYPE_EXIT_STATUS;
	if (token_type != 0)
	{
        *advance = 2;
        DBG_PRINTF(DEBUG_TOKENIZE, "is_double_token: Found token type %d (%s) for '%c%c'\n", 
                  token_type, get_token_str(token_type), input[pos], input[pos+1]);
    }
	return (token_type);
}

/* 
Master function to get token type at current position
Checks double tokens first, then single tokens
Returns token type and updates position via advance parameter
*/
t_tokentype	get_token_at(char *input, int pos, int *advance)
{
	t_tokentype	token_type;

	token_type = 0;
	*advance = 0;
	
	DBG_PRINTF(DEBUG_TOKENIZE, "get_token_at: Checking position %d: '%c'\n", 
		pos, input[pos]);
	// Try to match double-character tokens first
	token_type = is_double_token(input, pos, advance);
	if (token_type != 0)
	{
        DBG_PRINTF(DEBUG_TOKENIZE, "get_token_at: Found double token type %d, advance %d\n", 
            token_type, *advance);
        return token_type;
    }
	// Then try single-character tokens
	token_type = is_single_token(input, pos, advance);
	if (token_type != 0)
	{
        DBG_PRINTF(DEBUG_TOKENIZE, "get_token_at: Found single token type %d, advance %d\n", 
            token_type, *advance);
        return token_type;
    }
	// If not a special token, treat as part of a string/command
	*advance = 1;
	DBG_PRINTF(DEBUG_TOKENIZE, "get_token_at: No special token found, advance 1\n");
    return (0); // Will be classified as word/command later
}

/*
Processes single-character operators (|, >, <).
Creates token with the provided token type.
Updates position tracking.
Returns:
- Position after operator (i+1).
- Unchanged position if token creation fails.
*/
int handle_single_operator(char *input, t_vars *vars)
{
    char *token;
    DBG_PRINTF(DEBUG_TOKENIZE, "handle_single_operator: Creating token for '%c', type=%d\n", 
		input[vars->pos], vars->curr_type);
    token = ft_substr(input, vars->pos, 1);
    if (!token)
        return vars->pos;
    
    maketoken_with_type(token, vars->curr_type, vars);
    free(token);
    
    vars->pos++;
    vars->start = vars->pos;
    vars->prev_type = vars->curr_type;
    return vars->pos;
}
 
/*
Processes double-character operators (>>, <<).
Creates token with the provided token type.
Updates position tracking.
Returns:
 - Position after operator (i+advance).
 - Unchanged position if token creation fails.
*/
int	handle_double_operator(char *input, t_vars *vars)
{
    char	*token;
    int		advance;
    
    advance = 2;  // Double operators are always 2 characters
    
    token = ft_substr(input, vars->pos, advance);
    if (!token)
        return vars->pos;
    
    maketoken_with_type(token, vars->curr_type, vars);
    free(token);
    
    vars->pos += advance;
    vars->start = vars->pos;
    vars->prev_type = vars->curr_type;
    return vars->pos;
}
