/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/24 16:03:26 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets appropriate token type based on position and context.
Determines if current token should be a command or argument.
Updates vars->curr_type accordingly.
*/
void	set_token_type(t_vars *vars, char *input)
{
	int			moves;
	t_tokentype	special_type;

	special_type = 0;
	vars->prev_type = vars->curr_type;
	if (input && input[0] == '$')
	{
		vars->curr_type = TYPE_EXPANSION;
		return ;
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

/*
Checks if a node is orphaned (not properly linked in the token list)
and frees it if necessary to prevent memory leaks.
This function is called when a node is created and added to the token list.
*/
void	free_if_orphan_node(t_node *node, t_vars *vars)
{
	t_node	*check;
	int		found_in_list;
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
            fprintf(stderr, "DEBUG[maketoken]: Freeing orphaned node not in list. "
                    "node=%p, type=%d, content='%s'\n",
                    (void*)node, node->type, 
                    (node->args && node->args[0]) ? node->args[0] : "NULL");
            free_token_node(node);
        }
    }
}

/*
Creates a token node and adds it to the token linked list.
- Creates a node with the given token string and type.
- Links the node into the shell's token linked list.
- Handles orphaned nodes to prevent memory leaks.
- Updates current token pointer as needed.
Example: When tokenizing "echo hello"
- Creates separate nodes for "echo" and "hello"
- Links them in sequence with proper type assignment.
*/
void	maketoken(char *token, t_tokentype type, t_vars *vars)
{
    t_node	*node;
	int		node_freed;

	if (!token || !vars)
		return ;
	fprintf(stderr, "[TOK_DBG] maketoken: Called with token='%s', type=%d\n", token, type);
	node = initnode(type, token);
    if (!node)
        return ;
    node_freed = build_token_linklist(vars, node);
    if (!node_freed) 
        free_if_orphan_node(node, vars);
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
        handle_string(input, vars);
        if (token_preview)
		{
            free(token_preview);
        }
        vars->start = vars->pos;
        fprintf(stderr, "[TOK_DBG] handle_text: Reset Start to %d\n", vars->start); // DEBUG
    }
	else
	{
        fprintf(stderr, "[TOK_DBG] handle_text: No text to process (Pos <= Start).\n"); // DEBUG
    }
}

