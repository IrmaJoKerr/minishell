/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 02:05:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process quoted content, including variable expansion in double quotes.
Finds the appropriate command node for the content.
Modifies the content pointer if expansion is needed.
Returns:
- The command node.
- NULL if not found/error.
*/
// t_node	*process_quoted_str(char **content_ptr, int quote_type, t_vars *vars)
// {
// 	t_node	*cmd_node;
// 	char	*expanded;

// 	if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(*content_ptr, '$'))
// 	{
// 		expanded = expand_quoted_str(*content_ptr, vars);
// 		free(*content_ptr);
// 		if (!expanded)
// 			return (NULL);
// 		*content_ptr = expanded;
// 	}
// 	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
// 	return (cmd_node);
// }
t_node	*process_quoted_str(char **content_ptr, int quote_type, t_vars *vars)
{
    t_node	*cmd_node;
    char	*expanded_content;

    cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
    if (quote_type == TYPE_DOUBLE_QUOTE)
    {
        expanded_content = expand_quoted_str(*content_ptr, vars);
        if (!expanded_content)
        {
            free(*content_ptr);
            *content_ptr = NULL;
            return (NULL);
        }
        free(*content_ptr);
        *content_ptr = expanded_content;
    }
    return (cmd_node);
}

/*
Links a file node into the token list after a redirection node.
- Sets redirection->right pointer to the file node
- Handles proper linked list connections
- Updates vars->current pointer
Works with handle_redir_target().
*/
// void	link_file_to_redir(t_node *redir_node, t_node *file_node, t_vars *vars)
// {
// 	redir_node->right = file_node;
// 	if (redir_node->next)
// 	{
// 		file_node->next = redir_node->next;
// 		redir_node->next->prev = file_node;
// 	}
// 	redir_node->next = file_node;
// 	file_node->prev = redir_node;
// 	vars->current = file_node;
// }
void	link_file_to_redir(t_node *redir_node, t_node *file_node, t_vars *vars)
{
    if (!redir_node || !file_node || !vars)
        return ;
    redir_node->right = file_node;
    file_node->prev = redir_node;
    if (redir_node->next)
    {
        file_node->next = redir_node->next;
        redir_node->next->prev = file_node;
    }
    else
    {
        file_node->next = NULL;
        vars->current = file_node;
    }
    redir_node->next = file_node;
    vars->current = file_node;
}

/*
Handles joining quoted content with a cached redirection target.
Returns:
- 1 if joined successfully (content is freed).
- 0 if join failed or not applicable (content is NOT freed here).
*/
// static int join_with_cached_target(char *content, t_vars *vars)
// {
//     char *joined_target;
//     char *original_target;

//     fprintf(stderr, "[DEBUG-REDIR-JOIN-CACHE] Attempting join via cache. Target node: %p, type=%s\n",
//             (void *)vars->pipes->last_redir_target, get_token_str(vars->pipes->last_redir_target->type));

//     if (vars->pipes->last_redir_target->type != TYPE_ARGS)
//     {
//         fprintf(stderr, "[DEBUG-QUOTE] Cached target is not ARGS type, cannot join.\n");
//         return (0); // Cannot join if the target isn't an ARGS node
//     }

//     original_target = vars->pipes->last_redir_target->args[0];
//     fprintf(stderr, "[DEBUG-QUOTE] Joining with cached target: '%s' + '%s'\n", original_target, content);

//     joined_target = ft_strjoin(original_target, content);
//     if (!joined_target)
//     {
//         fprintf(stderr, "[DEBUG-QUOTE] ft_strjoin failed during cache join.\n");
//         free(content); // Free content as it won't be used
//         return (0);	   // Indicate failure, but content is handled
//     }

//     free(vars->pipes->last_redir_target->args[0]); // Free old target string
//     vars->pipes->last_redir_target->args[0] = joined_target; // Assign new joined string
//     fprintf(stderr, "[DEBUG-QUOTE] New joined target: '%s'\n", joined_target);

//     // Update quote types if necessary (append 0s for the joined part)
//     if (vars->pipes->last_redir_target->arg_quote_type && vars->pipes->last_redir_target->arg_quote_type[0])
//     {
//         update_quote_types(vars->pipes->last_redir_target, 0, content); // Use the target node directly
//     }

