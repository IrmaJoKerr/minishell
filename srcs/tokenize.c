/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 16:44:55 by bleow            ###   ########.fr       */
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
// void	free_if_orphan_node(t_node *node, t_vars *vars)
// {
// 	t_node	*check;
// 	int		found_in_list;
	
// 	if (node != vars->head && node != vars->current)
// 	{
// 		found_in_list = 0;
// 		check = vars->head;
// 		while (check && !found_in_list)
// 		{
// 			if (check == node)
// 				found_in_list = 1;
// 			check = check->next;	
// 		}
// 		if (!found_in_list)
// 		{
// 			// fprintf(stderr, "DEBUG[maketoken]: Freeing orphaned node not in list. "
// 			//         "node=%p, type=%d, content='%s'\n",
// 			//         (void*)node, node->type, 
// 			//         (node->args && node->args[0]) ? node->args[0] : "NULL");
// 			free_token_node(node);
// 		}
// 	}
// }
void free_if_orphan_node(t_node *node, t_vars *vars)
{
    fprintf(stderr, "[TOK_DBG] free_if_orphan_node: Entry with node=%p, type=%d\n",
            (void*)node, node->type);
    
    if (node == vars->head || node == vars->current) {
        fprintf(stderr, "[TOK_DBG] free_if_orphan_node: Not orphaned (head/current), returning\n");
        return;
    }
    
    t_node *check;
    int found_in_list = 0;
    
    check = vars->head;
    while (check && !found_in_list) {
        if (check == node) {
            found_in_list = 1;
            fprintf(stderr, "[TOK_DBG] free_if_orphan_node: Node found in list\n");
        }
        check = check->next;    
    }
    
    if (!found_in_list) {
        fprintf(stderr, "[TOK_DBG] free_if_orphan_node: Orphaned node detected, freeing\n");
        free_token_node(node);
    }
}

// Helper function to count arguments in a command
int count_args(char **args)
{
    int count = 0;
    if (!args) return 0;
    
    while (args[count]) {
        count++;
    }
    return count;
}

