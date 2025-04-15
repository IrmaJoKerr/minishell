/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 20:53:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/15 09:13:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copy all arguments from the original node to a newly allocated array
Returns NULL on failure
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

int **dup_quote_types(t_node *node, size_t len)
{
    size_t i;
    int **new_quote_types;
    size_t qlen;
    
    new_quote_types = malloc(sizeof(int*) * (len + 2));
    if (!new_quote_types)
        return (NULL);
    i = 0;
    while (i < len)
    {
        if (node->arg_quote_type && node->arg_quote_type[i])
        {
            qlen = ft_strlen(node->args[i]);
            new_quote_types[i] = malloc(sizeof(int) * (qlen + 1));
            if (!new_quote_types[i])
            {
                ft_free_int_2d(new_quote_types, i);
                return (NULL);
            }
            ft_memcpy(new_quote_types[i], node->arg_quote_type[i], sizeof(int) * qlen);
            new_quote_types[i][qlen] = -1; // Sentinel value
        }
        else
        {
            new_quote_types[i] = NULL;
        }
        i++;
    }
    return new_quote_types;
}

/*
Creates a character-level quote type array for a new argument.
- Takes the node, argument text, and default quote type.
- Creates an array with each character set to the given quote type.
Returns:
- Pointer to the new quote_types array on success
- NULL on failure
*/
int *set_character_quote_types(char *arg_text, int quote_type)
{
	int		*char_quote_types;
	size_t	len;
	size_t	i;
	
	fprintf(stderr, "DEBUG [set_character_quote_types]: ENTER with arg='%s', quote_type=%d\n", 
			arg_text ? arg_text : "NULL", quote_type);
	if (!arg_text)
	{
		fprintf(stderr, "DEBUG [set_character_quote_types]: NULL arg_text, returning NULL\n");
		return NULL;
	}
	len = ft_strlen(arg_text);
	fprintf(stderr, "DEBUG [set_character_quote_types]: arg length=%zu\n", len);
	char_quote_types = malloc(sizeof(int) * (len + 1));
	if (!char_quote_types)
	{
		fprintf(stderr, "DEBUG [set_character_quote_types]: Failed to allocate quote types\n");
		return NULL;
	}
	fprintf(stderr, "DEBUG [set_character_quote_types]: Allocated quote types array with %zu+1 slots\n", len);
	i = 0;
	while (i < len)
	{
		char_quote_types[i] = quote_type;
		i++;
	}
	char_quote_types[len] = -1;
	fprintf(stderr, "DEBUG [set_character_quote_types]: Set all %zu characters to type %d\n", 
			len, quote_type);
	fprintf(stderr, "DEBUG [set_character_quote_types]: First elements: ");
	for (i = 0; i < len && i < 5; i++)
	{
		fprintf(stderr, "[%zu]=%d ", i, char_quote_types[i]);
	}
	fprintf(stderr, "...[%zu]=%d\n", len, char_quote_types[len]);
	return char_quote_types;
}

/*
Creates a new quote types array with the original values and new quote type
Returns :
- Pointer to the new quote_types array on success
- NULL on failure
*/
int	**set_quote_type(t_node *node, size_t len, int quote_type)
{
	int		**quote_types;
	size_t	i;

	quote_types = malloc(sizeof(int*) * (len + 2));
	if (!quote_types)
		return (NULL);
	
	i = 0;
	while (i < len)
	{
		if (node->arg_quote_type)
			quote_types[i] = node->arg_quote_type[i];  // Now matching int* to int*
		else
			quote_types[i] = NULL;  // NULL for uninitialized quote types
		i++;
	}
	// Create a new quote type array for the new argument
	quote_types[len] = malloc(sizeof(int));
	if (!quote_types[len])
    {
        ft_free_int_2d(quote_types, i);
        return (NULL);
    }
	quote_types[len][0] = quote_type;  // Set the quote type
	quote_types[len + 1] = NULL;  // NULL terminate the array
	return (quote_types);
}

/*
Updates node with new arguments and quote types arrays
*/
void	update_node_args(t_node *node, char **new_args, int **quote_types)
{
	node->args = new_args;
	node->arg_quote_type = quote_types;
}

/*
Appends an argument to a node's argument array
Updates both the args array and arg_quote_type array with 
character-level tracking.
Example: 
String: "Hello"'world'!
Args: ["Hello", "world"]
Quote types: [[5, 5, 5, 5, 5], [4, 4, 4, 4]]
*/
// void append_arg(t_node *node, char *new_arg, int quote_type)
// {
//     char **new_args;
//     int **new_quote_types;
//     size_t len;
//     size_t new_arg_len;
//     size_t i;
//     size_t qlen;
    
