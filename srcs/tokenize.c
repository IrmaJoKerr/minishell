/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/09 23:23:18 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <stdio.h>

/*
Sets appropriate token type based on position and context
Determines if current token should be a command or argument
Updates vars->curr_type accordingly
*/
void set_token_type(t_vars *vars, char *input)
{
    int moves;
    t_tokentype special_type = 0;
    
    // Save previous type
    vars->prev_type = vars->curr_type;
    
    // Check for variable expansion
    if (input && input[0] == '$')
    {
        vars->curr_type = TYPE_EXPANSION;
        return;
    }
    
    // Check for special token types
    if (input && *input)
    {
        special_type = get_token_at(input, 0, &moves);
        if (special_type != 0)
        {
            vars->curr_type = special_type;
            return;
        }
    }
    
    // Default to position-based command/argument detection
    if (!vars->head || vars->prev_type == TYPE_PIPE)
    {
        vars->curr_type = TYPE_CMD;
    }
    else
    {
        vars->curr_type = TYPE_ARGS;
    }
}

/*
Creates a new token node and adds it to the token list.
- Creates node of current token type.
- Sets node data from the provided string.
- Adds node to linked list and updates current pointer.
Works with lexerlist() during token creation.

Example: For input "echo hello"
- When maketoken("echo") called with TYPE_CMD
- Creates command node with args[0] = "echo"
- Adds to token list as head or appends to current
*/
void	maketoken_with_type(char *token, t_tokentype type, t_vars *vars)
{
	t_node	*node;
	
	node = NULL;
	if (!token || !*token)
	{
        return;
    }
    
    node = initnode(type, token);
    if (node)
	{
        // Add to linked list
        build_token_linklist(vars, node);
    }
}

/*
 * Checks if tokens should be joined (no whitespace in between)
 * Returns 1 if they should be joined, 0 otherwise
 */
// int is_adjacent_token(char *input, int pos)
// {
//     if (pos <= 0)
//         return (0);
    
//     // Check if previous character is not whitespace
//     if (ft_isspace(input[pos - 1]))
//         return (0);
    
//     // It's adjacent if previous char is not whitespace
//     return (1);
// }
int is_adjacent_token(char *input, int pos)
{
    if (pos <= 0)
        return (0);
    
    // Don't join if previous char is an operator
    if (input[pos - 1] == '<' || input[pos - 1] == '>' || 
        input[pos - 1] == '|' || input[pos - 1] == ';')
        return (0);
        
    // Don't join if current position is an operator
    if ((size_t)pos < ft_strlen(input) && 
        (input[pos] == '<' || input[pos] == '>' || 
         input[pos] == '|' || input[pos] == ';'))
        return (0);
    
    // Check if previous character is not whitespace
    if (ft_isspace(input[pos - 1]))
        return (0);
    
    // It's adjacent if previous char is not whitespace or operator
    return (1);
}

// /*
//  * Joins token with the last argument of the command node
//  * Returns 1 if joined successfully, 0 if failed
//  */
// int join_with_cmd_arg(t_node *cmd_node, char *token_val)
// {
//     int     arg_count;
//     char    *last_arg;
//     char    *new_arg;
      
//     if (!cmd_node || !token_val)
//         return (0);
//     // Count arguments
//     arg_count = 0;
//     while (cmd_node->args[arg_count])
//         arg_count++; 
//     if (arg_count < 1)
//         return (0);     
//     // Get last argument
//     last_arg = cmd_node->args[arg_count - 1];
//     if (!last_arg)
//         return (0); 
//     // Join the strings
//     new_arg = ft_strjoin(last_arg, token_val);
//     if (!new_arg)
//         return (0); 
//     // Replace the last argument with joined string
//     free(cmd_node->args[arg_count - 1]);
//     cmd_node->args[arg_count - 1] = new_arg;
     
//     return (1);
// }

