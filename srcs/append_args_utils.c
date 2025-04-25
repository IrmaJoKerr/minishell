/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:16:05 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 22:19:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Duplicates an integer array used for tracking quote types.
The resulting array will have length+1 elements, with the
last one set to -1 as a guard value.
Returns:
- A newly allocated array with copied values and guard value (-1).
- NULL on error.
*/
int	*copy_int_arr(int *original, size_t length)
{
	int	*new_types;

	new_types = malloc(sizeof(int) * (length + 1));
	if (!new_types)
		return (NULL);
	ft_memcpy(new_types, original, sizeof(int) * length);
	new_types[length] = -1;
	return (new_types);
}

/*
Checks both left and right adjacency at the current token position.
Adjacency means the token is connected to another token without whitespace
or operators between them.
Looks for space AND operator characters to determine adjacency.
Updates adjacency state directly in vars->adj_state array:
  - vars->adj_state[0]: Left adjacency (previous character)
  - vars->adj_state[1]: Right adjacency (next character)
  - vars->adj_state[2]: Guard value (-1)
Examples:
- "hello world"   -> at 'world', adjacency is [0,0]
- "hello'world'"  -> at 'world', adjacency is [1,0]
- "'hello'world"  -> at 'hello', adjacency is [0,1]
- "he'llo'wo"     -> at 'llo', adjacency is [1,1]
*/
void	check_token_adj(char *input, t_vars *vars)
{
	int		has_left_adj;
	int		has_right_adj;
	char	left_char;
	char	right_char;

	has_left_adj = 0;
	has_right_adj = 0;
	if (vars->pos > 0)
	{
		left_char = input[vars->pos - 1];
		has_left_adj = !ft_isspace(left_char) && !ft_is_operator(left_char);
	}
	if (input[vars->pos + 1])
	{
		right_char = input[vars->pos + 1];
		has_right_adj = !ft_isspace(right_char) && !ft_is_operator(right_char);
	}
	vars->adj_state[0] = has_left_adj;
	vars->adj_state[1] = has_right_adj;
	vars->adj_state[2] = -1;
}

/*
Processes adjacency state and updates vars->start appropriately.
If i is not NULL and there's no right adjacency, updates 
vars->start with *i.
This tracks token boundaries during parsing.
Returns:
 - 2 if bidirectional adjacency (both left and right)
 - 1 if right-adjacent only (don't update start)
 - (-1) if left-adjacent only
 - 0 if no adjacency
*/
int	process_adj(int *i, t_vars *vars)
{
	int	result;

	if (vars->adj_state[0] && vars->adj_state[1])
		result = 2;
	else if (vars->adj_state[1])
		result = 1;
	else if (vars->adj_state[0])
		result = -1;
	else
		result = 0;
	if (i != NULL && !vars->adj_state[1])
	{
		vars->start = *i;
	}
	return (result);
}
