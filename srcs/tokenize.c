/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 01:57:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <stdio.h>

/*
Sets appropriate token type based on position and context
Determines if current token should be a command or argument
Updates vars->curr_type accordingly
*/
void set_token_type(t_vars *vars, char *input)
{
    int advance;
    t_tokentype special_type = 0;
    
	DBG_PRINTF(DEBUG_TOKENIZE, "set_token_type: input='%s', curr_type=%d\n", input, vars->curr_type);
    // Save previous type
    vars->prev_type = vars->curr_type;
    
    // Check for variable expansion
    if (input && input[0] == '$')
    {
        vars->curr_type = TYPE_EXPANSION;
        DBG_PRINTF(DEBUG_TOKENIZE, "Setting token as expansion: '%s'\n", input);
        return;
    }
    
    // Check for special token types
    if (input && *input)
    {
        special_type = get_token_at(input, 0, &advance);
        if (special_type != 0)
        {
            vars->curr_type = special_type;
            DBG_PRINTF(DEBUG_TOKENIZE, "Setting token as special type: %d (%s)\n", 
                    special_type, get_token_str(special_type));
            return;
        }
    }
    
    // Default to position-based command/argument detection
    if (!vars->head || vars->prev_type == TYPE_PIPE)
    {
        vars->curr_type = TYPE_CMD;
        DBG_PRINTF(DEBUG_TOKENIZE, "Setting token as command (position: %s)\n", 
                !vars->head ? "first token" : "after pipe");
    }
    else
    {
        vars->curr_type = TYPE_ARGS;
        DBG_PRINTF(DEBUG_TOKENIZE, "Setting token as argument\n");
    }
	DBG_PRINTF(DEBUG_TOKENIZE, "set_token_type: Setting type to %d\n", vars->curr_type);
}

/*
Creates a new token node and adds it to the token list.
- Creates node of current token type.
- Sets node data from the provided string.
- Adds node to linked list and updates current pointer.
Works with lexerlist() during token creation.

Example: For input "echo hello"
- When maketoken("echo") called with TYPE_CMD
- Creates command node with args[0] = "echo"
- Adds to token list as head or appends to current
*/
void	maketoken_with_type(char *token, t_tokentype type, t_vars *vars)
{
	t_node	*node;
 
	node = NULL;
	if (!token || !*token)
	{
        DBG_PRINTF(DEBUG_ARGS, "Empty token, not creating node\n");
        return ;
    }
	DBG_PRINTF(DEBUG_ARGS, "Creating token of type %d with content '%s'\n", 
		type, token);
    node = initnode(type, token);
    if (node)
	{
        DBG_PRINTF(DEBUG_ARGS, "Node created with args[0]='%s'\n", 
                  node->args ? node->args[0] : "NULL");
        // Add to linked list
        build_token_linklist(vars, node);
        if (vars->head == node) {
            DBG_PRINTF(DEBUG_ARGS, "Node is head of token list\n");
        }
    } 
	else
	{
        DBG_PRINTF(DEBUG_ARGS, "Failed to create node\n");
    }
}

int process_quoted_content(char *input, int *i, t_vars *vars)
{
    int advance;
    t_tokentype quote_type;
    char quote_char;
    int content_length;
    char *quoted_content;
    t_node *cmd_node;
    
    quote_type = get_token_at(input, *i, &advance);
    
    if (quote_type != TYPE_SINGLE_QUOTE && quote_type != TYPE_DOUBLE_QUOTE)
        return (0);
    
    if (quote_type == TYPE_SINGLE_QUOTE)
        quote_char = '\'';
    else
        quote_char = '"';
    
    // Store start position in vars->pos (reuse existing variable)
    vars->pos = *i;
    (*i)++;  // Skip past opening quote
    // Store content start in vars->start (reuse existing variable)
    vars->start = *i;
    
    // Find matching closing quote
    while (input[*i] && input[*i] != quote_char)
        (*i)++;
    
    if (input[*i] == quote_char)
    {
        content_length = *i - vars->start;
        
        // Extract content BETWEEN quotes (not including the quotes)
        quoted_content = ft_substr(input, vars->start, content_length);
        
        if (!quoted_content)
            return (0);
        
        cmd_node = find_last_command(vars->head);
        
        if (cmd_node)
        {
            // Use direct condition instead of ternary
            if (quote_type == TYPE_SINGLE_QUOTE)
                append_arg(cmd_node, quoted_content, 1);
            else
                append_arg(cmd_node, quoted_content, 2);
        }
        else
        {
            maketoken_with_type(quoted_content, TYPE_ARGS, vars);
            
            if (vars->head && vars->head == vars->current)
                vars->head->type = TYPE_CMD;
        }
        
        free(quoted_content);
        
        (*i)++;  // Skip past closing quote
        vars->start = *i;
        
        return (1);
    }
    else
    {
        // Handle unclosed quote - store context for later completion
        if (vars->quote_depth < 32)
        {
            vars->quote_ctx[vars->quote_depth].type = quote_char;
            vars->quote_ctx[vars->quote_depth].start_pos = vars->pos;
            vars->quote_ctx[vars->quote_depth].depth = vars->quote_depth + 1;
            vars->quote_depth++;
        }
        return (1);
    }
}

