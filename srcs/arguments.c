/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 02:22:46 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

void	append_arg(t_node *node, char *new_arg, int quote_type)
{
    char	**new_args;
    int		*quo_types;
    size_t	i;
    size_t  len;
    
    if (!node || !new_arg || !node->args)
    {
        DBG_PRINTF(DEBUG_ARGS, "Cannot append arg: %s (node exists: %d, args exist: %d)\n", 
                  new_arg ? new_arg : "NULL", 
                  node ? 1 : 0, 
                  (node && node->args) ? 1 : 0);
        return ;
    }
    
    len = ft_arrlen(node->args);
    DBG_PRINTF(DEBUG_ARGS, "Appending '%s' to node with %zu existing args\n", new_arg, len);
    DBG_PRINTF(DEBUG_ARGS, "Command is: '%s'\n", node->args[0]);

    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return;

    i = -1;
    while (++i < len)
    {
        new_args[i] = node->args[i];
        if (!new_args[i])
        {
            free(new_args);
            return ;
        }
    }
    new_args[len] = ft_strdup(new_arg); // Directly assign duplicated new_arg
    if (!new_args[len])
    {
        free(new_args);
        return ;
    }
    new_args[len + 1] = NULL;
    process_quotes_in_arg(&new_args[len]);

    quo_types = malloc(sizeof(int) * (len + 2));
    if (!quo_types)
    {
        free(new_args[len]);
        free(new_args);
        return;
    }
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

    free(node->arg_quote_type);
    free(node->args);
    node->args = new_args;
    node->arg_quote_type = quo_types;

    DBG_PRINTF(DEBUG_ARGS, "Arg appended, new count: %zu\n", len+1);
    for (i = 0; i <= len; i++) {
        DBG_PRINTF(DEBUG_ARGS, "args[%zu]='%s', quote_type=%d\n", 
                  i, node->args[i], node->arg_quote_type ? node->arg_quote_type[i] : -1);
    }
}