//     free(content); // Free the incoming content as it's now part of the target
//     return (1);	   // Indicate success
// }
static int join_with_cached_target(char *content, t_vars *vars)
{
    char *joined_target;
    char *original_target;
    int append_len; // Store length before freeing content

    fprintf(stderr, "[DEBUG-REDIR-JOIN-CACHE] Attempting join via cache. Target node: %p, type=%s\n",
            (void *)vars->pipes->last_redir_target, get_token_str(vars->pipes->last_redir_target->type));

    if (vars->pipes->last_redir_target->type != TYPE_ARGS)
    {
        fprintf(stderr, "[DEBUG-QUOTE] Cached target is not ARGS type, cannot join.\n");
        // Content is NOT freed here as join wasn't applicable
        return (0);
    }

    original_target = vars->pipes->last_redir_target->args[0];
    append_len = ft_strlen(content); // Get length before potential free
    fprintf(stderr, "[DEBUG-QUOTE] Joining with cached target: '%s' + '%s'\n", original_target, content);

    joined_target = ft_strjoin(original_target, content);
    if (!joined_target)
    {
        fprintf(stderr, "[DEBUG-QUOTE] ft_strjoin failed during cache join.\n");
        free(content); // Free content as it won't be used
        return (0);	   // Indicate failure, content handled
    }

    free(vars->pipes->last_redir_target->args[0]); // Free old target string
    vars->pipes->last_redir_target->args[0] = joined_target; // Assign new joined string
    fprintf(stderr, "[DEBUG-QUOTE] New joined target: '%s'\n", joined_target);

    // --- START Manual Quote Type Update ---
    // Update quote types if necessary (append 0s for the joined part)
    if (vars->pipes->last_redir_target->arg_quote_type && vars->pipes->last_redir_target->arg_quote_type[0])
    {
        int old_len = 0;
        int *old_types = vars->pipes->last_redir_target->arg_quote_type[0];
        while (old_types && old_types[old_len] != -1) {
            old_len++;
        }

        int new_len = old_len + append_len;
        int *new_types = malloc(sizeof(int) * (new_len + 1));

        if (new_types) {
            // Copy old types
            if (old_types) {
                ft_memcpy(new_types, old_types, sizeof(int) * old_len);
            }
            // Append 0s for the new part (assuming appended part has no quotes)
            int k = 0;
            while (k < append_len) {
                new_types[old_len + k] = 0;
                k++;
            }
            new_types[new_len] = -1; // Null terminate

            // Replace old array
            free(vars->pipes->last_redir_target->arg_quote_type[0]);
            vars->pipes->last_redir_target->arg_quote_type[0] = new_types;
            fprintf(stderr, "[DEBUG-QUOTE] Updated quote types for joined target.\n");
        } else {
            fprintf(stderr, "[DEBUG-QUOTE] Malloc failed updating quote types.\n");
            // Non-critical error? Proceeding without quote type update.
        }
    }
    // --- END Manual Quote Type Update ---

    free(content); // Free the incoming content as it's now part of the target
    return (1);	   // Indicate success
}

/*
Handles quoted text as a redirection target.
- Finds the relevant redirection node in the token list.
- Creates a file node with the quoted content.
- Links the file node to the redirection operator.
- Sets error code if no valid redirection is found.
Returns:
- 1 on success (content is freed)
- 0 on failure (content is freed)
Works with process_quote_char() for handling quoted filenames.
Example:
- For "echo > "file.txt"", creates file node for "file.txt".
*/
// int	handle_redir_target(char *content, t_vars *vars)
// {
// 	t_node	*redir_node;
// 	t_node	*file_node;

// 	redir_node = find_last_redir(vars);
// 	if (redir_node && is_redirection(redir_node->type))
// 	{
// 		file_node = initnode(TYPE_ARGS, content);
// 		if (!file_node)
// 		{
// 			free(content);
// 			return (0);
// 		}
// 		link_file_to_redir(redir_node, file_node, vars);
// 		free(content);
// 		return (1);
// 	}
// 	free(content);
// 	vars->error_code = ERR_SYNTAX;
// 	return (0);
// }
// int handle_redir_target(char *content, t_vars *vars)
// {
//     t_node *redir_node;
//     t_node *file_node;
//     t_node *target_redir;
//     t_node *target_check = NULL; // Initialize to NULL
//     int joined_via_cache = 0; // Flag to track if joined using cache

//     // ... (initial debug prints) ...

//     // --- START MODIFICATION ---
//     // Prioritize cached target if adjacent and in redirection context
//     if (vars->adj_state[0] && vars->pipes->in_redir_context && vars->pipes->last_redir_target)
//     {
//         target_check = vars->pipes->last_redir_target;
//         fprintf(stderr, "[DEBUG-REDIR-JOIN-CACHE] Attempting join via cache. Target node: %p, type=%s\n",
//                (void*)target_check, target_check ? get_token_str(target_check->type) : "NULL");

