/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/01 08:14:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

char	*get_cmd_path(char *cmd, char **envp)
{
    char	**paths;
    char	*path;
    size_t	i;
    char	*part_path;

    i = 0;
    while (!ft_strnstr(envp[i], "PATH", 4) && envp[i])
        i++;
    if (!envp[i])
        return (NULL);
    paths = ft_split(envp[i] + 5, ':');
    i = 0;
    while (paths && paths[i])
    {
        part_path = ft_strjoin(paths[i], "/");
        path = ft_strjoin(part_path, cmd);
        free(part_path);
        if (access(path, F_OK) == 0)
        {
            ft_free_2d(paths, ft_arrlen(paths));
            return (path);
        }
        free(path);
        i++;
    }
    ft_free_2d(paths, ft_arrlen(paths));
    return (NULL);
}

char	**dup_env(char **envp)
{
    char	**env;
    size_t	env_size;

    env_size = ft_arrlen(envp);
    env = malloc(sizeof(char *) * (env_size + 1));
    if (!env)
        return (NULL);
    while (env_size--)
    {
        env[env_size] = ft_strdup(envp[env_size]);
        if (!env[env_size])
        {
            ft_free_2d(env, env_size);
            return (NULL);
        }
    }
    env[ft_arrlen(envp)] = NULL;
    return (env);
}

int	execute_cmd(t_node *cmd_node, char **envp)
{
    char	*cmd_path;
    pid_t	pid;
    int		status;

    cmd_path = get_cmd_path(cmd_node->args[0], envp);
    if (!cmd_path)
        return (1);
    pid = fork();
    if (pid == 0)
    {
        if (execve(cmd_path, cmd_node->args, envp) == -1)
        {
            free(cmd_path);
            exit(1);
        }
    }
    free(cmd_path);
    waitpid(pid, &status, 0);
    return (WEXITSTATUS(status));
}

void	handle_pipe(t_node *pipe_node, char **envp)
{
    int		pipefd[2];
    pid_t	pid;

    if (pipe(pipefd) == -1)
        return ;
    pid = fork();
    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execute_cmd(pipe_node->left, envp);
        exit(0);
    }
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    execute_cmd(pipe_node->right, envp);
}

void	cleanup_vars(t_vars *vars)
{
    if (vars->env)
        ft_free_2d(vars->env, ft_arrlen(vars->env));
    if (vars->error_msg)
        free(vars->error_msg);
    if (vars->astroot)
        free_ast(vars->astroot);
    vars->squoflag = 0;
    vars->dquoflag = 0;
    vars->error_code = 0;
}

t_node *initnode(t_tokentype type, char *data)
{
    t_node *node;

    node = malloc(sizeof(t_node));
    if (!node)
        return (NULL);
    node->type = type;
    node->data = ft_strdup(data);
    node->args = NULL;
    node->left = NULL;
    node->right = NULL;
    return (node);
}

static int	is_quote(char c)
{
    return (c == '\'' || c == '\"');
}

static void	update_quote_state(char c, t_vars *vars)
{
    if (c == '\'' && !vars->dquoflag)
        vars->squoflag = !vars->squoflag;
    else if (c == '\"' && !vars->squoflag)
        vars->dquoflag = !vars->dquoflag;
}

void	create_args_array(t_node *node, char *token)
{
    char	**args;

    args = malloc(sizeof(char *) * 2);
    if (!args)
        return ;
    args[0] = ft_strdup(token);
    if (!args[0])
    {
        free(args);
        return ;
    }
    args[1] = NULL;
    node->args = args;
}

void	append_arg(t_node *node, char *new_arg)
{
    char	**new_args;
    size_t	len;
    size_t	i;

    len = ft_arrlen(node->args);
    new_args = malloc(sizeof(char *) * (len + 2));
    if (!new_args)
        return ;
    i = 0;
    while (i < len)
    {
        new_args[i] = ft_strdup(node->args[i]);
        if (!new_args[i])
        {
            ft_free_2d(new_args, i);
            return ;
        }
        i++;
    }
    new_args[len] = ft_strdup(new_arg);
    new_args[len + 1] = NULL;
    ft_free_2d(node->args, len);
    node->args = new_args;
}

