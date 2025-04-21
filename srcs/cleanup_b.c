/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:50 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 02:07:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Free a single token node and its arguments.
- Takes a node pointer and frees its arguments array.
- Properly frees character-level quote type arrays.
- Then frees the node itself.
Works with cleanup_token_list(), build_token_linklist(), maketoken().
*/
// void	free_token_node(t_node *node)
// {	
// 	if (!node)
// 		return ;
// 	if (node->args)
// 		ft_free_2d(node->args, ft_arrlen(node->args));
// 	if (node->arg_quote_type)
//     {
//         // Use ft_free_int_2d with length 1, as setup_quotes likely creates
//         // an outer array of size 1 containing the pointer to the inner int array.
//         ft_free_int_2d(node->arg_quote_type, 2);
//         node->arg_quote_type = NULL;
//     }
// 	free(node);
// }
void	free_token_node(t_node *node)
{	
    size_t arg_count; // Variable to hold the actual argument count

    if (!node)
        return ;

    // Determine the number of arguments to know the size for arg_quote_type
    arg_count = 0;
    if (node->args)
        arg_count = ft_arrlen(node->args);

    if (node->args)
        ft_free_2d(node->args, arg_count); // Use the calculated count

    if (node->arg_quote_type)
    {
        // Use the actual argument count to free the quote type array correctly
        ft_free_int_2d(node->arg_quote_type, arg_count); 
        node->arg_quote_type = NULL;
    }
    free(node);
}
