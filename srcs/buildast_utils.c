/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:05:30 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 16:06:43 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Associates argument tokens with their corresponding command tokens.
- Scans token linklist to identify commands and their args.
- Appends ARGS tokens to CMD tokens not redirection targets (filenames).

Process flow:
- Tracks current active command as it encounters CMD tokens.
- For each ARGS token, checks if it's a redirection target before appending.
- Resets command context when encountering pipe operators.

Example: For "cat file.txt > output.txt | grep pattern":
- Links "file.txt" with "cat" command.
- Skips "output.txt" (redirection target).
- Links "pattern" to "grep" command node.
*/
void	chk_args_match_cmd(t_vars *vars)
{
	t_node	*current;
	t_node	*node;
	int		is_target;
	int		is_heredoc;

	node = NULL;
	if (!vars || !vars->head)
		return ;
	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_CMD)
			node = current;
		else if (current->type == TYPE_ARGS && node)
		{
			is_target = is_redirection_target(current, vars);
			is_heredoc = is_heredoc_target(current, vars);
			if (!is_target && !is_heredoc)
				append_arg(node, current->args[0], 0);
		}
		else if (current->type == TYPE_PIPE)
			node = NULL;
		current = current->next;
	}
}

/*
Analyses if a token is the filename target of a redirection operator.
Prevents redirection filenames from being incorrectly added as command args.
- Scans the token list to find redirections before the current node.
Detection logic:
- If node follows a redirection with filename in arg[0]: NOT a target.
- If node follows a redirection without filename: IS a target.
- If node doesn't follow any redirection: NOT a target.
Example:
- [CMD: "cat"] [>: "output.txt"] [ARGS: "extra"] → "extra" == 0 (not a target)
- [CMD: "cat"] [>: ""] [ARGS: "output.txt"] → "output.txt" == 1 (is a target)

Returns:
- 0 if node is NOT a redirection target (Ok to append to command).
- 1 if node IS a redirection target (Don't append to command).
*/
int	is_redirection_target(t_node *node, t_vars *vars)
{
	t_node	*current;
	int		result;

	result = 0;
	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type) && current->type != TYPE_HEREDOC)
		{
			if (current->next == node)
			{
				if (current->args && current->args[0]
					&& ft_strcmp(current->args[0]
						, get_token_str(current->type)) != 0)
					result = 0;
				else
					result = 1;
				break ;
			}
		}
		current = current->next;
	}
	return (result);
}

/*
Checks if a node is the target of a heredoc redirection.
- Identifies both direct targets (node follows heredoc)
- Identifies delimiter nodes matching heredoc_delim

Returns:
- 1 if node is a heredoc target (don't append to command)
- 0 if not a heredoc target (can append to command)
*/
int	is_heredoc_target(t_node *node, t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_HEREDOC)
		{
			if (current->next == node)
				return (1);
			if (vars->pipes && vars->pipes->heredoc_delim)
			{
				if (node->args && node->args[0]
					&& ft_strcmp(node->args[0]
						, vars->pipes->heredoc_delim) == 0)
					return (1);
			}
		}
		current = current->next;
	}
	return (0);
}
