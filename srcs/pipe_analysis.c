/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_analysis.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:22:27 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 03:18:49 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Detects if a pipe token appears at the beginning of input.
- Checks if the first token is a pipe (syntax error).
- Sets error code and outputs error message if detected.
Returns:
- 1 if pipe found at beginning (error condition).
- 0 if no error detected or no tokens exist.
Works with analyze_pipe_syntax().
*/
int	check_pipe_at_start(t_vars *vars)
{
	if (!vars || !vars->head)
		return (0);
	if (vars->head->type == TYPE_PIPE)
	{
		tok_syntax_error_msg("|", vars);
		return (1);
	}
	return (0);
}

/*
Checks for consecutive pipe tokens in the token list.
- Tracks sequence of pipe tokens.
- Reports syntax error if multiple pipes found consecutively.
Returns:
- 1 if pipe syntax error found.
- 0 if no errors detected.
Works with analyze_pipe_syntax().
*/
int	check_consecutive_pipes(t_vars *vars)
{
	t_node	*current;
	int		expecting_command;

	if (!vars || !vars->head)
		return (0);
	current = vars->head;
	expecting_command = 0;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			if (expecting_command)
			{
				tok_syntax_error_msg("|", vars);
				return (1);
			}
			expecting_command = 1;
		}
		else if (current->type == TYPE_CMD || current->type == TYPE_ARGS)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	return (0);
}

/*
Checks if input has a pipe at end requiring more input.
- Uses state tracking to detect need for another command.
- Handles various token types properly.
Returns:
- 2 if pipe at end needs completion.
- 0 if input is complete.
Works with analyze_pipe_syntax().
*/
int	check_pipe_completion_needed(t_vars *vars)
{
	t_node	*current;
	int		expecting_command;

	if (!vars || !vars->head)
		return (0);
	current = vars->head;
	expecting_command = 0;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			expecting_command = 1;
		}
		else if ((current->type == TYPE_CMD || current->type == TYPE_ARGS)
			&& expecting_command)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	if (expecting_command)
		return (2);
	return (0);
}

/*
Master function for analyzing pipe syntax in token list.
- Performs checks in correct sequence.
- Detects all pipe syntax issues.
Returns:
- 0 if pipe syntax is valid.
- 1 if syntax error detected.
- 2 if more input needed (pipe at end).
Works with process_command() to validate and prepare input.
*/
int	analyze_pipe_syntax(t_vars *vars)
{
	int	result;

	if (!vars || !vars->head)
		return (0);
	result = check_pipe_at_start(vars);
	if (result != 0)
		return (result);
	result = check_consecutive_pipes(vars);
	if (result != 0)
		return (result);
	result = check_pipe_completion_needed(vars);
	if (result != 0)
		return (result);
	return (0);
}

/*
Master control function for completing an incomplete pipe command by
getting additional input.
- Gets continuation input for commands ending with a pipe
- Updates partial_input with the complete command
- Re-tokenizes the completed command
Returns:
- 1 on successful pipe completion.
- 0 on memory allocation or tokenization error.
Works with handle_pipe_syntax().

Example: When user types "ls |"
- Prompts for continuation after the pipe.
- User inputs "grep hello".
- Creates and tokenizes "ls | grep hello".
*/
int	finalize_pipes(t_vars *vars)
{
	char	*completed_cmd;

	completed_cmd = complete_pipe_cmd(vars->partial_input, vars);
	if (!completed_cmd)
	{
		free(vars->partial_input);
		vars->partial_input = NULL;
		return (0);
	}
	free(vars->partial_input);
	vars->partial_input = completed_cmd;
	free_null_token_stop(vars);
	cleanup_token_list(vars);
	if (!process_input_tokens(vars->partial_input, vars))
	{
		free(vars->partial_input);
		vars->partial_input = NULL;
		return (0);
	}
	return (1);
}
