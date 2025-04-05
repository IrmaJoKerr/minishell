/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 09:47:41 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates an argument array for a node with a single argument.
Allocates memory for the array and initializes it with the given token.
The quote type is set to default 0 (no quotes).
Returns NULL on failure.
*/
*/
void	create_args_array(t_node *node, char *token)
{
	char	**args;
	int		*quote_types;

	args = malloc(sizeof(char *) * 2);
	if (!args)
		return ;
	args[0] = ft_strdup(token);
	if (!args[0])
	{
		free(args);
		return ;
	}
	args[1] = NULL;
	node->args = args;
	quote_types = malloc(sizeof(int) * 2);
	if (!quote_types)
		return ;
	quote_types[0] = 0;
	quote_types[1] = 0;
	node->arg_quote_type = quote_types;
}

/*
Copy all arguments from the original node to a newly allocated array
Returns NULL on failure
*/
static char **dup_node_args(t_node *node, size_t len)
{
    char **new_args;
    size_t i;

    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return (NULL);
    i = -1;
    while (++i < len)
    {
        new_args[i] = node->args[i];
        if (!new_args[i])
        {
            free(new_args);
            return (NULL);
        }
    }
    return (new_args);
}

/*
 * Creates a new quote types array with the original values and new quote type
 * Returns NULL on failure
 */
static int *create_quote_types(t_node *node, size_t len, int quote_type)
{
    int *quo_types;
    size_t i;

    quo_types = malloc(sizeof(int) * (len + 2));
    if (!quo_types)
        return (NULL);
    
    i = 0;
    while (i < len)
    {
        if (node->arg_quote_type)
            quo_types[i] = node->arg_quote_type[i];
        else
            quo_types[i] = 0;
        i++;
    }
    quo_types[len] = quote_type;
    quo_types[len + 1] = 0;
    
    return (quo_types);
}

/*
Updates node with new arguments and quote types arrays
*/
static void update_node_args(t_node *node, char **new_args, int *quo_types)
{
    free(node->arg_quote_type);
    free(node->args);
    node->args = new_args;
    node->arg_quote_type = quo_types;
}

/*
Helper function to clean up allocated memory for new arguments
*/
static void clean_new_args(char **new_args, size_t len)
{
    free(new_args[len]);
    free(new_args);
}

/*
Appends an argument to a node's argument array
Also tracks the quote type of the argument
*/
void append_arg(t_node *node, char *new_arg, int quote_type)
{
    char **new_args;
    int *quo_types;
    size_t len;
    
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
    process_quotes_in_arg(&new_args[len]);
    quo_types = create_quote_types(node, len, quote_type);
    if (!quo_types)
    {
        clean_new_args(new_args, len);
        return ;
    }
    update_node_args(node, new_args, quo_types);
}
