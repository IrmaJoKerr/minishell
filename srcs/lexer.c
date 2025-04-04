/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 03:30:29 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips whitespace characters in the input string.
- Moves past spaces, tabs, and newlines.
- Updates vars->pos to point to the next non-whitespace.
Returns:
Nothing (void function).
Works with lexerlist() to handle token separation.

Example: Input "  echo hello"
- Skips the two spaces at the beginning
- Sets vars->pos to 2 (pointing at 'e')
*/
void	skip_whitespace(char *str, t_vars *vars)
{
	while (str[vars->pos] && (str[vars->pos] == ' '
			|| str[vars->pos] == '\t' || str[vars->pos] == '\n'))
		vars->pos++;
}

/*
Prompts for additional input when input is incomplete.
- Used when quotes are unclosed or pipes need more input.
- Provides visual indicator of input continuation.
Returns:
Allocated string with user input or NULL if readline fails.
Works with handle_unclosed_quotes() and handle_unfinished_pipes().

Example: read_added_input("quote> ")
- Shows "quote> " prompt to user
- Returns user input as newly allocated string
*/
char	*read_added_input(char *prompt)
{
	char	*line;

	line = readline(prompt);
	if (!line)
		return (NULL);
	return (line);
}

/*
Processes a text segment before reaching a special character.
- Creates a command token for the text segment.
- Updates token list with the new command node.
Returns:
Nothing (void function).
Works with handle_operator_token().

Example: For input "echo hello | grep"
- When reaching pipe, processes "echo hello" as command
- Creates command node and adds to token list
*/
void	handle_text_chunk(char *str, t_vars *vars)
{
	char	*cmd_str;

	if (vars->pos <= vars->start)
		return ;
	cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
	if (!cmd_str)
		return ;
	vars->curr_type = TYPE_CMD;
	free(cmd_str);
}

/*
Processes text and adds to token list with optional type override.
- Creates token from current text segment.
- Applies standard or override token type.
- Updates token list and position markers.
Returns:
Nothing (void function).
Works with handle_token_boundary() and handle_token().

Example: With first_token=1 and TYPE_CMD type
- Creates command token for current text segment
- Sets first_token to 0 after processing
*/
void	process_text(char *str, t_vars *vars
		, int *first_token, t_tokentype override_type)
{
	char	*token;
	int		len;

	len = vars->pos - vars->start;
	token = ft_substr(str, vars->start, len);
	if (!token)
		return ;
	if (override_type != TYPE_NULL)
		vars->curr_type = override_type;
	else if (*first_token || vars->prev_type == TYPE_PIPE)
	{
		vars->curr_type = TYPE_CMD;
		*first_token = 0;
	}
	else
		vars->curr_type = TYPE_ARGS;
	maketoken_with_type(token, vars->curr_type, vars);
	vars->start = vars->pos;
	vars->prev_type = vars->curr_type;
	free(token);
}

/*
Processes quoted content in the input string.
- Handles content inside single or double quotes.
- Updates quote depth and context tracking.
Returns:
Nothing (void function).
Works with handle_token().

Example: For input with "'Hello world'"
- Tracks single quote context
- Processes "Hello world" as quoted content
*/
void	handle_quote_content(char *str, t_vars *vars, int *first_token)
{
	if (vars->pos > vars->start)
		process_text(str, vars, first_token, 0);
	handle_quotes(str, vars);
    vars->start = vars->pos;
}

/*
Processes variable expansion tokens.
- Handles $VAR syntax for variable expansion.
- Creates appropriate expansion nodes.
- Special handling for $? to show exit status
Returns:
Nothing (void function).
Works with handle_token().
*/
void	handle_expansion_token(char *str, t_vars *vars, int *first_token)
{
    char	*expanded;
    t_node	*cmd_node;

    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    if (str[vars->pos] == '$' && str[vars->pos + 1] == '?')
    {
        cmd_node = find_last_command(vars->head);
        if (cmd_node && cmd_node->type == TYPE_CMD)
        {
            // Pass the vars pointer here
            append_arg(cmd_node, "$?", 0);
        }
        else
        {
            vars->curr_type = TYPE_EXIT_STATUS;
            maketoken_with_type("$?", vars->curr_type, vars);
        }
        vars->pos += 2;
    }
    else
    {
        expanded = handle_expansion(str, &vars->pos, vars);
        if (expanded)
        {
            vars->curr_type = TYPE_ARGS;
            maketoken_with_type(expanded, vars->curr_type, vars);
            free(expanded);
        }
        else
        {
            vars->curr_type = TYPE_ARGS;
            maketoken_with_type("", vars->curr_type, vars);
        }
    }
    vars->start = vars->pos;
}

/*
Handles token boundary at whitespace or end of input.
- Processes any accumulated text before boundary.
- Updates position markers for next token.
Returns:
Nothing (void function).
Works with handle_token().

Example: For "echo hello" when reaching space
- Processes "echo" as command token
- Updates position markers to start after space
*/
void	handle_token_boundary(char *str, t_vars *vars, int *first_token)
{
	if (vars->pos > vars->start)
	{
		process_text(str, vars, first_token, 0);
		skip_whitespace(str, vars);
		vars->start = vars->pos;
	}
	else
	{
		skip_whitespace(str, vars);
		vars->start = vars->pos;
	}
}

/*
Creates an operator token and adds to token list.
- Handles different operator types (pipe, redirections).
- Creates appropriate node based on operator type.
Returns:
Nothing (void function).
Works with handle_token().

Example: For | operator
- Creates TYPE_PIPE node
- Adds to token linked list
*/
void create_operator_token(t_vars *vars, t_tokentype type, char *symbol)
{
	t_node	*operator_node;

	operator_node = initnode(type, symbol);
	if (!operator_node)
		return ;
	vars->curr_type = type;
	build_token_linklist(vars, operator_node);
}

/*
Processes operator tokens like pipe and redirections.
- Handles single and double character operators.
- Updates position and token list appropriately.
Returns:
Nothing (void function).
Works with handle_token().

Example: For input with ">" operator
- Creates output redirection token
- Updates position past the operator
*/
void	handle_operator_token(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    if (str[vars->pos] == '|')
        create_operator_token(vars, TYPE_PIPE, "|");
    else if (str[vars->pos] == '>' && str[vars->pos + 1] == '>')
    {
        create_operator_token(vars, TYPE_APPEND_REDIRECT, ">>");
        vars->pos++; // Move increment after create_operator_token
    }
    else if (str[vars->pos] == '<' && str[vars->pos + 1] == '<')
    {
        create_operator_token(vars, TYPE_HEREDOC, "<<");
        vars->pos++; // Move increment after create_operator_token
    }
    else if (str[vars->pos] == '>')
        create_operator_token(vars, TYPE_OUT_REDIRECT, ">");
    else if (str[vars->pos] == '<')
        create_operator_token(vars, TYPE_IN_REDIRECT, "<");
    vars->pos++;
    vars->start = vars->pos;
}
