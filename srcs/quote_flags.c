/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_flags.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: automated <nobody@example.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 12:00:00 by automated         #+#    #+#             */
/*   Updated: 2025/11/18 12:00:00 by automated        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* Allocate an int array of length 'len' and initialize to 0. */
int *setup_arg_flags(int len)
{
    int *arr;
    int i;

    if (len <= 0)
        return (NULL);
    arr = malloc(sizeof(int) * len);
    if (!arr)
        return (NULL);
    i = 0;
    while (i < len)
    {
        arr[i] = 0;
        i++;
    }
    return (arr);
}

/* Free the compact per-arg flags array on a node. */
void free_node_arg_flags(t_node *node)
{
    if (!node)
        return;
    if (node->arg_quote_flags)
    {
        free(node->arg_quote_flags);
        node->arg_quote_flags = NULL;
    }
}

/* Duplicate existing node arg flags and append a flag for a new arg.
   new_args is the updated args array (NULL-terminated). */
int *dup_node_arg_flags_and_append(t_node *node, char *new_arg,
        int quote_flag, char **new_args)
{
    (void)new_arg;
    int old_len;
    int new_len;
    int *new_flags;
    int i;

    if (!new_args)
        return (NULL);
    new_len = ft_arrlen(new_args);
    new_flags = malloc(sizeof(int) * new_len);
    if (!new_flags)
        return (NULL);
    old_len = 0;
    if (node && node->args)
        old_len = ft_arrlen(node->args);
    i = 0;
    while (i < new_len)
    {
        if (i < old_len && node && node->arg_quote_flags)
            new_flags[i] = node->arg_quote_flags[i];
        else if (i == old_len)
            new_flags[i] = quote_flag;
        else
            new_flags[i] = 0;
        i++;
    }
    return (new_flags);
}

/* Read the compact per-arg flag for an argument. Returns -1 if unknown. */
int get_arg_quote_flag(t_node *node, int arg_idx)
{
    if (!node || arg_idx < 0)
        return (-1);
    if (node->arg_quote_flags && node->args && (size_t)arg_idx < ft_arrlen(node->args))
        return (node->arg_quote_flags[arg_idx]);
    /* Fallback to legacy path for now. Emit debug tripwire so we can
       identify callsites still using old metadata. */
    fprintf(stderr, "DEBUG OLD: %s falling back for arg %d\n", __func__, arg_idx);
    return (0);
}

/* Set the compact per-arg flag for an argument; allocate array if needed. */
int set_arg_quote_flag(t_node *node, int arg_idx, int flag)
{
    int len;
    int *new_flags;

    if (!node || arg_idx < 0)
        return (0);
    if (!node->args)
        return (0);
    len = ft_arrlen(node->args);
    if (!node->arg_quote_flags)
    {
        node->arg_quote_flags = setup_arg_flags(len);
        if (!node->arg_quote_flags)
            return (0);
    }
    if (arg_idx >= len)
    {
        /* Need to resize to accommodate; create a new array */
        new_flags = malloc(sizeof(int) * (arg_idx + 1));
        if (!new_flags)
            return (0);
        ft_memcpy(new_flags, node->arg_quote_flags, sizeof(int) * len);
        /* initialize new slots */
        {
            int i = len;
            while (i <= arg_idx)
            {
                new_flags[i] = 0;
                i++;
            }
        }
        free(node->arg_quote_flags);
        node->arg_quote_flags = new_flags;
    }
    node->arg_quote_flags[arg_idx] = flag;
    return (1);
}


/* Return 0 if not wholly quoted, or the quote type (QUOTE_SINGLE/QUOTE_DOUBLE)
   if the whole argument is single- or double-quoted. Prefers the compact per-arg flags
   and falls back to scanning the per-character `arg_quote_type` array. */
int is_arg_whole_quoted(t_node *node, int arg_idx)
{
    int flag;
    int *arr;
    size_t len;
    size_t i;

    if (!node || arg_idx < 0)
        return (0);
    flag = get_arg_quote_flag(node, arg_idx);
    if (flag != 0)
        return (flag);
    /* Fallback: inspect per-character metadata if present */
    if (!node->arg_quote_type)
        return (0);
    if (!node->args)
        return (0);
    if ((size_t)arg_idx >= ft_arrlen(node->args))
        return (0);
    arr = node->arg_quote_type[arg_idx];
    if (!arr)
        return (0);
    len = ft_intarrlen(arr);
    if (len == 0)
        return (0);
    /* Determine if all entries are the same and equal to a quote type */
    i = 0;
    while (i < len)
    {
        if (arr[i] != arr[0])
            return (0);
        i++;
    }
    if (arr[0] == QUOTE_SINGLE || arr[0] == QUOTE_DOUBLE)
        return (arr[0]);
    return (0);
}
