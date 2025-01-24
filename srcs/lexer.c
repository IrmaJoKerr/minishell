/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/01/24 15:49:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


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

void create_args_array(t_node *node, char *token)
{
    char **args;

    args = malloc(sizeof(char *) * 2);
    if (!args)
        return;
    args[0] = ft_strdup(token);
    args[1] = NULL;
    node->args = args;
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

void free_ast(t_node *node)
{
    int i;

    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    
    if (node->args)
    {
        i = 0;
        while (node->args[i])
        {
            free(node->args[i]);
            i++;
        }
        free(node->args);
    }
    if (node->data)
        free(node->data);
    free(node);
}

char *handle_unclosed_quotes(char *input, t_vars *vars)
{
    char    *line;
    char    *temp;
    char    *prompt;
    char    *result;

    if (vars->squoflag)
        prompt = "SQUOTE> ";
    else
        prompt = "DQUOTE> ";
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

void maketoken(char *input, t_vars *vars)
{
    char *token;
    t_node *node;

    token = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!token)
        return;
    node = initnode(vars->curr_type, token);
    if (!node)
    {
        free(token);
        return;
    }
    if (vars->curr_type == TYPE_CMD)
        create_args_array(node, token);
    add_child(vars->root, node);
    free(token);
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

void maketoken(char *input, t_vars *vars)
{
    char    *token;
    t_node  *node;

    if (vars->pos <= vars->start)
        return;
    token = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!token)
        return;
    if (vars->curr_type == TYPE_CMD)
    {
        node = make_cmdnode(token);
        if (!node)
        {
            free(token);
            return;
        }
        vars->current = node;
    }
    else if (vars->curr_type == TYPE_ARGS && vars->current)
        add_argument(vars->current, token);
    free(token);
}

t_tokentype get_operator_type(char c)
{
    if (c == '|')
        return (TYPE_PIPE);
    if (c == '>')
        return (TYPE_OUT_REDIRECT);
    if (c == '<')
        return (TYPE_IN_REDIRECT);
    return (TYPE_STRING);
}

static int handle_string(char *input, int i, int token_start, t_vars *vars)
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

static int process_token(char *input, int i, int *token_start, t_vars *vars)
{
    if (!vars->squoflag && !vars->dquoflag)
    {
        i = operators(input, i, *token_start, vars);
        *token_start = i;
        return (1);
    }
    return (0);
}

void tokenize(char *input, t_vars *vars)
{
    int     i;
    int     token_start;
    char    *new_input;

    i = 0;
    token_start = 0;
    while (input[i])
    {
        handle_quotes(input[i], vars);
        if (process_token(input, i, &token_start, vars))
            continue;
        i++;
    }
    if (vars->squoflag || vars->dquoflag)
    {
        new_input = handle_unclosed_quotes(input, vars);
        if (new_input)
        {
            tokenize(new_input, vars);
            free(input);
            input = new_input;
        }
    }
    maketoken(input, vars);
}

void lexerlist(char *str, t_vars *vars)
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

t_tokentype classify(char *str)
{
	if (ft_isdigit(str))
		return (1);
	else if (str[0] == '-' && str[1] != '\0' && str[1] != '"' && str[1] != '\'')
        return (TYPE_FLAG);
	else if (ft_strchr(str, '|'))
		return (TYPE_PIPE);
	else if (ft_strchr(str, ';'))
		return (TYPE_SEMICOLON);
	else if (ft_strchr(str, '<'))
		return (TYPE_REDIRECTION);
	else if (ft_strchr(str, '>'))
		return (TYPE_APPEND_REDIRECTION);
	else if (ft_strchr(str, '"'))
		return (TYPE_DOUBLE_QUOTE);
	else if (ft_strchr(str, '\''))
		return (TYPE_SINGLE_QUOTE);
	else if (ft_strchr(str, '$'))
		return (TYPE_EXPANSION);
	else if (ft_strchr(str, '\\'))
		return (TYPE_BACKSLASH);
	else if (ft_strchr(str, '?'))
		return (TYPE_EXIT_STATUS);
	else
		return (TYPE_STRING);
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
