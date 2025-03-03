/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:47:09 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:37:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handle memory allocation errors. Free all allocated memory and
exit with error code 1.
*/
void	ft_error(t_vars *vars)
{
	char	*error_msg;

	if (vars->error_msg)
		free(vars->error_msg);
	vars->error_msg = ft_strdup("Error: Memory allocation failed");
	if (!vars->error_msg)
	{
		error_msg = "Error: Memory allocation failed\n";
		write(2, error_msg, ft_strlen(error_msg));
		exit(1);
	}
	vars->error_code = 1;
	cleanup_vars(vars);
	fprintf(stderr, "%s\n", vars->error_msg);
	exit(vars->error_code);
}

/*
Handle error messages for redirection
*/
void	redirect_error(char *filename)
{
	ft_putstr_fd("bleshell: redirect file permission denied: ", 2);
	ft_putendl_fd(filename, 2);
}