/*
Process variable expansion characters ($VAR or $?)
- Handles variable expansion with $.
- Updates position after expansion.
- Extracts variable name and its value.
- Joins adjacent expansions with previous text
Returns:
- 1 if expansion was processed
- 0 if otherwise.

Example: For input "echo $HOME"
- Detects $ at position
- Creates expansion token for HOME
- Updates position past the variable name
*/
// int make_exp_token(char *input, int *i, t_vars *vars)
// {
//     int     start_pos;
//     char    *var_name;
//     char    *var_value;
//     t_node  *cmd_node;
//     int     is_adjacent;
    
//     start_pos = *i;
    
//     is_adjacent = is_adjacent_token(input, start_pos);
//     /* Skip $ */
//     (*i)++;
    
//     /* Special case for $? */
//     if (input[*i] == '?')
//     {
//         (*i)++;
//         var_value = ft_itoa(vars->error_code);
        
//         if (!var_value)
//             return (0);
        
//         cmd_node = find_cmd(vars->head, NULL, FIND_LAST, NULL);
        
//         // If adjacent to previous text, join with last argument
//         if (cmd_node && is_adjacent)
//         {
//             if (join_with_cmd_arg(cmd_node, var_value))
//             {
//                 free(var_value);
//                 vars->start = *i;
//                 return (1);
//             }
//         }
        
//         if (cmd_node)
//             append_arg(cmd_node, var_value, 0);
//         else
//             maketoken_with_type(var_value, TYPE_ARGS, vars);
        
//         free(var_value);
//         vars->start = *i;
        
//         return (1);
//     }
    
//     /* Process variable name */
//     if (ft_isalpha(input[*i]) || input[*i] == '_')
//     {
//         /* Read variable name */
//         start_pos = *i;
        
//         while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
//             (*i)++;
        
//         var_name = ft_substr(input, start_pos, *i - start_pos);
        
//         if (!var_name)
//             return (0);
        
//         var_value = get_env_val(var_name, vars->env);
//         free(var_name);
        
//         cmd_node = find_cmd(vars->head, NULL, FIND_LAST, NULL);
        
//         // If adjacent to previous text, try joining with last argument
//         if (cmd_node && is_adjacent)
//         {
//             if (join_with_cmd_arg(cmd_node, var_value ? var_value : ""))
//             {
//                 if (var_value)
//                     free(var_value);
//                 vars->start = *i;
//                 return (1);
//             }
//         }
        
//         // Fall back to standard behavior
//         if (cmd_node && var_value)
//             append_arg(cmd_node, var_value, 0);
//         else if (cmd_node)
//             append_arg(cmd_node, "", 0);
//         else if (var_value)
//             maketoken_with_type(var_value, TYPE_ARGS, vars);
//         else
//             maketoken_with_type("", TYPE_ARGS, vars);
        
//         if (var_value)
//             free(var_value);
            
//         vars->start = *i;
        
//         return (1);
//     }
    
//     /* Just a lone $ character */
//     (*i) = start_pos + 1;
//     vars->start = start_pos;
    
//     return (0);
// }
/*
 * Creates an expansion token from input
 * - Identifies $ variables and creates TYPE_EXPANSION tokens
 * - Handles $? as special case (exit status)
 * - Doesn't perform expansion - just identifies tokens
 * Returns 1 if expansion token was created, 0 otherwise
 */
int make_exp_token(char *input, int *i, t_vars *vars)
{
    char		*token;
    t_tokentype	type;
    
    vars->start = *i;
    if (input[*i] == '$' && input[*i + 1] == '?') 
    {
        token = ft_substr(input, vars->start, 2);
        if (!token)
            return (0); 
        type = TYPE_EXIT_STATUS;
        (*i) += 2;
    }
    else if (input[*i] == '$')
    {
        (*i)++;
        while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
            (*i)++; 
        token = ft_substr(input, vars->start, *i - vars->start);
        if (!token)
            return (0);
        type = TYPE_EXPANSION;
    }
    else
        return (0);
    maketoken_with_type(token, type, vars);
    free(token);
    vars->start = *i;
    return (1);
}

