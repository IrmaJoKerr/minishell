/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 20:53:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/21 20:09:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copies all arguments from the original node to a newly allocated array.
Returns:
- A newly allocated array containing copies of the arguments.
- NULL on error.
*/
// char	**dup_node_args(t_node *node,	size_t len)
// {
// 	char	**new_args;
// 	size_t	i;

// 	new_args = malloc(sizeof(char *) * (len + 2));
// 	if (!new_args)
// 		return (NULL);
// 	i = 0;
// 	while (i < len)
// 	{
// 		new_args[i] = ft_strdup(node->args[i]);
// 		if (!new_args[i])
// 		{
// 			ft_free_2d(new_args, i);
// 			return (NULL);
// 		}
// 		i++;
// 	}
// 	return (new_args);
// }
char	**dup_node_args(t_node *node,	size_t len)
{
    char	**new_args;
    size_t	i;

    // Allocate space for 'len' pointers + 1 for new arg + 1 for NULL terminator
    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return (NULL);
    i = 0;
    while (i < len)
    {
        new_args[i] = ft_strdup(node->args[i]);
        if (!new_args[i])
        {
            ft_free_2d(new_args, i); // Free only the ones successfully strdup'd
            return (NULL);
        }
        i++;
    }
    // Initialize the slots for the new argument and the NULL terminator
    new_args[len] = NULL;
    new_args[len + 1] = NULL;
    return (new_args);
}

/*
Duplicates an integer array used for tracking quote types.
The resulting array will have length+1 elements, with the
last one set to -1 as a guard value.
Returns:
- A newly allocated array with copied values and guard value (-1).
- NULL on error.
*/
// int	*copy_int_arr(int *original, size_t length)
// {
// 	int	*new_types;
	
