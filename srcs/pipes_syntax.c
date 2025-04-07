/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:31:06 by bleow             #+#    #+#             */
/*   Updated: 2025/04/07 03:14:25 by bleow            ###   ########.fr       */
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
Works with chk_pipe_syntax_err().
*/
int	chk_start_pipe(t_vars *vars)
{
	if (!vars || !vars->head)
		return (0);
	if (vars->head->type == TYPE_PIPE)
	{
		ft_putstr_fd("bleshell: unexpected syntax error at '|'\n", 2);
		vars->error_code = 2;
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
Works with chk_pipe_syntax_err().
*/
int	chk_next_pipes(t_vars *vars)
{
    t_node	*current;
    int		multi_pipes;

    if (!vars || !vars->head)
        return (0);
    current = vars->head;
    multi_pipes = 0;
    while (current)
    {
        if (current->type == TYPE_PIPE)
        {
            multi_pipes++;
            if (multi_pipes > 1)
            {
                ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
                vars->error_code = 2;
                return (1);
            }
        }
        else
            multi_pipes = 0;
        current = current->next;
    }
    return (0);
}

/*
Detects if a pipe token appears at the end of input.
- Checks if the last token is a pipe.
- Indicates more input is needed to complete command.
Returns:
- 2 if pipe found at end (requires more input).
- 0 if no pipe at end or no tokens exist.
Works with chk_pipe_syntax_err().
*/
int	chk_end_pipe(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current && current->next)
		current = current->next;
	if (current && current->type == TYPE_PIPE)
		return (2);
	return (0);
}

/*
Performs comprehensive check of pipe syntax in token list.
- Detects pipes at beginning (error).
- Detects consecutive pipes (error).
- Detects pipes at end (needs more input).
Returns:
- 0 if pipe syntax is valid.
- 1 if syntax error detected.
- 2 if more input needed (pipe at end).

Example: For input "| ls":
- Returns 1 (error: pipe at beginning)
For input "ls | grep a |":
- Returns 2 (needs more input)
*/
int	chk_pipe_syntax_err(t_vars *vars)
{
	int	result;

	if (!vars->head)
		return (0);
	result = chk_start_pipe(vars);
	if (result != 0)
		return (result);
	result = chk_next_pipes(vars);
	if (result != 0)
		return (result);
	result = chk_end_pipe(vars);
	if (result != 0)
		return (result);
	return (0);
}

/*
Handles pipe syntax validation and completion.
- Checks for pipe syntax errors.
- Handles unfinished pipes by prompting for continuation.
Returns:
- Updated command string after pipe processing.
- NULL on memory allocation failure or other error.
- Command unchanged if no pipe issues detected.
Works with process_command() as second processing stage.

Example: For input "ls |"
- Detects unfinished pipe
- Prompts for continuation
- Returns completed command with pipe and continuation
*/
char	*process_pipe_syntax(char *command, t_vars *vars)
{
	int		syntax_chk;
	char	*processed_cmd;

	processed_cmd = command;
	syntax_chk = chk_pipe_syntax_err(vars);
	if (syntax_chk == 1)
	{
		if (processed_cmd != vars->partial_input && processed_cmd != command)
			free(processed_cmd);
		if (vars->partial_input && vars->partial_input != command)
			free(vars->partial_input);
		return (NULL);
	}
	processed_cmd = handle_pipe_completion(processed_cmd, vars, syntax_chk);
	return (processed_cmd);
}