/*
Processes quoted characters in the input string.
- Handles single and double quotes.
- Updates position after quoted content.
- Creates tokens for quote content.
Returns:
1 if quote was processed, 0 otherwise.

Example: For input with quotes like "echo 'hello'"
Process single or double quoted text
Handles joining to previous token when adjacent
*/
// int process_quote_char(char *input, int *i, t_vars *vars)
// {
//     int		quote_start;
//     char	quote_char = input[*i];
//     int		quote_type;
//     char	*content;
//     t_node	*cmd_node;
//     int		is_adjacent;
// 	int		quote_len; //For debugging
// 	int		last_arg_idx;
// 	char	*joined;
	
//     // Check for adjacency with previous tokens
//     is_adjacent = is_adjacent_token(input, *i);
//     // Determine quote type (1=single, 2=double)
// 	if (quote_char == '\'')
// 		quote_type = 1;
// 	else
// 		quote_type = 2;
//     // Skip opening quote
//     quote_start = ++(*i);
//     // Find closing quote
//     while (input[*i] && input[*i] != quote_char)
//         (*i)++;
//     // If no closing quote, treat as normal text
//     if (!input[*i])
//     {
//         *i = quote_start;
//         return (0);
//     }
//     // Get quoted content length for debugging
//     quote_len = *i - quote_start;
//     fprintf(stderr, "DEBUG: Quote handling - quote_len=%d, quote_char=%c\n", 
//             quote_len, quote_char);
//     // Extract quoted content
//     content = ft_substr(input, quote_start, quote_len);
//     if (!content)
//         return (0);
//     // Skip closing quote
//     (*i)++;
//     // Special handling for empty quotes
//     if (quote_len == 0)
//         fprintf(stderr, "DEBUG: Empty quote detected\n");
// 	// Add debug print for quote type
//     fprintf(stderr, "DEBUG: Setting quote_type=%d for content='%s'\n", 
// 		quote_type, content);
//     // If adjacent to previous token, join with it
//     if (is_adjacent)
//     {
//         cmd_node = find_cmd(vars->head, NULL, FIND_LAST, NULL);
//         if (cmd_node && cmd_node->args)
//         {
//             // Find last argument
//             last_arg_idx = 0;
//             while (cmd_node->args[last_arg_idx+1])
//                 last_arg_idx++;
//             // Join with last argument
// 			if (cmd_node->arg_quote_type)
//                 fprintf(stderr, "DEBUG: Original arg '%s' had quote_type=%d\n", 
//                     cmd_node->args[last_arg_idx], cmd_node->arg_quote_type[last_arg_idx]);
//             joined = ft_strjoin(cmd_node->args[last_arg_idx], content);
//             fprintf(stderr, "DEBUG: Joining quote '%s' with '%s' = '%s'\n",
//                    cmd_node->args[last_arg_idx], content, joined);   
//             if (joined)
// 			{
//                 free(cmd_node->args[last_arg_idx]);
//                 cmd_node->args[last_arg_idx] = joined;
// 				// Update quote_type when joining with a single-quoted string
//     			// Single quotes (type 1) have highest precedence to prevent expansion
//     			if (quote_type == 1)
// 				{
//         			cmd_node->arg_quote_type[last_arg_idx] = 1;
//         			fprintf(stderr, "DEBUG: Updated quote_type to 1 (single quotes) for '%s'\n", joined);
//     			}
//                 fprintf(stderr, "DEBUG: After join, keeping quote_type=%d for '%s'\n", 
//                     cmd_node->arg_quote_type[last_arg_idx], joined);
//                 free(content);
//                 vars->start = *i;
//                 return (1);
//             }
//         }
//     }
//     // Otherwise add as separate argument
//     cmd_node = find_cmd(vars->head, NULL, FIND_LAST, NULL);
//     if (cmd_node)
//         append_arg(cmd_node, content, quote_type);
//     else
//         maketoken_with_type(content, TYPE_ARGS, vars);
//     free(content);
//     vars->start = *i;
//     return (1);
// }
/*
 * Processes quoted text in input
 * - Handles single and double quotes
 * - Manages empty quotes
 * - Handles adjacency with previous tokens
 * Returns 1 if quote processed successfully, 0 otherwise
 */
