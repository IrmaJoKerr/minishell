/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 04:31:10 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
// int	handle_heredoc(t_node *node, t_vars *vars)
// {
// 	if (!process_heredoc(node, vars))
// 	{
// 		return (0);
// 	}
// 	if (vars->pipes->heredoc_fd < 0)
// 	{
// 		return (0);
// 	}
// 	if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1)
// 	{
// 		close(vars->pipes->heredoc_fd);
// 		vars->pipes->heredoc_fd = -1;
// 		vars->error_code = ERR_DEFAULT;
// 		return (0);
// 	}
// 	close(vars->pipes->heredoc_fd);
// 	vars->pipes->heredoc_fd = -1;
// 	return (1);
// }
int	handle_heredoc(t_node *node, t_vars *vars)
{
    fprintf(stderr, "DEBUG-HEREDOC: Setting up heredoc redirection\n");
    
    if (!process_heredoc(node, vars))
    {
        fprintf(stderr, "DEBUG-HEREDOC: Failed to process heredoc content\n");
        fprintf(stderr, "DEBUG-HEREDOC: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    if (vars->pipes->heredoc_fd < 0)
    {
        fprintf(stderr, "DEBUG-HEREDOC: Invalid heredoc file descriptor\n");
        fprintf(stderr, "DEBUG-HEREDOC: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-HEREDOC: Redirecting stdin to heredoc fd %d\n", vars->pipes->heredoc_fd);
    
    if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-HEREDOC: dup2 failed (error: %s)\n", strerror(errno));
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
        vars->error_code = ERR_DEFAULT;
        fprintf(stderr, "DEBUG-HEREDOC: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    close(vars->pipes->heredoc_fd);
    vars->pipes->heredoc_fd = -1;
    fprintf(stderr, "DEBUG-HEREDOC: Successfully set up heredoc redirection\n");
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
// int	process_heredoc(t_node *node, t_vars *vars)
// {
// 	if (!node || !vars || !vars->pipes)
// 	{
// 		vars->error_code = ERR_DEFAULT;
// 		return (0);
// 	}
// 	if (vars->pipes->heredoc_fd >= 0)
// 	{
// 		close(vars->pipes->heredoc_fd);
// 		vars->pipes->heredoc_fd = -1;
// 	}
// 	if (!vars->hd_text_ready)
// 	{
// 		vars->error_code = ERR_DEFAULT;
// 		return (0);
// 	}
// 	if (!read_tmp_buf(vars))
// 	{
// 		return (0);
// 	}
// 	return (1);
// }
int	process_heredoc(t_node *node, t_vars *vars)
{
    fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Processing heredoc content\n");
    
    if (!node || !vars || !vars->pipes)
    {
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Invalid node or vars structure\n");
        vars->error_code = ERR_DEFAULT;
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    if (vars->pipes->heredoc_fd >= 0)
    {
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Closing previous heredoc fd %d\n", vars->pipes->heredoc_fd);
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
    }
    
    if (!vars->hd_text_ready)
    {
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Heredoc content not ready\n");
        vars->error_code = ERR_DEFAULT;
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Opening temporary buffer for heredoc content\n");
    
    if (!read_tmp_buf(vars))
    {
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Failed to read temporary buffer\n");
        fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-PROCESS-HEREDOC: Successfully opened heredoc content with fd %d\n", vars->pipes->heredoc_fd);
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
// int	read_tmp_buf(t_vars *vars)
// {
// 	int	fd;

// 	fd = open(TMP_BUF, O_RDONLY);
// 	if (fd == -1)
// 	{
// 		perror("bleshell: failed to open TMP_BUF file for reading");
// 		vars->error_code = ERR_DEFAULT;
// 		unlink(TMP_BUF);
// 		return (0);
// 	}
// 	vars->pipes->heredoc_fd = fd;
// 	return (1);
// }
int	read_tmp_buf(t_vars *vars)
{
    int	fd;
    
    fprintf(stderr, "DEBUG-TMP-BUF: Opening TMP_BUF for reading\n");
    
    fd = open(TMP_BUF, O_RDONLY);
    if (fd == -1)
    {
        perror("bleshell: failed to open TMP_BUF file for reading");
        fprintf(stderr, "DEBUG-TMP-BUF: Failed to open TMP_BUF (error: %s)\n", strerror(errno));
        vars->error_code = ERR_DEFAULT;
        unlink(TMP_BUF);
        fprintf(stderr, "DEBUG-TMP-BUF: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-TMP-BUF: Successfully opened TMP_BUF with fd %d\n", fd);
    vars->pipes->heredoc_fd = fd;
    return (1);
}