void add_child(t_node *parent, t_node *child)
{
    if (!parent)
        return;
    if (!parent->left)
        parent->left = child;
    else if (!parent->right)
        parent->right = child;
}

static t_node	*handle_cmd_token(char *token)
{
    t_node	*node;

    node = make_cmdnode(token);
    if (!node)
    {
        free(token);
        return (NULL);
    }
    return (node);
}

static t_node	*handle_other_token(char *token, t_tokentype type)
{
    t_node	*node;

    node = initnode(type, token);
    if (!node)
    {
        free(token);
        return (NULL);
    }
    return (node);
}

void	maketoken(char *input, t_vars *vars)
{
    char	*token;
    t_node	*node;

    if (vars->pos <= vars->start)
        return ;
    token = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!token)
        return ;
    if (vars->curr_type == TYPE_CMD)
        node = handle_cmd_token(token);
    else
        node = handle_other_token(token, vars->curr_type);
    if (node && vars->curr_type == TYPE_CMD)
        vars->current = node;
    else if (node)
        add_child(vars->root, node);
    free(token);
}

/*
Function to build AST from tokens
*/
t_node *build_ast(t_vars *vars)
{
    t_node *current = vars->head;
    t_node *pipe_node = NULL;
    
    while (current)
    {
        if (current->type == TYPE_PIPE)
        {
            pipe_node = current;
        }
        current = current->right;
    }
    return (vars->root);
}

char	*handle_unclosed_quotes(char *input, t_vars *vars)
{
    char	*line;
    char	*temp;
    char	*prompt;
    char	*result;

    prompt = "DQUOTE> ";
    if (vars->squoflag)
        prompt = "SQUOTE> ";
    while (vars->squoflag || vars->dquoflag)
    {
        line = readline(prompt);
        if (!line)
            return (NULL);
        temp = input;
        result = ft_strjoin(temp, "\n");
        free(temp);
        temp = result;
        result = ft_strjoin(temp, line);
        free(temp);
        free(line);
        input = result;
        tokenize(input, vars);
    }
    return (input);
}

t_node *make_cmdnode(char *token)
{
    t_node *node;

    node = initnode(TYPE_CMD, token);
    if (!node)
        return (NULL);
    node->args = malloc(sizeof(char *) * 2);
    if (!node->args)
    {
        free(node);
        return (NULL);
    }
    node->args[0] = ft_strdup(token);
    node->args[1] = NULL;
    return (node);
}

static int	handle_string(char *input, int i, int token_start, t_vars *vars)
{
    if ((input[i] == '>' || input[i] == '<' || input[i] == '|')
		&& i > token_start)
    {
        vars->start = token_start;
        vars->pos = i;
        vars->curr_type = TYPE_STRING;
        maketoken(input, vars);
    }
    return (i);
}

static int handle_double_operator(char *input, int i, t_vars *vars)
{
    if (input[i] == '>' && input[i + 1] == '>')
    {
        vars->start = i;
        vars->pos = i + 2;
        vars->curr_type = TYPE_APPEND_REDIRECT;
        maketoken(input, vars);
        return (i + 2);
    }
    if (input[i] == '<' && input[i + 1] == '<')
    {
        vars->start = i;
        vars->pos = i + 2;
        vars->curr_type = TYPE_HEREDOC;
        maketoken(input, vars);
        return (i + 2);
    }
    return (i);
}

static int handle_single_operator(char *input, int i, t_vars *vars)
{
    if (input[i] == '|' || input[i] == '>' || input[i] == '<')
    {
        vars->start = i;
        vars->pos = i + 1;
        vars->curr_type = get_operator_type(input[i]);
        maketoken(input, vars);
        return (i + 1);
    }
    return (i);
}

int operators(char *input, int i, int token_start, t_vars *vars)
{
    int next_pos;

    handle_string(input, i, token_start, vars);
    next_pos = handle_double_operator(input, i, vars);
    if (next_pos != i)
        return (next_pos);
    return (handle_single_operator(input, i, vars));
}

static void handle_quotes(char c, t_vars *vars)
{
    if (c == '\'')
    {
        if (!vars->squoflag)
            vars->squoflag = 1;
        else
            vars->squoflag = 0;
    }
    if (c == '\"')
    {
        if (!vars->dquoflag)
            vars->dquoflag = 1;
        else
            vars->dquoflag = 0;
    }
}