//         if (target_check && target_check->type == TYPE_ARGS)
//         {
//             // Ensure the cached target is linked to the current redirection context
//             target_redir = vars->pipes->current_redirect; // Use cached redirect node
//             if (target_redir && target_redir->right == target_check)
//             {
//                 fprintf(stderr, "[DEBUG-QUOTE] Cached target confirmed for current redirection.\n");
//                 fprintf(stderr, "[DEBUG-QUOTE] Joining with cached target: '%s' + '%s'\n",
//                       target_check->args[0], content);

//                 char *joined = ft_strjoin(target_check->args[0], content);
//                 if (!joined) { /* ... memory error handling ... */ return (0); }

//                 free(target_check->args[0]);
//                 target_check->args[0] = joined;
//                 fprintf(stderr, "[DEBUG-QUOTE] New joined target: '%s'\n", joined);
//                 free(content);

//                 // Context is already set, just ensure it's maintained
//                 vars->pipes->in_redir_context = 1;
//                 // last_redir_target remains the same
//                 fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Maintained context state via cache: in_redir_context=1, target=%p\n",
//                        (void*)target_check);

//                 joined_via_cache = 1; // Mark as joined
//                 return (1); // Successfully joined via cache
//             }
//              else {
//                  fprintf(stderr, "[DEBUG-REDIR-JOIN-CACHE-FAIL] Cached target mismatch or not linked to current redirection.\n");
//                  target_check = NULL; // Invalidate target_check if cache check failed
//              }
//         }
//          else {
//              fprintf(stderr, "[DEBUG-REDIR-JOIN-CACHE-FAIL] Cached target is invalid or not ARGS type.\n");
//              target_check = NULL; // Invalidate target_check
//          }
//     }
//     // --- END MODIFICATION ---


//     // Original logic as fallback (mainly for the first quote after redir operator)
//     if (!joined_via_cache && vars->adj_state[0] && vars->current)
//     {
//         target_check = vars->current; // Start with current
//          fprintf(stderr, "[DEBUG-REDIR-JOIN-INIT] Fallback: Initial target_check: %p, type=%s\n",
//                (void*)target_check, target_check ? get_token_str(target_check->type) : "NULL");

//         // If current node is a quote type and has a previous ARGS node, check that instead
//         if ((vars->current->type == TYPE_SINGLE_QUOTE || vars->current->type == TYPE_DOUBLE_QUOTE) &&
//             vars->current->prev)
//         {
//              fprintf(stderr, "[DEBUG-REDIR-JOIN-PREV] Fallback: Current is quote. Prev node: %p, type=%s\n",
//                   (void*)vars->current->prev,
//                   vars->current->prev ? get_token_str(vars->current->prev->type) : "NULL");
//             if (vars->current->prev->type == TYPE_ARGS)
//             {
//                 target_check = vars->current->prev;
//                 fprintf(stderr, "[DEBUG-REDIR-JOIN] Fallback: Using previous ARGS node: %p\n", (void*)target_check);
//             }
//              else {
//                  fprintf(stderr, "[DEBUG-REDIR-JOIN-PREV-FAIL] Fallback: Previous node is not ARGS type.\n");
//              }
//         }

//         if (target_check && target_check->type == TYPE_ARGS) // Check if target_check is valid
//         {
//             // ... (rest of the original joining logic using target_check) ...
//             // Find target_redir = find_redir_for_target(target_check, vars);
//             // If target_redir found, join content to target_check->args[0]
//             // Maintain context: vars->pipes->in_redir_context = 1; vars->pipes->last_redir_target = target_check;
//             // return (1);
//              fprintf(stderr, "[DEBUG-QUOTE] Fallback: Adjacent to previous token: '%s'\n", target_check->args[0]);
//              fprintf(stderr, "[DEBUG-REDIR-DETAILED] Fallback: Checking if node %p is a redirection target\n", (void*)target_check);

//              // Use cached result if possible, otherwise search (This might need adjustment)
//              if (vars->pipes->in_redir_context && vars->pipes->last_redir_target == target_check) {
//                  target_redir = vars->pipes->current_redirect;
//                  fprintf(stderr, "[DEBUG-REDIR-JOIN] Fallback: Using cached redirection info for target_check\n");
//              } else {
//                  target_redir = find_redir_for_target(target_check, vars);
//              }
//              fprintf(stderr, "[DEBUG-REDIR-DETAILED] Fallback: find_redir_for_target result: %p\n", (void*)target_redir);

//              if (target_redir) {
//                  fprintf(stderr, "[DEBUG-QUOTE] Fallback: Previous token is a redirection target\n");
//                  fprintf(stderr, "[DEBUG-QUOTE] Fallback: Joining with existing target: '%s' + '%s'\n", target_check->args[0], content);

