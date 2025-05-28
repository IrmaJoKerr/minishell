/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:01 by lechan            #+#    #+#             */
/*   Updated: 2025/05/28 02:44:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Built-in command: pwd. Prints working directory.
 * Returns 0 on success, 1 on failure.
 */
int	builtin_pwd(t_vars *vars)
{
	char	*pwd;
	int		cmdcode;

	cmdcode = 0;
	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		perror("pwd");
		cmdcode = 1;
	}
	else
	{
		ft_putendl_fd(pwd, STDOUT_FILENO);
		free(pwd);
	}
	vars->error_code = cmdcode;
	return (cmdcode);
}
// int builtin_pwd(t_vars *vars)
// {
//     char current_dir[PATH_MAX];

//     fprintf(stderr, "DEBUG-PWD: Executing pwd builtin\n");
    
//     if (getcwd(current_dir, PATH_MAX) == NULL) {
//         // If getcwd fails, try using PWD environment variable
//         char *pwd = get_env_val("PWD", vars->env);
//         fprintf(stderr, "DEBUG-PWD: getcwd failed, using PWD env: %s\n", 
//                 pwd && pwd[0] ? pwd : "NULL");
                
//         if (pwd && pwd[0]) {
//             printf("%s\n", pwd);
//             free(pwd);
//             return 0;
//         } else {
//             if (pwd) free(pwd);
//             perror("pwd: getcwd");
//             return 1;
//         }
//     }
    
//     fprintf(stderr, "DEBUG-PWD: Current directory: %s\n", current_dir);
//     printf("%s\n", current_dir);
//     return 0;
// }