// Helper function to find the last command node
t_node	*find_last_command(t_node *head)
{
	t_node	*current;
	t_node	*last_cmd;

	current = head;
	last_cmd = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
			last_cmd = current;
		current = current->next;
	}
	return (last_cmd);
}

/*
Determines if variable expansion is allowed in current context.
- Returns 1 if no quotes or in double quotes.
- Returns 0 if in single quotes.
Works with process_char() for expansion handling.

Example: For input "echo '$USER'"
- When inside single quotes, returns 0 (no expansion)
- For "echo "$USER"", returns 1 (expansion allowed)
*/
int	handle_expand(t_vars *vars)
{
	if (vars->quote_depth == 0)
		return (1);
	if (vars->quote_ctx[vars->quote_depth - 1].type == '"')
		return (1);
	return (0);
}

/*
Processes special characters like dollar sign for variable expansion.
- Detects $ character and expands the following variable name.
- Creates appropriate tokens with expanded values.
- Handles integration with the command argument structure.
- Updates position pointers to continue processing after expansion.
Returns:
- 1 if special char was processed.
- 0 if not processed.
Works with process_char() during tokenization.

Example: For "echo $TEST"
- Detects $ at position 5
- Expands TEST to its value (e.g., "123")
- Adds "123" as argument to "echo" command
*/
int	process_special_char(char *input, int *i, t_vars *vars)
{
    char	*expanded;
    char	*var_name;
    int		pos;
    int		first_token;

    if (input[*i] == '$' && handle_expand(vars))
    {
        first_token = 0;
        if (*i > vars->start)
            process_text(input, vars, &first_token, TYPE_NULL);
        pos = *i;
        pos++;
        var_name = get_var_name(input, &pos);
        expanded = get_env_val(var_name, vars->env);
        free(var_name);
        if (expanded)
        {
            maketoken_with_type(expanded, vars->curr_type, vars);
            *i = pos;
            vars->start = pos;
            free(expanded);
            return (1);
        }
        else
        {
            maketoken_with_type("", vars->curr_type, vars);
            *i = pos;
            vars->start = pos;
            return (1);
        }
    }
    return (0);
}

/*
 * Checks if tokens should be joined (no whitespace in between)
 * Returns 1 if they should be joined, 0 otherwise
 */
int is_adjacent_token(char *input, int pos)
{
    if (pos <= 0)
        return (0);
        
    DBG_PRINTF(DEBUG_TOKENIZE, "is_adjacent_token: Checking character at pos %d: '%c'\n", 
               pos-1, input[pos-1]);
    
    // Check if previous character is not whitespace
    if (ft_isspace(input[pos - 1]))
        return (0);
    
    // It's adjacent if previous char is not whitespace
    return (1);
}
/*
 * Joins token with the last argument of the command node
 * Returns 1 if joined successfully, 0 if failed
 */