//                  char *joined = ft_strjoin(target_check->args[0], content);
//                  if (!joined) { /* ... memory error handling ... */ return (0); }

//                  free(target_check->args[0]);
//                  target_check->args[0] = joined;
//                  fprintf(stderr, "[DEBUG-QUOTE] Fallback: New joined target: '%s'\n", joined);
//                  free(content);

//                  vars->pipes->in_redir_context = 1;
//                  vars->pipes->last_redir_target = target_check;
//                  // It's crucial that current_redirect is also correctly set/maintained here if find_redir_for_target was used
//                  if (!vars->pipes->current_redirect) vars->pipes->current_redirect = target_redir;
//                  fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Fallback: Maintained context state: in_redir_context=1, target=%p\n", (void*)target_check);

//                  return (1);
//              } else {
//                  fprintf(stderr, "[DEBUG-REDIR-JOIN] Fallback: Node is not a redirection target\n");
//              }
//         }
//          else if (target_check) { // Only print if target_check was set
//              fprintf(stderr, "[DEBUG-REDIR-JOIN] Fallback: Target check node (%p) is not ARGS type: %s\n",
//                    (void*)target_check, get_token_str(target_check->type));
//          }
//     }

//     // --- Regular redirection target handling (if no join occurred) ---
//     // Find last redirection node *without* a target
//     redir_node = find_last_redir(vars); // Ensure find_last_redir correctly finds only those needing a target
//     if (!joined_via_cache && redir_node /* && !redir_node->right */) // find_last_redir should guarantee !redir_node->right
//     {
//         // ... (original logic for creating the *first* file_node for a redirection) ...
//          fprintf(stderr, "[DEBUG-QUOTE] Found redirection node type: %s without target\n", get_token_str(redir_node->type));
//          file_node = initnode(TYPE_ARGS, content);
//          if (!file_node) { /* ... error handling ... */ return (0); }
//          link_file_to_redir(redir_node, file_node, vars);
//          fprintf(stderr, "[DEBUG-QUOTE] Successfully linked file to redirection\n");

//          vars->pipes->in_redir_context = 1;
//          vars->pipes->last_redir_target = file_node;
//          vars->pipes->current_redirect = redir_node; // Cache the redirection node
//          fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Setting context: in_redir_context=%d, target=%p, redir=%p\n",
//                vars->pipes->in_redir_context, (void*)file_node, (void*)redir_node);

//          free(content); // Content is now owned by file_node
//          return (1); // Handled as new redirection target
//     }

//     // --- Process as regular quoted argument (if no join and not a new redir target) ---
//     if (!joined_via_cache) {
//          fprintf(stderr, "[DEBUG-QUOTE] Processing as regular quoted argument\n");
//          fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Resetting context from %d to 0\n", vars->pipes->in_redir_context);
//          vars->pipes->in_redir_context = 0;
//          vars->pipes->last_redir_target = NULL; // Clear cache
//          vars->pipes->current_redirect = NULL; // Clear cache

//          t_node *node = initnode(TYPE_ARGS, content); // Create ARGS node for the quote content
//          if (!node) { free(content); return (0); }

//          // Add the quote node itself (TYPE_DOUBLE_QUOTE or TYPE_SINGLE_QUOTE)
//          t_node *quote_node = initnode(vars->current->type, vars->current->args[0]); // Recreate the quote node info
//          if (!quote_node) { free_token_node(node); free(content); return (0); }

//          // Link ARGS node then Quote node
//          int node_freed = build_token_linklist(vars, node);
//          if (!node_freed) free_if_orphan_node(node, vars);

//          int quote_node_freed = build_token_linklist(vars, quote_node);
//          if (!quote_node_freed) free_if_orphan_node(quote_node, vars);


//          free(content); // Content is owned by the ARGS node
//          return (1); // Handled as regular argument
//     }

