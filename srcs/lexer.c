/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:30:32 by bleow            ###   ########.fr       */
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
/*
void	handle_token(char *str, t_vars *vars)
{
	int	token_start;

	token_start = vars->pos;
	if (ft_isquote(str[vars->pos]))
		handle_quote_token(str, vars, &vars->pos);
	else if (str[vars->pos] == '$')
		handle_expansion(str, &vars->pos, vars);
	else
		vars->pos = operators(str, vars->pos, token_start, vars);
	if (vars->pos == token_start && str[vars->pos])
		vars->pos++;
}
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
*/
/*
void	lexerlist(char *str, t_vars *vars)
{
	vars->pos = 0;
	vars->current = NULL;
	vars->current = init_head_node(vars);
	if (!vars->current)
	{
		ft_error(vars);
		return;
	}
	printf("DEBUG: Initial head node created\n");
	if (str && !ft_strchr(str, '|') && !ft_strchr(str, '<') && !ft_strchr(str, '>'))
	{
		printf("DEBUG: Simple command detected: %s\n", str);
		vars->curr_type = TYPE_CMD;
		maketoken(str, vars);
	}
	else
	{
		while (str && str[vars->pos])
		{
			skip_whitespace(str, vars);
			if (str[vars->pos])
				handle_token(str, vars);
		}
	}
	if (vars->quote_depth > 0 && str)
		str = handle_unclosed_quotes(str, vars);
	printf("DEBUG: Tokens created, first token type: %d\n", 
		vars->current ? (int)vars->current->type : 0);
}
*/
/*
void lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->current = NULL;
    vars->head = init_head_node(vars);
    if (!vars->head)
    {
        ft_error(vars);
        return;
    }
    printf("DEBUG: Initial head node created\n");
    if (str && !ft_strchr(str, '|') && !ft_strchr(str, '<') && !ft_strchr(str, '>'))
    {
        printf("DEBUG: Simple command detected: %s\n", str);
        vars->curr_type = TYPE_CMD;
        maketoken(str, vars);
    }
    else
    {
        vars->current = vars->head;
        while (str && str[vars->pos])
        {
            skip_whitespace(str, vars);
            if (str[vars->pos])
                handle_token(str, vars);
        }
    }
    if (vars->quote_depth > 0 && str)
        str = handle_unclosed_quotes(str, vars);
    printf("DEBUG: Tokens created, first token type: %d\n", 
        vars->head ? (int)vars->head->type : 0);
}
*/
void lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->head = NULL;
    vars->current = NULL;
    
    printf("DEBUG: Processing command: '%s'\n", str);
    
    // First handle simple commands without special operators
    if (str && !ft_strchr(str, '|') && !ft_strchr(str, '<') && !ft_strchr(str, '>'))
    {
        printf("DEBUG: Simple command detected: %s\n", str);
        vars->curr_type = TYPE_CMD;
        maketoken(str, vars);
    }
    else
    {
        // Process first part before pipe
        char *pipe_pos = ft_strchr(str, '|');
        if (pipe_pos)
        {
            // Process the command before pipe
            int cmd_len = pipe_pos - str;
            if (cmd_len > 0)
            {
                char *cmd = ft_substr(str, 0, cmd_len);
                printf("DEBUG: Processing left command: '%s'\n", cmd);
                vars->curr_type = TYPE_CMD;
                maketoken(cmd, vars);
                ft_safefree((void **)&cmd);  // Safe free
            }
            
            // Process the pipe operator - create an actual PIPE token
        	printf("DEBUG: Processing pipe operator\n");
        	vars->curr_type = TYPE_PIPE;
        	t_node *pipe_node = initnode(TYPE_PIPE, "|");
       		if (pipe_node)
            	build_token_linklist(vars, pipe_node);  // Add the pipe node to the token list
            
            // Process the command after pipe - FIXED VERSION
            if (*(pipe_pos + 1))
            {
                char *right_cmd_original = ft_strdup(pipe_pos + 1); // Keep original pointer
                char *right_cmd = right_cmd_original; // Working copy
                
                // Skip leading whitespace but don't modify the pointer
                while (*right_cmd && (*right_cmd == ' ' || *right_cmd == '\t'))
                    right_cmd++;
                
                if (*right_cmd)
                {
                    printf("DEBUG: Processing right command: '%s'\n", right_cmd);
                    vars->curr_type = TYPE_CMD;
                    // Make a new copy of the trimmed string
                    char *trimmed = ft_strdup(right_cmd);
                    maketoken(trimmed, vars);
                    ft_safefree((void **)&trimmed);  // Safe free
                }
                
                ft_safefree((void **)&right_cmd_original);  // Safe free
            }
        }
    }
    
    // Handle unclosed quotes
    if (vars->quote_depth > 0 && str)
        str = handle_unclosed_quotes(str, vars);
    
    if (vars->head)
        printf("DEBUG: Tokens created, first token type: %d\n", vars->head->type);
    else
        printf("DEBUG: No tokens created!\n");
}