static t_tokentype	handle_redirection(char *str)
{
    if (!str[1])
    {
        if (str[0] == '<')
            return (TYPE_IN_REDIRECT);
        return (TYPE_OUT_REDIRECT);
    }
    if (str[0] == '<' && str[1] == '<')
        return (TYPE_HEREDOC);
    if (str[0] == '>' && str[1] == '>')
        return (TYPE_APPEND_REDIRECT);
    if (str[0] == '<')
        return (TYPE_IN_REDIRECT);
    return (TYPE_OUT_REDIRECT);
}

static t_tokentype	handle_special(char *str)
{
    if (str[0] == '$')
    {
        if (str[1] == '?')
            return (TYPE_EXIT_STATUS);
        return (TYPE_EXPANSION);
    }
    if (str[0] == '"')
        return (TYPE_DOUBLE_QUOTE);
    if (str[0] == '\'')
        return (TYPE_SINGLE_QUOTE);
    return (TYPE_STRING);
}

t_tokentype	classify(char *str)
{
    if (!str || !*str)
        return (TYPE_STRING);
    if (str[0] == '-' && str[1] && str[1] != '"' && str[1] != '\'')
        return (TYPE_ARGS);
    if (str[0] == '|')
        return (TYPE_PIPE);
    if (str[0] == '<' || str[0] == '>')
        return (handle_redirection(str));
    return (handle_special(str));
}

static void	handle_quote_token(char *input, t_vars *vars, int *i)
{
    handle_quotes(input[*i], vars);
    if (!vars->squoflag && !vars->dquoflag)
    {
        *i = operators(input, *i, vars->start, vars);
        vars->start = *i;
        return ;
    }
    (*i)++;
}

static char	*extract_quoted_content(char *input, int *pos, char quote)
{
    int		start;
    char	*content;

    start = *pos + 1;
    *pos = start;
    while (input[*pos] && input[*pos] != quote)
        (*pos)++;
    if (!input[*pos])
        return (NULL);
    content = ft_substr(input, start, *pos - start);
    (*pos)++;
    return (content);
}

static char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
    int		start;
    char	*var_name;
    char	*value;
    char	*result;

    start = *pos + 1;
    *pos = start;
    while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
        (*pos)++;
    var_name = ft_substr(input, start, *pos - start);
    if (!var_name)
        return (NULL);
    value = getenv(var_name);
    free(var_name);
    if (value)
        result = ft_strdup(value);
    else
        result = ft_strdup("");
    return (result);
}

void	tokenize(char *input, t_vars *vars)
{
    int		i;
    char	*content;

    i = 0;
    vars->start = 0;
    while (input[i])
    {
        if (is_quote(input[i]))
        {
            update_quote_state(input[i], vars);
            if (!vars->squoflag && !vars->dquoflag)
            {
                content = extract_quoted_content(input, &i, input[i]);
                if (content)
                {
                    maketoken(content, vars);
                    free(content);
                }
            }
        }
        else if (input[i] == '$' && !vars->squoflag)
            handle_expansion(input, &i, vars);
        else
            i++;
    }
}

void	lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->current = NULL;
    
	vars->current = init_head_node(vars);
    while (str[vars->pos])
    {
        if (str[vars->pos] == ' ' || str[vars->pos] == '\t' || 
            str[vars->pos] == '\n')
            vars->pos++;
        else
        {
            vars->curr_type = classify(str[vars->pos]);
            if (!makenode(vars, str[vars->pos]))
                ft_error(vars);
            vars->pos++;
        }
    }
}

/*
Helper function to create and link a new node.
*/
int makenode(t_vars *vars, char *data)
{
    t_node *newnode;

    newnode = initnode(data, vars->curr_type);
    if (!newnode)
        return (0);
    
    if (vars->current)
    {
        vars->current->next = newnode;
        newnode->prev = vars->current;
    }
    vars->current = newnode;
    return (1);
}

void ft_error(t_vars *vars)
{
    // Free all allocated memory
    if (vars->root)
        free_ast(vars->root);
    if (vars->head)
        free_ast(vars->head);
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
}