// int process_quote_char(char *input, int *i, t_vars *vars)
// {
//     int     quote_start;
//     char    quote_char;
//     int     quote_type;
//     char    *content;
//     t_node  *cmd_node;
//     int     is_adjacent;
//     int     quote_len; // For debugging
//     int     last_arg_idx;
//     char    *joined;
    
// 	quote_char = input[*i];
//     is_adjacent = is_adjacent_token(input, *i);
//     if (quote_char == '\'')
//         quote_type = 1;
//     else
//         quote_type = 2;
//     quote_start = ++(*i);
//     while (input[*i] && input[*i] != quote_char)
//         (*i)++;
//     if (!input[*i])
//     {
//         *i = quote_start;
//         return (0);
//     }
//     quote_len = *i - quote_start;
//     fprintf(stderr, "DEBUG: Quote handling - quote_len=%d, quote_char=%c\n", 
//             quote_len, quote_char);
//     content = ft_substr(input, quote_start, quote_len);
//     if (!content)
//         return (0);
//     (*i)++;
//     if (quote_len == 0)
//         fprintf(stderr, "DEBUG: Empty quote detected\n");
//     fprintf(stderr, "DEBUG: Setting quote_type=%d for content='%s'\n", 
//         quote_type, content);
//     if (is_adjacent)
//     {
//         cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
//         if (cmd_node && cmd_node->args)
//         {
//             last_arg_idx = 0;
//             while (cmd_node->args[last_arg_idx+1])
//                 last_arg_idx++;
//             if (cmd_node->arg_quote_type)
//                 fprintf(stderr, "DEBUG: Original arg '%s' had quote_type=%d\n", 
//                     cmd_node->args[last_arg_idx], cmd_node->arg_quote_type[last_arg_idx]);
//             joined = ft_strjoin(cmd_node->args[last_arg_idx], content);
//             fprintf(stderr, "DEBUG: Joining quote '%s' with '%s' = '%s'\n",
//                    cmd_node->args[last_arg_idx], content, joined);
//             if (joined)
//             {
//                 free(cmd_node->args[last_arg_idx]);
//                 cmd_node->args[last_arg_idx] = joined;
//                 if (quote_type == 1)
//                 {
//                     cmd_node->arg_quote_type[last_arg_idx] = 1;
//                     fprintf(stderr, "DEBUG: Updated quote_type to 1 (single quotes) for '%s'\n", joined);
//                 }
//                 fprintf(stderr, "DEBUG: After join, quote_type=%d for '%s'\n", 
//                     cmd_node->arg_quote_type[last_arg_idx], joined);
//                 free(content);
//                 vars->start = *i;
//                 return (1);
//             }
//         }
//     }
// 	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
//     if (cmd_node)
//         append_arg(cmd_node, content, quote_type);
//     else
//     {
//         if (!vars->head)
//             maketoken_with_type(content, TYPE_CMD, vars);
//         else
//             maketoken_with_type(content, TYPE_ARGS, vars);
//     }
//     free(content);
//     vars->start = *i;
//     return (1);
// }
int process_quote_char(char *input, int *i, t_vars *vars)
{
    int     quote_start;
    char    quote_char;
    int     quote_type;
    char    *content;
    t_node  *cmd_node;
    int     is_adjacent;
    int     quote_len; // For debugging
    int     last_arg_idx;
    char    *joined;
    
    quote_char = input[*i];
    is_adjacent = is_adjacent_token(input, *i);
    if (quote_char == '\'')
        quote_type = 1;
    else
        quote_type = 2;
    quote_start = ++(*i);
    while (input[*i] && input[*i] != quote_char)
        (*i)++;
    if (!input[*i])
    {
        *i = quote_start;
        return (0);
    }
    quote_len = *i - quote_start;
    fprintf(stderr, "DEBUG: Quote handling - quote_len=%d, quote_char=%c\n", 
            quote_len, quote_char);
    content = ft_substr(input, quote_start, quote_len);
    if (!content)
        return (0);
    (*i)++;
    if (quote_len == 0)
        fprintf(stderr, "DEBUG: Empty quote detected\n");
    fprintf(stderr, "DEBUG: Setting quote_type=%d for content='%s'\n", 
        quote_type, content);
    // Process variables in double-quoted strings
    if (quote_type == 2 && ft_strchr(content, '$'))
    {
        char *expanded = ft_strdup("");
        int pos = 0;
        char *chunk;
        char *temp;
        if (!expanded)
        {
            free(content);
            return (0);
        }
        fprintf(stderr, "DEBUG: Pre-processing double-quoted content '%s'\n", content);
        while (content[pos])
        {
            if (content[pos] == '$')
            {
                // Use handle_expansion to process the variable
                char *var_value = handle_expansion(content, &pos, vars);
                if (var_value)
                {
                    // Concatenate the expanded value
                    temp = expanded;
                    expanded = ft_strjoin(expanded, var_value);
                    free(temp);
                    free(var_value);
                    
                    if (!expanded)
                    {
                        free(content);
                        return (0);
                    }
                }
            }
            else
            {
                // Handle regular character
                int start = pos;
                while (content[pos] && content[pos] != '$')
                    pos++;
                chunk = ft_substr(content, start, pos - start);
                if (!chunk)
                {
                    free(expanded);
                    free(content);
                    return (0);
                }
                temp = expanded;
                expanded = ft_strjoin(expanded, chunk);
                free(temp);
                free(chunk);
                
                if (!expanded)
                {
                    free(content);
                    return (0);
                }
            }
        }
        fprintf(stderr, "DEBUG: Expanded double-quoted content from '%s' to '%s'\n", 
                content, expanded);
        
        free(content);
        content = expanded;
    }
    if (is_adjacent)
    {
        cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
        if (cmd_node && cmd_node->args)
        {
            last_arg_idx = 0;
            while (cmd_node->args[last_arg_idx+1])
                last_arg_idx++;
            if (cmd_node->arg_quote_type)
                fprintf(stderr, "DEBUG: Original arg '%s' had quote_type=%d\n", 
                    cmd_node->args[last_arg_idx], cmd_node->arg_quote_type[last_arg_idx]);
            joined = ft_strjoin(cmd_node->args[last_arg_idx], content);
            fprintf(stderr, "DEBUG: Joining quote '%s' with '%s' = '%s'\n",
                   cmd_node->args[last_arg_idx], content, joined);
            if (joined)
            {
                free(cmd_node->args[last_arg_idx]);
                cmd_node->args[last_arg_idx] = joined;
                if (quote_type == 1)
                {
                    cmd_node->arg_quote_type[last_arg_idx] = 1;
                    fprintf(stderr, "DEBUG: Updated quote_type to 1 (single quotes) for '%s'\n", joined);
                }
                fprintf(stderr, "DEBUG: After join, quote_type=%d for '%s'\n", 
                    cmd_node->arg_quote_type[last_arg_idx], joined);
                free(content);
                vars->start = *i;
                return (1);
            }
        }
    }
    cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
    if (cmd_node)
        append_arg(cmd_node, content, quote_type);
    else
    {
        if (!vars->head)
            maketoken_with_type(content, TYPE_CMD, vars);
        else
            maketoken_with_type(content, TYPE_ARGS, vars);
    }
    free(content);
    vars->start = *i;
    return (1);
}

