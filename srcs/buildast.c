/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/04/29 16:34:42 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes the entire token list to build the AST structure.
- Identifies command nodes in the token list.
- Builds pipe structures for commands separated by pipes.
- Processes redirections and integrates with pipe structure.
- Selects the appropriate root node for the final AST.
Example: For input "ls -l | grep a > output.txt":
- Finds "ls" and "grep" commands.
- Creates pipe structure connecting them.
- Attaches ">" redirection to "grep".
- Returns pipe node as root with redirection integrated.

Returns:
- Root node of the constructed AST.
- NULL if invalid syntax or no commands found.
Works with build_and_execute().
*/
t_node	*proc_token_list(t_vars *vars)
{
	if (DEBUG_AST) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Starting proc_token_list with error_code=%d\n", vars->error_code); //DEBUG PRINT
	
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	
	find_cmd(NULL, NULL, FIND_ALL, vars);
	
	if (DEBUG_AST) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Commands found: %d\n", vars->cmd_count); //DEBUG PRINT
		for (int i = 0; i < vars->cmd_count; i++) //DEBUG PRINT
		{ //DEBUG PRINT
			if (vars->cmd_nodes[i]->args) //DEBUG PRINT
				fprintf(stderr, "  Cmd %d: %s\n", i, vars->cmd_nodes[i]->args[0]); //DEBUG PRINT
		} //DEBUG PRINT
	} //DEBUG PRINT
	
	if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
		|| !vars->cmd_nodes[0]->args)
		return (NULL);
	vars->pipes->pipe_root = NULL;
	vars->pipes->redir_root = NULL;
	
	if (DEBUG_AST) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Processing pipes with error_code=%d\n", vars->error_code); //DEBUG PRINT
	vars->pipes->pipe_root = proc_pipes(vars);
	
	if (DEBUG_AST) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Processing redirections with error_code=%d\n", vars->error_code); //DEBUG PRINT
	vars->pipes->redir_root = proc_redir(vars);
	
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] After redirection processing, error_code=%d\n", vars->error_code); //DEBUG PRINT
	
	if (DEBUG_AST) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "[DEBUG] AST roots - Pipe: %p, Redir: %p\n",  //DEBUG PRINT
			   (void*)vars->pipes->pipe_root, (void*)vars->pipes->redir_root); //DEBUG PRINT
	} //DEBUG PRINT
	
	if (vars->pipes->pipe_root)
		return (vars->pipes->pipe_root);
	else if (vars->pipes->redir_root)
		return (vars->pipes->redir_root);
	else if (vars->cmd_count > 0)
		return (vars->cmd_nodes[0]);
	return (NULL);
}

/*
Builds the redirection AST by connecting commands to redirection operators.
- Traverses token list once, tracking commands and redirections.
- Links redirection nodes to their target commands and targets.
- Sets pipes->redir_root to the first valid redirection.
Works with proc_redir for redirection structure building.
*/
void	build_redir_ast(t_vars *vars)
{
	t_node	*current;

	if (DEBUG_REDIR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Starting build_redir_ast with error_code=%d\n", vars->error_code); //DEBUG PRINT

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_CMD)
		{
			if (DEBUG_REDIR && current->args) //DEBUG PRINT
				fprintf(stderr, "[DEBUG] Found command node: %s\n", current->args[0]); //DEBUG PRINT
			vars->pipes->last_cmd = current;
		}
		else if (is_redirection(current->type))
		{
			if (DEBUG_REDIR) //DEBUG PRINT
			{ //DEBUG PRINT
				fprintf(stderr, "[DEBUG] Found redirection node type: %s\n", get_token_str(current->type)); //DEBUG PRINT
				if (current->next && current->next->args) //DEBUG PRINT
					fprintf(stderr, "[DEBUG] Redirection target: %s\n", current->next->args[0]); //DEBUG PRINT
			} //DEBUG PRINT
			
			if (!is_valid_redir_node(current) || !current->next->args
				|| !current->next->args[0])
			{
				if (DEBUG_ERROR) //DEBUG PRINT
					fprintf(stderr, "[DEBUG] Invalid redirection, setting syntax error\n"); //DEBUG PRINT
				tok_syntax_error_msg("newline", vars);
				return ;
			}
			process_redir_node(current, vars);
		}
		current = current->next;
	}
	if (DEBUG_REDIR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Completed build_redir_ast with error_code=%d\n", vars->error_code); //DEBUG PRINT
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
Works with proc_token_list.
*/
t_node	*proc_pipes(t_vars *vars)
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
	process_addon_pipes(vars);
	return (pipe_root);
}

/*
Master redirection processing function.
- Controls the overall redirection handling workflow.
- Identifies commands and redirection operators.
- Builds redirection nodes and links them properly.
- Integrates redirections with pipe structures if present.
Returns:
- Root redirection node for the AST if found.
- NULL if no valid redirections exist in the token stream.
Works with proc_token_list.
*/
t_node	*proc_redir(t_vars *vars)
{
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	reset_redir_tracking(vars->pipes);
	build_redir_ast(vars);
	if (vars->error_code == 2)
	{
		return (NULL);
	}
	if (vars->pipes->pipe_root)
		link_redirs_pipes(vars);
	if (vars->pipes->redir_root)
	{
		vars->astroot = vars->pipes->redir_root;
	}
	return (vars->pipes->redir_root);
}

/*
Master control function for processing an individual redirection node.
- Finds the target command for the redirection
- Links the redirection node to command and target
- Chains with previous redirections
- Updates type-specific redirection trackers
- Sets the redirection root if this is the first redirection
Works with build_redir_ast() during AST construction.
*/
void	process_redir_node(t_node *redir_node, t_vars *vars)
{
	t_node	*cmd;

	if (DEBUG_REDIR) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "In process_redir_node().Processing redirection: %s -> ", //DEBUG PRINT
			get_token_str(redir_node->type)); //DEBUG PRINT
		if (redir_node->next && redir_node->next->args) //DEBUG PRINT
			fprintf(stderr, "target: %s\n", redir_node->next->args[0]); //DEBUG PRINT
		else //DEBUG PRINT
			fprintf(stderr, "target: NULL\n"); //DEBUG PRINT
	} //DEBUG PRINT
	
	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
	
	if (DEBUG_REDIR) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Found target command for redirection: "); //DEBUG PRINT
		if (cmd && cmd->args) //DEBUG PRINT
			fprintf(stderr, "%s\n", cmd->args[0]); //DEBUG PRINT
		else //DEBUG PRINT
			fprintf(stderr, "NULL\n"); //DEBUG PRINT
	} //DEBUG PRINT
	
	if (cmd && redir_node->next)
	{
		if (DEBUG_REDIR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] Setting up redirection links\n"); //DEBUG PRINT
		set_redir_node(redir_node, cmd, redir_node->next);
		link_prev_redirs(redir_node, cmd, vars);
		track_redirs(redir_node, cmd, vars);
		link_in_out_redirs(vars);
		if (!vars->pipes->redir_root)
		{
			if (DEBUG_REDIR) //DEBUG PRINT
				fprintf(stderr, "[DEBUG] Setting first redirection as root\n"); //DEBUG PRINT
			vars->pipes->redir_root = redir_node;
		}
	}
	
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] After process_redir_node, error_code=%d\n", vars->error_code); //DEBUG PRINT
}