int join_with_cmd_arg(t_node *cmd_node, char *token_val)
{
    int     arg_count;
    char    *last_arg;
    char    *new_arg;
      
    if (!cmd_node || !token_val)
        return (0);
        
    DBG_PRINTF(DEBUG_TOKENIZE, "join_with_cmd_arg: Joining '%s' with last arg of command\n", 
               token_val);
    
    // Count arguments
    arg_count = 0;
    while (cmd_node->args[arg_count])
        arg_count++; 
    if (arg_count < 1)
        return (0);     
    
    // Get last argument
    last_arg = cmd_node->args[arg_count - 1];
    if (!last_arg)
        return (0); 
    
    DBG_PRINTF(DEBUG_TOKENIZE, "Joining with last arg: '%s'\n", last_arg);
    
    // Join the strings
    new_arg = ft_strjoin(last_arg, token_val);
    if (!new_arg)
        return (0); 
    
    // Replace the last argument with joined string
    free(cmd_node->args[arg_count - 1]);
    cmd_node->args[arg_count - 1] = new_arg; 
    
    DBG_PRINTF(DEBUG_TOKENIZE, "Joined value with previous arg to form '%s'\n", new_arg);
     
    return (1);
}

/*
Processes expansion characters in the input.
- Handles variable expansion with $.
- Updates position after expansion.
- Extracts variable name and its value.
Returns:
1 if expansion was processed, 0 otherwise.
Works with process_char() during tokenization.

Example: For input "echo $HOME"
- Detects $ at position
- Creates expansion token for HOME
- Updates position past the variable name
*/
/*
Process variable expansion characters ($VAR or $?)
Now handles joining adjacent expansions with previous text
Returns 1 if expansion was processed, 0 otherwise
*/
int process_expand_char(char *input, int *i, t_vars *vars)
{
    int     start_pos;
    char    *var_name;
    char    *var_value;
    t_node  *cmd_node;
    int     is_adjacent;
    
    start_pos = *i;
    
    // Check if this expansion is adjacent to previous text
    // is_adjacent = is_adjacent_expansion(input, start_pos);
    is_adjacent = is_adjacent_token(input, start_pos);
    /* Skip $ */
    (*i)++;
    
    /* Special case for $? */
    if (input[*i] == '?')
    {
        (*i)++;
        var_value = ft_itoa(vars->error_code);
        
        if (!var_value)
            return (0);
        
        cmd_node = find_last_command(vars->head);
        
        // If adjacent to previous text, join with last argument
        if (cmd_node && is_adjacent)
        {
            if (join_with_cmd_arg(cmd_node, var_value))
            {
                free(var_value);
                vars->start = *i;
                return (1);
            }
        }
        
        if (cmd_node)
            append_arg(cmd_node, var_value, 0);
        else
            maketoken_with_type(var_value, TYPE_ARGS, vars);
        
        free(var_value);
        vars->start = *i;
        
        return (1);
    }
    
    /* Process variable name */
    if (ft_isalpha(input[*i]) || input[*i] == '_')
    {
        /* Read variable name */
        start_pos = *i;
        
        while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
            (*i)++;
        
        var_name = ft_substr(input, start_pos, *i - start_pos);
        
        if (!var_name)
            return (0);
        
        var_value = get_env_val(var_name, vars->env);
        free(var_name);
        
        cmd_node = find_last_command(vars->head);
        
        // If adjacent to previous text, try joining with last argument
        if (cmd_node && is_adjacent)
        {
            if (join_with_cmd_arg(cmd_node, var_value ? var_value : ""))
            {
                if (var_value)
                    free(var_value);
                vars->start = *i;
                return (1);
            }
        }
        
        // Fall back to standard behavior
        if (cmd_node && var_value)
            append_arg(cmd_node, var_value, 0);
        else if (cmd_node)
            append_arg(cmd_node, "", 0);
        else if (var_value)
            maketoken_with_type(var_value, TYPE_ARGS, vars);
        else
            maketoken_with_type("", TYPE_ARGS, vars);
        
        if (var_value)
            free(var_value);
            
        vars->start = *i;
        
        return (1);
    }
    
    /* Just a lone $ character */
    (*i) = start_pos + 1;
    vars->start = start_pos;
    
    return (0);
}

