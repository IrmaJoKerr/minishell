/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 20:53:44 by bleow             #+#    #+#             */
/*   Updated: 2025/11/18 15:37:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copies all arguments from the original node to a newly allocated array.
Returns:
- A newly allocated array containing copies of the arguments.
- NULL on error.
*/
char	**dup_node_args(t_node *node,	size_t len)
{
	char	**new_args;
	size_t	i;

	new_args = malloc(sizeof(char *) * (len + 2));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new_args[i] = ft_strdup(node->args[i]);
		if (!new_args[i])
		{
			ft_free_2d(new_args, i);
			return (NULL);
		}
		i++;
	}
	return (new_args);
}

/*
Duplicates the entire quote type tracking structure for all arguments.
This function creates a deep copy of the argument quote type tracking
structure, maintaining the same quote types for each character of 
each argument.
Returns:
- A newly allocated 2D array of quote types
- NULL (with proper cleanup of any partial allocations) on error.
*/
/*
Creates a character-level quote type array for a new argument.
- Single quotes are represented by 4, double quotes by 5.
- The last element is set to -1 as a guard value.
Used to maintain information about what kind of quote (if any) surrounded
each character in the shell command arguments.
Returns:
- Pointer to the new quote_types array on success
- NULL on failure
*/
/*
Creates a new quote types array with the new argument's quote type information.
This function expands the quote types array when adding a new argument.
Returns:
- A newly allocated 2D array of quote types with the new entry.
- NULL (with proper cleanup of all related memory) on error.
*/
/*
 * The legacy helpers for duplicating and resizing per-argument quote-type
 * arrays were removed from this compilation unit to centralize quote-array
 * management in `quote_accessor.c`. The append path already uses
 * `dup_node_quotetypes_and_append()` which is the canonical helper and
 * avoids direct manipulation of `node->arg_quote_type` here.
 */

/*
Appends an argument to a node's argument array
This function expands both the args and arg_quote_type arrays, adding the
new argument and its corresponding character-level quote information.
Example: 
String: "Hello"'world'!
Args: ["Hello", "world"]
Quote types: [[5, 5, 5, 5, 5], [4, 4, 4, 4]]
*/
void	append_arg(t_node *node, char *new_arg, int quote_type)
{
	char	**new_args;
	int		**new_quote_types;
	size_t	len;

	if (!node || !new_arg || !node->args)
		return ;
	len = ft_arrlen(node->args);
	new_args = dup_node_args(node, len);
	if (!new_args)
		return ;
	new_args[len] = ft_strdup(new_arg);
	if (!new_args[len])
	{
		ft_free_2d(new_args, len);
		return ;
	}
	new_args[len + 1] = NULL;
	new_quote_types = dup_node_quotetypes_and_append(node, new_arg,
			quote_type, new_args);
	if (!new_quote_types)
		return ;
	/* Duplicate compact per-arg flags as well so the lightweight accessor
	   remains populated during migration. If this allocation fails, undo
	   the newly allocated quote types and args to avoid leaks and return. */
	{
		int *new_flags;
		new_flags = dup_node_arg_flags_and_append(node, new_arg, quote_type,
			new_args);
		if (!new_flags)
		{
			ft_free_int_2d(new_quote_types, len);
			ft_free_2d(new_args, len + 1);
			return ;
		}
		/* Free old arg flags and assign the new compact flags */
		if (node->arg_quote_flags)
		{
			free(node->arg_quote_flags);
			node->arg_quote_flags = NULL;
		}
		node->arg_quote_flags = new_flags;
	}
	ft_free_2d(node->args, len);
	if (node->arg_quote_type)
		ft_free_int_2d(node->arg_quote_type, len);
	node->args = new_args;
	node->arg_quote_type = new_quote_types;

}
