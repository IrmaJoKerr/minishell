/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_exp_token.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 20:25:29 by bleow             #+#    #+#             */
/*   Updated: 2025/04/15 09:08:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Extract the variable name from an expansion token.
Updates token and var_name by pointer with the extracted variable name.
Helper function for get_var_token.
Example:
For input "$HOME/dir", it sets:
- token = "$HOME"
- var_name = "HOME"
- vars->pos points to the '/' after "HOME"
Returns:
- 1 on success.
- 0 on failure.
*/
int	get_expn_name(char *input, t_vars *vars, char **token ,char **var_name)
{
	char	*tokstr;
	char	*var_str;
	int		start;
	
	start = vars->pos + 1;
	vars->pos++;
	while (input[vars->pos] && (ft_isalnum(input[vars->pos])
		||input[vars->pos] == '_'))
		vars->pos++;
	tokstr = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!tokstr)
		return (0);
	var_str = ft_substr(input, start, vars->pos - start);
	if (!var_str)
	{
		free(tokstr);
		return (0);
	}
	*token = tokstr;
	*var_name = var_str;
	return (1);
}

/*
Extracts a variable token from the input string.
Updates token and var_name by pointer with the extracted variable name.
Handles "$?" as a special case.
Example:
- If input is "$VAR", it extracts "VAR".
- If input is "$?", it extracts "?".
Returns: 
- 1 on success.
- 0 on failure.
*/
int	get_var_token(char *input, t_vars *vars, char **token, char **var_name)
{
	if (input[vars->pos] == '$' && input[vars->pos + 1] == '?')
	{
		*token = ft_substr(input, vars->start, 2);
		if (!*token)
			return (0);
		*var_name = ft_strdup("?");
		if (!*var_name)
		{
			free(*token);
			*token = NULL;
			return (0);
		}
		vars->pos += 2;
		fprintf(stderr, "DEBUG[extract_variable]: Extracted exit status '$?', new pos=%d\n", vars->pos);
	}
	else if (input[vars->pos] == '$')
	{
		if (!get_expn_name(input, vars, token, var_name))
			return (0);
	}
	else
		return (0);
	return (1);
}

/*
Adds quote types to an int array for newly joined characters.
Copies existing quote types and appends 0s for expanded portion.
Adds a sentinel -1 at end of array.
Returns: 
- The new length of the quote type array.
*/
int	addon_quo_type_arr(int *dest, int *src, int new_len)
{
	int	j;
	int	end_pos;
	
	j = 0;
	while (src[j] != -1)
	{
		dest[j] = src[j];
		j++;
	}
	end_pos = j + new_len;
	while (j < end_pos)
	{
		dest[j] = 0;
		j++;
	}
	dest[j] = -1;
	return j;
}

/*
Main processing function for expanded token.
Handles adjacency rules.
Handles joining, creating new tokens, and processing right adjacency
Returns:
- 1 on success
- 0 on failure
*/
int	sub_make_exp_token(char *input, t_vars *vars, char *expanded_val
	,char *token)
{
	if (vars->adj_state[0] && vars->current && 
		(vars->current->type == TYPE_CMD || vars->current->type == TYPE_ARGS))
	{
		if (handle_tok_join(input, vars, expanded_val, token))
		{
			process_adj(NULL, vars);
			return (1);
		}
	}
	if (!new_exp_token(vars, expanded_val, token))
	{
		free(expanded_val);
		free(token);
		return (0);
	}
	if (vars->adj_state[1] && vars->current)
		process_right_adj(input, vars);
	process_adj(NULL, vars);
	return (1);
}

/*
Creates and processes an expansion token ($VAR or $?)
Returns:
- 1 if successful.
- 0 on failure.
*/
int	make_exp_token(char *input, t_vars *vars)
{
	char	*token;
	char	*expanded_val;
	char	*var_name;
	int		result;
	
	token = NULL;
	expanded_val = NULL;
	var_name = NULL;
	vars->start = vars->pos;
	check_token_adj(input, vars);
	fprintf(stderr, "DEBUG[make_exp_token]: At pos=%d, char='%c', input='%.10s...', adj_left=%d, adj_right=%d\n",
			vars->pos, input[vars->pos], input + vars->pos, vars->adj_state[0], vars->adj_state[1]);
	if (!get_var_token(input, vars, &token, &var_name))
		return (0);
	expanded_val = expand_value(var_name, vars);
	free(var_name);
	if (!expanded_val)
	{
		free(token);
		return (0);
	}
	fprintf(stderr, "DEBUG[make_exp_token]: Expanded '%s' to '%s', adj_left=%d, adj_right=%d\n", 
			token, expanded_val, vars->adj_state[0], vars->adj_state[1]);
	result = sub_make_exp_token(input, vars, expanded_val, token);
	return (result);
}