/*
Processes operator characters in the input string.
- Identifies redirection operators and pipe.
- Creates tokens for these operators.
- Updates position past the operator.
Returns:
1 if operator was processed, 0 otherwise.

Example: For input "cmd > file"
- Processes the '>' operator
- Creates redirect token
- Returns 1 to indicate operator was handled
*/
/* 
Processes operator characters like pipes and redirections.
Returns 1 if operator was processed, 0 otherwise.
*/
int process_operator_char(char *input, int *i, t_vars *vars)
{
    int moves;
    t_tokentype token_type;
    
    token_type = get_token_at(input, *i, &moves);
               
    if (token_type == 0)
	{
        return (0);
	}
	vars->curr_type = token_type;   
    // Handle double operators (>>, <<, etc.)
    if (moves == 2) {
        handle_double_operator(input, vars);
    }
    
    // Handle single operators (>, <, |, etc.)
    else {
        handle_single_operator(input, vars);
    }
    
    return (1);
}

/*
Processes redirection operators in the input.
- Handles <, >, << and >> operators.
- Creates tokens for the operators.
- Updates position past the operator.

Example: For input "cat > file.txt"
- When position is at '>'
- Creates redirect token
- Updates position past '>'
*/
int	handle_redirection(char *input, int *pos, t_vars *vars)
{
    int	result;
    
    if (input[*pos] == '<' || input[*pos] == '>' || input[*pos] == '|')
    {
        vars->pos = *pos;
        result = operators(input, vars);
        *pos = vars->pos;
        return (result);
    }
    return (0);
}

