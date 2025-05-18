/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 01:15:21 by bleow             #+#    #+#             */
/*   Updated: 2025/05/18 14:31:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check token position handling
- Increments position counter if not in heredoc mode
Returns:
-1 if next_flag is set (to continue main loop).
- 0 to continue normal processing.
*/
int	chk_move_pos(t_vars *vars, int hd_is_delim)
{
	if (vars->next_flag)
		return (1);
	if (!hd_is_delim)
		vars->pos++;
	return (0);
}

/*
Processes right-adjacent text for the current token.
- Extracts text between vars->start and vars->pos
- Joins this text with the last argument of the current token
- Handles memory allocation and cleanup
- Updates token content to include adjacent characters
Example: 
For "echo hello"world, joins "world" to "hello"
	to create a single argument "helloworld"
*/
// void	handle_right_adj(char *input, t_vars *vars)
// {
// 	char	*adjacent_text;
// 	char	*joined;
// 	int		arg_idx;

// 	if (vars->pos <= vars->start)
// 		return ;
// 	adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
// 	if (!adjacent_text)
// 		return ;
// 	if (vars->current && vars->current->args && vars->current->args[0])
// 	{
// 		arg_idx = 0;
// 		while (vars->current->args[arg_idx + 1])
// 			arg_idx++;
// 		joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
// 		if (joined)
// 		{
// 			free(vars->current->args[arg_idx]);
// 			vars->current->args[arg_idx] = joined;
// 		}
// 	}
// 	free(adjacent_text);
// }
// void handle_right_adj(char *input, t_vars *vars)
// {
//     char *adjacent_text;
//     char *joined;
//     int arg_idx;

//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Entering handle_right_adj (tokenize_utils.c). Processing text from %d to %d\n", 
//             vars->start, vars->pos);
            
//     if (vars->pos <= vars->start)
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: No text to process (pos %d <= start %d). Returning.\n", vars->pos, vars->start);
//         return;
//     }
        
//     adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_substr for adjacent_text (start %d, len %d) result: '%s' (%p)\n", vars->start, vars->pos - vars->start, adjacent_text ? adjacent_text : "NULL", (void*)adjacent_text);
//     if (!adjacent_text)
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_substr FAILED for adjacent_text. Returning.\n");
//         return;
//     }
        
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Found adjacent text: '%s'\n", adjacent_text);
    
//     // Add debug print to verify current token and its args
//     if (vars->current && vars->current->args) {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token: %p, type: %s, args[0]: '%s', num_args: %zu\n", 
//             (void*)vars->current, get_token_str(vars->current->type), vars->current->args[0] ? vars->current->args[0] : "NULL", ft_arrlen(vars->current->args));
//     } else if (vars->current) {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token: %p, type: %s, args: NULL\n", (void*)vars->current, get_token_str(vars->current->type));
//     } else {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token is NULL.\n");
//     }
    
//     // Fallback logic from previous suggestions (ensure it's active or tested)
//     if (!vars->current || !vars->current->args || !vars->current->args[0])
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: No valid current token or args[0] is NULL. Attempting to find last token.\n");
//         t_node *last = vars->head;
//         if (last) {
//             while (last->next)
//                 last = last->next;
//             vars->current = last; // Attempt to use the absolute last token
//             fprintf(stderr, "DEBUG-RIGHT-ADJ: Fallback: Set vars->current to last token in list: %p ('%s')\n", (void*)vars->current, (vars->current && vars->current->args && vars->current->args[0]) ? vars->current->args[0] : "N/A");
//         }
//         if (!vars->current || !vars->current->args || !vars->current->args[0])
//         {
//             fprintf(stderr, "DEBUG-RIGHT-ADJ: Fallback failed. Still no valid current token to join with. Freeing adjacent_text and returning.\n");
//             free(adjacent_text);
//             return;
//         }
//     }
    
//     arg_idx = 0;
//     while (vars->current->args[arg_idx + 1]) // find last arg index
//         arg_idx++;
            
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Attempting to join current_arg (idx %d) '%s' with adjacent_text '%s'\n", 
//             arg_idx, vars->current->args[arg_idx], adjacent_text);
                
//     joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin result for joined: %p ('%s')\n", (void*)joined, joined ? joined : "NULL STR");

