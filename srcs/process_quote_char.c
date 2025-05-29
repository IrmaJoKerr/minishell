/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 05:36:38 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process quoted content, including variable expansion in double quotes.
Finds the appropriate command node for the content.
Modifies the content pointer if expansion is needed.
Returns:
- The command node.
- NULL if not found/error.
*/
t_node	*process_quoted_str(char **content_ptr, int quote_type, t_vars *vars)
{
	t_node	*cmd_node;
	char	*expanded;

	if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(*content_ptr, '$'))
	{
		expanded = expand_quoted_str(*content_ptr, vars);
		free(*content_ptr);
		if (!expanded)
			return (NULL);
		*content_ptr = expanded;
	}
	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
	return (cmd_node);
}

/*
Links a file node into the token list after a redirection node.
- Sets redirection->right pointer to the file node
- Handles proper linked list connections
- Updates vars->current pointer
Works with handle_redir_target().
*/
void	link_file_to_redir(t_node *redir_node, t_node *file_node, t_vars *vars)
{
	redir_node->right = file_node;
	if (redir_node->next)
	{
		file_node->next = redir_node->next;
		redir_node->next->prev = file_node;
	}
	redir_node->next = file_node;
	file_node->prev = redir_node;
	vars->current = file_node;
}

/*
Attempts to merge new content with an existing redirection target's argument.
- Checks for left adjacency (vars->adj_state[0]).
- Checks that the redirection node (redir_node) has an existing target
	(redir_node->right) with at least one argument (redir_node->right->args[0])
- If conditions are met, concatenates the existing argument with the new content.
- Manages memory by freeing the old argument and the input content upon
successful merge.
- Sets error_code and frees content if string joining fails.
Returns:
- 1 if content was successfully merged with the existing target.
- 0 if a memory allocation error occurred during merging.
- -1 if merging was not applicable.
Works with handle_redir_target().
*/
int	try_merge_adj_target(t_node *redir_node, char *content,
				t_vars *vars)
{
	char	*existing;
	char	*merged;

	if (vars->adj_state[0] && redir_node->right && redir_node->right->args
		&& redir_node->right->args[0])
	{
		existing = redir_node->right->args[0];
		merged = ft_strjoin(existing, content);
		if (merged)
		{
			free(redir_node->right->args[0]);
			redir_node->right->args[0] = merged;
			free(content);
			return (1);
		}
		else
		{
			free(content);
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
	}
	return (-1);
}

/*
Attempts to append content as an argument to the last command.
This is called if a redirection node already has a target, but merging
was not applicable.
Return:
- 1 if successfully appended (content is freed).
- 0 if no command found to append to (content is NOT freed).
*/
int	try_append_to_prev_cmd(char *content, t_vars *vars)
{
	t_node	*cmd_node;

	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
	if (cmd_node)
	{
		append_arg(cmd_node, content, TYPE_DOUBLE_QUOTE);
		free(content);
		return (1);
	}
	return (0);
}

/*
Creates a new file node from content and links it to the redirection node.
This is the primary way a redirection target is established if one doesn't exist
or if previous conditional handling didn't consume the content.
- Return:
- 1 on success (content is freed).
- 0 on failure (e.g., initnode fails, content is freed).
*/
int	link_new_file_node_to_redir(t_node *redir_node,
		char *content, t_vars *vars)
{
	t_node	*file_node;

	if (!is_redirection(redir_node->type))
	{
		free(content);
		vars->error_code = ERR_SYNTAX;
		return (0);
	}
	file_node = initnode(TYPE_ARGS, content);
	if (!file_node)
	{
		free(content);
		if (vars->error_code == 0)
			vars->error_code = ERR_DEFAULT;
		return (0);
	}
	link_file_to_redir(redir_node, file_node, vars);
	free(content);
	return (1);
}

/*
Handles quoted text as a redirection target.
- Finds the relevant redirection node in the token list.
- Attempts to merge with existing target if adjacent.
- Else, attempts to append to last command if target already exists.
- Else, creates a new file node and links it to the redirection operator.
- Sets error code if no valid redirection is found or other errors occur.
Returns:
- 1 on success
- 0 on failure
Works with process_quote_char() for handling quoted filenames.
Example:
- For "echo > "file.txt"", creates file node for "file.txt".
*/
int	handle_redir_target(char *content, t_vars *vars)
{
	t_node	*redir_node;
	int		status;

	redir_node = find_last_redir(vars);
	if (!redir_node)
	{
		free(content);
		vars->error_code = ERR_SYNTAX;
		return (0);
	}
	if (redir_node->right != NULL)
	{
		status = try_merge_adj_target(redir_node, content, vars);
		if (status == 1)
			return (1);
		if (status == 0)
			return (0);
		if (try_append_to_prev_cmd(content, vars))
			return (1);
	}
	return (link_new_file_node_to_redir(redir_node, content, vars));
}


/*
Extracts content from quotes, saving and restoring parser position and
adjacency state. Called by process_quote_char().
- Populates content_out and quote_type_out.
- Returns:
- 1 on success.
- 0 on failure.
*/
int	extract_quoted_content_with_saved_state(char *input, t_vars *vars,
				char **content_out, int *quote_type_out)
{
	int		saved_adj[3];
	int		saved_pos;

	saved_pos = vars->pos;
	check_token_adj(input, vars);
	saved_adj[0] = vars->adj_state[0];
	saved_adj[1] = vars->adj_state[1];
	saved_adj[2] = vars->adj_state[2];
	*content_out = get_quoted_str(input, vars, quote_type_out);
	if (!*content_out)
	{
		vars->pos = saved_pos;
		return (0);
	}
	vars->adj_state[0] = saved_adj[0];
	vars->adj_state[1] = saved_adj[1];
	vars->adj_state[2] = saved_adj[2];
	return (1);
}


int	handle_fully_adjacent_empty_quote(char *input, t_vars *vars,
				char *current_content, int current_quote_type)
{
	t_node	*last_token;

	if (current_content && *current_content == '\0' && current_quote_type != 0)
	{
		if (vars->adj_state[0] && vars->adj_state[1])
		{
			last_token = vars->head;
			if (last_token)
			{
				while (last_token->next)
					last_token = last_token->next;
				vars->current = last_token;
			}
			free(current_content);
			process_right_adj(input, vars);
			return (1);
		}
	}
	return (0);
}

int	attempt_adhoc_left_merge_with_redir(char *current_content, t_vars *vars)
{
	t_node	*redir_node;
	char	*existing;
	char	*merged;

	redir_node = find_last_redir(vars);
	if (redir_node && redir_node->right && redir_node->right->args
		&& redir_node->right->args[0])
	{
		existing = redir_node->right->args[0];
		merged = ft_strjoin(existing, current_content);
		if (merged)
		{
			free(redir_node->right->args[0]);
			redir_node->right->args[0] = merged;
			free(current_content);
			return (1);
		}
		else
		{
			free(current_content);
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
	}
	return (-1);
}

int	process_general_quoted_string(char *input, t_vars *vars,
				char *current_content, int current_quote_type)
{
	t_node	*cmd_node;

	cmd_node = process_quoted_str(&current_content, current_quote_type, vars);
	if (!cmd_node && vars->adj_state[0] == 0)
		return (make_quoted_cmd(current_content, input, vars));
	else if (!cmd_node)
	{
		if (!merge_quoted_token(input, current_content, vars))
			return (token_cleanup_error(current_content, vars));
		return (1);
	}
	if (!merge_quoted_token(input, current_content, vars))
	{
		append_arg(cmd_node, current_content, current_quote_type);
		if (vars->adj_state[1] && current_content && *current_content != '\0')
			cleanup_and_process_adj(current_content, input, vars);
		else
			free(current_content);
	}
	return (1);
}

/*
Master control function for processing quoted text in shell input.
This function controls the complete handling of quoted strings:
 - Extracts content from between quotes (single or double).
 - Handles variable expansion for double-quoted text.
 - Creates appropriate token structures based on context.
 - Processes special cases like redirection targets (">file.txt").
 - Manages token adjacency and merging.
Return:
 - 1 on successful processing.
 - 0 on extraction failure or other errors.
Example flows:
 - "echo hello" -> Creates command with argument.
 - echo "hello world" -> Appends argument to existing command.
 - echo > "file.txt" -> Creates file node for redirection.
*/
int	process_quote_char(char *input, t_vars *vars, int redir_tgt_flag)
{
	char	*content;
	int		quote_type;
	int		result_status;

	if (!extract_quoted_content_with_saved_state(input, vars,
			&content, &quote_type))
		return (0);
	result_status = handle_fully_adjacent_empty_quote(input, vars,
			content, quote_type);
	if (result_status == 1)
		return (1);
	if (vars->adj_state[0] && !redir_tgt_flag)
	{
		result_status = attempt_adhoc_left_merge_with_redir(content, vars);
		if (result_status == 1)
			return (1);
		if (result_status == 0)
			return (0);
	}
	if (redir_tgt_flag)
		return (handle_redir_target(content, vars));
	return (process_general_quoted_string(input, vars, content, quote_type));
}

/*
Validates that all redirection operators in the token list have valid targets.
- Ensures no redirection is at the end of input
- Ensures no redirection is followed by another operator
- Reports appropriate syntax errors
Returns:
- 1 if all redirections have valid targets
- 0 otherwise (with error_code set)
*/
int	validate_redir_targets(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (!current->args || !current->args[0])
			{
				tok_syntax_error_msg("newline", vars);
				return (0);
			}
		}
		current = current->next;
	}
	return (1);
}