//     // Should not be reached if logic is correct
//     free(content);
//     return(0);
// }
/*
Handles quoted text specifically when it's identified as a redirection target.
- Checks if the quote is adjacent to a previous token AND if we are in a redirection context (using cache).
- If yes, attempts to join the content with the cached redirection target node.
- If not adjacent or not in context, finds the last redirection operator needing a target.
- If a suitable redirection operator is found, creates a new ARGS node for the content and links it as the target. Sets the redirection context cache.
- If no suitable redirection operator is found, treats the quoted text as a regular argument (fallback).
Returns:
- 1 on success (content ownership transferred or freed).
- 0 on failure (content is freed).
*/
int handle_redir_target(char *content, t_vars *vars)
{
    t_node *redir_node = NULL;
    t_node *file_node = NULL;
    int joined_via_cache = 0;

    fprintf(stderr, "[DEBUG-REDIR-ENTRY] Entering handle_redir_target for '%s'\n", content); // ADDED
    fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Current context: in_redir_context=%d, last_target=%p\n",
            vars->pipes->in_redir_context, (void *)vars->pipes->last_redir_target); // ADDED

    // Check adjacency and context first for potential joining
    check_token_adj(vars->partial_input, vars); // Ensure adjacency is up-to-date
    fprintf(stderr, "[DEBUG-QUOTE-FLOW] Current adjacency: left=%d, right=%d\n", vars->adj_state[0], vars->adj_state[1]); // ADDED

    if (vars->adj_state[0] && vars->pipes->in_redir_context && vars->pipes->last_redir_target)
    {
        fprintf(stderr, "[DEBUG-QUOTE] Adjacency and context cache hit. Attempting join.\n"); // ADDED
        if (join_with_cached_target(content, vars))
        {
            joined_via_cache = 1;
            // Maintain context state
            fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Maintained context state via cache: in_redir_context=%d, target=%p\n",
                    vars->pipes->in_redir_context, (void *)vars->pipes->last_redir_target); // ADDED
            return (1); // Successfully joined, content freed within join_with_cached_target
        }
        else
        {
            // Join failed, but content was freed in join_with_cached_target if join was attempted
            // If join wasn't attempted (e.g., wrong type), content is still held.
            // Fall through to treat as a new target or regular arg, but need to be careful about freeing content.
            // For now, assume join_with_cached_target frees content on failure if join was possible.
            // If join wasn't possible (e.g. wrong type), content is still valid.
            fprintf(stderr, "[DEBUG-QUOTE] Join via cache failed or not applicable.\n"); // ADDED
            // Let's assume content is freed if join_with_cached_target returns 0 after attempting join.
            // If it returned 0 because join wasn't applicable (e.g. wrong type), content is NOT freed yet.
            // To be safe, let's proceed assuming content might still need freeing if we error out later.
            // Resetting joined_via_cache just in case.
            joined_via_cache = 0;
        }
    }
    else
    {
        fprintf(stderr, "[DEBUG-QUOTE] No adjacency or context cache miss. Looking for new redir target.\n"); // ADDED
    }

    // If not joined via cache, find the redirection node that needs a target
    if (!joined_via_cache)
    {
        redir_node = find_last_redir(vars);
        if (redir_node)
        {
            fprintf(stderr, "[DEBUG-QUOTE] Found redirection node type: %s without target\n",
                    get_token_str(redir_node->type));

            file_node = initnode(TYPE_ARGS, content); // Create ARGS node for the content
            if (!file_node)
            {
                free(content);
                return (0);
            }

            link_file_to_redir(redir_node, file_node, vars); // Link ARGS node as the target
            fprintf(stderr, "[DEBUG-QUOTE] Successfully linked file to redirection\n"); // ADDED

            // Set redirection context cache
            vars->pipes->in_redir_context = 1;
            vars->pipes->last_redir_target = file_node; // Cache the new target node
            fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Setting context: in_redir_context=1, target=%p, redir=%p\n",
                    (void *)vars->pipes->last_redir_target, (void *)redir_node); // ADDED

            free(content); // Content is now owned by file_node
            return (1);	   // Success
        }
        else
        {
            // No suitable redirection node found, treat as a regular argument
            fprintf(stderr, "[DEBUG-QUOTE] No suitable redirection node found. Treating as regular argument.\n"); // ADDED
            // Reset redirection context as this quote breaks it
            vars->pipes->in_redir_context = 0;
            vars->pipes->last_redir_target = NULL;
            fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Resetting context due to non-redir quote.\n"); // ADDED

            // --- Fallback: Process as regular quoted argument ---
            // This part handles cases like `echo "hello" > file` where "hello" is not a redir target
            // Or if a quote appears without a preceding redirection operator needing a target.

            // Try merging first if adjacent
            if (merge_quoted_token(vars->partial_input, content, vars))
            {
                fprintf(stderr, "[DEBUG-QUOTE] Fallback: Merged '%s' via merge_quoted_token.\n", content); // ADDED
                free(content); // Merged, content no longer needed separately
                return (1);
            }

            // If merge fails or not applicable, create a new ARGS node
            t_node *node = initnode(TYPE_ARGS, content); // Create ARGS node for the quote content
            if (!node) { free(content); return (0); }

            // --- REMOVE/COMMENT OUT QUOTE NODE CREATION ---
            // // Add the quote node itself (TYPE_DOUBLE_QUOTE or TYPE_SINGLE_QUOTE)
            // t_node *quote_node = initnode(vars->current->type, vars->current->args[0]); // Recreate the quote node info - INCORRECT LOGIC ANYWAY
            // if (!quote_node) { free_token_node(node); free(content); return (0); }
            // --- END REMOVAL ---

            // Link ARGS node
            int node_freed = build_token_linklist(vars, node);
            if (!node_freed) free_if_orphan_node(node, vars);

            // --- REMOVE/COMMENT OUT QUOTE NODE LINKING ---
            // int quote_node_freed = build_token_linklist(vars, quote_node);
            // if (!quote_node_freed) free_if_orphan_node(quote_node, vars);
            // --- END REMOVAL ---


            free(content); // Content is owned by the ARGS node
            return (1); // Handled as regular argument
        }
    }

    // Should not be reached if logic is correct and content is handled in all paths
    fprintf(stderr, "[DEBUG-QUOTE-ERROR] Reached unexpected end of handle_redir_target for '%s'. Freeing content.\n", content); // ADDED
    free(content);
    return(0);
}

