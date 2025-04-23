/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 21:44:43 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Allocates memory and reads content from an open file descriptor.
- Allocates buffer of specified size plus null terminator
- Reads file content into the buffer
- Adds null terminator for string usage
Returns:
- Newly allocated string containing the file contents
- NULL if allocation failed or reading failed
*/
char	*allocate_and_read(int fd, size_t size)
{
    char	*buffer;
    ssize_t	bytes_read;
    
    buffer = malloc(size + 1);
    if (!buffer)
        return (NULL);
    bytes_read = read(fd, buffer, size);
    if (bytes_read == -1)
    {
        free(buffer);
        return (NULL);
    }
    buffer[bytes_read] = '\0';
    return (buffer);
}

/*
Function reads an entire file into memory.
Returns:
- A newly allocated string containing the file contents,
- NULL if an error occurred.
*/
char	*read_entire_file(const char *filename)
{
    int			fd;
    char		*content;
    struct stat	file_stats;
    
    fd = open(filename, O_RDONLY);
    if (fd == -1)
        return (NULL);
    if (fstat(fd, &file_stats) == -1)
    {
        close(fd);
        return (NULL);
    }
    content = allocate_and_read(fd, file_stats.st_size);
    close(fd);
    return (content);
}

/*
Processes multiline input.
- If the first line contains a valid heredoc, it stores the content in 
  TMP_BUF and sets mode=1.
- Otherwise, writes the full input to TMP_BUF and processes commands
  from there.
*/
int process_multiline_input(char *input, t_vars *vars)
{
    char	*first_line_end;
    char	*content_start;
    int		first_line_len;
    int		tokenize_success;
    int		write_fd;
    int		delimiter_found_in_buffer;
    char	*current_pos;
    char	*next_newline;
    char	*line;
    size_t	delim_len;

    DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: START\n");
    first_line_end = ft_strchr(input, '\n');
    if (!first_line_end)
	{
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: No newline found (shouldn't happen here)\n");
        process_command(input, vars);
        return (0);
    }
    first_line_len = first_line_end - input;
    content_start = first_line_end + 1;
    *first_line_end = '\0';
    DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: Tokenizing first line: '%.*s'\n", first_line_len, input);
    reset_shell(vars);
    tokenize_success = improved_tokenize(input, vars);
    *first_line_end = '\n';
    if (!tokenize_success || vars->error_code == ERR_SYNTAX) {
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: Tokenization failed or syntax error on first line.\n");
        return (0);
    }
    if (vars->pipes->heredoc_delim != NULL)
    {
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: HEREDOC path entered. Delim='%s'\n", vars->pipes->heredoc_delim);
        t_node *heredoc_node = vars->head;
        t_node *delimiter_node = NULL;
        while(heredoc_node) {
            if (heredoc_node->type == TYPE_HEREDOC) {
                delimiter_node = heredoc_node->next;
                while (delimiter_node && delimiter_node->type != TYPE_ARGS && delimiter_node->next) { // Skip non-arg tokens like whitespace if any
                    delimiter_node = delimiter_node->next;
                }
                if (delimiter_node && delimiter_node->type == TYPE_ARGS) break;
                else delimiter_node = NULL;
            }
            heredoc_node = heredoc_node->next;
        }
        if (!delimiter_node || !delimiter_node->args || !delimiter_node->args[0]) {
            fprintf(stderr, "[ERROR] process_multiline_input: Could not find delimiter token node after validation.\n");
            return (0);
        }
        char *heredoc_op_ptr = ft_strnstr(input, "<<", first_line_len);
        char *raw_delim_in_line = NULL;
        if (heredoc_op_ptr) {
            int search_start_offset = (heredoc_op_ptr - input) + 2;
            int remaining_len = first_line_len - search_start_offset;
            if (remaining_len > 0) {
                raw_delim_in_line = ft_strnstr(input + search_start_offset, delimiter_node->args[0], remaining_len);
            }
        }
        if (!raw_delim_in_line) {
            *first_line_end = '\0';
            fprintf(stderr, "[ERROR] process_multiline_input: Could not find raw delimiter '%s' in first line '%s'.\n", delimiter_node->args[0], input);
            *first_line_end = '\n';
            return 0;
        }
        int pos_after_raw_delim = (raw_delim_in_line - input) + ft_strlen(delimiter_node->args[0]);
        *first_line_end = '\0';
        if (!check_trailing_chars(input, pos_after_raw_delim)) {
            *first_line_end = '\n';
            vars->error_code = ERR_SYNTAX;
            return (0);
        }
        *first_line_end = '\n';
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: Trailing chars OK. Processing content buffer.\n");
        write_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (write_fd == -1)
		{
            perror("bleshell: process_multiline_input: open(TMP_BUF)");
            vars->error_code = ERR_DEFAULT;
            return (0);
        }
        delimiter_found_in_buffer = 0;
        delim_len = ft_strlen(vars->pipes->heredoc_delim);
        current_pos = content_start;
        while (current_pos && *current_pos)
        {
            next_newline = ft_strchr(current_pos, '\n');
            if (next_newline) {
                line = ft_substr(current_pos, 0, next_newline - current_pos);
                current_pos = next_newline + 1;
            } else {
                line = ft_strdup(current_pos);
                current_pos = NULL;
            }
            if (!line)
			{
				close(write_fd); vars->error_code = ERR_DEFAULT; return (0);
			}
            if (ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0 && line[delim_len] == '\0')
            {
                DBG_PRINTF(DEBUG_HEREDOC, "Delimiter found in buffer.\n");
                delimiter_found_in_buffer = 1;
                free(line);
                break ;
            }
            if (!write_to_hd(write_fd, line, vars)) {
                free(line);
                close(write_fd);
                unlink(TMP_BUF);
                return (0);
            }
            free(line);
            if (!next_newline)
				break ;
        }
        if (!delimiter_found_in_buffer)
        {
            DBG_PRINTF(DEBUG_HEREDOC, "Delimiter not in buffer, calling get_interactive_hd().\n");
            if (get_interactive_hd(write_fd, vars) == -1)
			{
                close(write_fd);
                unlink(TMP_BUF);
                return (0);
            }
        }
        close(write_fd);
        vars->hd_text_ready = 1;
        DBG_PRINTF(DEBUG_HEREDOC, "Heredoc content gathering complete. TMP_BUF ready.\n");
        DBG_PRINTF(DEBUG_HEREDOC, "Executing command from first line: '%.*s'\n", first_line_len, input);
        *first_line_end = '\0';
        process_command(input, vars);
        *first_line_end = '\n';
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: HEREDOC path finished.\n");
    }
    else
    {
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: NON-HEREDOC path entered.\n");
        int fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd == -1) {
            perror("bleshell: process_multiline_input: open(TMP_BUF)");
            vars->error_code = ERR_DEFAULT;
            return (0);
        }
        if (write(fd, input, ft_strlen(input)) == -1) {
            perror("bleshell: process_multiline_input: write(TMP_BUF)");
            close(fd);
            unlink(TMP_BUF);
            vars->error_code = ERR_DEFAULT;
            return (0);
        }
        close(fd);
        DBG_PRINTF(DEBUG_HEREDOC, "Finished writing full input to TMP_BUF.\n");
        tmp_buf_reader(vars);
    }
    DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: END\n");
    return (1);
}

