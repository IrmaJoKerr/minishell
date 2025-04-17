/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/16 16:04:01 by bleow            ###   ########.fr       */
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

void maketoken_with_type(char *token, t_tokentype type, t_vars *vars)
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

int is_adjacent_token(char *input, int pos) // POSSIBLE CANDIDATE FOR DEPRECATION
{
	if (pos <= 0)
		return (0);
	
	// Don't join if previous char is an operator
	if (input[pos - 1] == '<' || input[pos - 1] == '>' || 
		input[pos - 1] == '|' || input[pos - 1] == ';')
		return (0);
		
	// Don't join if current position is an operator
	if ((size_t)pos < ft_strlen(input) && 
		(input[pos] == '<' || input[pos] == '>' || 
		 input[pos] == '|' || input[pos] == ';'))
		return (0);
	
	// Check if previous character is not whitespace
	if (ft_isspace(input[pos - 1]))
		return (0);
	// It's adjacent if previous char is not whitespace or operator
	return (1);
}

/*
Creates an expansion token from input
- Identifies $ variables and creates TYPE_EXPANSION tokens
- Handles $? as special case (exit status)
- Doesn't perform expansion - just identifies tokens
Returns 1 if expansion token was created, 0 otherwise
*/

int init_quote_processing(char *input, int *i, int *is_adjacent, char *quote_char)
{
	int	quote_type;
	
	fprintf(stderr, "DEBUG: Entering init_quote_processing, quote char: '%c'\n", *quote_char);
    *quote_char = input[*i];
    *is_adjacent = is_adjacent_token(input, *i);
    if (*quote_char == '\'')
        quote_type = TYPE_SINGLE_QUOTE;
    else
        quote_type = TYPE_DOUBLE_QUOTE;
    return (quote_type);
}


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
static void handle_text(char *input, t_vars *vars)
{
	char *token_preview;

	if (vars->pos > vars->start)
	{
		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
		set_token_type(vars, token_preview);
		handle_string(input, vars);
		free(token_preview);
	}
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

static	void imp_tok_quote(char *input, t_vars *vars)
{
	int	saved_pos;
	
    fprintf(stderr, "DEBUG[imp_tok_quote]: ENTER with pos=%d, start=%d, char='%c'\n", 
        vars->pos, vars->start, input[vars->pos]);
    handle_text(input, vars);
    // Save current position to restore if process_quote_char fails
    saved_pos = vars->pos;
    
    if (process_quote_char(input, vars))
    {
        fprintf(stderr, "DEBUG[imp_tok_quote]: After process_quote - pos=%d, start=%d\n", 
            vars->pos, vars->start);
        fprintf(stderr, "DEBUG[imp_tok_quote]: Adjacency state: left=%d, right=%d\n",
            vars->adj_state[0], vars->adj_state[1]);
        vars->next_flag = 1;
        // Right adjacency handling now happens in process_quote_char
    }
    else
    {
        fprintf(stderr, "DEBUG[imp_tok_quote]: process_quote_char returned 0 (failed)\n");
        vars->pos = saved_pos; // Restore position on failure
    }
    fprintf(stderr, "DEBUG[imp_tok_quote]: EXIT - next_flag=%d\n", vars->next_flag);
}

static void imp_tok_expan(char *input, t_vars *vars)
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

/*
Handle operator tokens in the input.
Processes operators like >, <, |, etc.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_operat(char *input, t_vars *vars)
{
	handle_text(input, vars);
	if (process_operator_char(input, &vars->pos, vars))
		vars->next_flag = 1;
}

/*
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_white(char *input, t_vars *vars)
{
	handle_text(input, vars);
	while (input[vars->pos] && input[vars->pos] <= ' ')
		vars->pos++;
	vars->start = vars->pos;
	vars->next_flag = 1;
}

/* 
Tokenizes input string with improved quote handling.
Creates tokens for commands, args, quotes, and operators.
*/
int improved_tokenize(char *input, t_vars *vars)
{
	int			moves;
	t_tokentype	token_type;

	vars->pos = 0;
	vars->start = 0;
	vars->quote_depth = 0;
	while (input && input[vars->pos])
	{
		vars->next_flag = 0;
		token_type = get_token_at(input, vars->pos, &moves);
		fprintf(stderr, "DEBUG[tokenize]: At pos %d, char '%c', token_type=%d, moves=%d\n", 
			vars->pos, input[vars->pos], token_type, moves);
		if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
			imp_tok_quote(input, vars);
		if (!vars->next_flag && input[vars->pos] == '$' && !vars->quote_depth)
			imp_tok_expan(input, vars);
		if (!vars->next_flag && is_operator_token(get_token_at(input
			,vars->pos, &moves)))
			imp_tok_operat(input, vars);
		if (!vars->next_flag && input[vars->pos] && input[vars->pos] <= ' ')
			imp_tok_white(input, vars);
		if (vars->next_flag)
			continue ;
		vars->pos++;
	}
	handle_text(input, vars);
	DBG_PRINTF(DEBUG_TOKENIZE, "improved_tokenize: Token type=%d, content='%s'\n", 
			  vars->curr_type, (vars->current && vars->current->args) ? 
			  vars->current->args[0] : "(null)");
	DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
	debug_token_list(vars);
	return(1);
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
int	build_token_linklist(t_vars *vars, t_node *node)
{
    t_node *cmd_node;
    
    if (!vars || !node) 
        return (0);
    // Case 1: First node becomes the head
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
        return (0);
    }
    // Case 2: Heredoc operator (<<) handling
    if (node->type == TYPE_HEREDOC)
    {
        // Find the command this heredoc belongs to (could be current or previous)
        cmd_node = find_cmd(vars->head, node, FIND_PREV, vars);
        // Store the command relationship if needed
        if (cmd_node)
            fprintf(stderr, "DEBUG: Heredoc belongs to command '%s'\n", 
                   cmd_node->args[0]);
        // Add the heredoc node to the list
        token_link(node, vars);
        // Set flag for expecting delimiter in next token
        vars->heredoc_active = 1;
        return (0);
    }
    // Case 3: Heredoc delimiter handling
    if (vars->heredoc_active && vars->current && vars->current->type == TYPE_HEREDOC)
    {
        // Store the delimiter for later processing
        if (node->args && node->args[0])
        {
            // Free any existing delimiter
            if (vars->pipes->heredoc_delim)
                free(vars->pipes->heredoc_delim);
            vars->pipes->heredoc_delim = ft_strdup(node->args[0]);
            fprintf(stderr, "DEBUG: Set heredoc delimiter to '%s'\n", vars->pipes->heredoc_delim);
        }
        // Reset the heredoc flag
        vars->heredoc_active = 0;
        // Link the delimiter node
        token_link(node, vars);
        return (0);
    }
    // Existing functionality continues below...
    if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS)
    {
        node->type = TYPE_CMD;
    }
    if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD)
    {
        cmd_node = vars->current;
        append_arg(cmd_node, node->args[0], 0);
        if (node->args)
        {
            free(node->args[0]);
            free(node->args);
        }
        node->args = NULL;
        free_token_node(node);
        return (1);  // Indicate that the node was freed
    }
    else
    {
        token_link(node, vars);
        return (0);  // Node was not freed
    }
}

/* Debug function */
void debug_token_list(t_vars *vars)
{
	t_node *current = vars->head;
	int count = 0;
	
	DBG_PRINTF(DEBUG_TOKENIZE, "=== COMPLETE TOKEN LIST ===\n");
	while (current)
	{
		DBG_PRINTF(DEBUG_TOKENIZE, "Token %d: type=%d (%s), content='%s'\n", 
				  count++, current->type, get_token_str(current->type), 
				  current->args ? current->args[0] : "NULL");
		current = current->next;
	}
	DBG_PRINTF(DEBUG_TOKENIZE, "========================\n");
}