/*
Master control function for processing quoted text in shell input.
This function controls the complete handling of quoted strings:
 - Extracts content from between quotes (single or double).
 - Handles variable expansion for double-quoted text.
 - Creates appropriate token structures based on context.
 - Processes special cases like redirection targets (">file.txt").
 - Manages token adjacency and merging.
Return:
 - 1 on successful processing.
 - 0 on extraction failure or other errors.
Example flows:
 - "echo hello" -> Creates command with argument.
 - echo "hello world" -> Appends argument to existing command.
 - echo > "file.txt" -> Creates file node for redirection.
*/
// int	process_quote_char(char *input, t_vars *vars, int is_redir_target)
// {
// 	int		quote_type;
// 	char	*content;
// 	t_node	*cmd_node;

// 	content = get_quoted_str(input, vars, &quote_type);
// 	if (!content)
// 		return (0);
// 	if (is_redir_target)
// 		return (handle_redir_target(content, vars));
// 	cmd_node = (process_quoted_str(&content, quote_type, vars));
// 	if (!cmd_node && vars->adj_state[0] == 0)
// 		return (make_quoted_cmd(content, input, vars));
// 	else if (!cmd_node)
// 	{
// 		if (!merge_quoted_token(input, content, vars))
// 			return (token_cleanup_error(content, vars));
// 		return (1);
// 	}
// 	if (!merge_quoted_token(input, content, vars))
// 	{
// 		append_arg(cmd_node, content, quote_type);
// 		cleanup_and_process_adj(content, input, vars);
// 	}
// 	return (1);
// }
// int	process_quote_char(char *input, t_vars *vars, int is_redir_target)
// {
// 	int		quote_type;
// 	char	*content;
// 	t_node	*cmd_node;
	
// 	fprintf(stderr, "[DEBUG-QUOTE-FLOW] Starting quote processing at position %d\n", vars->pos);
// 	fprintf(stderr, "[DEBUG-QUOTE-FLOW] Current adjacency: left=%d, right=%d\n", 
//        vars->adj_state[0], vars->adj_state[1]);
// 	content = get_quoted_str(input, vars, &quote_type);
// 	if (!content)
// 		return (0);
// 	if (is_redir_target)
// 		return (handle_redir_target(content, vars));
// 	cmd_node = (process_quoted_str(&content, quote_type, vars));
// 	if (!cmd_node && vars->adj_state[0] == 0)
// 		return (make_quoted_cmd(content, input, vars));
// 	else if (!cmd_node)
// 	{
// 		if (!merge_quoted_token(input, content, vars))
// 			return (token_cleanup_error(content, vars));
// 		return (1);
// 	}
// 	if (!merge_quoted_token(input, content, vars))
// 	{
// 		append_arg(cmd_node, content, quote_type);
// 		cleanup_and_process_adj(content, input, vars);
// 	}
// 	return (1);
// }
// int process_quote_char(char *input, t_vars *vars, int is_redir_target)
// {
//     char *content;
//     int quote_type;
//     char *processed_content; // Use a separate pointer for potentially expanded content

//     // --- ADDED DEBUG PRINT ---
//     fprintf(stderr, "[DEBUG-QUOTE-FLOW] process_quote_char called for pos %d, is_redir_target=%d, prev_type=%s\n",
//             vars->pos, is_redir_target, get_token_str(vars->prev_type));
//     // --- END ADDED DEBUG PRINT ---

//     content = get_quoted_str(input, vars, &quote_type);
//     if (!content) return (0);