/*
Processes quoted characters in the input string.
- Handles single and double quotes.
- Updates position after quoted content.
- Creates tokens for quote content.
Returns:
1 if quote was processed, 0 otherwise.
Works with process_char() for quoted content handling.

Example: For input with quotes like "echo 'hello'"
- Processes the quoted content
- Creates appropriate token
- Returns 1 to indicate quote was handled
*/
/*
 * Process single or double quoted text
 * Handles joining to previous token when adjacent
 */
int process_quote_char(char *input, int *i, t_vars *vars)
{
    int quote_type;
    int start_pos;
    char *content;
    int is_adjacent;
    t_node *cmd_node;
    
    start_pos = *i;
    is_adjacent = is_adjacent_token(input, start_pos);
    
    // Determine quote type
    quote_type = (input[*i] == '\'') ? 1 : 2;
    
    // Skip opening quote
    (*i)++;
    
    // Find closing quote
    start_pos = *i;
    while (input[*i] && input[*i] != input[start_pos - 1])
        (*i)++;
    
    // If no closing quote found, treat as normal text and return
    if (!input[*i])
    {
        *i = start_pos;
        return (0);
    }
    
    // Extract text between quotes
    content = ft_substr(input, start_pos, *i - start_pos);
    if (!content)
        return (0);
    
    // Skip closing quote
    (*i)++;
    
    // If adjacent to previous token, join with it
    if (is_adjacent)
    {
        cmd_node = find_last_command(vars->head);
        if (cmd_node && join_with_cmd_arg(cmd_node, content))
        {
            free(content);
            vars->start = *i;
            return (1);
        }
    }
    
    // Otherwise add as separate argument with appropriate quote type
    cmd_node = find_last_command(vars->head);
    if (cmd_node)
        append_arg(cmd_node, content, quote_type);
    else
    {
        // If no command to append to, create new token
        maketoken_with_type(content, TYPE_ARGS, vars);
    }
    
    free(content);
    vars->start = *i;
    
    return (1);
}

/*
Processes operator characters in the input string.
- Identifies redirection operators and pipe.
- Creates tokens for these operators.
- Updates position past the operator.
Returns:
1 if operator was processed, 0 otherwise.
Works with process_char() for operator handling.

Example: For input "cmd > file"
- Processes the '>' operator
- Creates redirect token
- Returns 1 to indicate operator was handled
*/
/* 
Processes operator characters like pipes and redirections.
Returns 1 if operator was processed, 0 otherwise.
*/
int process_operator_char(char *input, int *i, t_vars *vars)
{
    int advance;
    t_tokentype token_type;
    
    token_type = get_token_at(input, *i, &advance);
    
    if (!is_operator_token(token_type))
        return (0);
    
    /* Create operator token */
    create_operator_token(vars, token_type, get_token_str(token_type));
    
    /* Advance position */
    (*i) += advance;
    vars->start = *i;
    
    return (1);
}

/*
Processes a single character in the input string.
- Updates token boundary at whitespace.
- Delegates to specialized handlers for:
  - Quotes
  - Variable expansion
  - Operators
- Advances position for regular characters.
Works with tokenize() for character-by-character processing.

Example: For input "echo hello"
- Sets token boundary at start of "echo"
- Advances through characters
- Handles special characters appropriately
*/
void	process_char(char *input, int *i, t_vars *vars)
{
	if (!input || !i || !vars)
		return ;
	if (*i == 0 || ft_isspace(input[*i - 1]))
		vars->start = *i;
	if (process_quote_char(input, i, vars))
		return ;
	if (process_expand_char(input, i, vars))
		return ;
	if (process_operator_char(input, i, vars))
		return ;
	(*i)++;
}

/*
Processes token position for quoted content.
- Advances position past opening quote.
- Searches for matching closing quote.
- Updates position accordingly.
Returns:
Position after scanning for quote.
Works with handle_quote_token() during quote processing.

Example: For input "echo 'hello world'"
- Advances past opening quote
- Scans for closing quote
- Returns position of closing quote or end of string
*/
int	scan_quote_position(char *str, int *pos, char quote_char)
{
	int	start;

	start = *pos;
	(*pos)++;
	while (str[*pos])
	{
		if (str[*pos] == quote_char)
			break ;
		(*pos)++;
	}
	return (start);
}

