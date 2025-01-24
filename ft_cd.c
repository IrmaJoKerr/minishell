#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int    ft_cd(char **p)
{
	char	cwd[1024];
	int ret;

	ret = 0;
	//printf("pwd: [%s]\n", getcwd(cwd, sizeof(cwd)));
	if (p[1] == NULL)
		ret = chdir(getenv("HOME"));
	else if (p[2])
	{
		printf("cd: too many arguments\n");
		ret = 1;
	}
	else if (p[1][0] == '-' && p[1][1] == '\0')
		ret = chdir(getenv("OLDPWD"));
	else
	{
		ret = chdir(p[1]);
		if ( ret < 0)
			printf("cd: no such file or directory: %s\n", p[1]);
	}
	if (ret == 0)
		ft_export(OLDPWD=PWD);
		PWD==????;;;
	// printf("pwd: [%s]\n", getcwd(cwd, sizeof(cwd)));
	// printf("ret: [%d]\n", ret);
	return (ret);
}

int main (int ac, char **av)
{
	ft_cd(av);
	return (0);
}