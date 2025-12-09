/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_accessor.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 09:28:10 by bleow             #+#    #+#             */
/*   Updated: 2025/12/09 13:56:01 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Return the quote type for the character at `pos` in argument `arg_idx` of
 * `node`. Returns 0 if no metadata is present or on invalid inputs.
 *
 * Quote type values follow the enum in minishell.h (QUOTE_SINGLE/QUOTE_DOUBLE).
 * Arrays use -1 as a sentinel; helper ft_intarrlen
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
    return (quote_type_at(node, arg_idx, pos) == QUOTE_SINGLE);
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


/*
 * Ensure the quotype array for `node->arg_quote_type[arg_idx]` exists and has
 * capacity for at least `needed_len` characters (not counting the -1 sentinel).
 * Returns 1 on success, 0 on allocation/error.
 */
int	ensure_arg_quotype_len(t_node *node, int arg_idx, size_t needed_len)
{
    int **top;
    int *old;
    size_t arg_count;
    size_t old_len;
    int *new_arr;

    if (!node || arg_idx < 0)
        return (0);
    /* arg array must exist and arg_idx must be valid */
    if (!node->args)
        return (0);
    arg_count = ft_arrlen(node->args);
    if ((size_t)arg_idx >= arg_count)
        return (0);
    if (!node->arg_quote_type)
    {
        node->arg_quote_type = setup_quotes((int)arg_count);
        if (!node->arg_quote_type)
            return (0);
    }
    top = node->arg_quote_type;
    old = top[arg_idx];
    if (old)
    {
        old_len = ft_intarrlen(old);
        if (old_len >= needed_len)
            return (1);
    }
    /* allocate new array sized needed_len + 1 (for sentinel -1) */
    new_arr = ft_calloc(needed_len + 1, sizeof(int));
    if (!new_arr)
        return (0);
    /* copy existing contents if any */
    if (old)
    {
        old_len = ft_intarrlen(old);
        ft_memcpy(new_arr, old, old_len * sizeof(int));
        free(old);
    }
    /* set sentinel */
    new_arr[needed_len] = -1;
    top[arg_idx] = new_arr;
    return (1);
}


/*
 * Set the quote type at a specific position. Will ensure capacity as needed.
 * Returns 1 on success, 0 on error.
 */
int	set_quote_type_at(t_node *node, int arg_idx, int pos, int quote_type)
{
    if (!node || arg_idx < 0 || pos < 0)
        return (0);
    if (!ensure_arg_quotype_len(node, arg_idx, (size_t)pos + 1))
        return (0);
    node->arg_quote_type[arg_idx][pos] = quote_type;
    return (1);
}


/*
 * Append a quote type for the next character in the argument. This is a
 * convenience wrapper used when the corresponding argument string is being
 * appended to. Returns 1 on success, 0 on error.
 */
int	push_quote_type(t_node *node, int arg_idx, int quote_type)
{
    size_t curr_len;

    if (!node || arg_idx < 0)
        return (0);
    if (!node->arg_quote_type || !node->arg_quote_type[arg_idx])
    {
        if (!ensure_arg_quotype_len(node, arg_idx, 1))
            return (0);
        node->arg_quote_type[arg_idx][0] = quote_type;
        return (1);
    }
    curr_len = ft_intarrlen(node->arg_quote_type[arg_idx]);
    /* resize to accommodate one more char */
    if (!ensure_arg_quotype_len(node, arg_idx, curr_len + 1))
        return (0);
    node->arg_quote_type[arg_idx][curr_len] = quote_type;
    return (1);
}


/*
 * Create a per-character quote-type array for `new_arg` where all
 * characters are initialized to `quote_type`. The returned array is
 * terminated with -1. This was previously defined in `append_args.c` but
 * is more appropriate here alongside other quote-accessor helpers.
 */
int	*set_char_quote_types(char *arg_text, int quote_type)
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


/* Free wrapper for a node's quote-type 2D array (safe to call on NULL). */
void	free_node_quotypes(t_node *node)
{
    if (!node)
        return;
    if (!node->arg_quote_type)
        return;
    /* ft_free_int_2d expects the number of argument slots; use ft_arrlen
     * on node->args when available, otherwise pass 0 which will still free
     * the top-level pointer safely.
     */
    if (node->args)
        ft_free_int_2d(node->arg_quote_type, ft_arrlen(node->args));
    else
        ft_free_int_2d(node->arg_quote_type, 0);
    node->arg_quote_type = NULL;
}


/*
 * Deep-copy the node's existing quote-type 2D array and append a new entry
 * corresponding to `new_arg` with `quote_type`. This mirrors the previous
 * `resize_quotype_arr` behaviour but centralises the allocation logic here so
 * callers don't manipulate `node->arg_quote_type` directly.
 *
 * Returns a newly malloc'd int** on success (terminated with NULL), or NULL on
 * allocation error. On error the caller is responsible for cleaning up any
 * allocations they hold (matching previous behaviour where `new_args` was
 * freed by the helper on failure).
 */
int **dup_node_quotetypes_and_append(t_node *node, char *new_arg,
        int quote_type, char **new_args)
{
    int **new_quote_types;
    size_t len;
    size_t i;

    if (!node || !node->args || !new_arg)
        return (NULL);
    len = ft_arrlen(node->args);
    new_quote_types = malloc(sizeof(int *) * (len + 2));
    if (!new_quote_types)
        return (NULL);
    i = 0;
    while (i < len)
    {
        if (has_arg_quotype(node, (int)i))
        {
            size_t qlen = ft_intarrlen(node->arg_quote_type[i]);
            new_quote_types[i] = copy_int_arr(node->arg_quote_type[i], qlen);
            if (!new_quote_types[i])
            {
                ft_free_int_2d(new_quote_types, i);
                if (new_args)
                    ft_free_2d(new_args, len + 1);
                return (NULL);
            }
        }
        else
            new_quote_types[i] = NULL;
        i++;
    }
    new_quote_types[len] = set_char_quote_types(new_arg, quote_type);
    if (!new_quote_types[len])
    {
        ft_free_int_2d(new_quote_types, len);
        if (new_args)
            ft_free_2d(new_args, len + 1);
        return (NULL);
    }
    new_quote_types[len + 1] = NULL;
    return (new_quote_types);
}
