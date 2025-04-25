/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 01:01:35 by bleow            ###   ########.fr       */
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
Handles quoted text as a redirection target.
- Finds the relevant redirection node in the token list.
- Creates a file node with the quoted content.
- Links the file node to the redirection operator.
- Sets error code if no valid redirection is found.
Returns:
- 1 on success (content is freed)
- 0 on failure (content is freed)
Works with process_quote_char() for handling quoted filenames.
Example:
- For "echo > "file.txt"", creates file node for "file.txt".
*/
int	handle_redir_target(char *content, t_vars *vars)
{
	t_node	*redir_node;
	t_node	*file_node;

	redir_node = find_last_redir(vars);
	if (redir_node && is_redirection(redir_node->type))
	{
		file_node = initnode(TYPE_ARGS, content);
		if (!file_node)
		{
			free(content);
			return (0);
		}
		link_file_to_redir(redir_node, file_node, vars);
		free(content);
		return (1);
	}
	free(content);
	vars->error_code = ERR_SYNTAX;
	return (0);
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
int	process_quote_char(char *input, t_vars *vars, int is_redir_target)
{
	int		quote_type;
	char	*content;
	t_node	*cmd_node;

	content = get_quoted_str(input, vars, &quote_type);
	if (!content)
		return (0);
	if (is_redir_target)
		return (handle_redir_target(content, vars));
	cmd_node = (process_quoted_str(&content, quote_type, vars));
	if (!cmd_node && vars->adj_state[0] == 0)
		return (make_quoted_cmd(content, input, vars));
	else if (!cmd_node)
	{
		if (!merge_quoted_token(input, content, vars))
			return (token_cleanup_error(content, vars));
		return (1);
	}
	if (!merge_quoted_token(input, content, vars))
	{
		append_arg(cmd_node, content, quote_type);
		cleanup_and_process_adj(content, input, vars);
	}
	return (1);
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
			if (!validate_single_redir(current, vars))
				return (0);
		}
		current = current->next;
	}
	return (1);
}
