/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 09:28:03 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Create an array of arguments(flags) for the node.
Works like malloc for the array.
Example: "ls" -> args array: ["ls", NULL]
OLD VERSION
void	create_args_array(t_node *node, char *token)
{
	char	**args;

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
}
*/
void create_args_array(t_node *node, char *token)
{
    char **args;
    int *quote_types;

    args = malloc(sizeof(char *) * 2);
    if (!args)
        return;
    args[0] = ft_strdup(token);
    if (!args[0])
    {
        ft_safefree((void **)&args);
        return;
    }
    args[1] = NULL;
    node->args = args;
    
    // Initialize quote types array
    quote_types = malloc(sizeof(int) * 2);
    if (!quote_types)
        return;
    quote_types[0] = 0; // By default, first arg is not quoted
    quote_types[1] = 0; // NULL terminator
    node->arg_quote_type = quote_types;
}

/*
Append a new argument to the node's argument array.
Works like realloc for the array.
Example: node->args is ["ls", "-l", NULL]
After append_arg(node, "-a"), node->args becomes ["ls", "-l", "-a", NULL]
OLD VERSION
void	append_arg(t_node *node, char *new_arg, int quote_type)
{
    char	**new_args;
    size_t	len;
    size_t	i;

    if (!node || !new_arg || !node->args)
        return ;
    len = ft_arrlen(node->args);
    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return ;
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
    new_args[len] = ft_strdup(new_arg);
    new_args[len + 1] = NULL;
    process_quotes_in_arg(&new_args[len]);
    free(node->args);
    node->args = new_args;
}
*/
void append_arg(t_node *node, char *new_arg, int quote_type)
{
    char    **new_args;
    int     *quo_types;
    size_t  len;
    size_t  i;

    fprintf(stderr, "DEBUG: [append_arg] ENTRY: Called with arg='%s', quote_type=%d\n", 
            new_arg, quote_type);
    
    if (!node || !new_arg || !node->args)
    {
        fprintf(stderr, "DEBUG: [append_arg] ERROR: Invalid parameters\n");
        return;
    }
    len = ft_arrlen(node->args);
    fprintf(stderr, "DEBUG: [append_arg] Found %zu existing args\n", len);
    
    /* Show existing quote types */
    fprintf(stderr, "DEBUG: [append_arg] Current quote types:");
    if (node->arg_quote_type)
    {
        for (i = 0; i < len; i++)
            fprintf(stderr, " %d", node->arg_quote_type[i]);
        fprintf(stderr, "\n");
    }
    else
        fprintf(stderr, " (null)\n");
    
    /* Allocate new arguments array */
    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
    {
        fprintf(stderr, "DEBUG: [append_arg] ERROR: Failed to allocate new_args\n");
        return;
    }
        
    /* Copy existing arguments */
    i = -1;
    while (++i < len)
    {
        new_args[i] = node->args[i];
        if (!new_args[i])
        {
            fprintf(stderr, "DEBUG: [append_arg] ERROR: Found null arg during copy\n");
            ft_safefree((void **)&new_args);
            return;
        }
    }
    
    /* Add new argument */
    new_args[len] = ft_strdup(new_arg);
    new_args[len + 1] = NULL;
    
    /* Process quotes but keep quote type info */
    fprintf(stderr, "DEBUG: [append_arg] Before process_quotes_in_arg: arg='%s'\n", 
            new_args[len]);
    process_quotes_in_arg(&new_args[len]);
    fprintf(stderr, "DEBUG: [append_arg] After process_quotes_in_arg: arg='%s', quote_type still=%d\n", 
            new_args[len], quote_type);
    
    /* Update quote type tracking */
    quo_types = malloc(sizeof(int) * (len + 2));
    if (!quo_types)
    {
        fprintf(stderr, "DEBUG: [append_arg] ERROR: Failed to allocate quote type array\n");
    }
    else
    {
        /* Copy existing quote types if they exist */
        if (node->arg_quote_type)
        {
            fprintf(stderr, "DEBUG: [append_arg] Copying existing quote types\n");
            i = 0;
            while (i < len)
            {
                quo_types[i] = node->arg_quote_type[i];
                fprintf(stderr, "DEBUG: [append_arg] Copied quote_type[%zu]=%d\n", 
                        i, quo_types[i]);
                i++;
            }
            ft_safefree((void **)&node->arg_quote_type);
        }
        else
        {
            fprintf(stderr, "DEBUG: [append_arg] No existing quote types, initializing as unquoted\n");
            i = 0;
            while (i < len)
            {
                quo_types[i] = 0;
                i++;
            }
        }
        
        /* Set quote type for new argument and null terminator */
        quo_types[len] = quote_type;
        quo_types[len + 1] = 0;
        fprintf(stderr, "DEBUG: [append_arg] Set new arg quote_type[%zu]=%d\n", 
                len, quo_types[len]);
        
        /* Update node with new quote types */
        node->arg_quote_type = quo_types;
        
        /* Verify the update */
        fprintf(stderr, "DEBUG: [append_arg] Verification of quote types after update:");
        for (i = 0; i <= len; i++)
            fprintf(stderr, " %d", node->arg_quote_type[i]);
        fprintf(stderr, "\n");
    }
    
    /* Update node with new arguments */
    ft_safefree((void **)&node->args);
    node->args = new_args;
    fprintf(stderr, "DEBUG: [append_arg] COMPLETE: Added argument '%s' with quote_type=%d\n", 
           new_args[len], quote_type);
}
