/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_delim.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:52:36 by bleow             #+#    #+#             */
/*   Updated: 2025/05/26 02:38:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

/*
Finds the delimiter token (TYPE_ARGS) following a TYPE_HEREDOC token.
Returns: 
- Pointer to the delimiter token node.
- NULL if not found/valid.
*/
t_node	*find_delim_token(t_node *head)
{
	t_node	*current;
	t_node	*delimiter_node;

	current = head;
	while (current)
	{
		if (current->type == TYPE_HEREDOC)
		{
			delimiter_node = current->next;
			while (delimiter_node && delimiter_node->type != TYPE_ARGS)
			{
				delimiter_node = delimiter_node->next;
			}
			if (delimiter_node && delimiter_node->type == TYPE_ARGS)
				return (delimiter_node);
			else
				return (NULL);
		}
		current = current->next;
	}
	return (NULL);
}

/**
Validates the heredoc delimiter presence and trailing characters
on the first line.
Returns:
- 1 if valid.
- 0 if otherwise (sets vars->error_code on failure).
*/
int	chk_hd_first_line(char *line_start, int len, t_vars *vars)
{
	t_node	*delim_node;
	char	*raw_delim_ptr;
	char	*delim_arg;
	char	original_char;
	int		is_ok;

	delim_node = find_delim_token(vars->head);
	if (!delim_node || !delim_node->args || !delim_node->args[0])
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	delim_arg = delim_node->args[0];
	raw_delim_ptr = chk_raw_delim(line_start, len, delim_arg, vars);
	if (!raw_delim_ptr)
		return (0);
	original_char = line_start[len];
	line_start[len] = '\0';
	is_ok = chk_hd_tail(line_start, raw_delim_ptr, delim_arg, vars);
	line_start[len] = original_char;
	if (!is_ok)
		return (0);
	return (1);
}

/*
Processes a heredoc line: 
- Checks if it matches delimiter. If match, it will return 1 and exit.
- Else it writes the line to file.
Returns:
- 1 if delimiter found.
- 0 if processed normally.
- -1 on error.
*/
int	proc_hd_line(int write_fd, char *line, t_vars *vars)
{
	size_t	delim_len;
	int		result;

	delim_len = ft_strlen(vars->pipes->heredoc_delim);
	if (ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0
		&& line[delim_len] == '\0')
	{
		free(line);
		return (1);
	}
	result = write_to_hd(write_fd, line, vars);
	free(line);
	if (!result)
		return (-1);
	return (0);
}

/*
Processes a heredoc delimiter by removing quotes if present.
Modifies the string pointed to by delimiter directly.
*/
// void	strip_outer_quotes(char **delimiter, t_vars *vars)
// {
// 	char	*str;
// 	char	*new_str;
// 	size_t	len;

// 	if (!delimiter || !*delimiter)
// 		return ;
// 	str = *delimiter;
// 	len = ft_strlen(str);
// 	if (len < 2)
// 		return ;
// 	if ((str[0] == '"' && str[len - 1] == '"')
// 		|| (str[0] == '\'' && str[len - 1] == '\''))
// 	{
// 		new_str = ft_substr(str, 1, len - 2);
// 		if (!new_str)
// 		{
// 			vars->error_code = ERR_DEFAULT;
// 			return ;
// 		}
// 		free(*delimiter);
// 		*delimiter = new_str;
// 	}
// }
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
	if ((str[0] == '"' && str[len - 1] == '"') ||
		(str[0] == '\'' && str[len - 1] == '\''))
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