/*
Processes user input and directs to appropriate command handler.
- Detects and handles multiline input with heredocs
- Processes single-line commands directly
- Cleans up heredoc resources after processing
- Resets redirection file descriptors
Main entry point for all command input processing in the shell.
*/
void	handle_input(char *input, t_vars *vars)
{
    if (ft_strchr(input, '\n'))
    {
    	process_multiline_input(input, vars);
    }
    else
    {
        reset_shell(vars);
        process_command(input, vars);
    }
    if (vars->pipes && vars->pipes->heredoc_delim)
    {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    reset_redirect_fds(vars);
}

/*
Sets up terminal settings specifically for heredoc input.
- Modifies termios structure to disable ECHO and ICANON flags.
- Uses the original terminal settings as a base.
- Makes user input invisible when entering heredoc content.
- Disables canonical mode for raw character input.
Works with manage_terminal_state() to provide special input handling
for heredocs.
*/
void	term_heredoc(t_vars *vars)
{
	struct termios	heredoc_term;
	
	heredoc_term = vars->ori_term_settings;
    heredoc_term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &heredoc_term);
}

/*
Manages terminal states throughout different operations in the shell.
- Handles saving the original terminal settings (TERM_SAVE)
- Sets up special heredoc input mode with echo/canonical disabled
  (TERM_HEREDOC)
- Restores saved terminal settings when needed (TERM_RESTORE)
- Safely checks if settings were previously saved before operations
- Ensures terminal state consistency during command execution

Example:
- manage_terminal_state(vars, TERM_SAVE); == Save original settings
- manage_terminal_state(vars, TERM_HEREDOC); == Set up heredoc mode
- manage_terminal_state(vars, TERM_RESTORE); == Restore original settings
Works with interactive prompt handling and heredoc processing.
*/
void	manage_terminal_state(t_vars *vars, int action)
{
    if (action == TERM_SAVE)
	{
        if (!vars->ori_term_saved)
		{
            tcgetattr(STDIN_FILENO, &vars->ori_term_settings);
            vars->ori_term_saved = 1;
        }
	}  
	else if (action == TERM_HEREDOC)
	{
		term_heredoc(vars);
	}
    else if (action == TERM_RESTORE)
	{
        if (vars->ori_term_saved)
		{
            tcsetattr(STDIN_FILENO, TCSANOW, &vars->ori_term_settings);
            rl_on_new_line();
        }
    }
	return ;
}

/*
Checks for non-whitespace characters after a certain position on a line.
Returns:
- 1 if only whitespace/end-of-string found.
- 0 if found something that's not whitespace and not the end. It's an error.
*/
int check_trailing_chars(const char *line, int start_pos)
{
    int i = start_pos;
    while (line[i] && ft_isspace(line[i])) {
        i++;
    }
    if (line[i] != '\0')
	{
        return (0);
    }
    return (1);
}
