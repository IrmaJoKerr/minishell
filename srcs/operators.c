/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 21:08:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Operator handling function that processes both string content and operators.
1) Checks if there's a string to process before the 
   current operator with handle_string()
2) Checks for a double operator (>> or <<) with handle_double_operator()
3) Checks for a single operator (|, >, <) with handle_single_operator()
Returns the new position after operator is processed. Works with handle_token()
*/
int operators(char *input, int i, int token_start, t_vars *vars)
{
	// If there's text before the operator, process it first as a string/command
	if (i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
		vars->start = i;  // Reset start position to current position
	}
	// Then process the operator
	int next_pos = handle_double_operator(input, i, vars);
	if (next_pos != i)
		return (next_pos);
	return (handle_single_operator(input, i, vars));
}

/*
Creates a string token whenever an operator is encountered mid-text.
1) Checks if the current position contains an operator
   character ('>', '<', '|') and if it already processed some characters
   since the token_start.
2) If yes, it backtracks and creates a string token from token_start to
   the current position.
3) Returns the current position without advancing.
Works with operators().
*/
int	handle_string(char *input, int i, int token_start, t_vars *vars)
{
	if ((input[i] == '>' || input[i] == '<' || input[i] == '|')
		&& i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
	}
	return (i);
}

/*
Converts single operator characters to their token types.
Example:
- '|' == TYPE_PIPE
- '>' == TYPE_OUT_REDIRECT
- '<' == TYPE_IN_REDIRECT
Returns TYPE_STRING as a fallback for unrecognized characters.
Used by handle_single_operator().
*/
t_tokentype	get_operator_type(char op)
{
	if (op == '|')
		return (TYPE_PIPE);
	else if (op == '>')
		return (TYPE_OUT_REDIRECT);
	else if (op == '<')
		return (TYPE_IN_REDIRECT);
	return (TYPE_STRING);
}

/*
Processes single character operators (|, >, <).
When a recognized operator is found:
1) Remembers the token start and end positions
2) Sets the appropriate token type using get_operator_type()
3) Creates a token using maketoken()
Returns the position after the operator (i+1) or the unchanged position
if no operator was found.
Works with operators().
*/
int handle_single_operator(char *input, int i, t_vars *vars)
{
	if (input[i] == '|' || input[i] == '>' || input[i] == '<')
	{
		// If there's text before the operator, process it first as a string/command
		if (i > vars->start)
		{
			int temp_pos = i;
			vars->curr_type = TYPE_STRING;
			vars->pos = vars->start; // Save current position
			maketoken(ft_substr(input, vars->start, temp_pos - vars->start), vars);
		}
		// Now process the operator token
		vars->start = i;
		vars->pos = i + 1;
		vars->curr_type = get_operator_type(input[i]);
		// Debug the operator being processed
		fprintf(stderr, "DEBUG: Processing operator %s token\n", get_token_str(vars->curr_type));
		// Make a token for the operator
		maketoken(ft_substr(input, i, 1), vars);
		// Set start position for next token
		vars->start = i + 1;
		return (i + 1);
	}
	return (i);
}

/*
Processes two-character operators (>> for append, << for heredoc).
When double operator found:
1) Remembers the token start and end positions
2) Sets the appropriate token type (TYPE_APPEND_REDIRECT or TYPE_HEREDOC)
3) Creates a token using maketoken()
Returns the position after the operator (i+2) or the unchanged position
if no double operator was found.
Works with operators().
*/
int	handle_double_operator(char *input, int i, t_vars *vars)
{
	if (input[i] == '>' && input[i + 1] == '>')
	{
		vars->start = i;
		vars->pos = i + 2;
		vars->curr_type = TYPE_APPEND_REDIRECT;
		maketoken(input, vars);
		return (i + 2);
	}
	if (input[i] == '<' && input[i + 1] == '<')
	{
		vars->start = i;
		vars->pos = i + 2;
		vars->curr_type = TYPE_HEREDOC;
		maketoken(input, vars);
		return (i + 2);
	}
	return (i);
}