//     if (joined)
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin SUCCEEDED. Old arg: '%s'. New joined: '%s'\n", vars->current->args[arg_idx], joined);
//         free(vars->current->args[arg_idx]);
//         vars->current->args[arg_idx] = joined;
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Successfully joined. Current token's arg %d is now: '%s'\n", arg_idx, vars->current->args[arg_idx]);
//         // Assuming quote types for the joined part need to be updated
//         // This was a missing piece if adjacent_text comes from non-quoted source
//         // For simplicity, let's assume update_quote_types would be called by a higher level if needed
//         // or that adjacent_text is treated as unquoted (type 0)
//     }
//     else
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin FAILED. adjacent_text '%s' was not joined.\n", adjacent_text);
//         // If ft_strjoin fails, adjacent_text is still freed, but the original arg is untouched.
//     }
//     free(adjacent_text);
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Exiting handle_right_adj.\n");
// }
// void handle_right_adj(char *input, t_vars *vars)
// {
//     char *adjacent_text;
//     char *joined;
//     int arg_idx;

//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Processing text from %d to %d\n", 
//             vars->start, vars->pos);
            
//     if (vars->pos <= vars->start)
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: No text to process (pos %d <= start %d)\n", 
//                 vars->pos, vars->start);
//         return;
//     }
        
//     adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
//     if (!adjacent_text)
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Failed to extract adjacent text\n");
//         return;
//     }
        
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Found adjacent text: '%s'\n", adjacent_text);
    
//     // Add debug print to verify current token and its args
//     if (vars->current) {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token: %p, type: %s, has args: %d\n", 
//                 (void*)vars->current, get_token_str(vars->current->type),
//                 (vars->current->args && vars->current->args[0]) ? 1 : 0);
//     } else {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token is NULL\n");
//     }
    
//     // Add fallback logic for when vars->current is not valid
//     if (!vars->current || !vars->current->args || !vars->current->args[0])
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: No valid current token, finding last token\n");
        
//         // Find the last valid token in the list as fallback
//         t_node *last = vars->head;
//         if (last) {
//             while (last->next)
//                 last = last->next;
            
//             vars->current = last;
//             fprintf(stderr, "DEBUG-RIGHT-ADJ: Found last token: %p ('%s')\n", 
//                    (void*)last, (last && last->args) ? last->args[0] : "NULL");
//         }
        
//         if (!vars->current || !vars->current->args || !vars->current->args[0])
//         {
//             fprintf(stderr, "DEBUG-RIGHT-ADJ: Still no valid token, aborting join\n");
//             free(adjacent_text);
//             return;
//         }
//     }
    
//     arg_idx = 0;
//     while (vars->current->args[arg_idx + 1])
//         arg_idx++;
            
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Joining '%s' with '%s'\n", 
//             vars->current->args[arg_idx], adjacent_text);
                
//     joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
    
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin result: %p\n", (void*)joined);
    
//     if (joined)
//     {
//         free(vars->current->args[arg_idx]);
//         vars->current->args[arg_idx] = joined;
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: Result: '%s'\n", joined);
        
//         // Add this to update quote types when joining adjacent text
//         if (vars->current->arg_quote_type && vars->current->arg_quote_type[arg_idx])
//         {
//             fprintf(stderr, "DEBUG-RIGHT-ADJ: Updating quote types for joined text\n");
//             int result = update_quote_types(vars, arg_idx, adjacent_text);
//             fprintf(stderr, "DEBUG-RIGHT-ADJ: update_quote_types result: %d\n", result);
//         }
//     }
//     else
//     {
//         fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin failed\n");
//     }
    
