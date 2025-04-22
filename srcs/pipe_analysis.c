/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_analysis.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:22:27 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 15:34:30 by bleow            ###   ########.fr       */
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
static int	check_pipe_at_start(t_vars *vars)
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
static int	check_consecutive_pipes(t_vars *vars)
{
	t_node *current;
	int expecting_command;

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
static int check_pipe_completion_needed(t_vars *vars)
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
		else if ((current->type == TYPE_CMD || current->type == TYPE_ARGS) && 
				 expecting_command)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	// If we end with expecting_command=1, we need more input
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
int analyze_pipe_syntax(t_vars *vars)
{
	int result;

	if (!vars || !vars->head)
		return (0);
	// Check for pipe at beginning (error)
	result = check_pipe_at_start(vars);
	if (result != 0)
		return (result);
	// Check for consecutive pipes (error)
	result = check_consecutive_pipes(vars);
	if (result != 0)
		return (result);
	// Check for pipe at end (needs completion)
	result = check_pipe_completion_needed(vars);
	if (result != 0)
		return (result);
	
	return (0);
}

/*
Process pipe input for completion when needed.
- Prompts for and handles additional input when pipe at end.
- Manages integration with the tokenizer.
Returns:
- Updated command string with pipe completion.
- NULL on error.
Works with process_command() to handle unfinished pipes.
*/
char *complete_pipe_command(char *command, t_vars *vars)
{
	char	*pipe_cmd;
	char	*temp;
	int		syntax_result;
	
	if (!command || !vars)
		return (NULL);
	// Check if pipe completion is needed using our new analyzer
	syntax_result = analyze_pipe_syntax(vars);
	if (syntax_result != 2)  // If not needing completion (0=valid, 1=error)
		return (ft_strdup(command)); 
	// Prepare working copies for pipe completion
	pipe_cmd = ft_strdup(command);
	if (!pipe_cmd)
		return (NULL);
	// Do the actual pipe completion
	if (handle_unfinished_pipes(&pipe_cmd, vars) < 0)
	{
		free(pipe_cmd);
		return (NULL);
	}
	// Return the completed command
	temp = ft_strdup(pipe_cmd);
	free(pipe_cmd);
	return (temp);
}