void	imp_tok_quote(char *input, t_vars *vars)
{
    fprintf(stderr, "DEBUG[imp_tok_quote]: ENTER with pos=%d, char='%c', prev_type=%d\n", 
        vars->pos, input[vars->pos], vars->prev_type);
	fprintf(stderr, "DEBUG: imp_tok_quote: Previous token type=%d, pos=%d\n",
        vars->prev_type, vars->pos);
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
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
void	imp_tok_white(char *input, t_vars *vars)
{
    fprintf(stderr, "[TOK_DBG] imp_tok_white: Called. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
    handle_text(input, vars);
    int initial_pos = vars->pos;
    while (input[vars->pos] && input[vars->pos] <= ' ')
        vars->pos++;
    fprintf(stderr, "[TOK_DBG] imp_tok_white: Skipped whitespace from %d to %d.\n", initial_pos, vars->pos); // DEBUG
    vars->start = vars->pos;
    vars->next_flag = 1;
    fprintf(stderr, "[TOK_DBG] imp_tok_white: Set Start=%d, next_flag=1.\n", vars->start); // DEBUG
}

void handle_right_adj(char *input, t_vars *vars)
{
	char	*adjacent_text;
	char	*joined;
	int		arg_idx;
    fprintf(stderr, "DEBUG[handle_right_adj]: ENTER - pos=%d, start=%d\n",
            vars->pos, vars->start);
    if (vars->pos <= vars->start)
    {
        fprintf(stderr, "DEBUG[handle_right_adj]: No text to extract (pos <= start)\n");
        return ;
    }
    adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!adjacent_text)
    {
        fprintf(stderr, "DEBUG[handle_right_adj]: Failed to extract text\n");
        return ;
    }
    fprintf(stderr, "DEBUG[handle_right_adj]: Extracted '%s'\n", adjacent_text);
    if (vars->current && vars->current->args && vars->current->args[0])
    {
        arg_idx = 0;
        while (vars->current->args[arg_idx + 1])
            arg_idx++;
        fprintf(stderr, "DEBUG[handle_right_adj]: Current token last arg='%s'\n",
                vars->current->args[arg_idx]);
        joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
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


/*
Tokenizes input string. Calls delimiter validation when << is found.
Returns 1 on success, 0 on failure (syntax error or malloc error).
*/
int improved_tokenize(char *input, t_vars *vars)
{
    t_tokentype	token_type;
	char		*raw_delimiter_str;
	int			moves;
    int			heredoc_expecting_delim;
	
	heredoc_expecting_delim = 0;
    vars->pos = 0;
    vars->start = 0;
    vars->quote_depth = 0;
    if (vars->pipes->heredoc_delim)
	{
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    vars->pipes->hd_expand = 0;
    fprintf(stderr, "[TOK_DBG] improved_tokenize: START, input='%.*s...'\n", 20, input); // DEBUG
    while (input && input[vars->pos])
    {
        vars->next_flag = 0;
        token_type = get_token_at(input, vars->pos, &moves);
        fprintf(stderr, "[TOK_DBG] Loop Top: Pos=%d, Char='%c', Start=%d, ExpectDelim=%d\n",
            vars->pos, input[vars->pos] ? input[vars->pos] : '0', vars->start, heredoc_expecting_delim); // DEBUG
        if (heredoc_expecting_delim) {
            fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Current char '%c'\n", input[vars->pos]); // DEBUG
            if (ft_isspace(input[vars->pos]))
			{
                fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Skipping whitespace at pos %d\n", vars->pos); // DEBUG
                vars->pos++;
                vars->start = vars->pos;
                continue ;
            }
            fprintf(stderr, "[TOK_DBG] Expecting Delimiter: Found non-whitespace '%c' at pos %d. Processing delimiter.\n", input[vars->pos], vars->pos); // DEBUG
            vars->start = vars->pos;
            fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Start set to %d\n", vars->start);
            while (input[vars->pos] && !ft_isspace(input[vars->pos]) &&
                   !is_operator_token(get_token_at(input, vars->pos, &moves)))
			{
                vars->pos++;
            }
            fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Loop finished. Pos=%d (Delimiter end)\n", vars->pos); // DEBUG
            if (vars->pos == vars->start)
			{
                tok_syntax_error_msg("newline", vars);
            	return (0);
            }
            raw_delimiter_str = ft_substr(input, vars->start, vars->pos - vars->start);
            if (!raw_delimiter_str)
			{
				vars->error_code = ERR_DEFAULT;
				return (0);
			}
            fprintf(stderr, "[TOK_DBG] Delimiter Extraction: Raw delimiter string = '%s'\n", raw_delimiter_str);
            if (!is_valid_delim(raw_delimiter_str, vars))
			{
                fprintf(stderr, "[TOK_DBG] Delimiter Validation FAILED for '%s'\n", raw_delimiter_str);
                free(raw_delimiter_str);
                return (0);
            }
             fprintf(stderr, "[TOK_DBG] Delimiter Validation SUCCEEDED for '%s'. Stored: '%s', Expand=%d\n",
                     raw_delimiter_str, vars->pipes->heredoc_delim, vars->pipes->hd_expand);
            maketoken(raw_delimiter_str, TYPE_ARGS, vars);
            free(raw_delimiter_str);
            heredoc_expecting_delim = 0;
            fprintf(stderr, "[TOK_DBG] Delimiter Processed: ExpectDelim reset to 0.\n");
            vars->start = vars->pos;
            vars->next_flag = 1;
        }
        if (!vars->next_flag && !heredoc_expecting_delim)
        {
            fprintf(stderr, "[TOK_DBG] Regular Token Handling: Pos=%d, Char='%c', Type=%d\n", vars->pos, input[vars->pos], token_type); // DEBUG
            if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
			{
                fprintf(stderr, "[TOK_DBG] Calling imp_tok_quote\n");
                imp_tok_quote(input, vars);
            }
            else if (input[vars->pos] == '$' && !vars->quote_depth)
			{
                 fprintf(stderr, "[TOK_DBG] Calling imp_tok_expan\n");
                imp_tok_expan(input, vars);
            }
            else if (is_operator_token(token_type))
            {
                 fprintf(stderr, "[TOK_DBG] Handling Operator Token: Type=%d\n", token_type);
                handle_text(input, vars);
                if (token_type == TYPE_HEREDOC)
				{
                    fprintf(stderr, "[TOK_DBG] Operator is HEREDOC (<<)\n");
                    maketoken("<<", TYPE_HEREDOC, vars);
                    vars->pos += 2;
                    vars->start = vars->pos;
                    heredoc_expecting_delim = 1;
                    fprintf(stderr, "[TOK_DBG] Set ExpectDelim=1. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
                }
				else
				{
                    fprintf(stderr, "[TOK_DBG] Handling other operator (calling process_operator_char)\n"); // DEBUG
                    if (!process_operator_char(input, &vars->pos, vars))
					{
                        return (0);
                    }
                }
                vars->next_flag = 1;
            }
            else if (ft_isspace(input[vars->pos]))
			{
                fprintf(stderr, "[TOK_DBG] Calling imp_tok_white\n"); // DEBUG
                imp_tok_white(input, vars);
            }
            else
			{
                fprintf(stderr, "[TOK_DBG] Regular char, advancing pos. Pos=%d -> %d\n", vars->pos, vars->pos + 1); // DEBUG
                // This is where regular characters accumulate before handle_text is called
            }
        }
        if (vars->next_flag)
		{
            fprintf(stderr, "[TOK_DBG] next_flag is set, continuing loop. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
            continue ;
        }
        if (!heredoc_expecting_delim)
		{
            vars->pos++;
        }
    }
    fprintf(stderr, "[TOK_DBG] Loop finished. Pos=%d, Start=%d\n", vars->pos, vars->start); // DEBUG
    if (heredoc_expecting_delim)
	{
        tok_syntax_error_msg("newline", vars);
    	return (0);
    }
    fprintf(stderr, "[TOK_DBG] Processing final text chunk if any (Start=%d, Pos=%d)\n", vars->start, vars->pos); // DEBUG
    handle_text(input, vars);
    DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
    debug_token_list(vars);
    fprintf(stderr, "[TOK_DBG] improved_tokenize: END\n");
    return (1);
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
	t_node *next_node;

    if (!vars || !node)
        return (0);
    DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Adding node type=%d (%s), content='%s'\n",
        node->type, get_token_str(node->type),
        node->args ? node->args[0] : "NULL");
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Set as head %p\n", (void*)node);
        return (0);
    }
    if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS)
    {
        node->type = TYPE_CMD;
    }
    if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD)
    {
        cmd_node = vars->current;
        next_node = node->next;
        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Merging ARG '%s' into CMD '%s'\n",
                  node->args[0], cmd_node->args[0]);
        append_arg(cmd_node, node->args[0], 0);
        if (next_node)
        {
            cmd_node->next = next_node;
            next_node->prev = cmd_node;
        }
		else
		{
            cmd_node->next = NULL;
            vars->current = cmd_node;
        }
        free_token_node(node);
        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: After merge, CMD '%s', next=%p\n",
            cmd_node->args[0], (void*)cmd_node->next);
        return (1);
    }
    else
    {
        DBG_PRINTF(DEBUG_TOKENIZE, "build_token_linklist: Linking node %p to current %p\n",
            (void*)node, (void*)vars->current);
        token_link(node, vars);
        return (0);
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
