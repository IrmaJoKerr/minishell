/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_delim_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:06:08 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 07:42:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks if the original delimiter is a valid quoted delimiter
("" or "..." or '...').
If valid, allocates and sets clean_delim and sets quoted.
Returns:
- 1 if a valid quoted delimiter was processed.
- 0 if otherwise.
*/
// int	chk_quoted_delim(char *orig_delim, size_t len,
// 	char **clean_delim_ptr, int *quoted_ptr)
// {
// 	if (len == 2 && orig_delim[0] == '\"' && orig_delim[1] == '\"')
// 	{
// 		*clean_delim_ptr = ft_strdup("");
// 		*quoted_ptr = 1;
// 		return (1);
// 	}
// 	else if (len >= 2 && orig_delim[0] == '\"'
// 		&& orig_delim[len - 1] == '\"')
// 	{
// 		*clean_delim_ptr = ft_substr(orig_delim, 1, len - 2);
// 		*quoted_ptr = 1;
// 		return (1);
// 	}
// 	else if (len >= 2 && orig_delim[0] == '\''
// 		&& orig_delim[len - 1] == '\'')
// 	{
// 		*clean_delim_ptr = ft_substr(orig_delim, 1, len - 2);
// 		*quoted_ptr = 1;
// 		return (1);
// 	}
// 	if (*quoted_ptr == 1 && !*clean_delim_ptr)
// 	{
// 		return (0);
// 	}
// 	return (0);
// }
int chk_quoted_delim(char *orig_delim, size_t len,
    char **clean_delim_ptr, int *quoted_ptr)
{
    // Empty quoted delimiter
    if (len == 2 && (
        (orig_delim[0] == '\"' && orig_delim[1] == '\"') ||
        (orig_delim[0] == '\'' && orig_delim[1] == '\'')))
    {
        *clean_delim_ptr = ft_strdup("");
        *quoted_ptr = 1;
        return (1);
    }
    // Double-quoted delimiter
    else if (len >= 2 && orig_delim[0] == '\"' && orig_delim[len - 1] == '\"')
    {
        *clean_delim_ptr = ft_substr(orig_delim, 1, len - 2);
        *quoted_ptr = 1;
        return (1);
    }
    // Single-quoted delimiter
    else if (len >= 2 && orig_delim[0] == '\'' && orig_delim[len - 1] == '\'')
    {
        *clean_delim_ptr = ft_substr(orig_delim, 1, len - 2);
        *quoted_ptr = 1;
        return (1);
    }
    
    // No matching quotes or error
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
// void	store_cln_delim(t_vars *vars, char *clean_delim, int quoted)
// {
// 	t_node *cmd = vars->pipes->cmd_redir; // DEBUG

// 	if (cmd && cmd->args) // DEBUG
//         debug_args_before_after(cmd, clean_delim, 1); //DEBUG
// 	if (vars->pipes->heredoc_delim)
// 		free(vars->pipes->heredoc_delim);
// 	vars->pipes->heredoc_delim = clean_delim;
// 	vars->pipes->hd_expand = !quoted;
// }
// void store_cln_delim(t_vars *vars, char *clean_delim, int quoted)
// {
//     fprintf(stderr, "DEBUG-HEREDOC-DELIM: Storing delimiter '%s' (quoted: %d)\n", 
//             clean_delim, quoted);
            
//     if (vars->pipes->heredoc_delim)
//     {
//         fprintf(stderr, "DEBUG-HEREDOC-DELIM: Replacing old delimiter '%s'\n", 
//                 vars->pipes->heredoc_delim);
//         free(vars->pipes->heredoc_delim);
//     }
    
//     vars->pipes->heredoc_delim = clean_delim;
//     vars->pipes->hd_expand = !quoted;
    
//     fprintf(stderr, "DEBUG-HEREDOC-DELIM: New delimiter set to '%s', expansion: %s\n", 
//             clean_delim, !quoted ? "ON" : "OFF");
// }
void store_cln_delim(t_vars *vars, char *clean_delim, int quoted)
{
    fprintf(stderr, "DEBUG-HEREDOC-DELIM: Storing delimiter '%s' (quoted: %d)\n", 
            clean_delim, quoted);
            
    if (vars->pipes->heredoc_delim)
    {
        fprintf(stderr, "DEBUG-HEREDOC-DELIM: Replacing old delimiter '%s'\n", 
                vars->pipes->heredoc_delim);
        free(vars->pipes->heredoc_delim);
    }
    
    vars->pipes->heredoc_delim = clean_delim;
    vars->pipes->hd_expand = !quoted;
    
    fprintf(stderr, "DEBUG-HEREDOC-DELIM: New delimiter set to '%s', expansion: %s\n", 
            clean_delim, !quoted ? "ON" : "OFF");
    
    // Print special characters in delimiter for debugging
    fprintf(stderr, "DEBUG-HEREDOC-DELIM: Hex dump of delimiter: ");
    for (int i = 0; clean_delim[i]; i++) {
        fprintf(stderr, "%02x ", (unsigned char)clean_delim[i]);
    }
    fprintf(stderr, "\n");
}

/*
Finds the raw delimiter string within the first line, after the '<<'.
Returns:
- Pointer to the start of the raw delimiter in the line.
- NULL if error.
*/
char	*find_raw_delim(char *line_start, int len, const char *delim)
{
	char	*hd_operator_ptr;
	char	*raw_ptr;
	int		offset;
	int		rem_len;

	hd_operator_ptr = ft_strnstr(line_start, "<<", len);
	if (!hd_operator_ptr)
		return (NULL);
	offset = (hd_operator_ptr - line_start) + 2;
	rem_len = len - offset;
	if (rem_len <= 0)
		return (NULL);
	raw_ptr = ft_strnstr(line_start + offset, delim, rem_len);
	return (raw_ptr);
}