void debug_token_creation(char *function_name, char *token, t_tokentype type, t_vars *vars)
{
    static int token_count = 0;
    
    fprintf(stderr, "[TOKEN_CREATE] #%d From: %s\n", ++token_count, function_name);
    fprintf(stderr, "  - Token: '%s', Type: %d\n", token, type);
    fprintf(stderr, "  - Prev token: %s\n", vars->current ? 
            (vars->current->args && vars->current->args[0] ? 
             vars->current->args[0] : "NULL") : "NULL");
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

	fprintf(stderr, "[TOK_DBG] maketoken: Entry with token='%s', type=%d\n", 
		token ? token : "NULL", type);
	if (!token || !vars)
	{
        fprintf(stderr, "[TOK_DBG] maketoken: Early return, token=%p, vars=%p\n", 
                token, vars);
        return;
    }
	node = initnode(type, token);
	if (!node)
	{
        fprintf(stderr, "[TOK_DBG] maketoken: initnode failed for '%s'\n", token);
        return;
    }
	fprintf(stderr, "[TOK_DBG] maketoken: Created node=%p, type=%d, content='%s'\n",
            (void*)node, node->type, node->args[0]);
    
    fprintf(stderr, "[TOK_DBG] maketoken: Calling build_token_linklist\n");
    node_freed = build_token_linklist(vars, node);
    
    fprintf(stderr, "[TOK_DBG] maketoken: build_token_linklist returned %d (1=node_freed)\n", 
            node_freed);
    
	// node_freed = build_token_linklist(vars, node);
	if (!node_freed)
	{
        fprintf(stderr, "[TOK_DBG] maketoken: Calling free_if_orphan_node\n");
        free_if_orphan_node(node, vars);
    }
	fprintf(stderr, "[TOK_DBG] maketoken: Current token list:");
    t_node *curr = vars->head;
    while (curr) {
        fprintf(stderr, " [%d:%s]", curr->type, 
                (curr->args && curr->args[0]) ? curr->args[0] : "NULL");
        curr = curr->next;
    }
    fprintf(stderr, "\n");
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

/*
Process any accumulated text before a special character.
Creates a token from the text between vars->start and vars->pos.
*/
void	handle_text(char *input, t_vars *vars)
{
	char	*token_preview;

	token_preview = NULL;
	if (vars->pos > vars->start)
	{
		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
		set_token_type(vars, token_preview);
		handle_string(input, vars);
		if (token_preview)
		{
			free(token_preview);
		}
		vars->start = vars->pos;
	}
}

/*
Processes quoted text in the input string.
- Detects if quote is a redirection target based on previous token
- Handles any accumulated text before the quote
- Calls process_quote_char to extract and process quoted content
- Sets next_flag if processing succeeds
- Restores original position on failure
*/
void	tokenize_quote(char *input, t_vars *vars)
{
	int	is_redir_target;
	int	saved_pos;

	is_redir_target = 0;
	if (vars->prev_type == TYPE_IN_REDIRECT
		|| vars->prev_type == TYPE_OUT_REDIRECT
		|| vars->prev_type == TYPE_APPEND_REDIRECT
		|| vars->prev_type == TYPE_HEREDOC)
	{
		is_redir_target = 1;
	}
	handle_text(input, vars);
	saved_pos = vars->pos;
	if (process_quote_char(input, vars, is_redir_target))
	{
		vars->next_flag = 1;
	}
	else
	{
		vars->pos = saved_pos;
	}
}

/*
Processes variable expansion tokens in the input string.
- Handles any accumulated text before the $ character
- Extracts and processes the expanded variable token
- Sets next_flag if expansion processing succeeds
- Updates position markers for continued tokenization
Triggered when $ character is encountered outside quotes.
*/
void	tokenize_expan(char *input, t_vars *vars)
{
	handle_text(input, vars);
	if (make_exp_token(input, vars))
		vars->next_flag = 1;
}

/*
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
void	tokenize_white(char *input, t_vars *vars)
{
	handle_text(input, vars);
	while (input[vars->pos] && input[vars->pos] <= ' ')
	{
		vars->pos++;
	}
	vars->start = vars->pos;
	vars->next_flag = 1;
}

/*
Processes right-adjacent text for the current token.
- Extracts text between vars->start and vars->pos
- Joins this text with the last argument of the current token
- Handles memory allocation and cleanup
- Updates token content to include adjacent characters
Example: 
For "echo hello"world, joins "world" to "hello"
    to create a single argument "helloworld"
*/
void	handle_right_adj(char *input, t_vars *vars)
{
	char	*adjacent_text;
	char	*joined;
	int		arg_idx;

	if (vars->pos <= vars->start)
		return ;
	adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!adjacent_text)
		return ;
	if (vars->current && vars->current->args && vars->current->args[0])
	{
		arg_idx = 0;
		while (vars->current->args[arg_idx + 1])
			arg_idx++;
		joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
		if (joined)
		{
			free(vars->current->args[arg_idx]);
			vars->current->args[arg_idx] = joined;
		}
	}
	free(adjacent_text);
}

/*
Helper function to link a new token node to the current node.
- Sets the next pointer of the current node to the new node.
- Sets the prev pointer of the new node to the current node.
- Updates the current pointer to the new node.
- Works with build_token_linklist().
*/
// void	token_link(t_node *node, t_vars *vars)
// {
// 	vars->current->next = node;
// 	node->prev = vars->current;
// 	vars->current = node;
// }
void token_link(t_node *node, t_vars *vars)
{
    fprintf(stderr, "[TOK_DBG] token_link: Entry with node=%p, type=%d, content='%s'\n",
            (void*)node, node->type, node->args[0]);
    
    fprintf(stderr, "[TOK_DBG] token_link: Before - current=%p\n", 
            (void*)vars->current);
    
    // Check if this is an operator token
    if (is_operator_token(node->type)) {
        fprintf(stderr, "[TOK_DBG] token_link: Linking OPERATOR token\n");
    }
    
    vars->current->next = node;
    node->prev = vars->current;
    vars->current = node;
    
    fprintf(stderr, "[TOK_DBG] token_link: After - current=%p, prev=%p\n",
            (void*)vars->current, (void*)vars->current->prev);
}

/*
Merges an argument node into a command node.
- Appends argument text to command's argument list
- Updates linked list connections after merging
- Frees the now-redundant argument node
Returns:
- 1 to indicate node was merged and old arg node freed.
*/
// int	merge_arg_with_cmd(t_vars *vars, t_node *arg_node)
// {
// 	t_node	*cmd_node;
// 	t_node	*next_node;

