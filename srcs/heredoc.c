/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 06:51:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Joins a chunk to an existing string, freeing the original string.
- Takes base string and chunk to append.
- Creates new joined string and frees original.
Returns:
Newly allocated string with combined content.
Original input string if chunk is NULL.
New copy of chunk if base string is NULL.
Works with hd_expander().

Example: str = "Hello", chunk = " World"
- Returns: "Hello World"
- Original "Hello" string is freed
*/
char	*merge_and_free(char *str, char *chunk)
{
	char	*new_str;

	if (!chunk)
		return (str);
	if (!str)
		return (ft_strdup(chunk));
	new_str = ft_strjoin(str, chunk);
	free(str);
	return (new_str);
}

/*
Extracts and expands a variable from heredoc line.
- Processes a variable starting with $ character.
- Falls back to returning "$" if expansion fails.
Returns:
Newly allocated string with expanded variable.
"$" if variable can't be expanded.
Works with expand_one_line().

Example: At position of "$HOME" in a line
- Returns: "/Users/username" (expanded value)
- Updates position to after variable name
*/
char	*expand_hd_var(char *line, int *pos, t_vars *vars)
{
	char	*expanded;
	int		old_pos;

	old_pos = *pos;
	expanded = expand_variable(line, pos, NULL, vars);
	if (!expanded)
	{
		*pos = old_pos + 1;
		expanded = ft_strdup("$");
		return (expanded);
	}
	return (expanded);
}

/*
Extracts a regular text string up to $ character.
- Processes text segment from current position to next $.
- Creates substring of this text segment.
Returns:
- Newly allocated substring from start to next variable.
- NULL if no characters to extract.
Works with expand_one_line().

Example: At start of "Hello $USER"
- Returns: "Hello "
- Updates position to the $ character
*/
char	*read_hd_str(char *line, int *pos)
{
	int	start;

	start = *pos;
	while (line[*pos] && line[*pos] != '$')
		(*pos)++;
	if (*pos > start)
		return (ft_substr(line, start, *pos - start));
	return (NULL);
}

