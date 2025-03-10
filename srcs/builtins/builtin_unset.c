#include "../includes/minishell.h"

/*
Find the position of an environment variable in the environment array.
Returns the position if found, or the end of array if not found.
*/
int get_env_pos(char *var, char **env)
{
    int i;
    int j;
    int len;

    i = 0;
    len = ft_strlen(var);
    while (env[i])
    {
        j = 0;
        while (env[i][j] == var[j] && j < len)
            j++;
        if (j == len && env[i][j] == '=')  // Added check for '='
            return (i);
        i++;
    }
    return (i);
}

/*
Reallocate environment array until the variable to modify.
Changes: -1 for removal, +1 for addition.
*/
char **realloc_until_var(int changes, char **env, char *var, int count, int pos)
{
    char **new_env;
    int i;

    i = 0;
    new_env = (char **)malloc((count + changes + 1) * sizeof(char *));
    while (i < pos)
    {
        new_env[i] = env[i];
        i++;
    }
    if (changes == -1)
    {
        free(env[i]);
        i++;  // Skip the removed variable
    }
    else if (changes == 1)
        new_env[i++] = ft_strdup(var);
    
    // Copy remaining variables
    while (i - changes < count)
    {
        new_env[i] = env[i - changes];
        i++;
    }
    new_env[i] = NULL;  // Null-terminate the array
    return (new_env);
}

/*
Modify the environment by adding or removing a variable.
Changes: -1 for removal, +1 for addition.
*/
void modify_env(char ***env, int changes, char *var)
{
    char **new_env;
    int count;
    int pos;

    count = 0;
    while ((*env)[count])
        count++;
    pos = get_env_pos(var, *env);
    if ((changes == -1 && pos == count) || 
        (changes == 1 && pos < count && (*env)[pos][ft_strlen(var)] == '='))
        return;  // Nothing to remove or variable already exists
        
    new_env = realloc_until_var(changes, *env, var, count, pos);
    free(*env);
    *env = new_env;
}

/*
Builtin unset command - removes variables from environment.
*/
int builtin_unset(char **args, t_vars *vars)
{
    int i;

    if (!args || !args[1])
        return (0);  // Nothing to unset
    
    i = 1;  // Skip command name
    while (args[i])
    {
        if (get_env_value(args[i], vars->env))
            modify_env(&vars->env, -1, args[i]);
        i++;
    }
    return (0);
}