/*
Creates token for quoted content.
- Updates token boundary for quote content.
- Sets token type based on quote type.
- Creates token with the quoted content.
- Updates position for next token.
Works with handle_quote_token() for token creation.

Example: For input "echo "hello world""
- Creates token with TYPE_DOUBLE_QUOTE
- Sets content to "hello world"
- Updates position past closing quote
*/
/*
Creates token for quoted content.
- Updates token boundary for quote content.
- Sets token type based on quote type.
- Creates token with the quoted content.
- Updates position for next token.
Works with handle_quote_token() for token creation.
*/
void	create_quote_token(char *str, t_vars *vars, int *pos, int start)
{
	char	quote_char;

	quote_char = str[start];
	(*pos)++;
	vars->start = start;
	if (quote_char == '"')
		vars->curr_type = TYPE_DOUBLE_QUOTE;
	else
		vars->curr_type = TYPE_SINGLE_QUOTE;
	maketoken_with_type(str, vars->curr_type, vars);
	vars->start = *pos;
}

/*
Processes redirection operators in the input.
- Handles <, >, << and >> operators.
- Creates tokens for the operators.
- Updates position past the operator.
Works with process_char() for operator handling.

Example: For input "cat > file.txt"
- When position is at '>'
- Creates redirect token
- Updates position past '>'
*/
int	handle_redirection(char *input, int *pos, t_vars *vars)
{
    int	result;

    if (input[*pos] == '<' || input[*pos] == '>' || input[*pos] == '|')
    {
        vars->pos = *pos;
        result = operators(input, vars);
        *pos = vars->pos;
        return (result);
    }
    return (0);
}

/* 
Tokenizes input string with improved quote handling.
Creates tokens for commands, args, quotes, and operators.
*/
int improved_tokenize(char *input, t_vars *vars)
{
    int advance;
    t_tokentype token_type;
    char *token_preview;
    
    /* Initialize tokenization state */
    vars->pos = 0;
    vars->start = 0;
    vars->quote_depth = 0;
    
    DBG_PRINTF(DEBUG_TOKENIZE, "Starting tokenization of: '%s'\n", input);
    
    while (input && input[vars->pos])
    {
        /* Handle quoted text */
        token_type = get_token_at(input, vars->pos, &advance);
        
        if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE) 
        {
            /* Process any text before the quote */
            if (vars->pos > vars->start)
            {
                token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
                set_token_type(vars, token_preview);
                handle_string(input, vars);
                free(token_preview);
            }
            
            /* Process the quoted content */
            if (process_quote_char(input, &vars->pos, vars))
                continue;
        }
        
        /* Handle expansion variables */
        if (input[vars->pos] == '$' && !vars->quote_depth)
        {
            /* Process any text before the $ sign */
            if (vars->pos > vars->start)
            {
                token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
                set_token_type(vars, token_preview);
                handle_string(input, vars);
                free(token_preview);
            } 
            
            /* Process expansion */
            if (process_expand_char(input, &vars->pos, vars))
                continue;
        }
        
        /* Handle operators (pipes, redirections) */
        if (is_operator_token(get_token_at(input, vars->pos, &advance))) 
        {
            /* Process any text before the operator */
            if (vars->pos > vars->start)
            {
                token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
                set_token_type(vars, token_preview);
                handle_string(input, vars);
                free(token_preview);
            }
            
            /* Process the operator */
            if (process_operator_char(input, &vars->pos, vars))
                continue;
        }
        
        /* Handle whitespace */
        if (input[vars->pos] && input[vars->pos] <= ' ')
        {
            /* Process text before whitespace */
            if (vars->pos > vars->start)
            {
                token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
                set_token_type(vars, token_preview);
                DBG_PRINTF(DEBUG_TOKENIZE, "Whitespace boundary at pos %d, processing text from %d to %d\n", 
                          vars->pos, vars->start, vars->pos);
                handle_string(input, vars);
                free(token_preview);
            }
            
            /* Skip whitespace */
            while (input[vars->pos] && input[vars->pos] <= ' ')
                vars->pos++;
                
            vars->start = vars->pos;
            continue;
        }
        
        /* Move to next character */
        vars->pos++;
    }
    
    /* Process any remaining text */
    if (vars->pos > vars->start)
    {
        token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
        set_token_type(vars, token_preview);
        DBG_PRINTF(DEBUG_TOKENIZE, "Final text from %d to %d\n", vars->start, vars->pos);
        handle_string(input, vars);
        free(token_preview);
    }
    
    DBG_PRINTF(DEBUG_TOKENIZE, "improved_tokenize: Token type=%d, content='%s'\n", 
              vars->curr_type, (vars->current && vars->current->args) ? 
              vars->current->args[0] : "(null)");
    DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
	return(1);
}

