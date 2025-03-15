/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:26 by lechan            #+#    #+#             */
/*   Updated: 2025/03/14 09:09:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handle directory change based on arguments.
Returns 0 on success, non-zero on failure.
*/
int	handle_cd_path(char **args, t_vars *vars)
{
	char *oldpwd_value;
	
	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')))
	{
		if (chdir(get_env_value("HOME", vars->env)) != 0)
			return (printf("cd: HOME not set or no access\n"), 1);
	}
	else if (args[1][0] == '-' && args[1][1] == '\0')
	{
		oldpwd_value = get_env_value("OLDPWD", vars->env);
		if (chdir(get_env_value("OLDPWD", vars->env)) != 0)
			return (printf("cd: OLDPWD not set or no access\n"), 1);
		printf("%s\n", oldpwd_value); // Print the destination directory
	}
	else if (chdir(args[1]) != 0)
		return (printf("cd: no such file or directory: %s\n", args[1]), 1);
	return (0);
}

/*
Updates PWD and OLDPWD environment variables after directory change.
Returns 0 on success, 1 on failure.
*/
int	update_env_pwd(t_vars *vars, char *oldpwd)
{
	char	cwd[1024];
	char	*tmp;
	
	tmp = ft_strjoin("OLDPWD=", oldpwd);
	if (!tmp)
		return (1);
	modify_env(&vars->env, 1, tmp);
	ft_safefree((void **)&tmp);
	if (!getcwd(cwd, sizeof(cwd)))
		return (printf("cd: error retrieving current directory\n"), 1);
	tmp = ft_strjoin("PWD=", cwd);
	if (!tmp)
		return (1);
	modify_env(&vars->env, 1, tmp);
	ft_safefree((void **)&tmp);
	return (0);
}

/*
Built-in command: cd. Changes the current working directory.
*/
int builtin_cd(char **args, t_vars *vars)
{
	char	*oldpwd;
	int		change_errcode;

	oldpwd = ft_strdup(get_env_value("OLDPWD", vars->env));
	if (!oldpwd)
		return (printf("cd: memory allocation error\n"), 1);
	change_errcode = handle_cd_path(args, vars);
	if (change_errcode != 0)
	{
		ft_safefree((void **)&oldpwd);
		return (change_errcode);
	}
	if (update_env_pwd(vars, oldpwd) != 0)
	{
		ft_safefree((void **)&oldpwd);
		return (1);
	}
	ft_safefree((void **)&oldpwd);
	return (0);
}
