/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 20:53:44 by bleow             #+#    #+#             */
/*   Updated: 2025/05/26 02:11:31 by bleow            ###   ########.fr       */
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
int	**dup_quote_types(t_node *node, size_t len)
{
	size_t	i;
	size_t	qlen;
	int		**new_quote_types;

	new_quote_types = malloc(sizeof(int *) * (len + 2));
	if (!new_quote_types)
		return (NULL);
	i = 0;
	while (i < len)
	{
		if (node->arg_quote_type && node->arg_quote_type[i])
		{
			qlen = ft_strlen(node->args[i]);
			new_quote_types[i] = copy_int_arr(node->arg_quote_type[i], qlen);
			if (!new_quote_types[i])
			{
				ft_free_int_2d(new_quote_types, i);
				return (NULL);
			}
		}
		else
			new_quote_types[i] = NULL;
		i++;
	}
	return (new_quote_types);
}

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
int	*set_char_quote_types(char *arg_text, int quote_type)
{
	int		*char_quote_types;
	size_t	len;
	size_t	i;

	if (!arg_text)
		return (NULL);
	len = ft_strlen(arg_text);
	char_quote_types = malloc(sizeof(int) * (len + 1));
	if (!char_quote_types)
		return (NULL);
	i = 0;
	while (i < len)
	{
		char_quote_types[i] = quote_type;
		i++;
	}
	char_quote_types[len] = -1;
	return (char_quote_types);
}

/*
Creates a new quote types array with the new argument's quote type information.
This function expands the quote types array when adding a new argument.
Returns:
- A newly allocated 2D array of quote types with the new entry.
- NULL (with proper cleanup of all related memory) on error.
*/
int	**resize_quotype_arr(t_node *node, char *new_arg, int quote_type,
			char **new_args)
{
	int		**new_quote_types;
	size_t	len;

	if (!node || !node->args || !new_args)
		return (NULL);
	len = ft_arrlen(node->args);
	new_quote_types = dup_quote_types(node, len);
	if (!new_quote_types)
	{
		ft_free_2d(new_args, len + 1);
		return (NULL);
	}
	new_quote_types[len] = set_char_quote_types(new_arg, quote_type);
	if (!new_quote_types[len])
	{
		ft_free_int_2d(new_quote_types, len);
		ft_free_2d(new_args, len + 1);
		return (NULL);
	}
	new_quote_types[len + 1] = NULL;
	return (new_quote_types);
}

/*
Appends an argument to a node's argument array
This function expands both the args and arg_quote_type arrays, adding the
new argument and its corresponding character-level quote information.
Example: 
String: "Hello"'world'!
Args: ["Hello", "world"]
Quote types: [[5, 5, 5, 5, 5], [4, 4, 4, 4]]
*/
// void	append_arg(t_node *node, char *new_arg, int quote_type)
// {
// 	char	**new_args;
// 	int		**new_quote_types;
// 	size_t	len;

// 	if (!node || !new_arg || !node->args)
// 		return ;
// 	len = ft_arrlen(node->args);
// 	new_args = dup_node_args(node, len);
// 	if (!new_args)
// 		return ;
// 	new_args[len] = ft_strdup(new_arg);
// 	if (!new_args[len])
// 	{
// 		ft_free_2d(new_args, len);
// 		return ;
// 	}
// 	new_args[len + 1] = NULL;
// 	new_quote_types = resize_quotype_arr(node, new_arg, quote_type, new_args);
// 	if (!new_quote_types)
// 		return ;
// 	ft_free_2d(node->args, len);
// 	if (node->arg_quote_type)
// 		ft_free_int_2d(node->arg_quote_type, len);
// 	node->args = new_args;
// 	node->arg_quote_type = new_quote_types;
// }
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
	new_quote_types = resize_quotype_arr(node, new_arg, quote_type, new_args);
	if (!new_quote_types)
		return ;
	ft_free_2d(node->args, len);
	if (node->arg_quote_type)
		ft_free_int_2d(node->arg_quote_type, len);
	node->args = new_args;
	node->arg_quote_type = new_quote_types;
}
