/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 07:54:40 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 00:43:49 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Debug print function for command arguments
*/
void	debug_cmd_tokens(char **args)
{
	int	i;

	fprintf(stderr, "DEBUG: Command tokens: ");
	i = 0;
	while (args[i])
	{
        fprintf(stderr, "'%s'", args[i]);
        if (args[i+1])
            fprintf(stderr, ", ");
        else
            fprintf(stderr, "\n");
        i++;
    }
}
 
/*
Process command node arguments from split input
*/
t_node	*build_cmdarg_node(char **args)
{
	t_node	*node;
	int		i;
	 
	if (!args || !args[0])
		return (NULL);
	node = new_cmd_node(args[0]);
	if (!node)
		 return (NULL);
	i = 1;
	while (args[i])
	{
		append_arg(node, args[i]);
		i++;
	}
	return (node);
}
 
/*
Update the node pointers in the token chain
*/
void build_token_linklist(t_vars *vars, t_node *node)
{
	if (!vars->head)
	{
		// First token becomes the head
		vars->head = node;
		vars->current = node;
		fprintf(stderr, "DEBUG: First token created as head: type=%d\n", node->type);
	}
	else
	{
		// Add to the end of the list
		vars->current->next = node;
		node->prev = vars->current;
		vars->current = node;
		fprintf(stderr, "DEBUG: Added token to list: type=%d\n", node->type);
	}
}

/*
Process command tokens by splitting input string
with whitespace as delimiter. Uses ft_splitstr()
to handle quotes during splitting.
*/
void process_cmd_token(char *input, t_vars *vars)
{
    char    **args;
    t_node  *node;
    int     i;
 
    args = ft_splitstr(input, " \t\n\v\f\r");
    if (!args)
        return ;
    // Process quotes in arguments
    i = 0;
    while (args[i])
    {
        process_quotes_in_arg(&args[i]);
        // Handle flags with dash correctly - don't split them
        if (i > 0 && args[i][0] == '-' && ft_strlen(args[i]) == 1 && args[i+1] && ft_isalpha(args[i+1][0]))
        {
            // The argument is just a '-' and the next arg is a letter - combine them
            char *combined = ft_strjoin(args[i], args[i+1]);
            if (combined)
            {
                free(args[i]);
                free(args[i+1]);
                args[i] = combined;
                
                // Shift remaining arguments
                int j = i + 1;
                while (args[j+1])
                {
                    args[j] = args[j+1];
                    j++;
                }
                args[j] = NULL;
            }
        }
        i++;
    }
    debug_cmd_tokens(args);
    // Create command node
    if (args[0])
    {
        node = build_cmdarg_node(args);
        if (node)
            build_token_linklist(vars, node);
    }
    ft_free_2d(args, ft_arrlen(args));
}

/*
Process non-command tokens
*/
void	process_other_token(char *input, t_vars *vars)
{
	char	*token;
	t_node	*node;
	if (vars->pos <= vars->start)
		return ;
	token = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!token)
		return ;
	if (vars->curr_type == TYPE_ARGS)
		node = new_cmd_node(token);
	else
		node = new_other_node(token, vars->curr_type);
	ft_safefree((void **)&token);
	if (!node)
		return ;
	if (vars->curr_type != TYPE_CMD && vars->current)
		add_child(vars->current, node);
	else
		vars->current = node;
}
