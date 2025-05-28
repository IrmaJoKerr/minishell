/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redir_node.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:25:08 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 02:55:30 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Finds and links previous redirections targeting the same command.
- Scans backward through token list
- Links earlier redirections to this one if they target the same command
Works with process_redir_node().
*/
void link_cmd_redirs(t_node *cmd_node, t_vars *vars)
{
	t_node *current;
	t_node *first_redir = NULL;
	t_node *prev_redir = NULL;
	
	fprintf(stderr, "DEBUG-FIX-REDIR: Starting to link redirections for command '%s'\n", 
			cmd_node->args ? cmd_node->args[0] : "NULL");
	
	current = cmd_node;
	
	// Traverse forward from command
	while (current && current->next) {
		current = current->next;
		
		// Stop at pipe or next command (boundary check)
		if (current->type == TYPE_PIPE || current->type == TYPE_CMD)
			break;
		
		// Process redirections
		if (is_redirection(current->type)) {
			fprintf(stderr, "DEBUG-FIX-REDIR: Found redirection %s '%s' for command '%s'\n",
					get_token_str(current->type),
					current->args ? current->args[0] : "NULL",
					cmd_node->args ? cmd_node->args[0] : "NULL");
			
			// CRITICAL FIX #1: Use redir field ONLY to point to target command
			current->redir = cmd_node;
			current->left = cmd_node;
			
			// Update redirection tracking based on type
			if (current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC)
				vars->pipes->last_in_redir = current;
			else if (current->type == TYPE_OUT_REDIRECT || current->type == TYPE_APPEND_REDIRECT)
				vars->pipes->last_out_redir = current;
			
			// First redirection becomes the command's redir pointer
			if (!first_redir) {
				first_redir = current;
				cmd_node->redir = first_redir;
				fprintf(stderr, "DEBUG-FIX-REDIR: Set first redirection for command '%s'\n",
						cmd_node->args ? cmd_node->args[0] : "NULL");
			}
			
			// CRITICAL FIX #2: Chain redirections using next_redir field ONLY
			if (prev_redir) {
				prev_redir->next_redir = current;
				fprintf(stderr, "DEBUG-FIX-REDIR: Linked %s '%s' -> %s '%s'\n",
						get_token_str(prev_redir->type),
						prev_redir->args ? prev_redir->args[0] : "NULL",
						get_token_str(current->type),
						current->args ? current->args[0] : "NULL");
			}
			
			// This redirection becomes the previous for the next one
			prev_redir = current;
			// CRITICAL FIX #3: Always explicitly terminate the chain
			current->next_redir = NULL;
		}
	}
	
	// Print the final redirection chain for debugging
	if (first_redir) {
		fprintf(stderr, "DEBUG-FIX-REDIR: Final redirection chain for cmd '%s':\n",
				cmd_node->args ? cmd_node->args[0] : "NULL");
		
		current = first_redir;
		int chain_pos = 0;
		while (current) {
			fprintf(stderr, "  %d: %s '%s' -> %s\n",
					chain_pos++,
					get_token_str(current->type),
					current->args ? current->args[0] : "NULL",
					current->next_redir ? 
						(current->next_redir->args ? current->next_redir->args[0] : "NULL") : 
						"END");
			
			current = current->next_redir;
		}
	} else {
		fprintf(stderr, "DEBUG-FIX-REDIR: No redirections found for cmd '%s'\n",
				cmd_node->args ? cmd_node->args[0] : "NULL");
	}
}
/*
Updates redirection type trackers and chains with existing redirections.
- Updates last_in_redir or last_out_redir based on type
- Chains with existing redirections targeting same command
Works with process_redir_node().
*/
void	track_redirs(t_node *redir_node, t_node *cmd, t_vars *vars)
{
	if (redir_node->type == TYPE_IN_REDIRECT
		|| redir_node->type == TYPE_HEREDOC)
	{
		if (vars->pipes->last_in_redir
			&& vars->pipes->last_in_redir != redir_node
			&& get_redir_target(vars->pipes->last_in_redir
				, vars->pipes->last_cmd) == cmd)
		{
			vars->pipes->last_in_redir->redir = redir_node;
		}
		vars->pipes->last_in_redir = redir_node;
	}
	else if (redir_node->type == TYPE_OUT_REDIRECT
		|| redir_node->type == TYPE_APPEND_REDIRECT)
	{
		if (vars->pipes->last_out_redir
			&& vars->pipes->last_out_redir != redir_node
			&& get_redir_target(vars->pipes->last_out_redir
				, vars->pipes->last_cmd) == cmd)
		{
			vars->pipes->last_out_redir->redir = redir_node;
		}
		vars->pipes->last_out_redir = redir_node;
	}
}

/*
Links input and output redirection nodes targeting the same command.
- If input and output redirections target the same command, links them
- Creates a redirection chain from input to output
Works with process_redir_node().
*/
void	link_in_out_redirs(t_vars *vars)
{
	t_node	*in_target;
	t_node	*out_target;
	t_node	*last_in;
	t_node	*last_out;
	t_node	*last_cmd;

	last_in = vars->pipes->last_in_redir;
	last_out = vars->pipes->last_out_redir;
	last_cmd = vars->pipes->last_cmd;
	if (last_in && last_out)
	{
		in_target = get_redir_target(last_in, last_cmd);
		out_target = get_redir_target(last_out, last_cmd);
		if (in_target == out_target && !last_in->redir)
		{
			last_in->redir = last_out;
		}
	}
}

/*
Retrieves the target command for a redirection node.
- Checks if the previous node is a command.
- If not, uses the last command seen in the pipe structure.
- Ensures the target command is valid.
Returns:
- Pointer to the target command node.
- NULL if no valid target found.
*/
t_node	*get_redir_target(t_node *current, t_node *last_cmd)
{
	t_node	*target;

	target = NULL;
	if (current->prev && current->prev->type == TYPE_CMD)
		target = current->prev;
	else
		target = last_cmd;
	return (target);
}

/*
Updates pipe structure when commands are redirected.
- Traverses pipe chain looking for references to the command.
- Replaces command references with redirection node references.
- Preserves pipe structure while incorporating redirections.
- Handles both left and right side command replacements.
*/
void	upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
{
	t_node	*pipe_node;

	if (!pipe_root || !cmd || !redir)
		return ;
	pipe_node = pipe_root;
	while (pipe_node)
	{
		if (pipe_node->left == cmd)
		{
			pipe_node->left = redir;
		}
		else if (pipe_node->right == cmd)
		{
			pipe_node->right = redir;
		}
		if (pipe_node->right && pipe_node->right->type == TYPE_PIPE)
			pipe_node = pipe_node->right;
		else
			break ;
	}
}