//     // Process content (expansion for double quotes)
//     if (quote_type == TYPE_DOUBLE_QUOTE)
//     {
//         processed_content = expand_quoted_str(content, vars);
//         free(content); // Free original content
//         if (!processed_content) return (0); // Expansion failed
//     }
//     else
//     {
//         processed_content = content; // Use original content for single quotes
//     }


//     if (is_redir_target)
//     {
//         // --- ADDED DEBUG PRINT ---
//         fprintf(stderr, "[DEBUG-QUOTE-FLOW] Entering redir target path for '%s'\n", processed_content);
//         // --- END ADDED DEBUG PRINT ---
//         if (handle_redir_target(processed_content, vars))
//             return (1); // Success (handle_redir_target frees processed_content)
//         else
//         {
//             // handle_redir_target should free content on failure too
//             return (0); // Error
//         }
//     }
//     else // is_redir_target is 0
//     {
//         // --- ADDED DEBUG PRINT ---
//         fprintf(stderr, "[DEBUG-QUOTE-FLOW] Entering NON-redir target path for '%s'\n", processed_content);
//         // --- END ADDED DEBUG PRINT ---

//         // --- ADDED: Check context state here too ---
//         fprintf(stderr, "[DEBUG-QUOTE-FLOW-NONREDIR] Context check: adj[0]=%d, in_ctx=%d, last_target=%p\n",
//                 vars->adj_state[0], vars->pipes->in_redir_context, (void*)vars->pipes->last_redir_target);
//         // --- END ADDED ---

//         // --- ADDED: Explicitly call handle_redir_target if context suggests it ---
//         // This is experimental - might cause issues if is_redir_target was correctly 0.
//         check_token_adj(input, vars); // Re-check adjacency right before decision
//         if (vars->adj_state[0] && vars->pipes->in_redir_context && vars->pipes->last_redir_target) {
//              fprintf(stderr, "[DEBUG-QUOTE-FLOW-NONREDIR] Forcing handle_redir_target call due to context...\n");
//              if (handle_redir_target(processed_content, vars)) { // Pass processed_content
//                  fprintf(stderr, "[DEBUG-QUOTE-FLOW-NONREDIR] Forced handle_redir_target succeeded.\n");
//                  return (1); // handle_redir_target frees processed_content on success
//              } else {
//                  fprintf(stderr, "[DEBUG-QUOTE-FLOW-NONREDIR] Forced handle_redir_target failed.\n");
//                  // If forced call fails, handle_redir_target should have freed processed_content
//                  return (0);
//              }
//         }
//         // --- END ADDED ---


//         // Original non-redir path
//         // Reset context if we reach here, as it's definitely not a redirection target continuation
//         vars->pipes->in_redir_context = 0;
//         vars->pipes->last_redir_target = NULL;
//         fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Resetting context in non-redir path.\n"); // ADDED

//         if (merge_quoted_token(input, processed_content, vars))
//         {
//             // --- ADDED Safer DEBUG PRINT ---
//             fprintf(stderr, "[DEBUG-QUOTE-FLOW] Merged content successfully via merge_quoted_token.\n");
//             // --- END ADDED ---

