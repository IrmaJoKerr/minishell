/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:04:06 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 01:56:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Validates if all quotes in a command are properly closed
Returns true if all quotes are balanced, false otherwise
Also updates quote_ctx and quote_depth in vars
*/
int validate_quotes(char *input, t_vars *vars)
{
    int i;
    char in_quote;
     
    i = 0;
    in_quote = 0;
    vars->quote_depth = 0; 
    while (input[i])
    {
        if (!in_quote && (input[i] == '\'' || input[i] == '"'))
        {
            in_quote = input[i];
            if (vars->quote_depth < 32)
            {
                vars->quote_ctx[vars->quote_depth].type = in_quote;
                vars->quote_ctx[vars->quote_depth].start_pos = i;
                vars->quote_ctx[vars->quote_depth].depth = vars->quote_depth + 1;
                vars->quote_depth++;
            }
        }
        else if (in_quote && input[i] == in_quote)
        {
            in_quote = 0;
            vars->quote_depth--; // Pop completed quote
        }
        i++;
    } 
    // If we're still in a quote at the end, quotes are incomplete
    return (in_quote == 0);
}

/*
Prompts for and processes additional input to complete unclosed quotes
Returns completed input or NULL on error
*/
char *complete_quoted_input(t_vars *vars, char *original_input)
{
	char *prompt;
	char *additional_input;
	char *combined_input;
	 
	if (vars->quote_depth == 0)
		return (ft_strdup(original_input)); 
	// Create appropriate prompt based on quote type
	if (vars->quote_ctx[vars->quote_depth - 1].type == '\'')
		prompt = "SQUOTE> ";
	else
		prompt = "DQUOTE> "; 
	// Get additional input
	additional_input = readline(prompt); 
	if (!additional_input)
		return (NULL);
 
	// Add to history if not empty
	if (additional_input && *additional_input)
		add_history(additional_input); 
	// Combine inputs - use your existing append_input function
	combined_input = append_input(original_input, additional_input);
	free(additional_input); 
	if (!combined_input)
		return (NULL); 
	// Check if this resolved all quotes
	if (!validate_quotes(combined_input, vars))
	{
		// Still have unclosed quotes, recursively get more input
		char *temp = combined_input;
		combined_input = complete_quoted_input(vars, temp);
		free(temp);
	} 
	return (combined_input);
}
 
/*
Creates a token for quoted content using the context info from vars struct
- Directly uses the quote context information
- Makes the token with appropriate type based on quote character
*/
void make_quote_token(char *input, t_vars *vars)
{
    char *quoted_token;
    
    if (vars->quote_depth <= 0)
        return;
    
    quoted_token = ft_substr(input, 
                             vars->quote_ctx[vars->quote_depth - 1].start_pos,
                             vars->pos - vars->quote_ctx[vars->quote_depth - 1].start_pos);
    
    if (quoted_token)
    {
        vars->prev_type = vars->curr_type;
        
        if (vars->quote_ctx[vars->quote_depth - 1].type == '"')
            vars->curr_type = TYPE_DOUBLE_QUOTE;
        else
            vars->curr_type = TYPE_SINGLE_QUOTE;
        
        maketoken_with_type(quoted_token, vars->curr_type, vars);
        free(quoted_token);
    }
    
    vars->start = vars->pos;
}

