/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_multiline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 18:34:38 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 19:36:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"


/*
Stores the content provided in the 'content' buffer into the heredoc
temporary file (TMP_BUF). Stops if the delimiter is found within the buffer.
Performs expansion based on vars->pipes->hd_expand.
Returns:
 0: Delimiter found within the 'content' buffer.
 1: Delimiter NOT found within the 'content' buffer (reached end of buffer).
-1: Error (e.g., file open/write failed).
*/
// int store_multiline_hd(char *content, t_vars *vars)
// {
// 	// --- Block 1: Initialization & Pre-checks ---
// 	char    *curr_pos;
// 	int     fd;
// 	int     has_delim; // Moved closer to usage (Block 5) but needed for return logic

// 	curr_pos = content;
// 	has_delim = 0;
// 	DBG_PRINTF(DEBUG_HEREDOC, "Entering store_multiline_hd().\n");
// 	if (!vars || !vars->pipes || !vars->pipes->heredoc_delim) // Added vars check
// 		return (-1); // Should not happen if called correctly
// 	DBG_PRINTF(DEBUG_HEREDOC, "Storing multiline content, Delim='%s', Expand=%d\n",
// 			   vars->pipes->heredoc_delim, vars->pipes->hd_expand);
// 	// --- Block 2: File Opening ---
// 	fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
// 	if (fd == -1)
// 	{
// 		perror("bleshell: store_multiline_hd: open");
// 		vars->error_code = ERR_DEFAULT;
// 		return (-1);
// 	}
// 	// --- Block 3: Loop Setup ---
// 	size_t  delim_len = ft_strlen(vars->pipes->heredoc_delim); // Moved declaration here
// 	while (curr_pos && *curr_pos)
// 	{
// 		// --- Block 4: Line Extraction ---
// 		char    *line;          // Moved declaration inside loop
// 		char    *next_newline;  // Moved declaration inside loop

// 		next_newline = ft_strchr(curr_pos, '\n');
// 		if (next_newline)
// 		{
// 			line = ft_substr(curr_pos, 0, next_newline - curr_pos);
// 			curr_pos = next_newline + 1;
// 		}
// 		else
// 		{
// 			line = ft_strdup(curr_pos);
// 			curr_pos = NULL;
// 		}
// 		if (!line)
// 		{
// 			close(fd);
// 			vars->error_code = ERR_DEFAULT;
// 			return (-1); // Malloc error
// 		}
// 		// --- Block 5: Delimiter Check ---
// 		if ((ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0)
// 			&& (line[delim_len] == '\0'))
// 		{
// 			DBG_PRINTF(DEBUG_HEREDOC, "Found delimiter in multiline buffer.\n");
// 			has_delim = 1;
// 			free(line);
// 			break ; // Delimiter found, stop processing buffer
// 		}
// 		// --- Block 6: Line Expansion ---
// 		if (vars->pipes->hd_expand)
// 		{
// 			char *expanded_line; // Moved declaration inside block

// 			expanded_line = hd_expander(line, vars);
// 			free(line); // Free the original line
// 			if (!expanded_line)
// 			{ // Check if expansion failed
// 				 close(fd);
// 				 vars->error_code = ERR_DEFAULT; // Assuming expansion failure sets error
// 				 return (-1);
// 			}
// 			line = expanded_line; // Use the expanded line for writing
// 		}
// 		// --- Block 7: Write to File ---
// 		if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
// 		{
// 			DBG_PRINTF(DEBUG_HEREDOC, "bleshell: store_multiline_hd: write error\n");
// 			perror("bleshell: store_multiline_hd: write"); // Added perror
// 			free(line); // Free line (original or expanded) before returning
// 			close(fd);
// 			vars->error_code = ERR_DEFAULT;
// 			return (-1);
// 		}
// 		// --- Block 8: Loop Cleanup & Iteration ---
// 		free(line); // Free line (original or expanded) after successful write
// 		if (!next_newline)
// 			break ; // Reached end if no newline was found
// 	}
// 	// --- Block 9: Post-Loop Cleanup & Return Value Determination ---
// 	close(fd); // Close the file descriptor
// 	if (!has_delim)
// 	{
// 		DBG_PRINTF(DEBUG_HEREDOC, "Delimiter NOT found in initial buffer.\n");
// 		return (1); // Signal that interactive reading is needed
// 	}
// 	DBG_PRINTF(DEBUG_HEREDOC, "Multiline content stored in %s (from buffer)\n", TMP_BUF);
// 	return (0); // Delimiter was found in the buffer
// }
/*
Validates input parameters and opens heredoc file.
- Checks all required pointers
- Opens TMP_BUF for writing with proper permissions
- Sets error code on failure
- Initializes current position pointer
Returns:
- File descriptor on success
- -1 on failure
*/
int	open_hd_file(char *content, t_vars *vars, char **curr_pos)
{
    int	fd;
    
    *curr_pos = content;
    DBG_PRINTF(DEBUG_HEREDOC, "Entering store_multiline_hd().\n");
    if (!vars || !vars->pipes || !vars->pipes->heredoc_delim)
        return (-1);
    DBG_PRINTF(DEBUG_HEREDOC, "Storing multiline content, Delim='%s', Expand=%d\n",
              vars->pipes->heredoc_delim, vars->pipes->hd_expand);
    fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
    {
        perror("bleshell: store_multiline_hd: open error");
        vars->error_code = ERR_DEFAULT;
    }
    return (fd);
}

