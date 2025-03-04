/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 11:55:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process input for heredoc redirection by reading lines until delimiter or EOF.
Supports variable expansion for heredocs with unquoted delimiters.
Takes:
- fd: Pipe file descriptor array
- delimiter: String that ends heredoc input
- vars: Shell variables struct
- expand_vars: Whether to expand variables in this heredoc
Returns 1 on success (all writes completed), 0 on failure.
Example with delimiter "EOF" and expand_vars=true:
- Reads lines like "line 1", "$USER", "EOF" 
- Writes "line 1" and expanded "$USER" to fd[1]
- Stops when "EOF" is encountered or readline returns NULL
Works with handle_heredoc().
*/
int	read_heredoc(int *fd, char *delimiter, t_vars *vars, int expand_vars)
{
	char	*line;
	int		write_success;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write_success = write_to_heredoc(fd[1], line, vars, expand_vars);
		free(line);
		if (!write_success)
			return (0);
	}
	return (1);
}

/*
Write line to heredoc pipe with optional variable expansion.
Adds a newline after each input line.
If expand_vars() is true, expands environment variables in the line.
Takes:
- fd: File descriptor to write to.
- line: Input string to write.
- vars: Shell variables structure (for environment variables).
- expand_vars: Whether to expand variables (based on delimiter quotes).
- frees memory for expanded_line if it was allocated.
Returns 1 on success, 0 on failure.
Example: For line "echo $HOME" with expand_vars=true:
- Expands to "echo /Users/bleow"
- Writes to fd and adds newline
- Returns 1 if both writes succeed, 0 otherwise
Works with read_heredoc().
*/
int	write_to_heredoc(int fd, char *line, t_vars *vars, int expand_vars)
{
	char	*expanded_line;
	int		write_result;
	int		newline_result;

	if (!line)
		return (0);
	if (expand_vars && vars)
	{
		expanded_line = expand_heredoc_line(line, vars);
		if (!expanded_line)
			return (0);
		write_result = write(fd, expanded_line, ft_strlen(expanded_line));
		newline_result = write(fd, "\n", 1);
		ft_safefree((void **)&expanded_line);
		if (write_result == -1 || newline_result == -1)
			return (0);
		return (1);
	}
	write_result = write(fd, line, ft_strlen(line));
	newline_result = write(fd, "\n", 1);
	if (write_result == -1 || newline_result == -1)
		return (0);
	return (1);
}

/*
Check if a delimiter contains quotes - if so, don't expand variables.
Takes a delimiter string.
Returns 1 if variables should be expanded (no quotes), 0 if not.
Example: "EOF" -> 1 (expand variables)
		 "'EOF'" -> 0 (don't expand variables)
Works with handle_heredoc().
*/
int	expand_heredoc_check(char *delimiter)
{
	int	i;
	int	is_quote;

	if (!delimiter)
		return (0);
	i = 0;
	while (delimiter[i])
	{
		is_quote = ft_isquote(delimiter[i]);
		if (is_quote == 1)
			return (0);
		i++;
	}
	return (1);
}

/*
Handle heredoc redirection for a node in the AST.
Creates a pipe, reads content into it, checks if variables should be expanded.
Takes a node containing the delimiter as args[0] and vars for error handling.
Returns the read fd on success, or -1 on any error.
Example for node with unquoted delimiter "EOF":
- Creates pipe
- Expands variables in content before writing to pipe
- Returns read end of pipe for later use
Example for node with quoted delimiter "'EOF'": 
- Creates pipe
- Does NOT expand variables in content
- Returns read end of pipe for later use
Works with run_heredoc().
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
	int	fd[2];
	int	expand_vars;
	int	read_success;

	if (!node || !node->args || !node->args[0])
	{
		vars->error_code = 1;
		return (-1);
	}
	if (pipe(fd) == -1)
	{
		vars->error_code = 1;
		return (-1);
	}
	expand_vars = expand_heredoc_check(node->args[0]);
	read_success = read_heredoc(fd, node->args[0], vars, expand_vars);
	if (!read_success)
	{
		close(fd[0]);
		close(fd[1]);
		vars->error_code = 1;
		return (-1);
	}
	close(fd[1]);
	return (fd[0]);
}

/*
Main function controlling the heredoc redirection process.
This is the main entry point for heredoc functionality.
Takes a node with the heredoc delimiter and vars for error handling.
Works in these steps:
1) Calls handle_heredoc() to get a fd with heredoc content
2) Redirects stdin using dup2() to read from the heredoc file descriptor
3) Cleans up by closing the original fd
Returns 1 on success, 0 on failure.
Example: For shell command "cat << EOF"
- Input: "cat << EOF" 
- Shell prompts for input with "> "
- User enters multiple lines: "Hello", "World", "EOF"
- handle_heredoc() captures "Hello" and "World" into a pipe
- run_heredoc() redirects stdin to read from this pipe
- When "cat" executes, it reads "Hello\nWorld\n" from stdin
- Output: "Hello" and "World" appear on screen
This creates the effect of an inline document for commands that read from stdin.
*/
int	run_heredoc(t_node *node, t_vars *vars)
{
	int	fd;

	fd = handle_heredoc(node, vars);
	if (fd == -1)
		return (0);
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		vars->error_code = 1;
		return (0);
	}
	close(fd);
	return (1);
}
