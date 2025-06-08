/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proc_quoted_redir_tgt.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 10:46:04 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 03:08:43 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
int	proc_quoted_redir_tgt(char *content, t_vars *vars)
{
	t_node	*redir_node;
	int		status;

	redir_node = find_last_redir(vars);
	if (!redir_node)
	{
		ft_safefree((void **)&content);
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
Finds the most relevant redirection node in the token list.
Checks in the following order of priority:
 - Current node if it's a redirection
 - Recent previous nodes (up to 3 back)
 - Last redirection in the entire token list
Returns:
 - Pointer to the located redirection node
 - NULL if no redirection node exists
Works with process_quote_char() for redirection target handling.
*/
t_node	*find_last_redir(t_vars *vars)
{
	t_node	*current;
	t_node	*last_redir;
	int		i;

	last_redir = NULL;
	if (vars->current && is_redirection(vars->current->type))
		return (vars->current);
	current = vars->current;
	i = 0;
	while (current && current->prev && i < 3)
	{
		current = current->prev;
		i++;
		if (is_redirection(current->type))
			return (current);
	}
	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
			last_redir = current;
		current = current->next;
	}
	return (last_redir);
}

/*
Attempts to merge new content with an existing redirection target's argument.
- Checks for left adjacency (vars->adj_state[0]).
- Checks that the redirection node (redir_node) has an existing target
	(redir_node->right) with at least one argument (redir_node->right->args[0])
- If conditions are met, concatenates the existing argument with the new content.
Returns:
- 1 if content was successfully merged with the existing target.
- 0 if a memory allocation error occurred during merging.
- -1 if merging was not applicable.
Works with proc_quoted_redir_tgt().
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
			ft_safefree((void **)&redir_node->right->args[0]);
			redir_node->right->args[0] = merged;
			ft_safefree((void **)&content);
			return (1);
		}
		else
		{
			ft_safefree((void **)&content);
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
		ft_safefree((void **)&content);
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
		ft_safefree((void **)&content);
		vars->error_code = ERR_SYNTAX;
		return (0);
	}
	file_node = initnode(TYPE_ARGS, content);
	if (!file_node)
	{
		ft_safefree((void **)&content);
		if (vars->error_code == 0)
			vars->error_code = ERR_DEFAULT;
		return (0);
	}
	link_file_to_redir(redir_node, file_node, vars);
	ft_safefree((void **)&content);
	return (1);
}