// 	cmd_node = vars->current;
// 	next_node = arg_node->next;
// 	append_arg(cmd_node, arg_node->args[0], 0);
// 	if (next_node)
// 	{
// 		cmd_node->next = next_node;
// 		next_node->prev = cmd_node;
// 	}
// 	else
// 	{
// 		cmd_node->next = NULL;
// 		vars->current = cmd_node;
// 	}
// 	free_token_node(arg_node);
// 	return (1);
// }
int merge_arg_with_cmd(t_vars *vars, t_node *arg_node)
{
    fprintf(stderr, "[TOK_DBG] merge_arg_with_cmd: Entry with arg_node=%p, content='%s'\n",
            (void*)arg_node, arg_node->args[0]);
    
    t_node *cmd_node;
    t_node *next_node;

    cmd_node = vars->current;
    next_node = arg_node->next;
    
    fprintf(stderr, "[TOK_DBG] merge_arg_with_cmd: cmd_node=%p, args_count=%d\n",
            (void*)cmd_node, count_args(cmd_node->args));
    
    fprintf(stderr, "[TOK_DBG] merge_arg_with_cmd: Calling append_arg with '%s'\n",
            arg_node->args[0]);
            
    append_arg(cmd_node, arg_node->args[0], 0);
    
    if (next_node) {
        fprintf(stderr, "[TOK_DBG] merge_arg_with_cmd: Linking with next_node=%p\n",
                (void*)next_node);
        cmd_node->next = next_node;
        next_node->prev = cmd_node;
    }
    else {
        fprintf(stderr, "[TOK_DBG] merge_arg_with_cmd: No next node, cmd_node becomes current\n");
        cmd_node->next = NULL;
        vars->current = cmd_node;
    }
    
    fprintf(stderr, "[TOK_DBG] merge_arg_with_cmd: Freeing arg_node=%p\n", 
            (void*)arg_node);
    free_token_node(arg_node);
    return (1);
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
// int	build_token_linklist(t_vars *vars, t_node *node)
// {
// 	if (!vars || !node)
// 		return (0);
// 	if (!vars->head)
// 	{
// 		vars->head = node;
// 		vars->current = node;
// 		return (0);
// 	}
// 	if (vars->current && vars->current->type == TYPE_PIPE
// 		&& node->type == TYPE_ARGS)
// 		node->type = TYPE_CMD;
// 	if (node->type == TYPE_ARGS && vars->current
// 		&& vars->current->type == TYPE_CMD)
// 		return (merge_arg_with_cmd(vars, node));
// 	else
// 	{
// 		token_link(node, vars);
// 		return (0);
// 	}
// }
int build_token_linklist(t_vars *vars, t_node *node)
{
    fprintf(stderr, "[TOK_DBG] build_token_linklist: Entry with node=%p, type=%d, content='%s'\n",
            (void*)node, node->type, node->args[0]);
    
    if (!vars || !node) {
        fprintf(stderr, "[TOK_DBG] build_token_linklist: Early return, vars=%p, node=%p\n", 
                vars, node);
        return (0);
    }
    
    if (!vars->head) {
        fprintf(stderr, "[TOK_DBG] build_token_linklist: First token in list\n");
        vars->head = node;
        vars->current = node;
        return (0);
    }
    
    fprintf(stderr, "[TOK_DBG] build_token_linklist: Current head=%p, current=%p\n",
            (void*)vars->head, (void*)vars->current);
    
    fprintf(stderr, "[TOK_DBG] build_token_linklist: Current->type=%d, node->type=%d\n",
            vars->current->type, node->type);
    
    // Check if node after pipe should become a command
    if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS) {
        fprintf(stderr, "[TOK_DBG] build_token_linklist: Converting ARGS to CMD after PIPE\n");
        node->type = TYPE_CMD;
    }
    
    // Check if this is an argument that should be merged with a command
    if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD) {
        fprintf(stderr, "[TOK_DBG] build_token_linklist: Merging arg with command\n");
        return (merge_arg_with_cmd(vars, node)); // Returns 1 (node freed)
    }
    else {
        fprintf(stderr, "[TOK_DBG] build_token_linklist: Linking node to list\n");
        token_link(node, vars);
        return (0); // Node not freed
    }
}
