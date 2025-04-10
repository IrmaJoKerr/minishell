/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 20:53:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 13:33:52 by bleow            ###   ########.fr       */
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
Creates a character-level quote type array for a new argument.
- Takes the node, argument text, and default quote type.
- Creates an array with each character set to the given quote type.
Returns:
- Pointer to the new quote_types array on success
- NULL on failure
*/
int *set_character_quote_types(char *arg_text, int quote_type)
{
    int *char_quote_types;
    size_t len;
    size_t i;
    
    // Debug entry
    fprintf(stderr, "DEBUG [set_character_quote_types]: ENTER with arg='%s', quote_type=%d\n", 
            arg_text ? arg_text : "NULL", quote_type);
    
    if (!arg_text) {
        fprintf(stderr, "DEBUG [set_character_quote_types]: NULL arg_text, returning NULL\n");
        return NULL;
    }
    
    // Get length of argument
    len = ft_strlen(arg_text);
    fprintf(stderr, "DEBUG [set_character_quote_types]: arg length=%zu\n", len);
    
    // Allocate character-level quote type array
    char_quote_types = malloc(sizeof(int) * (len + 1));
    if (!char_quote_types) {
        fprintf(stderr, "DEBUG [set_character_quote_types]: Failed to allocate quote types\n");
        return NULL;
    }
    
    fprintf(stderr, "DEBUG [set_character_quote_types]: Allocated quote types array with %zu+1 slots\n", len);
    
    // Set each character's quote type to the given value
    i = 0;
    while (i < len)
    {
        char_quote_types[i] = quote_type;
        i++;
    }
    
    // Add terminator value
    char_quote_types[len] = -1;
    
    fprintf(stderr, "DEBUG [set_character_quote_types]: Set all %zu characters to type %d\n", 
            len, quote_type);
    
    // Debug first several elements
    fprintf(stderr, "DEBUG [set_character_quote_types]: First elements: ");
    for (i = 0; i < len && i < 5; i++) {
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
// int	*set_quote_type(t_node *node, size_t len, int quote_type)
// {
// 	int		*quote_types;
// 	size_t	i;

// 	quote_types = malloc(sizeof(int) * (len + 2));
// 	if (!quote_types)
// 		return (NULL);
// 	i = 0;
// 	while (i < len)
// 	{
// 		if (node->arg_quote_type)
// 			quote_types[i] = node->arg_quote_type[i];
// 		else
// 			quote_types[i] = 0;
// 		i++;
// 	}
// 	quote_types[len] = quote_type;
// 	quote_types[len + 1] = 0;
// 	return (quote_types);
// }
int	**set_quote_type(t_node *node, size_t len, int quote_type)
{
    int **quote_types;
    size_t i;

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
        // Clean up previously allocated arrays
        i = 0;
        while (i < len)
        {
            free(quote_types[i]);
            i++;
        }
        free(quote_types);
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
	// free(node->arg_quote_type);
	// free(node->args);
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

// /*
// Appends an argument to a node's argument array
// Also tracks the quote type of the argument
// */
// void	append_arg(t_node *node, char *new_arg, int quote_type)
// {
// 	char	**new_args;
// 	int		*quote_types;
// 	size_t	len;
	
//     // Add debug print to trace the input values
//     fprintf(stderr, "DEBUG: append_arg called with arg='%s', quote_type=%d\n", 
// 		new_arg ? new_arg : "NULL", quote_type);
// 	if (!node || !new_arg || !node->args)
// 		return;
// 	len = ft_arrlen(node->args);
// 	new_args = dup_node_args(node, len);
// 	if (!new_args)
// 		return;
// 	new_args[len] = ft_strdup(new_arg);
// 	if (!new_args[len])
// 	{
// 		free(new_args);
// 		return ;
// 	}
// 	new_args[len + 1] = NULL;
// 	process_arg_quotes(&new_args[len]);
// 	quote_types = set_quote_type(node, len, quote_type);
// 	if (!quote_types)
// 	{
// 		clean_new_args(new_args, len);
// 		return ;
// 	}
// 	update_node_args(node, new_args, quote_types);
// }
/*
Appends an argument to a node's argument array
Updates both the args array and arg_quote_type array with character-level tracking
*/
// void	append_arg(t_node *node, char *new_arg, int quote_type)
// {
//     char	**new_args;
//     int		**new_quote_types;
//     size_t	arg_count;
//     int		i;
    
//     // Debug trace
//     fprintf(stderr, "DEBUG: append_arg called with arg='%s', quote_type=%d\n", 
//             new_arg ? new_arg : "NULL", quote_type);
//     // Validate inputs
//     if (!node || !new_arg || !node->args)
//         return ;
//     // Count existing arguments
//     arg_count = 0;
//     while (node->args[arg_count])
//         arg_count++;
//     // Allocate new arguments array (size + 1 for new arg + 1 for NULL)
//     new_args = malloc(sizeof(char *) * (arg_count + 2));
//     if (!new_args)
//         return ;
//     // Copy existing arguments
//     i = 0;
//     while (i < arg_count)
//     {
//         new_args[i] = ft_strdup(node->args[i]);
//         if (!new_args[i])
//         {
//             // Clean up on error
//             while (i > 0)
//             {
//                 i--;
//                 free(new_args[i]);
//             }
//             free(new_args);
//             return ;
//         }
//         i++;
//     }
//     // Add new argument
//     new_args[arg_count] = ft_strdup(new_arg);
//     if (!new_args[arg_count])
//     {
//         // Clean up on error
//         i = 0;
//         while (i < arg_count)
//         {
//             free(new_args[i]);
//             i++;
//         }
//         free(new_args);
//         return ;
//     }
//     // Process quotes in the new argument (remove outer quotes)
//     process_arg_quotes(&new_args[arg_count]);
//     // Null terminate
//     new_args[arg_count + 1] = NULL;
//     // Allocate new quote types array
//     new_quote_types = malloc(sizeof(int *) * (arg_count + 2));
//     if (!new_quote_types)
//     {
//         // Clean up on error
//         i = 0;
//         while (i <= arg_count)
//         {
//             free(new_args[i]);
//             i++;
//         }
//         free(new_args);
//         return ;
//     }
//     // Copy existing quote type arrays
//     i = 0;
//     while (i < arg_count)
//     {
//         if (node->arg_quote_type && node->arg_quote_type[i])
//         {
//             // Count characters in this argument
//             int j = 0;
//             while (node->args[i][j])
//                 j++;
//             // Allocate and copy character quote types
//             new_quote_types[i] = malloc(sizeof(int) * (j + 1));
//             if (!new_quote_types[i])
//             {
//                 // Clean up on error
//                 int k = 0;
//                 while (k < i)
//                 {
//                     free(new_quote_types[k]);
//                     k++;
//                 }
//                 k = 0;
//                 while (k <= arg_count)
//                 {
//                     free(new_args[k]);
//                     k++;
//                 }
//                 free(new_quote_types);
//                 free(new_args);
//                 return ;
//             }
//             // Copy character quote types
//             j = 0;
//             while (node->args[i][j])
//             {
//                 new_quote_types[i][j] = node->arg_quote_type[i][j];
//                 j++;
//             }
//             new_quote_types[i][j] = -1; // End marker
//         }
//         else
//         {
//             // If no quote types existed, initialize with zeros
//             new_quote_types[i] = set_character_quote_types(new_args[i], 0);
//             if (!new_quote_types[i])
//             {
//                 // Clean up on error
//                 int k = 0;
//                 while (k < i)
//                 {
//                     free(new_quote_types[k]);
//                     k++;
//                 }
//                 k = 0;
//                 while (k <= arg_count)
//                 {
//                     free(new_args[k]);
//                     k++;
//                 }
//                 free(new_quote_types);
//                 free(new_args);
//                 return ;
//             }
//         }
//         i++;
//     }
//     // Add character-level quote types for the new argument
//     new_quote_types[arg_count] = set_character_quote_types(new_args[arg_count], quote_type);
//     if (!new_quote_types[arg_count])
//     {
//         // Clean up on error
//         i = 0;
//         while (i < arg_count)
//         {
//             free(new_quote_types[i]);
//             i++;
//         }
//         i = 0;
//         while (i <= arg_count)
//         {
//             free(new_args[i]);
//             i++;
//         }
//         free(new_quote_types);
//         free(new_args);
//         return ;
//     }
//     // Null terminate
//     new_quote_types[arg_count + 1] = NULL;
//     // Free old data
//     i = 0;
//     while (node->args[i])
//     {
//         free(node->args[i]);
//         i++;
//     }
//     free(node->args);
//     if (node->arg_quote_type)
//     {
//         i = 0;
//         while (node->arg_quote_type[i])
//         {
//             free(node->arg_quote_type[i]);
//             i++;
//         }
//         free(node->arg_quote_type);
//     }
//     // Update node with new arrays
//     node->args = new_args;
//     node->arg_quote_type = new_quote_types;
// }
/*
Appends an argument to a node's argument array
Updates both the args array and arg_quote_type array with character-level tracking
*/
/*
Appends an argument to a node's argument array
Updates both the args array and arg_quote_type array with character-level tracking
*/
/*
Appends an argument to a node's argument array
Updates both the args array and arg_quote_type array with character-level tracking
*/
void append_arg(t_node *node, char *new_arg, int quote_type)
{
    char **new_args;
    int **new_quote_types;
    size_t len;
    
    fprintf(stderr, "DEBUG [append_arg]: ENTER with arg='%s', quote_type=%d, node=%p\n", 
            new_arg ? new_arg : "NULL", quote_type, (void*)node);
    
    if (!node || !new_arg || !node->args)
        return;
    
    // Count existing arguments
    len = 0;
    while (node->args[len])
        len++;
    
    fprintf(stderr, "DEBUG [append_arg]: Found %zu existing arguments\n", len);
    
    // Create new args array with space for new arg plus NULL terminator
    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return;
    
    // Copy existing args
    for (size_t i = 0; i < len; i++) {
        new_args[i] = ft_strdup(node->args[i]);
        if (!new_args[i]) {
            // Cleanup on failure
            for (size_t j = 0; j < i; j++)
                free(new_args[j]);
            free(new_args);
            return;
        }
    }
    
    // Add new arg
    new_args[len] = ft_strdup(new_arg);
    if (!new_args[len]) {
        // Cleanup on failure
        for (size_t j = 0; j < len; j++)
            free(new_args[j]);
        free(new_args);
        return;
    }
    new_args[len + 1] = NULL;
    
    // Handle quote types
    new_quote_types = malloc(sizeof(int*) * (len + 2));
    if (!new_quote_types) {
        // Cleanup on failure
        for (size_t j = 0; j <= len; j++)
            free(new_args[j]);
        free(new_args);
        return;
    }
    
    // Copy existing quote types
    for (size_t i = 0; i < len; i++) {
        if (node->arg_quote_type && node->arg_quote_type[i]) {
            size_t qlen = strlen(node->args[i]);
            new_quote_types[i] = malloc(sizeof(int) * (qlen + 1));
            if (!new_quote_types[i]) {
                // Cleanup on failure
                for (size_t j = 0; j < i; j++)
                    free(new_quote_types[j]);
                for (size_t j = 0; j <= len; j++)
                    free(new_args[j]);
                free(new_quote_types);
                free(new_args);
                return;
            }
            memcpy(new_quote_types[i], node->arg_quote_type[i], sizeof(int) * qlen);
            new_quote_types[i][qlen] = -1; // Sentinel value
        } else {
            new_quote_types[i] = NULL;
        }
    }
    
    // Create quote type for new arg
    size_t new_arg_len = strlen(new_arg);
    new_quote_types[len] = malloc(sizeof(int) * (new_arg_len + 1));
    if (!new_quote_types[len]) {
        // Cleanup on failure
        for (size_t j = 0; j < len; j++)
            if (new_quote_types[j])
                free(new_quote_types[j]);
        for (size_t j = 0; j <= len; j++)
            free(new_args[j]);
        free(new_quote_types);
        free(new_args);
        return;
    }
    
    // Set all characters to the specified quote type
    for (size_t i = 0; i < new_arg_len; i++)
        new_quote_types[len][i] = quote_type;
    new_quote_types[len][new_arg_len] = -1; // Sentinel value
    new_quote_types[len + 1] = NULL;
    
    // Free old arrays AFTER fully setting up the new ones
    for (size_t i = 0; i < len; i++)
        free(node->args[i]);
    free(node->args);
    
    if (node->arg_quote_type) {
        for (size_t i = 0; i < len; i++)
            if (node->arg_quote_type[i])
                free(node->arg_quote_type[i]);
        free(node->arg_quote_type);
    }
    
    // Update node with new arrays
    node->args = new_args;
    node->arg_quote_type = new_quote_types;
    
    fprintf(stderr, "DEBUG [append_arg]: EXIT successful - updated node with %zu+1 args\n", len);
}
