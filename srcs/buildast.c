/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 16:11:05 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes the entire token list to build the AST structure.
- Identifies command nodes in the token list.
- Builds pipe structures for commands separated by pipes.
- Processes redirections and integrates with command/pipe structure.
- Sets the appropriate root node for the final AST.

Returns:
- Root node of the constructed AST.
- NULL if invalid syntax or no commands found.
Works with build_and_execute().
*/
t_node	*ast_builder(t_vars *vars)
{
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	find_cmd(NULL, NULL, FIND_ALL, vars);
	if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
		|| !vars->cmd_nodes[0]->args)
		return (NULL);
	vars->pipes->pipe_root = proc_ast_pipes(vars);
	vars->pipes->redir_root = proc_ast_redir(vars);
	chk_args_match_cmd(vars);
	if (vars->pipes->pipe_root)
		return (vars->pipes->pipe_root);
	else if (vars->pipes->redir_root)
		return (vars->pipes->redir_root);
	else if (vars->cmd_count > 0)
		return (vars->cmd_nodes[0]);
	return (NULL);
}

/*
Master redirection node preprocessor for AST.
- Handles solo redirections by immediate execution and marking as TYPE_NULL.
- Validates remaining redirection syntax and reports syntax errors.
- Sets the first valid redirection as the redirection root.
- Links commands to their associated redirections for AST construction.
Example: For "echo hi | >./outfile echo bye > output.txt":
- Processes ">./outfile" as solo redirection (executes immediately).
- Validates "> output.txt" redirection syntax.
- Sets "> output.txt" as redirection root.
- Links "echo bye" command with "> output.txt" redirection.

Works with proc_ast_redir() as part of the redirection preprocessing pipeline.
Called before main AST construction to prepare redirection structures.
*/
void	pre_ast_redir_proc(t_vars *vars)
{
	t_node	*current;
	t_node	*first_redir;
	t_node	*prev_redir;

	if (!proc_solo_redirs(vars))
		return ;
	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (!is_valid_redir_node(current))
			{
				tok_syntax_error_msg("newline", vars);
				return ;
			}
			if (!vars->pipes->redir_root)
				vars->pipes->redir_root = current;
		}
		else if (current->type == TYPE_CMD)
			make_cmd_redir_chain(current, vars, &first_redir, &prev_redir);
		current = current->next;
	}
}

/*
Processes solo redirections in pipeline contexts.
- Looks for redirections between pipes and commands (e.g., "| >file cmd").
- Executes solo redirections to create/truncate target files.
- Marks processed redirection nodes as TYPE_NULL so not in AST.
- Handles both output (>) and append (>>) redirections.
- Continues processing the pipeline normally after redirection.

Example: For "echo hi | >./outfile echo bye":
- Detects ">./outfile" as solo (between | and echo).
- Executes redirection to create/truncate "./outfile".
- Marks redirection node as TYPE_NULL.
- Continues with normal pipeline: "echo hi | echo bye".

Returns:
- 1 on success.
- 0 on failure.
Works with pre_ast_redir_proc() and exec_solo_redir().
*/
int	proc_solo_redirs(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (current->prev && current->prev->type == TYPE_PIPE
				&& current->next && current->next->type == TYPE_CMD)
			{
				if (!exec_solo_redir(current, vars))
					return (0);
				current->type = TYPE_NULL;
			}
		}
		current = current->next;
	}
	return (1);
}

/*
Master function for pipe node processing in token list.
- Initializes pipe tracking structures.
- Identifies and processes the first pipe.
- Processes any additional pipes in sequence.
- Builds a complete pipe chain structure.
Returns:
Root pipe node for AST if pipes found.
NULL if no valid pipes in token stream.
Works with ast_builder.
*/
t_node	*proc_ast_pipes(t_vars *vars)
{
	t_node	*pipe_root;

	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	vars->pipes->pipe_root = NULL;
	vars->pipes->last_pipe = NULL;
	vars->pipes->last_cmd = NULL;
	pipe_root = process_first_pipe(vars);
	if (!pipe_root)
		return (NULL);
	vars->pipes->pipe_root = pipe_root;
	if (vars->pipes->pipe_root)
	{
		vars->pipes->in_pipe = 1;
		process_addon_pipes(vars);
	}
	return (pipe_root);
}

/*
Master redirection processing function with pipe linking.
- Controls the overall redirection handling workflow.
- Identifies commands and redirection operators.
- Builds redirection nodes and links them properly.
- Integrates redirections with pipe structures if present.
- Performs command-redirection swapping in pipe nodes directly.

Process flow:
- Resets redirection tracking structures.
- Identifies all commands in the token stream.
- Preprocesses redirections (handles orphaned, validates, links).
- If pipes exist, integrates redirections with pipe structure.

Returns:
- Root redirection node for the AST if found.
- NULL if no valid redirections exist in the token stream.

Works with ast_builder() for complete AST construction.
*/
t_node	*proc_ast_redir(t_vars *vars)
{
	if (!vars || !vars->head)
		return (NULL);
	reset_redir_tracking(vars->pipes);
	pre_ast_redir_proc(vars);
	if (vars->pipes && vars->pipes->pipe_root && vars->pipes->redir_root)
	{
		link_redir_to_cmd_node(&(vars->pipes->pipe_root->left), vars);
		if (vars->pipes->pipe_root->right)
		{
			if (vars->pipes->pipe_root->right->type == TYPE_CMD)
				link_redir_to_cmd_node(&(vars->pipes->pipe_root->right), vars);
			else if (vars->pipes->pipe_root->right->type == TYPE_PIPE)
				proc_pipe_chain(vars->pipes->pipe_root->right, vars);
		}
	}
	return (vars->pipes->redir_root);
}