/*
Creates a new command node from a token.
- Allocates node with command type.
- Sets up args array with initial token.
- Handles memory errors properly.
Returns:
Pointer to new command node or NULL on failure.
Works with process_cmd_token() for command creation.

Example: For command "echo"
- Creates node with TYPE_CMD type
- Sets args[0] to "echo"
- Returns node pointer
*/
t_node	*make_cmdnode(char *token)
{
	t_node	*node;

	node = initnode(TYPE_CMD, token);
	if (!node)
		return (NULL);
	if (node->args)
	{
		if (node->args[0])
			free(node->args[0]);
		free(node->args);
	}
	node->args = malloc(sizeof(char *) * 2);
	if (!node->args)
	{
		free(node);
		return (NULL);
	}
	node->args[0] = ft_strdup(token);
	if (!node->args[0])
	{
		free(node->args);
		free(node);
		return (NULL);
	}
	node->args[1] = NULL;
	return (node);
}

/*
Creates a new command node with proper setup.
- Wraps make_cmdnode with error handling.
- Ensures memory cleanup on failure.
Returns:
- Pointer to new command node or NULL on failure.
- Works with process_cmd_token() for command creation.

Example: For command token "grep"
- Creates new command node
- Handles memory errors
- Returns node or NULL on failure
*/
t_node	*new_cmd_node(char *token)
{
	t_node	*node;

	node = make_cmdnode(token);
	if (!node)
	{
		free(token);
		return (NULL);
	}
	return (node);
}

/*
Creates a non-command token node.
- Allocates node with specified token type.
- Handles memory errors properly.
Returns:
Pointer to new token node or NULL on failure.
Works with process_other_token() for token creation.

Example: For redirect token ">"
- Creates node with TYPE_OUT_REDIRECT
- Returns node pointer or NULL on failure
*/
t_node	*new_other_node(char *token, t_tokentype type)
{
	t_node	*node;

	node = initnode(type, token);
	if (!node)
	{
		free(token);
		return (NULL);
	}
	return (node);
}

/*
Updates the token list with a new node.
- Handles first token as head.
- Otherwise adds to end of list.
- Updates current pointer.
Works with process_cmd_token() and process_other_token().

Example: When adding command node
- If first token, sets as head
- Otherwise links to previous token
- Updates current pointer
*/
void build_token_linklist(t_vars *vars, t_node *node)
{
    t_node *cmd_node;
    
    if (!vars || !node)
    {   
        return;
    }
    
    DBG_PRINTF(DEBUG_ARGS, "build_token_linklist: type=%d (%s), content='%s'\n", 
              node->type, get_token_str(node->type), node->args ? node->args[0] : "NULL");
              
    // If this is the first token
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
        DBG_PRINTF(DEBUG_ARGS, "First token in list\n");
        return;
    }
    
    // Check if previous node was a pipe - if so, enforce this node as a command
    if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS)
    {
        node->type = TYPE_CMD;
        DBG_PRINTF(DEBUG_ARGS, "Converting node to command after pipe\n");
    }
    
    // If this is an argument and previous node is a command, attach it
    if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD)
    {
        cmd_node = vars->current;
        
        DBG_PRINTF(DEBUG_ARGS, "build_token_linklist: Appending '%s' to command '%s'\n", 
                  node->args[0], vars->current->args[0]);
        
        // Attach as argument
        append_arg(cmd_node, node->args[0], 0);
        
        // Clean up the node - we don't need it in the list
        node->args = NULL;  // Prevent double-free
        free_token_node(node);  // Changed from free_node to free_token_node
    }
    else 
    {
        // Add as new node to the list
        vars->current->next = node;
        node->prev = vars->current;
        vars->current = node;
        DBG_PRINTF(DEBUG_ARGS, "Added as new node in list\n");
    }
}

