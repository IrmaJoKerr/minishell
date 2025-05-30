/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 12:56:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

	if (!extract_quoted_strs(input, vars,
			&content, &quote_type))
		return (0);
	result_status = handle_empty_quote(input, vars,
			content, quote_type);
	if (result_status == 1)
		return (1);
	if (vars->adj_state[0] && !redir_tgt_flag)
	{
		result_status = try_left_merge(content, vars);
		if (result_status == 1)
			return (1);
		if (result_status == 0)
			return (0);
	}
	if (redir_tgt_flag)
		return (proc_quoted_redir_tgt(content, vars));
	return (handle_quo_str(input, vars, content, quote_type));
}

/*
Extracts content from quotes, saving and restoring parser position and
adjacency state. Called by process_quote_char().
- Populates content_out and quote_type_out.
- Returns:
- 1 on success.
- 0 on failure.
*/
int	extract_quoted_strs(char *input, t_vars *vars,
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

/*
Handles special case of empty quotes between adjacent tokens.
- Detects empty quoted strings ('' or "") with bidirectional adjacency
- Updates the current token pointer if needed
- Processes right adjacency when applicable
Returns:
- 1 if handled empty quotes successfully
- 0 if not an empty quote or not bidirectionally adjacent
Works with process_quote_char().
*/
int	handle_empty_quote(char *input, t_vars *vars, char *curr_text,
		int curr_quo_type)
{
	t_node	*last_token;

	if (curr_text && *curr_text == '\0' && curr_quo_type != 0)
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
			free(curr_text);
			process_right_adj(input, vars);
			return (1);
		}
	}
	return (0);
}

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
Processes quoted strings in normal command contexts.
- Tries to find existing command node for quoted content
- Creates new command if no command exists and no left adjacency
- Merges with previous token if left-adjacent with no command
- Appends as argument to existing command when found
- Handles right adjacency and memory management
Returns:
- 1 on successful processing
- 0 on failure (memory error)
Works with process_quote_char() for non-redirection quotes.
*/
int	handle_quo_str(char *input, t_vars *vars,
				char *curr_text, int curr_quo_type)
{
	t_node	*cmd_node;

	cmd_node = process_quoted_str(&curr_text, curr_quo_type, vars);
	if (!cmd_node && vars->adj_state[0] == 0)
		return (make_quoted_cmd(curr_text, input, vars));
	else if (!cmd_node)
	{
		if (!merge_quoted_token(input, curr_text, vars))
			return (token_cleanup_error(curr_text, vars));
		return (1);
	}
	if (!merge_quoted_token(input, curr_text, vars))
	{
		append_arg(cmd_node, curr_text, curr_quo_type);
		if (vars->adj_state[1] && curr_text && *curr_text != '\0')
			cleanup_and_process_adj(curr_text, input, vars);
		else
			free(curr_text);
	}
	return (1);
}
