/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/15 08:50:11 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips whitespace characters specifically space, \t, and \n.
Advances vars->pos to the next non-whitespace character.
For skipping whitespace between tokens.
Works with lexerlist().
*/
void	skip_whitespace(char *str, t_vars *vars)
{
	while (str[vars->pos] && (str[vars->pos] == ' '
			|| str[vars->pos] == '\t' || str[vars->pos] == '\n'))
		vars->pos++;
}

/*
Identifies and processes different token types in the input string.
Handles these specific cases:
1. Quoted content (single or double quotes)
2. Expansion variables (starting with $)
3. Operators (like pipes and redirections)
Advances vars->pos past the processed token.
If no special token is found, advances to next character.
Works with lexerlist().
*/
void handle_token(char *str, t_vars *vars)
{
	// Process operators and quotes
	if (str[vars->pos] == '|' || str[vars->pos] == '<' || str[vars->pos] == '>')
	{
		// If there's text before the operator, process it first
		if (vars->pos > vars->start)
		{
			int temp_pos = vars->pos;
			vars->pos = vars->start;
			vars->curr_type = TYPE_STRING;
			maketoken(str, vars);
			vars->pos = temp_pos;
			vars->start = temp_pos;
		}
		vars->pos = operators(str, vars->pos, vars->pos, vars);
	}
	else if (ft_isquote(str[vars->pos]))
		handle_quote_token(str, vars, &vars->pos);
	else if (str[vars->pos] == '$')
		handle_expansion(str, &vars->pos, vars);
	else
		vars->pos++;  // Move to next character for normal text
	// If we reached whitespace or end, process accumulated text
	if (!str[vars->pos] || ft_isspace(str[vars->pos]))
	{
		if (vars->pos > vars->start)
		{
			vars->curr_type = TYPE_STRING;
			maketoken(str, vars);
			vars->start = vars->pos;
		}
	}
}

