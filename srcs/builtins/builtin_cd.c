/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:26 by lechan            #+#    #+#             */
/*   Updated: 2025/06/01 18:18:10 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: cd. Changes the current working directory.
- Master control function for cd command.
- Handles special directories (home and previous).
- Changes to the directory specified in args[1].
- Updates PWD and OLDPWD environment variables.
- Frees memory allocated for oldpwd.
Returns 0 on success, 1 on failure.
*/
int	builtin_cd(char **args, t_vars *vars)
{
	char	*oldpwd;
	int		cmdcode;

	if (args[1] && args[2])
		return (1);
	oldpwd = NULL;
	oldpwd = get_env_val("OLDPWD", vars->env);
	if (!oldpwd)
		return (1);
	cmdcode = handle_cd_path(args, vars);
	if (cmdcode != 0)
	{
		free(oldpwd);
		return (cmdcode);
	}
	cmdcode = update_env_pwd(vars, oldpwd);
	if (cmdcode != 0)
	{
		free(oldpwd);
		vars->error_code = cmdcode;
		return (1);
	}
	free(oldpwd);
	return (vars->error_code = cmdcode);
}

/*
Handle changing to special directories (home or previous).
- Changes to HOME directory if no arguments or "~" is given.
- Changes to OLDPWD directory if "-" is given.
- Gets the path from the environment variables.
Returns 0 on success, 1 on failure.
*/
int	handle_cd_special(char **args, t_vars *vars, char **path_value)
{
	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')))
	{
		*path_value = get_env_val("HOME", vars->env);
		if (!(*path_value) || chdir(*path_value) != 0)
		{
			printf("cd: HOME not set or no access\n");
			return (1);
		}
		return (0);
	}
	*path_value = get_env_val("OLDPWD", vars->env);
	if (!(*path_value) || chdir(*path_value) != 0)
	{
		printf("cd: OLDPWD not set or no access\n");
		return (1);
	}
	printf("%s\n", *path_value);
	return (0);
}

/*
Handle directory change based on arguments.
- Changes to the directory specified in args[1].
- Prints error message if directory does not exist.
Returns 0 on success, 1 on failure.
*/
int	handle_cd_path(char **args, t_vars *vars)
{
	int		cmdcode;
	char	*path_value;

	path_value = NULL;
	cmdcode = 0;
	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')) ||
		(args[1][0] == '-' && args[1][1] == '\0'))
	{
		cmdcode = handle_cd_special(args, vars, &path_value);
		if (path_value)
			free(path_value);
		return (cmdcode);
	}
	cmdcode = chdir(args[1]);
	if (cmdcode != 0)
	{
		shell_error(args[1], ERR_CMD_NOT_FOUND, vars);
		return (1);
	}
	return (cmdcode);
}

/*
Export a single environment variable using builtin_export.
Combines key and value, builds argv, and calls builtin_export.
Returns:
- 0 on success.
- 1 on allocation failure.
*/
int	export_env_var(t_vars *vars, char *key, char *value)
{
	char	*tmp;
	char	*dummy_argv[3];

	tmp = ft_strjoin(key, value);
	if (!tmp)
		return (1);
	dummy_argv[0] = "export";
	dummy_argv[1] = tmp;
	dummy_argv[2] = NULL;
	builtin_export(dummy_argv, vars);
	free(tmp);
	return (0);
}

/*
Updates PWD and OLDPWD environment variables after directory change.
- Temporarily stores the old working directory in OLDPWD.
- Temporarily buffers the current working directory path in cwd[].
- Updates the PWD environment variable with the new path.
Returns 0 on success, 1 on failure.
*/
int	update_env_pwd(t_vars *vars, char *oldpwd)
{
	char	cwd[1024];
	char	*result;

	result = getcwd(cwd, sizeof(cwd));
	if (result == NULL)
	{
		ft_putstr_fd("cd: error retrieving current directory\n", 2);
		return (1);
	}
	if (export_env_var(vars, "OLDPWD=", oldpwd) != 0)
		return (1);
	if (export_env_var(vars, "PWD=", cwd) != 0)
		return (1);
	return (0);
}