/* 
Tokenizes input string with improved quote handling.
Creates tokens for commands, args, quotes, and operators.
*/
// int improved_tokenize(char *input, t_vars *vars)
// {
//     int			moves;
//     t_tokentype	token_type;
//     char		*token_preview;

//     vars->pos = 0;
//     vars->start = 0;
//     vars->quote_depth = 0;
//     while (input && input[vars->pos])
//     {
//         token_type = get_token_at(input, vars->pos, &moves);
//         if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
//         {
//             if (vars->pos > vars->start)
//             {
//                 token_preview = ft_substr(input, vars->start
// 					,vars->pos - vars->start);
//                 set_token_type(vars, token_preview);
//                 handle_string(input, vars);
//                 free(token_preview);
//             }
//             if (process_quote_char(input, &vars->pos, vars))
//                 continue;
//         }
//         if (input[vars->pos] == '$' && !vars->quote_depth)
//         {
//             if (vars->pos > vars->start)
//             {
//                 token_preview = ft_substr(input, vars->start
// 					,vars->pos - vars->start);
//                 set_token_type(vars, token_preview);
//                 handle_string(input, vars);
//                 free(token_preview);
//             } 
//             if (make_exp_token(input, &vars->pos, vars))
//                 continue ;
//         }
//         if (is_operator_token(get_token_at(input, vars->pos, &moves))) 
//         {
//             if (vars->pos > vars->start)
//             {
//                 token_preview = ft_substr(input, vars->start
// 					,vars->pos - vars->start);
//                 set_token_type(vars, token_preview);
//                 handle_string(input, vars);
//                 free(token_preview);
//             }
//             if (process_operator_char(input, &vars->pos, vars))
//                 continue ;
//         }
//         if (input[vars->pos] && input[vars->pos] <= ' ')
//         {
//             if (vars->pos > vars->start)
//             {
//                 token_preview = ft_substr(input, vars->start
// 					,vars->pos - vars->start);
//                 set_token_type(vars, token_preview);
//                 handle_string(input, vars);
//                 free(token_preview);
//             }
//             while (input[vars->pos] && input[vars->pos] <= ' ')
//                 vars->pos++;
//             vars->start = vars->pos;
//             continue ;
//         }
//         vars->pos++;
//     }
//     if (vars->pos > vars->start)
//     {
//         token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
//         set_token_type(vars, token_preview);
//         handle_string(input, vars);
//         free(token_preview);
//     }
//     // DBG_PRINTF(DEBUG_TOKENIZE, "improved_tokenize: Token type=%d, content='%s'\n", 
//     //           vars->curr_type, (vars->current && vars->current->args) ? 
//     //           vars->current->args[0] : "(null)");
//     // DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
//     // debug_token_list(vars);
//     return(1);
// }
/*
Process any accumulated text before a special character.
Creates a token from the text between vars->start and vars->pos.
*/
static void handle_text(char *input, t_vars *vars)
{
    char *token_preview;

    if (vars->pos > vars->start)
    {
        token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
        set_token_type(vars, token_preview);
        handle_string(input, vars);
        free(token_preview);
    }
}