/*
Processes quotes in input by directly updating quote context in the vars struct
- Records quote information in the context array
- Finds matching closing quote
- Creates a token for quoted content when quote is closed
*/
void handle_quotes(char *input, t_vars *vars)
{
    // Process any content before the quote
    if (vars->pos > vars->start)
	{
        DBG_PRINTF(DEBUG_QUOTES, "Processing text before quote: '%.*s'\n", 
                  vars->pos - vars->start, input + vars->start);
        handle_string(input, vars);
    }
    // Store quote information directly in the context array
    vars->quote_ctx[vars->quote_depth].type = input[vars->pos];
    vars->quote_ctx[vars->quote_depth].start_pos = vars->pos;
    vars->quote_ctx[vars->quote_depth].depth = vars->quote_depth + 1;
    DBG_PRINTF(DEBUG_QUOTES, "Quote found at pos %d: '%c'\n", 
		vars->pos, input[vars->pos]);
    // Increment depth and move past opening quote
    vars->quote_depth++;
    vars->pos++;
    
    // Find closing quote
    while (input[vars->pos] && input[vars->pos] != vars->quote_ctx[vars->quote_depth - 1].type)
        vars->pos++;
    
    if (input[vars->pos] == vars->quote_ctx[vars->quote_depth - 1].type)
    {
        DBG_PRINTF(DEBUG_QUOTES, "Closing quote found at pos %d\n", vars->pos);
        // Found closing quote
        vars->pos++;  // Skip the closing quote
        vars->quote_depth--;
        
        // Create token for quoted content
        make_quote_token(input, vars);
    }
	else {
        DBG_PRINTF(DEBUG_QUOTES, "No closing quote found!\n");
        // Quote wasn't closed
    }
}

/*
Reads additional input to complete unclosed quotes
Returns completed input or NULL on error
*/
char *fix_open_quotes(char *original_input, t_vars *vars)
{
    char *prompt;
    char *additional_input;
    char *combined_input;
    
    // Check if we need quote completion
    if (!vars->quote_depth)
        return (ft_strdup(original_input));
    
    // Create appropriate prompt based on quote type
    if (vars->quote_ctx[vars->quote_depth - 1].type == '\'')
        prompt = "SQUOTE> ";
    else
        prompt = "DQUOTE> ";
    
    // Get additional input
    additional_input = readline(prompt);
    
    if (!additional_input)
        return (NULL);
    
    // Add to history if not empty
    if (additional_input && *additional_input)
        add_history(additional_input);
    
    // Combine inputs
    combined_input = append_input(original_input, additional_input);
    free(additional_input);
    
    if (!combined_input)
        return (NULL);
    
    // Check if we have resolved all quotes
    if (!validate_quotes(combined_input, vars))
    {
        // Still have unclosed quotes, recursively get more input
        char *temp = combined_input;
        combined_input = fix_open_quotes(temp, vars);
        free(temp);
    }
    
    return (combined_input);
}

/*
Extracts content between quote characters.
- Starts after opening quote character.
- Reads until matching closing quote or end of string.
- Creates substring without quote characters.
- Updates position past the closing quote.
Returns:
- Quoted content as new string.
- NULL if unclosed quote.
Works with handle_quote_token() during tokenization.

Example: For input "Hello 'world'" at position of first quote
- Returns "world" as extracted content
- Updates position to character after closing quote
*/
char	*read_quoted_content(char *input, int *pos, char quote)
{
	int		start;
	char	*content;
	int		input_len;

	if (!input || !pos)
		return (NULL);
	input_len = ft_strlen(input);
	if (*pos >= input_len)
		return (NULL);
	start = *pos;
	while (input[*pos] && input[*pos] != quote)
	{
		if (*pos >= input_len)
			break ;
		(*pos)++;
	}
	if (*pos > input_len)
		*pos = input_len;
	content = ft_substr(input, start, *pos - start);
	if (input[*pos] == quote)
		(*pos)++;
	return (content);
}

/*
Removes outer quotes from a string.
- Checks for matching quotes at start and end.
- Creates new string without the quotes.
- Updates the original string pointer.
Works with process_args_tokens() during argument parsing.

Example: For string "\"hello\""
- Creates new string "hello"
- Frees original string
- Updates pointer to new string
*/
void	strip_quotes(char **str_ptr, char quote_char)
{
	char	*str;
	char	*new_str;
	size_t	len;

	str = *str_ptr;
	if (!str)
		return ;
	len = ft_strlen(str);
	if (len < 2)
		return ;
	if (str[0] == quote_char && str[len - 1] == quote_char)
	{
		new_str = ft_substr(str, 1, len - 2);
		if (new_str)
		{
			free(str);
			*str_ptr = new_str;
		}
	}
}