/*
Extracts next line from buffer.
- Finds next newline
- Updates position pointer
- Returns extracted line
Returns:
- Newly allocated line
- NULL on error (sets error code)
*/
char	*get_hd_line(char **curr_pos, int fd, t_vars *vars)
{
	char	*line;
	char	*next_newline;
	
	next_newline = ft_strchr(*curr_pos, '\n');
	if (next_newline)
	{
		line = ft_substr(*curr_pos, 0, next_newline - *curr_pos);
		*curr_pos = next_newline + 1;
	}
	else
	{
		line = ft_strdup(*curr_pos);
		*curr_pos = NULL;
	}
	if (!line)
	{
		close(fd);
		vars->error_code = ERR_DEFAULT;
	}
	return (line);
}

/*
Checks if line matches heredoc delimiter.
- Compares line with delimiter
- Updates delimiter found flag
- Frees line if it's a delimiter
Returns:
- 1 if line is delimiter
- 0 otherwise
*/
int	is_hd_delim(char *line, char *delim, int *has_delim)
{
	size_t	delim_len;
	
	delim_len = ft_strlen(delim);
	if ((ft_strncmp(line, delim, delim_len) == 0) && (line[delim_len] == '\0'))
	{
		*has_delim = 1;
		free(line);
		return (1);
	}
	
	return (0);
}

/*
Processes line with expansion if needed and writes to file.
- Expands variables if hd_expand flag is set
- Writes line + newline to file
- Handles errors
Returns:
- 1 on success
- 0 on error (sets error code and closes file)
*/
int	process_and_write_line(int fd, char *line, t_vars *vars)
{
	char	*parsed_line;
	char	*expanded_line;
	int		len;
	
	parsed_line = line;
	if (vars->pipes->hd_expand)
	{
		expanded_line = hd_expander(line, vars);
		free(line);
		if (!expanded_line)
		{
			handle_fd_error(fd, vars, NULL);
			return (0);
		}
		parsed_line = expanded_line;
	}
	len = ft_strlen(parsed_line);
	if (write(fd, parsed_line, len == -1) || write(fd, "\n", 1) == -1)
	{
		free(parsed_line);
		handle_fd_error(fd, vars, "bleshell: store_multiline_hd: write error");
		return (0);
	}
	free(parsed_line);
	return (1);
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
int	store_multiline_hd(char *content, t_vars *vars)
{
	char	*curr_pos;
	int		fd;
	int		has_delim;
	char	*line;
	
	has_delim = 0;
	fd = open_hd_file(content, vars, &curr_pos);
	if (fd == -1)
		return (-1);
	while (curr_pos && *curr_pos)
	{
		line = get_hd_line(&curr_pos, fd, vars);
		if (!line)
			return (-1);
		if (is_hd_delim(line, vars->pipes->heredoc_delim, &has_delim))
			break ;
		if (!process_and_write_line(fd, line, vars))
			return (-1);
		if (!curr_pos)
			break ;
	}
	close(fd);
	if (!has_delim)
		return (1);
	return (0);
}