/*
Handle quoted text in the input.
Processes single and double quotes.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_quote(char *input, t_vars *vars)
{
    handle_text(input, vars);
    if (process_quote_char(input, &vars->pos, vars))
        vars->next_flag = 1;
}

/*
Handle variable expansion in the input.
Processes $ characters for variable substitution.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_expan(char *input, t_vars *vars)
{
    handle_text(input, vars);
    if (make_exp_token(input, &vars->pos, vars))
        vars->next_flag = 1;
}

/*
Handle operator tokens in the input.
Processes operators like >, <, |, etc.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_operat(char *input, t_vars *vars)
{
    handle_text(input, vars);
    if (process_operator_char(input, &vars->pos, vars))
        vars->next_flag = 1;
}

/*
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
static void imp_tok_white(char *input, t_vars *vars)
{
    handle_text(input, vars);
    while (input[vars->pos] && input[vars->pos] <= ' ')
        vars->pos++;
    vars->start = vars->pos;
    vars->next_flag = 1;
}

/* 
Tokenizes input string with improved quote handling.
Creates tokens for commands, args, quotes, and operators.
*/
int improved_tokenize(char *input, t_vars *vars)
{
    int			moves;
    t_tokentype	token_type;

    vars->pos = 0;
    vars->start = 0;
    vars->quote_depth = 0;
    while (input && input[vars->pos])
    {
        vars->next_flag = 0;
        token_type = get_token_at(input, vars->pos, &moves);
        if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
            imp_tok_quote(input, vars);
        if (!vars->next_flag && input[vars->pos] == '$' && !vars->quote_depth)
            imp_tok_expan(input, vars);
        if (!vars->next_flag && is_operator_token(get_token_at(input
			,vars->pos, &moves)))
            imp_tok_operat(input, vars);
        if (!vars->next_flag && input[vars->pos] && input[vars->pos] <= ' ')
            imp_tok_white(input, vars);
        if (vars->next_flag)
            continue;
        vars->pos++;
    }
    handle_text(input, vars);
    // DBG_PRINTF(DEBUG_TOKENIZE, "improved_tokenize: Token type=%d, content='%s'\n", 
    //           vars->curr_type, (vars->current && vars->current->args) ? 
    //           vars->current->args[0] : "(null)");
    // DBG_PRINTF(DEBUG_TOKENIZE, "Tokenization complete\n");
    // debug_token_list(vars);
    return(1);
}



/*
Helper function to link a new token node to the current node.
- Sets the next pointer of the current node to the new node.
- Sets the prev pointer of the new node to the current node.
- Updates the current pointer to the new node.
- Works with build_token_linklist().
*/
void	token_link(t_node *node, t_vars *vars)
{
    vars->current->next = node;
    node->prev = vars->current;
    vars->current = node;
}

/*
Updates the token list with a new node.
- Handles first token as head.
- Otherwise adds to end of list.
- Updates current pointer.

Example: When adding command node
- If first token, sets as head
- Otherwise links to previous token
- Updates current pointer
*/
void	build_token_linklist(t_vars *vars, t_node *node)
{
    t_node	*cmd_node;
    
    if (!vars || !node) 
        return ;
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
        return ;
    }
    if (vars->current && vars->current->type == TYPE_PIPE
		&& node->type == TYPE_ARGS)
    {
        node->type = TYPE_CMD;
    }
    if (node->type == TYPE_ARGS && vars->current
		&& vars->current->type == TYPE_CMD)
    {
        cmd_node = vars->current;
        append_arg(cmd_node, node->args[0], 0);
        node->args = NULL;
        free_token_node(node);
    }
    else 
       token_link(node, vars);
}

/* Debug function */
void debug_token_list(t_vars *vars)
{
    t_node *current = vars->head;
    int count = 0;
    
    DBG_PRINTF(DEBUG_TOKENIZE, "=== COMPLETE TOKEN LIST ===\n");
    while (current)
    {
        DBG_PRINTF(DEBUG_TOKENIZE, "Token %d: type=%d (%s), content='%s'\n", 
                  count++, current->type, get_token_str(current->type), 
                  current->args ? current->args[0] : "NULL");
        current = current->next;
    }
    DBG_PRINTF(DEBUG_TOKENIZE, "========================\n");
}