// 	new_types = malloc(sizeof(int) * (length + 1));
// 	if (!new_types)
// 		return (NULL);
// 	ft_memcpy(new_types, original, sizeof(int) * length);
// 	new_types[length] = -1;
// 	return (new_types);
// }
int	*copy_int_arr(int *original, size_t length)
{
    int	*new_types;

    if (!original) // Handle case where original inner array might be NULL
        return (NULL);

    new_types = malloc(sizeof(int) * (length + 1));
    if (!new_types)
        return (NULL);
    ft_memcpy(new_types, original, sizeof(int) * length);
    new_types[length] = -1; // Ensure sentinel value
    return (new_types);
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
// int	**dup_quote_types(t_node *node, size_t len)
// {
// 	size_t	i;
// 	size_t	qlen;
// 	int		**new_quote_types;
	
// 	new_quote_types = malloc(sizeof(int*) * (len + 2));
// 	if (!new_quote_types)
// 		return (NULL);
// 	i = 0;
// 	while (i < len)
// 	{
// 		if (node->arg_quote_type && node->arg_quote_type[i])
// 		{
// 			qlen = ft_strlen(node->args[i]);
// 			new_quote_types[i] = copy_int_arr(node->arg_quote_type[i], qlen);
// 			if (!new_quote_types[i])
// 			{
// 				ft_free_int_2d(new_quote_types, i);
// 				return (NULL);
// 			}
// 		}
// 		else
// 			new_quote_types[i] = NULL;
// 		i++;
// 	}
// 	return new_quote_types;
// }
/*
Duplicates the entire quote type tracking structure for all arguments.
Allocates N+1 outer pointers for N arguments, NULL-terminating it.
*/
int	**dup_quote_types(t_node *node, size_t len) // len = number of args to copy
{
    size_t	i;
    size_t	qlen;
    int		**new_quote_types;

    // Allocate space for 'len' pointers + 1 NULL terminator
    new_quote_types = malloc(sizeof(int*) * (len + 1));
    if (!new_quote_types)
        return (NULL);
    i = 0;
    while (i < len)
    {
        if (node->arg_quote_type && node->arg_quote_type[i] && node->args && node->args[i]) // Check node->args[i] exists
        {
            qlen = ft_strlen(node->args[i]); // Use corresponding arg length
            new_quote_types[i] = copy_int_arr(node->arg_quote_type[i], qlen);
            if (!new_quote_types[i])
            {
                // Free previously allocated inner arrays and the outer array
                ft_free_int_2d(new_quote_types, i); // Free only up to 'i'
                return (NULL);
            }
        }
        else
            new_quote_types[i] = NULL; // Handle case where original inner was NULL or args missing
        i++;
    }
    // NULL-terminate the outer array
    new_quote_types[len] = NULL;
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
int *set_char_quote_types(char *arg_text, int quote_type)
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
// int	**resize_quotype_arr(t_node *node, char *new_arg, int quote_type
// 				,char **new_args)
// {
// 	int		**new_quote_types;
// 	size_t	len;
	
// 	if (!node || !node->args || !new_args)
// 		return (NULL);
// 	len = ft_arrlen(node->args);
// 	new_quote_types = dup_quote_types(node, len);
// 	if (!new_quote_types)
// 	{
// 		ft_free_2d(new_args, len + 1);
// 		return (NULL);
// 	}
// 	new_quote_types[len] = set_char_quote_types(new_arg, quote_type);
// 	if (!new_quote_types[len])
// 	{
// 		ft_free_int_2d(new_quote_types, len);
// 		ft_free_2d(new_args, len + 1);
// 		return (NULL);
// 	}
// 	new_quote_types[len + 1] = NULL;
// 	return (new_quote_types);
// }
/*
Creates a new quote types array with the new argument's quote type information.
Reallocates the duplicated array to add space for the new argument's types.
*/
int	**resize_quotype_arr(t_node *node, char *new_arg, int quote_type
                ,char **new_args) // new_args is only needed for cleanup on failure
{
    int		**temp_quote_types;
    int		**final_quote_types;
    size_t	len; // Old number of args
    size_t	i;

    if (!node || !node->args || !new_args) // Check node->args
        return (NULL);
    len = ft_arrlen(node->args); // Get old number of args

    // Duplicate the existing 'len' inner arrays + NULL terminator
    temp_quote_types = dup_quote_types(node, len);
    if (!temp_quote_types)
    {
        ft_free_2d(new_args, len + 1); // Free the newly created args array (len + new + NULL)
        return (NULL);
    }

    // --- Manual Reallocation ---
    // Allocate new outer array with space for one more pointer + NULL terminator
    final_quote_types = malloc(sizeof(int *) * (len + 2));
    if (!final_quote_types)
    {
        ft_free_int_2d(temp_quote_types, len); // Free the duplicated types (len inner arrays)
        ft_free_2d(new_args, len + 1); // Free the newly created args array
        return (NULL);
    }
    // Copy pointers from temp_quote_types to final_quote_types
    i = 0;
    while (i < len + 1) // Copy 'len' pointers + the NULL terminator
    {
        final_quote_types[i] = temp_quote_types[i];
        i++;
    }
    // Free the old outer array (temp_quote_types), but not the inner arrays it pointed to
    free(temp_quote_types);
    // --- End Manual Reallocation ---


    // Create the inner array for the new argument
    final_quote_types[len] = set_char_quote_types(new_arg, quote_type);
    if (!final_quote_types[len])
    {
        // Free the manually reallocated outer array (which contains old copied pointers)
        // Note: ft_free_int_2d expects length of valid inner pointers
        ft_free_int_2d(final_quote_types, len); // Free only the 'len' copied inner arrays
        // Also need to free the outer array itself (already done by ft_free_int_2d)
        // free(final_quote_types); // Redundant if ft_free_int_2d frees outer array
        ft_free_2d(new_args, len + 1); // Free the newly created args array
        return (NULL);
    }

    // NULL-terminate the final outer array
    final_quote_types[len + 1] = NULL;
    return (final_quote_types);
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
// 	DBG_PRINTF(DEBUG_ARGS, "append_arg: Adding '%s' to node %p\n", 
// 		new_arg, (void*)node);
// 	len = ft_arrlen(node->args);
// 	DBG_PRINTF(DEBUG_ARGS, "append_arg: Current args count: %zu\n", len);
    
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
// 	DBG_PRINTF(DEBUG_ARGS, "append_arg: Freeing old arrays: args=%p, quotes=%p\n", 
// 		(void*)node->args, (void*)node->arg_quote_type);
// 	ft_free_2d(node->args, len);
// 	if (node->arg_quote_type)
// 		ft_free_int_2d(node->arg_quote_type, len);
// 	node->args = new_args;
// 	node->arg_quote_type = new_quote_types;
// 	DBG_PRINTF(DEBUG_ARGS, "append_arg: Added arg, now node has %zu arguments\n", 
// 		ft_arrlen(node->args));
// }
void	append_arg(t_node *node, char *new_arg, int quote_type)
{
    char	**new_args;
    int		**new_quote_types;
    size_t	len; // Old number of args

    if (!node || !new_arg || !node->args) // Check node->args
        return ;
    DBG_PRINTF(DEBUG_ARGS, "append_arg: Adding '%s' to node %p\n",
        new_arg, (void*)node);
    len = ft_arrlen(node->args); // Get old number of args
    DBG_PRINTF(DEBUG_ARGS, "append_arg: Current args count: %zu\n", len);

    // Duplicate old args and allocate space for new one + NULL
    new_args = dup_node_args(node, len);
    if (!new_args)
        return ; // dup_node_args handles its internal cleanup

    // Add the new argument string
    new_args[len] = ft_strdup(new_arg);
    if (!new_args[len])
    {
        ft_free_2d(new_args, len); // Free the duplicated args + the outer array
        return ;
    }
    new_args[len + 1] = NULL; // Ensure new args is NULL terminated

    // Resize quote types (allocates new outer/inner, needs new_args for cleanup path)
    new_quote_types = resize_quotype_arr(node, new_arg, quote_type, new_args);
    if (!new_quote_types) {
        // resize_quotype_arr handles freeing new_args on failure
        return ;
    }

    DBG_PRINTF(DEBUG_ARGS, "append_arg: Freeing old arrays: args=%p, quotes=%p\n",
        (void*)node->args, (void*)node->arg_quote_type);

    // Free old args array (contains 'len' strings)
    ft_free_2d(node->args, len); // Use old length 'len'

    // Free old quote types array (contains 'len' inner arrays)
    if (node->arg_quote_type)
        ft_free_int_2d(node->arg_quote_type, len); // Use old length 'len'

    // Assign new arrays
    node->args = new_args;
    node->arg_quote_type = new_quote_types;
    DBG_PRINTF(DEBUG_ARGS, "append_arg: Added arg, now node has %zu arguments\n",
        ft_arrlen(node->args));
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
void check_token_adj(char *input, t_vars *vars)
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
