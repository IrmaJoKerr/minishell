/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_accessor.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: autopatch <autopatch@local>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 12:00:00 by autopatch         #+#    #+#             */
/*   Updated: 2025/11/17 12:00:00 by autopatch        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Return the quote type for the character at `pos` in argument `arg_idx` of
 * `node`. Returns 0 if no metadata is present or on invalid inputs.
 *
 * Quote type values follow the enum in minishell.h (e.g. TYPE_SINGLE_QUOTE==4,
 * TYPE_DOUBLE_QUOTE==5). Arrays use -1 as a sentinel; helper ft_intarrlen
 * (from lib_ft) is used to check bounds safely.
 */
int	quote_type_at(t_node *node, int arg_idx, int pos)
{
    int *arr;
    size_t len;

    if (!node || arg_idx < 0 || pos < 0 || !node->arg_quote_type)
        return (0);
    arr = node->arg_quote_type[arg_idx];
    if (!arr)
        return (0);
    len = ft_intarrlen(arr);
    if ((size_t)pos >= len)
        return (0);
    return (arr[pos]);
}

/*
 * Convenience helper that returns 1 when the given character position in an
 * argument is single-quoted, 0 otherwise.
 */
int	is_pos_single_quoted(t_node *node, int arg_idx, int pos)
{
    return (quote_type_at(node, arg_idx, pos) == TYPE_SINGLE_QUOTE);
}

/* Return 1 if the node contains a quotype array for the given argument */
int	has_arg_quotype(t_node *node, int arg_idx)
{
    if (!node || arg_idx < 0 || !node->arg_quote_type)
        return (0);
    if (node->arg_quote_type[arg_idx])
        return (1);
    return (0);
}
