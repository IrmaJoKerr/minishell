/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 17:01:34 by bleow            ###   ########.fr       */
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
Works with expand_one_line().

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
char	*expand_heredoc_var(char *line, int *pos, t_vars *vars)
{
	char	*expanded;
	int		old_pos;

	old_pos = *pos;
	expanded = expand_variable(line, pos, NULL, vars);
	if (!expanded)
	{
		*pos = old_pos + 1;
		expanded = ft_strdup("$");
		fprintf(stderr, "DEBUG: expand_heredoc_var: Expansion failed, returning '$'\n");
		return (expanded);
	}
	fprintf(stderr, "DEBUG: expand_heredoc_var: Expanded to '%s'\n", expanded);
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
char	*read_heredoc_str(char *line, int *pos)
{
	int	start;

	start = *pos;
	while (line[*pos] && line[*pos] != '$')
		(*pos)++;
	if (*pos > start)
		return (ft_substr(line, start, *pos - start));
	return (NULL);
}

// /*
// Processes one segment of a heredoc line.
// - Handles either variable expansion or regular text.
// - Joins processed segment to result string.
// - Updates position for next segment processing.
// Returns:
// - Updated result string with new segment added.
// Works with hd_expander().
// Example: For input "Hello $HOME"
// - First call processes "Hello " (regular text)
// - Second call processes "$HOME" (variable)
// - Returns combined result after each call
// */
// char	*expand_one_line(char *line, int *pos, t_vars *vars, char *result)
// {
// 	char	*segment;

// 	if (line[*pos] == '$')
// 	{
// 		segment = expand_heredoc_var(line, pos, vars);
// 		result = merge_and_free(result, segment);
// 		free(segment);
// 	}
// 	else
// 	{
// 		segment = read_heredoc_str(line, pos);
// 		result = merge_and_free(result, segment);
// 		free(segment);
// 	}
// 	return (result);
// }

/*
Expands variables in a heredoc line
Returns:
- Newly allocated string with variables expanded.
- Empty string on NULL input or if no expansions/error.
*/
char *hd_expander(char *line, t_vars *vars)
{
    int     pos;
    char    *result;
    char    *segment;
    
    if (!line || !vars)
        return (ft_strdup(""));
    result = NULL;
    pos = 0;
    while (line[pos])
    {
        if (line[pos] == '$')
        {
            segment = expand_heredoc_var(line, &pos, vars);
            result = merge_and_free(result, segment);
        }
        else
        {
            segment = read_heredoc_str(line, &pos);
            result = merge_and_free(result, segment);
        }
		free(segment);
    }
    if (!result)
        return (ft_strdup(""));
    return (result);
}

// /*
// Expands all variables in a heredoc line.
// - Processes entire line character by character.
// - Builds result by combining expanded segments.
// - Handles all variable expansions in the line.
// Returns:
// New string with all variables expanded.
// Empty string on NULL input or if no expansions.
// Works with write_to_heredoc().

// Example: Input "Hello $USER world"
// - Expands to "Hello username world"
// - All $VAR references replaced with their values
// */
// char	*hd_expander(char *line, t_vars *vars)
// {
// 	int		pos;
// 	char	*result;

// 	if (!line || !vars)
// 		return (ft_strdup(""));
// 	result = NULL;
// 	pos = 0;
// 	while (line[pos])
// 		result = expand_one_line(line, &pos, vars, result);
// 	if (!result)
// 		return (ft_strdup(""));
// 	return (result);
// }

/*
MODIFIED: Processes a heredoc delimiter by removing quotes if present.
Modifies the string pointed to by delimiter directly.
Returns: void
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
    DBG_PRINTF(DEBUG_HEREDOC, "strip_outer_quotes: Processing '%s'\n", str);
    if (len < 2)
        return ;
    if ((str[0] == '"' && str[len - 1] == '"') ||
        (str[0] == '\'' && str[len - 1] == '\''))
    {
        DBG_PRINTF(DEBUG_HEREDOC, "Removing quotes from '%s'\n", str);
        new_str = ft_substr(str, 1, len - 2);
        if (!new_str)
        {
            perror("bleshell: strip_outer_quotes: ft_substr failed");
            vars->error_code = ERR_DEFAULT;
            return ;
        }
        // If new_str is valid:
        free(*delimiter);
        *delimiter = new_str;
        DBG_PRINTF(DEBUG_HEREDOC, "After removing quotes: '%s'\n", *delimiter);
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
Writes line to heredoc fd (TMP_BUF) with variable expansion
based on vars->pipes->hd_expand.
Returns:
- 1 on success.
- 0 on failure.
*/
int write_to_heredoc(int fd, char *line, t_vars *vars)
{
	char	*expanded_line;
	int		write_result;

	if (!line || fd < 0 || !vars || !vars->pipes)
		return (0);
	if (vars->pipes->hd_expand == 1)
	{
		expanded_line = hd_expander(line, vars);
		if (!expanded_line)
			return (0);
	}
	else
	{
		expanded_line = ft_strdup(line);
		if (!expanded_line)
			return (0);
	}
	write_result = write(fd, expanded_line, ft_strlen(expanded_line));
	if (write_result != -1)
		write(fd, "\n", 1);
	free(expanded_line);
	if (write_result == -1)
	{
		perror("bleshell: write error in heredoc");
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	else
		return (1);
}

/*
NEW: Reads lines interactively until the delimiter is found or EOF is reached.
- Writes lines to the given open file descriptor `write_fd`.
- Uses vars->pipes->heredoc_delim as the delimiter.
- Performs expansion based on vars->pipes->hd_expand via write_to_heredoc.
Returns:
- 0 on success (delimiter found or EOF reached after warning).
- -1 on failure (write error or malloc error).
*/
int	get_interactive_hd(int write_fd, t_vars *vars)
{
    char	*line;
    int		status;

    DBG_PRINTF(DEBUG_HEREDOC, "Entering get_interactive_hd(). Delim='%s'\n",
               vars->pipes->heredoc_delim);
    status = 0;
    while (1)
    {
        line = readline("heredoc> ");
        if (!line)
        {
            fprintf(stderr, "\nbleshell: warning: here-document delimited by end-of-file (wanted `%s')\n", vars->pipes->heredoc_delim);
            break ;
        }
        if (ft_strcmp(line, vars->pipes->heredoc_delim) == 0)
        {
            free(line);
            break ;
        }
        if (!write_to_heredoc(write_fd, line, vars))
        {
            free(line);
            status = -1;
            break ;
        }
        free(line);
        line = NULL;
    }
    DBG_PRINTF(DEBUG_HEREDOC, "Exiting get_interactive_hd() with status %d.\n", status);
    return (status);
}

/*
Stores the content provided in the 'content' buffer into the heredoc
temporary file (TMP_BUF). Stops if the delimiter is found within the buffer.
Performs expansion based on vars->pipes->hd_expand.
Returns:
 0: Delimiter found within the 'content' buffer.
 1: Delimiter NOT found within the 'content' buffer (reached end of buffer).
-1: Error (e.g., file open/write failed).
*/
// int store_multiline_heredoc_content(char *content, t_vars *vars)
// {
// 	int     fd;
// 	char    *line;
// 	char    *expanded_line;
// 	size_t  delim_len;
// 	int     has_delim = 0;
// 	char    *curr_pos = content;
// 	char    *next_newline;

// 	DBG_PRINTF(DEBUG_HEREDOC, "Entering store_multiline_heredoc_content().\n");
// 	if (!vars->pipes->heredoc_delim)
// 		return (-1); // Should not happen if called correctly
// 	DBG_PRINTF(DEBUG_HEREDOC, "Storing multiline content, Delim='%s', Expand=%d\n",
// 			   vars->pipes->heredoc_delim, vars->pipes->hd_expand);
// 	fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
// 	if (fd == -1) {
// 		perror("bleshell: store_multiline_heredoc_content: open");
// 		vars->error_code = ERR_DEFAULT;
// 		return (-1);
// 	}
// 	delim_len = ft_strlen(vars->pipes->heredoc_delim);
// 	while (curr_pos && *curr_pos)
// 	{
// 		next_newline = ft_strchr(curr_pos, '\n');
// 		if (next_newline)
// 		{
// 			// Extract line without newline
// 			line = ft_substr(curr_pos, 0, next_newline - curr_pos);
// 			curr_pos = next_newline + 1; // Move past newline for next iteration
// 		}
// 		else
// 		{
// 			// Last part of the buffer without a trailing newline
// 			line = ft_strdup(curr_pos);
// 			curr_pos = NULL; // Signal end of buffer
// 		}
// 		if (!line)
// 		{
// 			close(fd); vars->error_code = ERR_DEFAULT;
// 			return (-1);
// 		} // Malloc error
// 		// Check for delimiter
// 		if ((ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0)
// 			&& (line[delim_len] == '\0'))
// 		{
// 			DBG_PRINTF(DEBUG_HEREDOC, "Found delimiter in multiline buffer.\n");
// 			has_delim = 1;
// 			free(line);
// 			break ; // Delimiter found, stop processing buffer
// 		}
// 		// Expand if necessary
// 		if (vars->pipes->hd_expand) {
// 			// Replace perform_expansion with hd_expander
// 			expanded_line = hd_expander(line, vars);
// 			free(line); // Free the original line
// 			if (!expanded_line) { // Check if expansion failed
// 				 close(fd);
// 				 vars->error_code = ERR_DEFAULT; // Assuming expansion failure sets error
// 				 return (-1);
// 			}
// 			line = expanded_line; // Use the expanded line
// 		}
// 		// Write line + newline to temp file
// 		if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
// 		{
// 			DBG_PRINTF(DEBUG_HEREDOC, "bleshell: store_multiline_heredoc_content: write\n");
// 			free(line); // Free line (original or expanded) before returning
// 			close(fd);
// 			vars->error_code = ERR_DEFAULT;
// 			return (-1);
// 		}
// 		free(line); // Free line (original or expanded) after successful write
// 		if (!next_newline)
// 			break ; // Reached end if no newline was found
// 	}
// 	close(fd)
// 	if (!has_delim)
// 	{
// 		// Print warning only if we reached the end of the *initial* buffer without finding delim
// 		 fprintf(stderr, "bleshell: warning: here-document delimited by end-of-file (wanted `%s')\n", vars->pipes->heredoc_delim);
// 		 DBG_PRINTF(DEBUG_HEREDOC, "Delimiter NOT found in initial buffer.\n");
// 		 return (1); // Signal that interactive reading is needed
// 	}
// 	DBG_PRINTF(DEBUG_HEREDOC, "Multiline content stored in %s (from buffer)\n", TMP_BUF);
// 	return (0); // Delimiter was found in the buffer
// }
int store_multiline_heredoc_content(char *content, t_vars *vars)
{
    // --- Block 1: Initialization & Pre-checks ---
    char    *curr_pos;
    int     fd;
    int     has_delim; // Moved closer to usage (Block 5) but needed for return logic

	curr_pos = content;
	has_delim = 0;
    DBG_PRINTF(DEBUG_HEREDOC, "Entering store_multiline_heredoc_content().\n");
    if (!vars || !vars->pipes || !vars->pipes->heredoc_delim) // Added vars check
        return (-1); // Should not happen if called correctly
    DBG_PRINTF(DEBUG_HEREDOC, "Storing multiline content, Delim='%s', Expand=%d\n",
               vars->pipes->heredoc_delim, vars->pipes->hd_expand);
    // --- Block 2: File Opening ---
    fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
	{
        perror("bleshell: store_multiline_heredoc_content: open");
        vars->error_code = ERR_DEFAULT;
        return (-1);
    }
    // --- Block 3: Loop Setup ---
    size_t  delim_len = ft_strlen(vars->pipes->heredoc_delim); // Moved declaration here
    while (curr_pos && *curr_pos)
    {
        // --- Block 4: Line Extraction ---
        char    *line;          // Moved declaration inside loop
        char    *next_newline;  // Moved declaration inside loop

        next_newline = ft_strchr(curr_pos, '\n');
        if (next_newline)
        {
            line = ft_substr(curr_pos, 0, next_newline - curr_pos);
            curr_pos = next_newline + 1;
        }
        else
        {
            line = ft_strdup(curr_pos);
            curr_pos = NULL;
        }
        if (!line)
        {
            close(fd);
            vars->error_code = ERR_DEFAULT;
            return (-1); // Malloc error
        }
        // --- Block 5: Delimiter Check ---
        if ((ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0)
            && (line[delim_len] == '\0'))
        {
            DBG_PRINTF(DEBUG_HEREDOC, "Found delimiter in multiline buffer.\n");
            has_delim = 1;
            free(line);
            break ; // Delimiter found, stop processing buffer
        }
        // --- Block 6: Line Expansion ---
        if (vars->pipes->hd_expand)
		{
            char *expanded_line; // Moved declaration inside block

            expanded_line = hd_expander(line, vars);
            free(line); // Free the original line
            if (!expanded_line)
			{ // Check if expansion failed
                 close(fd);
                 vars->error_code = ERR_DEFAULT; // Assuming expansion failure sets error
                 return (-1);
            }
            line = expanded_line; // Use the expanded line for writing
        }
        // --- Block 7: Write to File ---
        if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
        {
            DBG_PRINTF(DEBUG_HEREDOC, "bleshell: store_multiline_heredoc_content: write error\n");
            perror("bleshell: store_multiline_heredoc_content: write"); // Added perror
            free(line); // Free line (original or expanded) before returning
            close(fd);
            vars->error_code = ERR_DEFAULT;
            return (-1);
        }
        // --- Block 8: Loop Cleanup & Iteration ---
        free(line); // Free line (original or expanded) after successful write
        if (!next_newline)
            break ; // Reached end if no newline was found
    }
    // --- Block 9: Post-Loop Cleanup & Return Value Determination ---
    close(fd); // Close the file descriptor
    if (!has_delim)
    {
        DBG_PRINTF(DEBUG_HEREDOC, "Delimiter NOT found in initial buffer.\n");
        return (1); // Signal that interactive reading is needed
    }
    DBG_PRINTF(DEBUG_HEREDOC, "Multiline content stored in %s (from buffer)\n", TMP_BUF);
    return (0); // Delimiter was found in the buffer
}

/*
MODIFIED: Sets up redirection using the heredoc fd stored in vars->pipes->heredoc_fd.
Calls process_heredoc to ensure content is ready (either reads interactively or opens pre-stored file).
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
	fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Setting up heredoc for command\n");
	if (!process_heredoc(node, vars))
	{
		fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() process_heredoc failed.\n");
		// process_heredoc should set error_code
		return (0); // Failure
	}
	if (vars->pipes->heredoc_fd < 0)
	{
		fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Invalid heredoc_fd after process_heredoc.\n");
		return (0) ;
	}
	if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1)
	{
		perror("bleshell: failed to redirect stdin for heredoc");
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	close(vars->pipes->heredoc_fd);
	vars->pipes->heredoc_fd = -1;
	fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Heredoc connected to stdin\n");
	return (1);
}

/*
MODIFIED: Assumes heredoc content is already fully gathered in TMP_BUF.
Opens TMP_BUF for reading and stores the fd in vars->pipes->heredoc_fd.
*/
int	process_heredoc(t_node *node, t_vars *vars)
{
    DBG_PRINTF(DEBUG_HEREDOC, "Entering process_heredoc()\n");
    if (!node || !vars || !vars->pipes)
    {
        DBG_PRINTF(DEBUG_HEREDOC, "process_heredoc() Invalid state (node or vars missing).\n");
        vars->error_code = ERR_DEFAULT;
        return (0);
    }
    if (vars->pipes->heredoc_fd >= 0)
    {
        DBG_PRINTF(DEBUG_HEREDOC, "Closing pre-existing heredoc_fd %d\n", vars->pipes->heredoc_fd);
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
    }
    if (!vars->hd_text_ready)
	{
        DBG_PRINTF(DEBUG_HEREDOC, "process_heredoc() called but hd_text_ready is not set!\n");
        vars->error_code = ERR_DEFAULT;
        return (0);
    }
    DBG_PRINTF(DEBUG_HEREDOC, "Opening TMP_BUF for reading.\n");
    if (!read_tmp_buf(vars))
    {
        DBG_PRINTF(DEBUG_HEREDOC, "process_heredoc() failed to open TMP_BUF via read_tmp_buf.\n");
        return (0);
    }
    DBG_PRINTF(DEBUG_HEREDOC, "Heredoc content ready on fd=%d\n", vars->pipes->heredoc_fd);
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
int	chk_quoted_delim(char *orig_delim, size_t len
	,char **clean_delim_ptr, int *quoted_ptr)
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
	fprintf(stderr, "[DBG_HEREDOC] Validated Delimiter: '%s', expand_vars=%d\n",
		vars->pipes->heredoc_delim, vars->pipes->hd_expand);
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
			perror("bleshell: malloc error during delimiter processing");
			vars->error_code = ERR_DEFAULT;
		}
		return (0);
	}
	store_cln_delim(vars, clean_delim, quoted);
	return (1);
}
