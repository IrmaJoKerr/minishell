/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/11 00:41:52 by bleow            ###   ########.fr       */
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
	int			moves;
	t_tokentype	special_type;
	// Save previous type
	special_type = 0;
	vars->prev_type = vars->curr_type;
	// Check for variable expansion
	if (input && input[0] == '$')
	{
		vars->curr_type = TYPE_EXPANSION;
		return;
	}
	// Check for special token types
	if (input && *input)
	{
		special_type = get_token_at(input, 0, &moves);
		if (special_type != 0)
		{
			vars->curr_type = special_type;
			return ;
		}
	}
	// Default to position-based command/argument detection
	if (!vars->head || vars->prev_type == TYPE_PIPE)
		vars->curr_type = TYPE_CMD;
	else
		vars->curr_type = TYPE_ARGS;
}

// Helper function to count nodes in a linked list.DEBUG FUNCTION
size_t count_nodes(t_node *head)
{
	size_t count = 0;
	while (head)
	{
		count++;
		head = head->next;
	}
	return count;
}

void maketoken_with_type(char *token, t_tokentype type, t_vars *vars)
{
	t_node *node;
	int node_freed;
	
	node = initnode(type, token);
	if (!node)
		return;
	
	// Check if node was freed by build_token_linklist_with_status
	node_freed = build_token_linklist_with_status(vars, node);
	
	// Only try to free if it wasn't already freed
	if (!node_freed)
	{
		// Additional checks for orphaned nodes
		size_t original_count = count_nodes(vars->head);
		if (count_nodes(vars->head) == original_count)
		{
			if (node != vars->head && node != vars->current)
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

int is_adjacent_token(char *input, int pos)
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
int make_exp_token(char *input, int *i, t_vars *vars)
{
	char		*token;
	t_tokentype	type;
	
	vars->start = *i;
	if (input[*i] == '$' && input[*i + 1] == '?') 
	{
		token = ft_substr(input, vars->start, 2);
		if (!token)
			return (0); 
		type = TYPE_EXIT_STATUS;
		(*i) += 2;
	}
	else if (input[*i] == '$')
	{
		(*i)++;
		while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
			(*i)++; 
		token = ft_substr(input, vars->start, *i - vars->start);
		if (!token)
			return (0);
		type = TYPE_EXPANSION;
	}
	else
		return (0);
	maketoken_with_type(token, type, vars);
	free(token);
	vars->start = *i;
	return (1);
}

/*
Process quoted text during tokenization
- Extracts quote content
- Sets appropriate character-level quote types
- Handles joining adjacent quoted strings
*/
int process_quote_char(char *input, int *i, t_vars *vars)
{
	int     quote_start;
	char    quote_char;
	int     quote_type;
	char    *content;
	t_node  *cmd_node;
	int     is_adjacent;
	int     quote_len;
	int     last_arg_idx;
	char    *joined;
	
	// Get quote character and determine quote type
	quote_char = input[*i];
	is_adjacent = is_adjacent_token(input, *i);
	quote_type = (quote_char == '\'') ? TYPE_SINGLE_QUOTE : TYPE_DOUBLE_QUOTE;
	
	// Extract quoted content
	quote_start = ++(*i);
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	// Handle unclosed quotes
	if (!input[*i])
	{
		*i = quote_start;
		return (0);
	}
	// Calculate quoted content length
	quote_len = *i - quote_start;
	fprintf(stderr, "DEBUG: Quote handling - quote_len=%d, quote_char=%c\n", 
			quote_len, quote_char); 
	// Extract quoted content
	content = ft_substr(input, quote_start, quote_len);
	if (!content)
		return (0);
	// Move past closing quote
	(*i)++;
	// Debug messaging
	if (quote_len == 0)
		fprintf(stderr, "DEBUG: Empty quote detected\n");
	fprintf(stderr, "DEBUG: Setting quote_type=%d for content='%s'\n", 
			quote_type, content);
	// Process variables in double-quoted strings
	if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(content, '$'))
	{
		char *expanded = ft_strdup("");
		int pos = 0;
		char *chunk;
		char *temp;
		if (!expanded)
		{
			free(content);
			return (0);
		}
		fprintf(stderr, "DEBUG: Pre-processing double-quoted content '%s'\n", content);
		// Process variable expansions in double quotes
		while (content[pos])
		{
			if (content[pos] == '$')
			{
				// Use handle_expansion to process the variable
				char *var_value = handle_expansion(content, &pos, vars);
				if (var_value)
				{
					// Concatenate the expanded value
					temp = expanded;
					expanded = ft_strjoin(expanded, var_value);
					free(temp);
					free(var_value);
					
					if (!expanded)
					{
						free(content);
						return (0);
					}
				}
			}
			else
			{
				// Handle regular character
				int start = pos;
				while (content[pos] && content[pos] != '$')
					pos++;
				chunk = ft_substr(content, start, pos - start);
				if (!chunk)
				{
					free(expanded);
					free(content);
					return (0);
				}
				temp = expanded;
				expanded = ft_strjoin(expanded, chunk);
				free(temp);
				free(chunk);
				if (!expanded)
				{
					free(content);
					return (0);
				}
			}
		}
		fprintf(stderr, "DEBUG: Expanded double-quoted content from '%s' to '%s'\n", 
				content, expanded);
		free(content);
		content = expanded;
	}
	// Handle adjacent token (joining quotes)
	if (is_adjacent)
	{
		cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
		if (cmd_node && cmd_node->args)
		{
			// Find last argument
			last_arg_idx = 0;
			while (cmd_node->args[last_arg_idx+1])
				last_arg_idx++;
			// Debug quote types
			if (cmd_node->arg_quote_type)
				fprintf(stderr, "DEBUG: Original arg '%s' had quote_type array\n", 
					cmd_node->args[last_arg_idx]);
			// Join adjacent tokens
			joined = ft_strjoin(cmd_node->args[last_arg_idx], content);
			fprintf(stderr, "DEBUG: Joining quote '%s' with '%s' = '%s'\n",
				   cmd_node->args[last_arg_idx], content, joined);
			if (joined)
			{
				// Get original argument length
				int orig_len = ft_strlen(cmd_node->args[last_arg_idx]);
				// Get new content length
				int new_len = ft_strlen(content);
				// Get total length
				int total_len = orig_len + new_len;
				// Create new character-level quote type array
				int *new_quote_types = malloc(sizeof(int) * (total_len + 1));
				if (!new_quote_types)
				{
					free(joined);
					free(content);
					return (0);
				}
				// Copy original quote types if they exist
				if (cmd_node->arg_quote_type && cmd_node->arg_quote_type[last_arg_idx])
				{
					int j = 0;
					while (j < orig_len)
					{
						new_quote_types[j] = cmd_node->arg_quote_type[last_arg_idx][j];
						j++;
					}
				}
				else
				{
					// Default to unquoted if original didn't have quote types
					int j = 0;
					while (j < orig_len)
					{
						new_quote_types[j] = 0;
						j++;
					}
				}
				// Set quote types for new content
				int j = 0;
				while (j < new_len)
				{
					new_quote_types[orig_len + j] = quote_type;
					j++;
				}
				// End marker
				new_quote_types[total_len] = -1;
				// Update the argument
				free(cmd_node->args[last_arg_idx]);
				cmd_node->args[last_arg_idx] = joined;
				// Update the quote type array
				if (cmd_node->arg_quote_type && cmd_node->arg_quote_type[last_arg_idx])
				{
					free(cmd_node->arg_quote_type[last_arg_idx]);
				}
				else if (!cmd_node->arg_quote_type)
				{
					// If arg_quote_type doesn't exist, create it
					int arg_count = last_arg_idx + 1;
					cmd_node->arg_quote_type = malloc(sizeof(int*) * (arg_count + 1));
					if (!cmd_node->arg_quote_type)
					{
						free(new_quote_types);
						free(content);
						return (0); 
					}
					// Initialize others to NULL
					int k = 0;
					while (k < last_arg_idx)
					{
						cmd_node->arg_quote_type[k] = NULL;
						k++;
					}
					cmd_node->arg_quote_type[arg_count] = NULL;
				}
				cmd_node->arg_quote_type[last_arg_idx] = new_quote_types;
				fprintf(stderr, "DEBUG: After join, created character-level quote types\n");
				free(content);
				vars->start = *i;
				return (1);
			}
		}
	}
	// Handle non-adjacent quotes (new arguments)
	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
	if (cmd_node)
	{
		// Append as a new argument
		append_arg(cmd_node, content, quote_type);
	}
	else
	{
		// Create a new token
		if (!vars->head)
			maketoken_with_type(content, TYPE_CMD, vars);
		else
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

Example: For input "cmd > file"
- Processes the '>' operator
- Creates redirect token
- Returns 1 to indicate operator was handled
*/
int process_operator_char(char *input, int *i, t_vars *vars)
{
	int moves;
	t_tokentype token_type;
	
	token_type = get_token_at(input, *i, &moves);
			   
	if (token_type == 0)
	{
		return (0);
	}
	vars->curr_type = token_type;   
	// Handle double operators (>>, <<, etc.)
	if (moves == 2) {
		handle_double_operator(input, vars);
	}
	
	// Handle single operators (>, <, |, etc.)
	else {
		handle_single_operator(input, vars);
	}
	
	return (1);
}

/*
Processes redirection operators in the input.
- Handles <, >, << and >> operators.
- Creates tokens for the operators.
- Updates position past the operator.

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

/*
Handle quoted text in the input.
Processes single and double quotes.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_quote(char *input, t_vars *vars)
{
	handle_text(input, vars);
	if (process_quote_char(input, &vars->pos, vars))
		vars->next_flag = 1;
}

/*
Handle variable expansion in the input.
Processes $ characters for variable substitution.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_expan(char *input, t_vars *vars)
{
	handle_text(input, vars);
	if (make_exp_token(input, &vars->pos, vars))
		vars->next_flag = 1;
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
			continue;
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
int build_token_linklist_with_status(t_vars *vars, t_node *node)
{
	t_node *cmd_node;
	
	if (!vars || !node) 
		return 0;
	if (!vars->head)
	{
		vars->head = node;
		vars->current = node;
		return 0;
	}
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
		return 1;  // Indicate that the node was freed
	}
	else
	{
		token_link(node, vars);
		return 0;  // Node was not freed
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