/*
Checks if current argument is a flag separator.
- Identifies standalone '-' followed by alphabetic arg.
- Used to detect cases like "cmd - x" that should be "cmd -x".
Returns:
- 1 if it's a flag arg pattern.
- 0 if not.
Works with process_cmd_token() for argument processing.

Example: For arguments ["ls", "-", "l"]
- Returns 1 for position 1
- Indicates "-" and "l" should be joined as "-l"
*/
int	is_flag_arg(char **args, int i)
{
	if (i > 0 && args[i][0] == '-' && ft_strlen(args[i]) == 1
		&& args[i + 1] && ft_isalpha(args[i + 1][0]))
		return (1);
	return (0);
}

/*
Joins flag arguments that were incorrectly split.
- Combines "-" and the following argument.
- Shifts remaining arguments up.
- Updates the args array in place.
Works with process_cmd_token() during argument processing.

Example: For arguments ["ls", "-", "l", "a"]
- Joins "-" and "l" to become "-l"
- Shifts "a" up in position
- Results in ["ls", "-l", "a", NULL]
*/
void	join_flag_args(char **args, int i)
{
	char	*combined;
	int		j;

	combined = ft_strjoin(args[i], args[i + 1]);
	if (combined)
	{
		free(args[i]);
		free(args[i + 1]);
		args[i] = combined;
		j = i + 1;
		while (args[j + 1])
		{
			args[j] = args[j + 1];
			j++;
		}
		args[j] = NULL;
	}
}

/*
Creates command node from parsed arguments.
- Uses first arg as command name.
- Adds remaining args to node's args array.
- Handles memory allocation properly.
Returns:
- New command node or NULL on failure.
Works with process_cmd_token() during command creation.

Example: For arguments ["grep", "pattern", "file"]
- Creates command node with "grep"
- Adds "pattern" and "file" as arguments
- Returns the complete command node
*/
t_node	*build_cmdarg_node(char **args)
{
	t_node	*node;
	int		i;

	if (!args || !args[0])
		return (NULL);
	node = new_cmd_node(args[0]);
	if (!node)
		return (NULL);
	i = 1;
	while (args[i])
	{
		append_arg(node, args[i], 0);
		i++;
	}
	return (node);
}

/*
Processes arguments by removing outer quotes.
- Applies quote removal to each argument.
- Ensures proper argument interpretation.
Works with process_cmd_token() for argument processing.

Example: For arguments ["'hello'", "\"world\""]
- Processes to remove outer quotes
- Transforms to ["hello", "world"]
*/
void	process_args_tokens(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		process_quotes_in_arg(&args[i]);
		i++;
	}
}

/*
Handles quoted tokens during tokenization.
- Detects quote character type (single/double).
- Processes content between quotes.
- Updates position past closing quote.
- Creates token with appropriate type.
Works with process_char() for quoted content.

Example: For input "echo "hello world""
- Detects quote at start position
- Extracts "hello world" as content
- Creates token for quoted content
- Updates position past closing quote
*/
void	handle_quote_token(char *str, t_vars *vars, int *pos)
{
    char	quote_char;
    int		start;

    if (!str || !vars || !pos)
        return ;
    quote_char = str[*pos];
    start = *pos;
    (*pos)++;
    while (str[*pos] && str[*pos] != quote_char)
        (*pos)++;
    if (str[*pos] == quote_char)
    {
        vars->start = start;
        if (quote_char == '"')
            vars->curr_type = TYPE_DOUBLE_QUOTE;
        else
            vars->curr_type = TYPE_SINGLE_QUOTE;
        maketoken_with_type(str, vars->curr_type, vars);
        vars->start = ++(*pos);
    }
    else
    {
        vars->quote_depth++;
        vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
    }
}

/*
Validate token type and content.
*/
int	validate_token(t_node *token)
{
	if (!token)
		return (0);
	if (token->type == TYPE_PIPE)
	{
		if (!token->next)
			return (0);
	}
	if (redirection_type(NULL, 0, token->type, 0))
	{
		if (!token->next)
			return (0);
	}
	return (1);
}