/*
Removes outer quotes from an argument string.
- Checks for both single and double quotes.
- Creates new string without the quotes.
- Updates the original string pointer.
Works with process_args_tokens() during argument processing.

Example: For argument "'hello'"
- Creates new string "hello"
- Frees original string
- Updates pointer to new string
*/
/* 
Removes outer quotes from an argument string while preserving inner quotes.
- Checks for both single and double quotes at the beginning/end.
- Creates new string without the outer quotes only.
- Updates the original string pointer.
Works with append_arg() during argument processing.
*/
/*
Strips quotes but preserves expansions appropriately
 - For single quotes: Preserves literal contents including $ signs
 - For double quotes: Keeps content for later expansion
*/
/*
 * Processes quotes in argument tokens
 * - Removes the outside quotes but preserves the content
 * - For single quotes: preserves literal contents including $ signs
 * - For double quotes: keeps content for later expansion
 * - Handles adjacent quoted text by joining with previous arg when needed
 */
void process_quotes_in_arg(char **arg)
{
    char *str;
    size_t len;
    char *new_str;
    
    if (!arg || !*arg)
        return;
        
    str = *arg;
    len = ft_strlen(str);
    
    DBG_PRINTF(DEBUG_QUOTES, "process_quotes_in_arg: Processing '%s'\n", str);
    
    // Need at least 2 chars for quotes
    if (len < 2)
        return;
        
    // Check for matching quotes at start and end
    if ((str[0] == '"' && str[len - 1] == '"') || 
        (str[0] == '\'' && str[len - 1] == '\''))
    {
        DBG_PRINTF(DEBUG_QUOTES, "Removing quotes from '%s'\n", str);
        new_str = ft_substr(str, 1, len - 2);
        if (new_str)
        {
            free(*arg);  // Free the string content, not the pointer
            *arg = new_str;
            DBG_PRINTF(DEBUG_QUOTES, "After removing quotes: '%s'\n", *arg);
        }
    }
}

/*
Scans for the ending quote character.
- Searches from current position for matching quote.
- Updates position to the quote if found.
- Provides validation for quoted content.
Returns:
- 1 if closing quote found.
- 0 if not found.
Works with valid_quote_token() for quote validation.

Example: For input "echo 'hello world'"
- When scanning for closing single quote
- Returns 1 when closing quote found
- Updates position to closing quote
*/
int	scan_for_endquote(char *str, int *pos, char quote_char)
{
	int	i;

	i = *pos + 1;
	while (str[i] && str[i] != quote_char)
		i++;
	if (str[i] == quote_char)
	{
		*pos = i;
		return (1);
	}
	return (0);
}

/*
Validates and processes a quoted token.
- Checks if the token has matching quotes.
- Creates appropriate token based on quote type.
- Handles content between quotes.
Works with handle_token() during tokenization.
*/
void	valid_quote_token(char *str, t_vars *vars, int *pos, int start)
{
    char	quote_char;
    char	*token;
    int		end_pos;
    t_tokentype token_type;

    quote_char = str[start];
    end_pos = *pos;
    if (scan_for_endquote(str, &end_pos, quote_char))
    {
        token = ft_substr(str, start + 1, end_pos - start - 1);
        if (token)
        {
            token_type = (quote_char == '"') ? TYPE_DOUBLE_QUOTE : TYPE_SINGLE_QUOTE;
            vars->curr_type = token_type;
            maketoken_with_type(token, token_type, vars);
            free(token);
            *pos = end_pos + 1;
        }
    }
}

/* 
Skip quoted content when tokenizing
Updates position to after the closing quote
Returns 1 if quote was properly closed, 0 if unclosed
*/
int	skip_quoted_content(char *input, int *pos, char quote_char)
{
	int	start;
 
	start = *pos;
	(*pos)++;  // Skip opening quote 
	while (input[*pos] && input[*pos] != quote_char)
		(*pos)++; 
	if (!input[*pos])
	{
		// Unclosed quote
		*pos = start;
		return (0);
	}
	 
	(*pos)++;  // Skip closing quote
	return (1);
}