//     free(adjacent_text);
//     fprintf(stderr, "DEBUG-RIGHT-ADJ: Exiting handle_right_adj\n");
// }
void handle_right_adj(char *input, t_vars *vars)
{
    char *adjacent_text;
    char *joined;
    int arg_idx;

    fprintf(stderr, "DEBUG-RIGHT-ADJ: Processing text from %d to %d\n", 
            vars->start, vars->pos);
            
    if (vars->pos <= vars->start)
    {
        fprintf(stderr, "DEBUG-RIGHT-ADJ: No text to process (pos %d <= start %d)\n", 
                vars->pos, vars->start);
        return;
    }
        
    adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!adjacent_text)
    {
        fprintf(stderr, "DEBUG-RIGHT-ADJ: Failed to extract adjacent text\n");
        return;
    }
        
    fprintf(stderr, "DEBUG-RIGHT-ADJ: Found adjacent text: '%s'\n", adjacent_text);
    
    // Add debug print to verify current token and its args
    if (vars->current) {
        fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token: %p, type: %s, has args: %d\n", 
                (void*)vars->current, get_token_str(vars->current->type),
                (vars->current->args && vars->current->args[0]) ? 1 : 0);
    } else {
        fprintf(stderr, "DEBUG-RIGHT-ADJ: Current token is NULL\n");
    }
    
    // Add fallback logic for when vars->current is not valid
    if (!vars->current || !vars->current->args || !vars->current->args[0])
    {
        fprintf(stderr, "DEBUG-RIGHT-ADJ: No valid current token, finding last token\n");
        
        // Find the last valid token in the list as fallback
        t_node *last = vars->head;
        if (last) {
            while (last->next)
                last = last->next;
            
            vars->current = last;
            fprintf(stderr, "DEBUG-RIGHT-ADJ: Found last token: %p ('%s')\n", 
                   (void*)last, (last && last->args) ? last->args[0] : "NULL");
        }
        
        if (!vars->current || !vars->current->args || !vars->current->args[0])
        {
            fprintf(stderr, "DEBUG-RIGHT-ADJ: Still no valid token, aborting join\n");
            free(adjacent_text);
            return;
        }
    }
    
    arg_idx = 0;
    while (vars->current->args[arg_idx + 1])
        arg_idx++;
            
    fprintf(stderr, "DEBUG-RIGHT-ADJ: Joining '%s' with '%s'\n", 
            vars->current->args[arg_idx], adjacent_text);
                
    joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
    
    fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin result: %p\n", (void*)joined);
    
    if (joined)
    {
        free(vars->current->args[arg_idx]);
        vars->current->args[arg_idx] = joined;
        fprintf(stderr, "DEBUG-RIGHT-ADJ: Result: '%s'\n", joined);
        
        // Add this to update quote types when joining adjacent text
        if (vars->current->arg_quote_type && vars->current->arg_quote_type[arg_idx])
        {
            fprintf(stderr, "DEBUG-RIGHT-ADJ: Updating quote types for joined text\n");
            int result = update_quote_types(vars, arg_idx, adjacent_text);
            fprintf(stderr, "DEBUG-RIGHT-ADJ: update_quote_types result: %d\n", result);
        }
    }
    else
    {
        fprintf(stderr, "DEBUG-RIGHT-ADJ: ft_strjoin failed\n");
    }
    
    free(adjacent_text);
    fprintf(stderr, "DEBUG-RIGHT-ADJ: Exiting handle_right_adj\n");
}

/*
Extracts raw delimiter string from input
- Sets start position at current position
- Advances position until delimiter end is found
- Validates non-empty delimiter
- Handles memory allocation
Returns:
- Raw delimiter string on success
- NULL on error (with error_code set)
*/
char	*get_delim_str(char *input, t_vars *vars, int *error_code)
{
	int		moves;
	char	*ori_delim_str;

	vars->start = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos])
		&& !is_operator_token(get_token_at(input, vars->pos, &moves)))
	{
		vars->pos++;
	}
	if (vars->pos == vars->start)
	{
		tok_syntax_error_msg("newline", vars);
		*error_code = 1;
		return (NULL);
	}
	ori_delim_str = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!ori_delim_str)
	{
		vars->error_code = ERR_DEFAULT;
		*error_code = 1;
		return (NULL);
	}
	return (ori_delim_str);
}

/*
Process any accumulated text before a special character.
Creates a token from the text between vars->start and vars->pos.
*/
// void	handle_text(char *input, t_vars *vars) PRE ADDED DEBUG PRINTS
// {
// 	char	*token_preview;

// 	token_preview = NULL;
// 	if (vars->pos > vars->start)
// 	{
// 		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
// 		set_token_type(vars, token_preview);
// 		handle_string(input, vars);
// 		if (token_preview)
// 		{
// 			free(token_preview);
// 		}
// 		vars->start = vars->pos;
// 	}
// }
void	handle_text(char *input, t_vars *vars)
{
	char	*token_preview;

	token_preview = NULL;
	if (vars->pos > vars->start)
	{
		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
		fprintf(stderr, "DEBUG-TOKEN: Creating token from '%s'\n", token_preview ? token_preview : "NULL");
		set_token_type(vars, token_preview);
		fprintf(stderr, "DEBUG-TOKEN: Token type set to %d\n", vars->curr_type);
		handle_string(input, vars);
		if (token_preview)
		{
			free(token_preview);
		}
		vars->start = vars->pos;
	}
}