/*
Expands variables in a heredoc line
Returns:
- Newly allocated string with variables expanded.
- Empty string on NULL input or if no expansions/error.
*/
char	*hd_expander(char *line, t_vars *vars)
{
	int		pos;
	char	*result;
	char	*segment;

	if (!line || !vars)
		return (ft_strdup(""));
	result = NULL;
	pos = 0;
	while (line[pos])
	{
		if (line[pos] == '$')
		{
			segment = expand_hd_var(line, &pos, vars);
			result = merge_and_free(result, segment);
		}
		else
		{
			segment = read_hd_str(line, &pos);
			result = merge_and_free(result, segment);
		}
		free(segment);
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}

/*
Processes a heredoc delimiter by removing quotes if present.
Modifies the string pointed to by delimiter directly.
*/
void	strip_outer_quotes(char **delimiter, t_vars *vars)
{
	char	*str;
	char	*new_str;
	size_t	len;

	if (!delimiter || !*delimiter)
		return ;
	str = *delimiter;
	len = ft_strlen(str);
	if (len < 2)
		return ;
	if ((str[0] == '"' && str[len - 1] == '"')
		|| (str[0] == '\'' && str[len - 1] == '\''))
	{
		new_str = ft_substr(str, 1, len - 2);
		if (!new_str)
		{
			vars->error_code = ERR_DEFAULT;
			return ;
		}
		free(*delimiter);
		*delimiter = new_str;
	}
}

/*
Triggers interactive heredoc gathering for single-line commands.
- Opens TMP_BUF (O_TRUNC).
- Calls get_interactive_hd.
- Closes TMP_BUF.
- Sets hd_text_ready flag.
Returns:
- 1 on success.
- 0 on failure.
*/
int	interactive_hd_mode(t_vars *vars)
{
	int	write_fd;

	if (!vars || !vars->pipes || !vars->pipes->heredoc_delim)
	{
		return (0);
	}
	write_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (write_fd == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (get_interactive_hd(write_fd, vars) == -1)
	{
		close(write_fd);
		unlink(TMP_BUF);
		return (0);
	}
	close(write_fd);
	vars->hd_text_ready = 1;
	return (1);
}

/*
Prepares a line for heredoc by conditionally expanding variables.
- Expands variables if hd_expand flag is set
- Creates a simple copy otherwise
Returns:
- Newly allocated string with expanded or copied content.
- NULL on memory allocation failure.
*/
char	*prepare_hd_line(char *line, t_vars *vars)
{
	char	*result;

	if (!line || !vars || !vars->pipes)
		return (NULL);
	if (vars->pipes->hd_expand == 1)
		result = hd_expander(line, vars);
	else
		result = ft_strdup(line);
	return (result);
}

/*
Writes line to heredoc fd (TMP_BUF) with variable expansion
based on vars->pipes->hd_expand.
Returns:
- 1 on success.
- 0 on failure.
*/
int	write_to_hd(int fd, char *line, t_vars *vars)
{
	char	*expanded_line;
	int		write_result;

	if (!line || fd < 0 || !vars || !vars->pipes)
		return (0);
	expanded_line = prepare_hd_line(line, vars);
	if (!expanded_line)
		return (0);
	write_result = write(fd, expanded_line, ft_strlen(expanded_line));
	if (write_result != -1)
		write(fd, "\n", 1);
	free(expanded_line);
	if (write_result == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	else
		return (1);
}

/*
Reads lines interactively until the delimiter is found or EOF is reached.
- Writes lines to the given open file descriptor `write_fd`.
- Uses vars->pipes->heredoc_delim as the delimiter.
- Performs expansion based on vars->pipes->hd_expand via write_to_hd.
Returns:
- 0 on success (delimiter found or EOF reached after warning).
- -1 on failure (write error or malloc error).
*/
int	get_interactive_hd(int write_fd, t_vars *vars)
{
	char	*line;
	int		status;

	status = 0;
	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
			break ;
		if (ft_strcmp(line, vars->pipes->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		if (!write_to_hd(write_fd, line, vars))
		{
			free(line);
			status = -1;
			break ;
		}
		free(line);
		line = NULL;
	}
	return (status);
}

/*
Sets up redirection using the heredoc fd stored in vars->pipes->heredoc_fd.
Calls process_heredoc() to ensure content is ready
(either reads interactively or opens pre-stored file).
Returns:
- 1 on success.
- 0 on failure (e.g., TMP_BUF not ready, fd already open).
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
	if (!process_heredoc(node, vars))
	{
		return (0);
	}
	if (vars->pipes->heredoc_fd < 0)
	{
		return (0);
	}
	if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1)
	{
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	close(vars->pipes->heredoc_fd);
	vars->pipes->heredoc_fd = -1;
	return (1);
}

/*
Opens TMP_BUF for reading and stores the fd in vars->pipes->heredoc_fd.
Assumes heredoc content is already fully gathered in TMP_BUF.
Returns:
- 1 on success.
- 0 on failure (e.g., TMP_BUF not ready, fd already open).
- Sets vars->error_code to ERR_DEFAULT on failure.
*/
int	process_heredoc(t_node *node, t_vars *vars)
{
	if (!node || !vars || !vars->pipes)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (vars->pipes->heredoc_fd >= 0)
	{
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
	}
	if (!vars->hd_text_ready)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (!read_tmp_buf(vars))
	{
		return (0);
	}
	return (1);
}

/*
Opens the temporary heredoc file (TMP_BUF) for reading.
Stores the file descriptor in vars->pipes->heredoc_fd on success.
Handles errors and attempts to unlink the TMP_BUF on failure.
Returns:
- 1 on success.
- 0 on failure.
*/
int	read_tmp_buf(t_vars *vars)
{
	int	fd;

	fd = open(TMP_BUF, O_RDONLY);
	if (fd == -1)
	{
		perror("bleshell: failed to open TMP_BUF file for reading");
		vars->error_code = ERR_DEFAULT;
		unlink(TMP_BUF);
		return (0);
	}
	vars->pipes->heredoc_fd = fd;
	return (1);
}

/*
Checks if the original delimiter is a valid quoted delimiter
("" or "..." or '...').
If valid, allocates and sets clean_delim and sets quoted.
Returns:
- 1 if a valid quoted delimiter was processed.
- 0 if otherwise.
*/
int	chk_quoted_delim(char *orig_delim, size_t len,
		char **clean_delim_ptr, int *quoted_ptr)
{
	if (len == 2 && orig_delim[0] == '\"' && orig_delim[1] == '\"')
	{
		*clean_delim_ptr = ft_strdup("");
		*quoted_ptr = 1;
		return (1);
	}
	else if (len >= 2 && orig_delim[0] == '\"'
		&& orig_delim[len - 1] == '\"')
	{
		*clean_delim_ptr = ft_substr(orig_delim, 1, len - 2);
		*quoted_ptr = 1;
		return (1);
	}
	else if (len >= 2 && orig_delim[0] == '\''
		&& orig_delim[len - 1] == '\'')
	{
		*clean_delim_ptr = ft_substr(orig_delim, 1, len - 2);
		*quoted_ptr = 1;
		return (1);
	}
	if (*quoted_ptr == 1 && !*clean_delim_ptr)
	{
		return (0);
	}
	return (0);
}

/*
Checks if the original delimiter is a valid unquoted delimiter.
- An unquoted delimiter cannot contain any quote characters.
- If valid, allocates and sets clean_delim and sets quoted to 0.
- Calls tok_syntax_error_msg if invalid internal quotes are found.
Return: 
- 1 if a valid unquoted delimiter was processed.
- 0 if syntax error or malloc error.
*/
int	chk_normal_delim(char *orig_delim, size_t len
	, char **clean_delim_ptr, int *quoted_ptr)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		if (orig_delim[i] == '\'' || orig_delim[i] == '\"')
		{
			return (0);
		}
		i++;
	}
	*clean_delim_ptr = ft_strdup(orig_delim);
	if (!*clean_delim_ptr)
		return (0);
	*quoted_ptr = 0;
	return (1);
}

/*
Cleans up resources after a heredoc error.
- Closes the file descriptor.
- Removes the temporary file.
Works with process_heredoc_path to centralize error handling.
*/
void	cleanup_heredoc_fd(int write_fd)
{
	close(write_fd);
	unlink(TMP_BUF);
}

/*
Stores the validated clean delimiter and expansion flag
in vars->pipes->heredoc_delim.
Frees any previously stored delimiter.
 */
void	store_cln_delim(t_vars *vars, char *clean_delim, int quoted)
{
	if (vars->pipes->heredoc_delim)
		free(vars->pipes->heredoc_delim);
	vars->pipes->heredoc_delim = clean_delim;
	vars->pipes->hd_expand = !quoted;
}

/*
Validates the original heredoc delimiter string whether it is
quoted or unquoted.
Sets vars->pipes->heredoc_delim and vars->pipes->hd_expand on success.
Returns:
- 1 on success.
- 0 on syntax error or malloc error.
*/
int	is_valid_delim(char *orig_delim, t_vars *vars)
{
	char	*clean_delim;
	size_t	len;
	int		quoted;
	int		processed;

	clean_delim = NULL;
	quoted = 0;
	processed = 0;
	if (!orig_delim)
		return (0);
	len = ft_strlen(orig_delim);
	processed = chk_quoted_delim(orig_delim, len, &clean_delim, &quoted);
	if (!processed)
		processed = chk_normal_delim(orig_delim, len, &clean_delim, &quoted);
	if (!processed)
	{
		if (!clean_delim && vars->error_code != ERR_SYNTAX)
		{
			vars->error_code = ERR_DEFAULT;
		}
		return (0);
	}
	store_cln_delim(vars, clean_delim, quoted);
	return (1);
}
