/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_quo_token.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:52:50 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 02:54:02 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tries to merge the quoted token with existing tokens based on adjacency.
Returns:
 - 1 if successfully merged.
 - 0 otherwise.
*/
int	merge_quoted_token(char *input, char *content, t_vars *vars)
{
	char	*dummy_token;
	int		join_success;

	if (!vars->adj_state[0])
		return (0);
	dummy_token = ft_strdup("");
	if (!dummy_token)
		return (0);
	join_success = handle_tok_join(input, vars, content, dummy_token);
	if (!join_success)
	{
		ft_safefree((void **)&dummy_token);
	}
	else
	{
		if (vars->adj_state[1])
			process_right_adj(input, vars);
		process_adj(NULL, vars);
	}
	return (join_success);
}

/*
Creates a new command from quoted content
- Creates command node 
- Links to token list
- Handles token adjacency
Returns:
- 1 on success (takes ownership of content)
- 0 on failure (frees content)
*/
int	make_quoted_cmd(char *content, char *input, t_vars *vars)
{
	t_node	*cmd_node;

	cmd_node = initnode(TYPE_CMD, content);
	if (!cmd_node)
	{
		return (token_cleanup_error(content, vars));
	}
	build_token_linklist(vars, cmd_node);
	cleanup_and_process_adj(content, input, vars);
	return (1);
}