//     fprintf(stderr, "DEBUG [append_arg]: ENTER with arg='%s', quote_type=%d, node=%p\n", 
//             new_arg ? new_arg : "NULL", quote_type, (void*)node);
//     if (!node || !new_arg || !node->args)
//         return ;
//     len = 0;
//     while (node->args[len])
//         len++;
//     fprintf(stderr, "DEBUG [append_arg]: Found %zu existing arguments\n", len);
//     new_args = malloc(sizeof(char *) * (len + 2));
//     if (!new_args)
//         return ;
//     i = 0;
//     while (i < len)
//     {
//         new_args[i] = ft_strdup(node->args[i]);
//         if (!new_args[i])
//         {
//             ft_free_2d(new_args, i);  // Using ft_free_2d here
//             return ;
//         }
//         i++;
//     }
//     new_args[len] = ft_strdup(new_arg);
//     if (!new_args[len])
//     {
//         ft_free_2d(new_args, len);  // Using ft_free_2d here
//         return ;
//     }
//     new_args[len + 1] = NULL;
//     new_quote_types = malloc(sizeof(int*) * (len + 2));
//     if (!new_quote_types)
//     {
//         ft_free_2d(new_args, len + 1);  // Using ft_free_2d here
//         return ;
//     }
//     i = 0;
//     while (i < len)
//     {
//         if (node->arg_quote_type && node->arg_quote_type[i])
//         {
//             qlen = ft_strlen(node->args[i]);
//             new_quote_types[i] = malloc(sizeof(int) * (qlen + 1));
//             if (!new_quote_types[i])
//             {
//                 ft_free_int_2d(new_quote_types, i);  // Using ft_free_int_2d here
//                 ft_free_2d(new_args, len + 1);       // Using ft_free_2d here
//                 return;
//             }
//             ft_memcpy(new_quote_types[i], node->arg_quote_type[i], sizeof(int) * qlen);
//             new_quote_types[i][qlen] = -1; // Sentinel value
//         }
//         else
//         {
//             new_quote_types[i] = NULL;
//         }
//         i++;
//     }
//     new_arg_len = ft_strlen(new_arg);
//     new_quote_types[len] = malloc(sizeof(int) * (new_arg_len + 1));
//     if (!new_quote_types[len])
//     {
//         ft_free_int_2d(new_quote_types, len);  // Using ft_free_int_2d here
//         ft_free_2d(new_args, len + 1);         // Using ft_free_2d here
//         return ;
//     }
//     i = 0;
//     while (i < new_arg_len)
//     {
//         new_quote_types[len][i] = quote_type;
//         i++;
//     }
//     new_quote_types[len][new_arg_len] = -1;
//     new_quote_types[len + 1] = NULL;
//     // Free original arrays
//     ft_free_2d(node->args, len);  // Using ft_free_2d here
//     if (node->arg_quote_type)
//         ft_free_int_2d(node->arg_quote_type, len);  // Using ft_free_int_2d here
    
//     // Update node
//     node->args = new_args;
//     node->arg_quote_type = new_quote_types;
//     fprintf(stderr, "DEBUG [append_arg]: EXIT successful - updated node with %zu+1 args\n", len);
// }
void append_arg(t_node *node, char *new_arg, int quote_type)
{
    char	**new_args;
    int		**new_quote_types;
    size_t	len;
    
    fprintf(stderr, "DEBUG [append_arg]: ENTER with arg='%s', quote_type=%d, node=%p\n", 
            new_arg ? new_arg : "NULL", quote_type, (void*)node); 
    if (!node || !new_arg || !node->args)
        return ;
    len = ft_arrlen(node->args);
    fprintf(stderr, "DEBUG [append_arg]: Found %zu existing arguments\n", len);
    // Duplicate existing arguments
    new_args = dup_node_args(node, len);
    if (!new_args)
        return ;
    // Add new argument
    new_args[len] = ft_strdup(new_arg);
    if (!new_args[len])
    {
        ft_free_2d(new_args, len);
        return ;
    }
    new_args[len + 1] = NULL;
    // Duplicate quote types
    new_quote_types = dup_quote_types(node, len);
    if (!new_quote_types)
    {
        ft_free_2d(new_args, len + 1);
        return ;
    }
    // Set quote type for new argument
    new_quote_types[len] = set_character_quote_types(new_arg, quote_type);
    if (!new_quote_types[len])
    {
        ft_free_int_2d(new_quote_types, len);
        ft_free_2d(new_args, len + 1);
        return ;
    }
    new_quote_types[len + 1] = NULL;
    // Free original arrays
    ft_free_2d(node->args, len);
    if (node->arg_quote_type)
        ft_free_int_2d(node->arg_quote_type, len);
    // Update node
    node->args = new_args;
    node->arg_quote_type = new_quote_types;
    fprintf(stderr, "DEBUG [append_arg]: EXIT successful - updated node with %zu+1 args\n", len);
}

/*
Checks both left and right adjacency at a position
Updates adjacency state directly in vars->adj_state
*/
void	check_token_adj(char *input, t_vars *vars)
{
	// Left adjacency - using vars->pos directly
	vars->adj_state[0] = (vars->pos > 0 && !ft_isspace(input[vars->pos-1])
	 	&& !ft_is_operator(input[vars->pos-1]));
	// Right adjacency - using vars->pos directly
	vars->adj_state[1] = (input[vars->pos+1]
		&& !ft_isspace(input[vars->pos+1])
		&& !ft_is_operator(input[vars->pos+1]));
	// Guard value
	vars->adj_state[2] = -1;
}

/*
Processes adjacency state and updates vars->start appropriately
Returns:
 - 2 if bidirectional adjacency (both left and right)
 - 1 if right-adjacent only (don't update start)
 - (-1) if left-adjacent only
 - 0 if no adjacency
*/
int	process_adj(int *i, t_vars *vars)
{
    int	result;
    
    // Determine result based on adjacency combination
    if (vars->adj_state[0] && vars->adj_state[1])
        result = 2;       // Both left and right adjacency
    else if (vars->adj_state[1])
        result = 1;       // Right adjacency only
    else if (vars->adj_state[0])
        result = -1;      // Left adjacency only
    else
        result = 0;       // No adjacency
    // Only update start position if no right adjacency
    if (!vars->adj_state[1])
    {
        vars->start = *i;
    }
    // Log the determined adjacency state
    fprintf(stderr, "DEBUG: Adjacency state: left=%d, right=%d (result=%d)\n",
            vars->adj_state[0], vars->adj_state[1], result);
    return (result);
}

/*
 * Resets adjacency state to defaults
 */
void reset_adjacency_state(t_vars *vars)
{
    vars->adj_state[0] = 0;  // left adjacency
    vars->adj_state[1] = 0;  // right adjacency
    vars->adj_state[2] = -1; // guard value
}
