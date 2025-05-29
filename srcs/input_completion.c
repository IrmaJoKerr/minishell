/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 17:59:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Combines original input with continuation input
Returns:
- A newly allocated string with both inputs joined.
- Null on error.
*/
char	*append_input(char *original, char *additional)
{
	char	*temp;
	char	*result;

	temp = ft_strjoin(original, "\n");
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, additional);
	free(temp);
	return (result);
}

/*
Recursively reads additional input to complete unclosed quotes.
Returns:
- Completed input string.
- NULL on error.
*/
char	*fix_open_quotes(char *original_input, t_vars *vars)
{
	char	*addon_input;
	char	*merged_input;
	char	*temp;

	if (!vars->quote_depth)
		return (ft_strdup(original_input));
	addon_input = quote_prompt(vars->quote_ctx[vars->quote_depth - 1].type);
	if (!addon_input)
		return (NULL);
	merged_input = append_input(original_input, addon_input);
	free(addon_input);
	if (!merged_input)
		return (NULL);
	if (!validate_quotes(merged_input, vars))
	{
		temp = merged_input;
		merged_input = fix_open_quotes(temp, vars);
		free(temp);
	}
	return (merged_input);
}

/*
Processes input with quotes that need completion.
- Gets additional input for unclosed quotes.
- Re-tokenizes the completed command.
- Updates token list with completed input.
Returns:
Newly allocated complete command string.
NULL on memory allocation failure.
Works with process_command().

Example: When user types "echo "hello
- Prompts for completion of the double quote
- User types "world" and Enter
- Returns combined string: echo "hello world"
*/
char	*handle_quote_completion(char *cmd, t_vars *vars)
{
	char	*new_cmd;

	vars->quote_depth = 0;
	if (validate_quotes(cmd, vars))
		return (cmd);
	new_cmd = fix_open_quotes(cmd, vars);
	if (!new_cmd)
		return (NULL);
	if (new_cmd != cmd)
	{
		free(cmd);
		cmd = new_cmd;
	}
	cleanup_token_list(vars);
	tokenizer(cmd, vars);
	return (cmd);
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
char	*complete_pipe_cmd(char *command, t_vars *vars)
{
	char	*pipe_cmd;
	char	*temp;
	int		syntax_result;

	if (!command || !vars)
		return (NULL);
	syntax_result = analyze_pipe_syntax(vars);
	if (syntax_result != 2)
		return (ft_strdup(command));
	pipe_cmd = ft_strdup(command);
	if (!pipe_cmd)
		return (NULL);
	if (handle_unfinished_pipes(&pipe_cmd, vars) < 0)
	{
		free(pipe_cmd);
		return (NULL);
	}
	temp = ft_strdup(pipe_cmd);
	free(pipe_cmd);
	return (temp);
}

/*
Check for unfinished pipes in input and handle them.
Runs recursively to handle multiple unfinished pipes.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
*/
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars)
{
	char	*addon_input;

	addon_input = read_until_complete();
	if (!addon_input)
		return (-1);
	if (append_to_cmdline(processed_cmd, addon_input) == -1)
	{
		free(addon_input);
		return (-1);
	}
	free(addon_input);
	if (!process_input_tokens(*processed_cmd, vars))
		return (-1);
	if (analyze_pipe_syntax(vars) == 2)
		return (handle_unfinished_pipes(processed_cmd, vars));
	return (1);
}