//             // --- REMOVED DOUBLE FREE ---
//             // free(processed_content); // Merged, content already freed in handle_tok_join
//             // --- END REMOVAL ---
//             return (1);
//         }
//         else // If merge fails or not applicable
//         {
//             // Determine if it should be CMD or ARGS
//             set_token_type(vars, processed_content); // Use processed_content to determine type
//             if (vars->curr_type == TYPE_CMD)
//             {
//                 if (make_quoted_cmd(processed_content, input, vars)) // make_quoted_cmd takes ownership
//                     return (1);
//                 else
//                 {
//                     // make_quoted_cmd frees content on failure
//                     return (0);
//                 }
//             }
//             else // Treat as ARGS
//             {
//                 t_node *arg_node = initnode(TYPE_ARGS, processed_content);
//                 if (!arg_node)
//                 {
//                     free(processed_content);
//                     return (token_cleanup_error(NULL, vars)); // Pass NULL as content already freed
//                 }
//                 build_token_linklist(vars, arg_node); // Takes ownership of node content via initnode
//                 cleanup_and_process_adj(processed_content, input, vars); // Frees processed_content
//                 return (1);
//             }
//         }
//         // Should not be reached if logic covers all cases
//         fprintf(stderr, "[DEBUG-QUOTE-ERROR] Reached unexpected end of non-redir path for '%s'. Freeing content.\n", processed_content); // ADDED
//         free(processed_content);
//         return(0);
//     }
// }
int	process_quote_char(char *input, t_vars *vars, int is_redir_target)
{
    char	*content;
    int		quote_type;
    char	*processed_content; // Use a separate variable for potentially expanded content

    content = get_quoted_str(input, vars, &quote_type); // Extracts content between quotes
    if (!content)
    {
        vars->error_code = ERR_SYNTAX; // Unclosed quote likely
        return (0);
    }

    // Process potential expansion *before* deciding path
    processed_content = content; // Start with original content
    if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(content, '$'))
    {
        processed_content = expand_quoted_str(content, vars); // Expands variables in double quotes
        free(content); // Free original content
        if (!processed_content)
        {
            vars->error_code = ERR_DEFAULT; // Malloc error during expansion
            return (0);
        }
    }
    // Now use 'processed_content' which is either original or expanded (and owns the memory)

    fprintf(stderr, "[DEBUG-QUOTE-FLOW] process_quote_char called for pos %d, is_redir_target=%d, prev_type=%s\n",
            vars->pos, is_redir_target, get_token_str(vars->prev_type));

    if (is_redir_target)
    {
        fprintf(stderr, "[DEBUG-QUOTE-FLOW] Entering redir target path for '%s'\n", processed_content);
        // handle_redir_target now takes ownership of processed_content
        if (!handle_redir_target(processed_content, vars))
        {
            // handle_redir_target should free processed_content on failure
            vars->error_code = ERR_SYNTAX; // Or appropriate error
            return (0);
        }
        // Success for redir target path
    }
    else // is_redir_target == 0
    {
        fprintf(stderr, "[DEBUG-QUOTE-FLOW] Entering NON-redir target path for '%s'\n", processed_content);

        // --- REMOVED PROBLEMATIC FORCED CALL ---
        // The check `if (vars->adj_state[0] == 0 && vars->pipes->in_redir_context == 1 ...)` is removed.
        // A non-adjacent token should not be forced into the redirection target path.

        // Standard non-redir target handling: merge if adjacent, else create new node
        // merge_quoted_token now takes ownership of processed_content if it succeeds
        if (merge_quoted_token(input, processed_content, vars))
        {
            fprintf(stderr, "[DEBUG-QUOTE-FLOW] Merged content successfully via merge_quoted_token.\n");
            // merge_quoted_token handles freeing processed_content on success
        }
        else
        {
            fprintf(stderr, "[DEBUG-QUOTE-FLOW] Merge failed or not applicable, creating new token.\n");
            // make_quoted_cmd now takes ownership of processed_content
            if (!make_quoted_cmd(processed_content, vars))
            {
                // make_quoted_cmd should free processed_content on failure
                return (0); // Error already set by make_quoted_cmd
            }
            // Success, make_quoted_cmd handled processed_content
        }
    }
    // Reset context if we just finished processing a non-target quote
    // This prevents subsequent non-adjacent args from thinking they are in context
    if (!is_redir_target) {
        vars->pipes->in_redir_context = 0;
        vars->pipes->last_redir_target = NULL;
        fprintf(stderr, "[DEBUG-REDIR-CONTEXT] Reset context after non-target quote.\n");
    }

    return (1); // General success
}

/*
Validates that all redirection operators in the token list have valid targets.
- Ensures no redirection is at the end of input
- Ensures no redirection is followed by another operator
- Reports appropriate syntax errors
Returns:
- 1 if all redirections have valid targets
- 0 otherwise (with error_code set)
*/
int	validate_redir_targets(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (!validate_single_redir(current, vars))
				return (0);
		}
		current = current->next;
	}
	return (1);
}

/*
Creates and adds a new token to the token list.
- Creates a node with the given content and type
- Adds the node to the token list
- Handles error conditions
Returns:
- 1 on success
- 0 on failure
Works with process_quote_char() for handling quoted arguments.
*/
// int add_token(char *content, t_tokentype type, t_vars *vars)
// {
//     t_node *node;
//     int node_freed;
    
//     if (!content || !vars)
//         return (0);
    
//     // Create a new token node
//     node = initnode(type, content);
//     if (!node)
//         return (0);
    
//     // Add the node to the token list
//     node_freed = build_token_linklist(vars, node);
//     if (!node_freed)
//     {
//         // If the node wasn't freed (merged), check if it's orphaned
//         free_if_orphan_node(node, vars);
//     }
    
//     return (1);
// }
int add_token(char *content, t_tokentype type, t_vars *vars)
{
    t_node *node;
    int node_freed;
    
    if (!content || !vars)
        return (0);
    
    // Create a new token node
    node = initnode(type, content);
    if (!node)
        return (0);
    
    // Add the node to the token list
    node_freed = build_token_linklist(vars, node);
    if (!node_freed)
    {
        // If the node wasn't freed (merged), check if it's orphaned
        free_if_orphan_node(node, vars);
    }
    
    return (1);
}