/*
Main controller function for lex analysis.
Takes an input string and converts it into a linked list of tokens.
Steps:
1. Initializes position tracking and creates a head node
2. Processes tokens until end of input is reached, while skipping any
   whitespaces between tokens using skip_whitespace().
3. Calls handle_token() to process each token.
4. Handles any unclosed quotes at the end of input with
   handle_unclosed_quotes().
Result: vars->current populated with the token list.
Works with tokenize().
OLD VERSION 
void lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->head = NULL;
    vars->current = NULL;
    
    // Process each character looking for operators and handling quotes
    while (str && str[vars->pos])
    {
        // Skip whitespace at the beginning of a token
        if (str[vars->pos] == ' ' && vars->pos == vars->start)
        {
            vars->pos++;
            vars->start++;
            continue;
        }
        
        // Check for operators: pipe or redirections
        if (vars->quote_depth == 0 && 
            (str[vars->pos] == '|' || str[vars->pos] == '>' || str[vars->pos] == '<'))
        {
            // If there's text before the operator, process it as a command
            if (vars->pos > vars->start)
            {
                char *cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
                if (cmd_str)
                {
                    fprintf(stderr, "DEBUG: Processing command: '%s'\n", cmd_str);
                    vars->curr_type = TYPE_CMD;
                    maketoken(cmd_str, vars);
                    ft_safefree((void **)&cmd_str);
                }
            }
            
            // Check for double operators (>> or <<)
            if ((str[vars->pos] == '>' && str[vars->pos + 1] == '>') ||
                (str[vars->pos] == '<' && str[vars->pos + 1] == '<'))
            {
                // Create double operator token
                t_tokentype op_type = (str[vars->pos] == '>') ? 
                                        TYPE_APPEND_REDIRECT : TYPE_HEREDOC;
                                        
                fprintf(stderr, "DEBUG: Processing %s operator\n", 
                       get_token_str(op_type));
                       
                t_node *op_node = initnode(op_type, 
                                   (op_type == TYPE_APPEND_REDIRECT) ? ">>" : "<<");
                                   
                if (op_node)
                    build_token_linklist(vars, op_node);
                
                vars->pos += 2;
                vars->start = vars->pos;
            }
            else
            {
                // Create single operator token
                t_tokentype op_type;
                if (str[vars->pos] == '|')
                    op_type = TYPE_PIPE;
                else if (str[vars->pos] == '>')
                    op_type = TYPE_OUT_REDIRECT;
                else // '<'
                    op_type = TYPE_IN_REDIRECT;
                    
                fprintf(stderr, "DEBUG: Processing %s operator\n", 
                       get_token_str(op_type));
                       
                t_node *op_node = initnode(op_type, 
                                  (op_type == TYPE_PIPE) ? "|" : 
                                  ((op_type == TYPE_OUT_REDIRECT) ? ">" : "<"));
                                  
                if (op_node)
                    build_token_linklist(vars, op_node);
                
                vars->pos++;
                vars->start = vars->pos;
            }
        }
        // Handle quotes
        else if ((str[vars->pos] == '\'' || str[vars->pos] == '"') && 
                 vars->quote_depth == 0)
        {
            char quote = str[vars->pos];
            vars->pos++;
            vars->quote_depth = 1;
            vars->quote_ctx[0].type = quote;
            
            while (str[vars->pos] && str[vars->pos] != quote)
                vars->pos++;
                
            if (str[vars->pos] == quote)
            {
                vars->pos++;
                vars->quote_depth = 0;
            }
        }
        else
        {
            vars->pos++;
        }
    }
    
    // Process any remaining content after the last operator
    if (vars->pos > vars->start)
    {
        char *cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
        if (cmd_str)
        {
            fprintf(stderr, "DEBUG: Processing final command: '%s'\n", cmd_str);
            vars->curr_type = TYPE_CMD;
            maketoken(cmd_str, vars);
            ft_safefree((void **)&cmd_str);
        }
    }
    
    // Handle unclosed quotes
    if (vars->quote_depth > 0 && str)
        str = handle_unclosed_quotes(str, vars);
        
    if (vars->head)
        fprintf(stderr, "DEBUG: Tokens created, first token type: %d\n", vars->head->type);
    else
        fprintf(stderr, "DEBUG: No tokens created!\n");
}
*/
void lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->head = NULL;
    vars->current = NULL;
    
    fprintf(stderr, "DEBUG: Starting lexer list processing for: '%s'\n", str);
    
    // Process each character looking for operators and handling quotes
    while (str && str[vars->pos])
    {
        // Skip whitespace at the beginning of a token
        if (str[vars->pos] == ' ' && vars->pos == vars->start)
        {
            vars->pos++;
            vars->start++;
            continue;
        }
        
        // Process operators: pipe or redirections
        if (vars->quote_depth == 0 && 
            (str[vars->pos] == '|' || str[vars->pos] == '>' || str[vars->pos] == '<'))
        {
            // If there's text before the operator, process it as a command
            if (vars->pos > vars->start)
            {
                char *cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
                if (cmd_str)
                {
                    vars->curr_type = TYPE_CMD;
                    fprintf(stderr, "DEBUG: Processing command token: '%s'\n", cmd_str);
                    process_cmd_token(cmd_str, vars);
                    ft_safefree((void **)&cmd_str);
                }
            }
            
            // Process the operator token
            fprintf(stderr, "DEBUG: Processing operator at position %d\n", vars->pos);
            if (str[vars->pos] == '|')
            {
                vars->curr_type = TYPE_PIPE;
                t_node *pipe_node = initnode(TYPE_PIPE, "|");
                if (pipe_node)
                    build_token_linklist(vars, pipe_node);
            }
            else if (str[vars->pos] == '>' && str[vars->pos + 1] == '>')
            {
                vars->curr_type = TYPE_APPEND_REDIRECT;
                t_node *append_node = initnode(TYPE_APPEND_REDIRECT, ">>");
                if (append_node)
                    build_token_linklist(vars, append_node);
                vars->pos++; // Skip the extra '>'
            }
            else if (str[vars->pos] == '<' && str[vars->pos + 1] == '<')
            {
                vars->curr_type = TYPE_HEREDOC;
                t_node *heredoc_node = initnode(TYPE_HEREDOC, "<<");
                if (heredoc_node)
                    build_token_linklist(vars, heredoc_node);
                vars->pos++; // Skip the extra '<'
            }
            else if (str[vars->pos] == '>')
            {
                vars->curr_type = TYPE_OUT_REDIRECT;
                t_node *out_node = initnode(TYPE_OUT_REDIRECT, ">");
                if (out_node)
                    build_token_linklist(vars, out_node);
            }
            else if (str[vars->pos] == '<')
            {
                vars->curr_type = TYPE_IN_REDIRECT;
                t_node *in_node = initnode(TYPE_IN_REDIRECT, "<");
                if (in_node)
                    build_token_linklist(vars, in_node);
            }
            
            // Set start position for next token
            vars->pos++;
            vars->start = vars->pos;
            continue;
        }
        else if ((str[vars->pos] == '\'' || str[vars->pos] == '"') && 
                 vars->quote_depth == 0)
        {
            // Handle quotes
            handle_quotes(str, &vars->pos, vars);
            continue;
        }
        
        vars->pos++;
    }
    
    // Process any remaining content after the last operator
    if (vars->pos > vars->start)
    {
        char *cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
        if (cmd_str)
        {
            vars->curr_type = TYPE_CMD;
            fprintf(stderr, "DEBUG: Processing final command token: '%s'\n", cmd_str);
            process_cmd_token(cmd_str, vars);
            ft_safefree((void **)&cmd_str);
        }
    }
    
    // Handle unclosed quotes
    if (vars->quote_depth > 0 && str)
        str = handle_unclosed_quotes(str, vars);
        
    if (vars->head)
        fprintf(stderr, "DEBUG: Tokens created, first token type: %d\n", vars->head->type);
    else
        fprintf(stderr, "DEBUG: No tokens (vars->head is NULL)!\n");
}
