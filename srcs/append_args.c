/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 20:53:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/09 11:35:46 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copy all arguments from the original node to a newly allocated array
Returns NULL on failure
*/
// char	**dup_node_args(t_node *node, size_t len)
// {
// 	char	**new_args;
// 	size_t	i;

// 	new_args = malloc(sizeof(char *) * (len + 2));
// 	if (!new_args)
// 		return (NULL);
// 	i = -1;
// 	while (++i < len)
// 	{
// 		new_args[i] = node->args[i];
// 		if (!new_args[i])
// 		{
// 			free(new_args);
// 			return (NULL);
// 		}
// 	}
// 	return (new_args);
// }
// Fix the dup_node_args function:
char **dup_node_args(t_node *node, size_t len)
{
    char **new_args;
    size_t i;

    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return (NULL);
    
    i = 0;
    while (i < len)
    {
        // PROPERLY DUPLICATE each string instead of copying pointers
        new_args[i] = ft_strdup(node->args[i]);
        if (!new_args[i])
        {
            // Clean up previously allocated strings on failure
            while (i > 0)
                free(new_args[--i]);
            free(new_args);
            return (NULL);
        }
        i++;
    }
    
    return (new_args);
}

/*
Creates a new quote types array with the original values and new quote type
Returns :
- Pointer to the new quote_types array on success
- NULL on failure
*/
int	*set_quote_type(t_node *node, size_t len, int quote_type)
{
	int		*quote_types;
	size_t	i;

	quote_types = malloc(sizeof(int) * (len + 2));
	if (!quote_types)
		return (NULL);
	i = 0;
	while (i < len)
	{
		if (node->arg_quote_type)
			quote_types[i] = node->arg_quote_type[i];
		else
			quote_types[i] = 0;
		i++;
	}
	quote_types[len] = quote_type;
	quote_types[len + 1] = 0;
	return (quote_types);
}

/*
Updates node with new arguments and quote types arrays
*/
void	update_node_args(t_node *node, char **new_args, int *quote_types)
{
	free(node->arg_quote_type);
	free(node->args);
	node->args = new_args;
	node->arg_quote_type = quote_types;
}

/*
Helper function to clean up allocated memory for new arguments
*/
void	clean_new_args(char **new_args, size_t len)
{
	free(new_args[len]);
	free(new_args);
}

/*
Appends an argument to a node's argument array
Also tracks the quote type of the argument
*/
void	append_arg(t_node *node, char *new_arg, int quote_type)
{
	char	**new_args;
	int		*quote_types;
	size_t	len;
	
    // Add debug print to trace the input values
    fprintf(stderr, "DEBUG: append_arg called with arg='%s', quote_type=%d\n", 
		new_arg ? new_arg : "NULL", quote_type);
	if (!node || !new_arg || !node->args)
		return;
	len = ft_arrlen(node->args);
	new_args = dup_node_args(node, len);
	if (!new_args)
		return;
	new_args[len] = ft_strdup(new_arg);
	if (!new_args[len])
	{
		free(new_args);
		return ;
	}
	new_args[len + 1] = NULL;
	process_arg_quotes(&new_args[len]);
	quote_types = set_quote_type(node, len, quote_type);
	if (!quote_types)
	{
		clean_new_args(new_args, len);
		return ;
	}
	update_node_args(node, new_args, quote_types);
}
