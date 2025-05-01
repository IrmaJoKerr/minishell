/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:07 by bleow             #+#    #+#             */
/*   Updated: 2025/05/02 03:17:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Analyses a redirection node during pipe integration.
- Checks if the node is the head of its redirection chain
- Finds the target command for the redirection
- Updates pipe structure to use redirection nodes
Returns:
1 if current node should be processed further
0 if we should skip to the next node
*/
int	chk_redir_nodes(t_vars *vars, t_node *current)
{
	t_node	*chain_head;
	t_node	*target_cmd;

	chain_head = find_redir_chain_head(current, vars->pipes->last_cmd);
	if (chain_head != current)
	{
		return (0);
	}
	target_cmd = get_redir_target(current, vars->pipes->last_cmd);
	if (target_cmd && vars->pipes->pipe_root)
	{
		upd_pipe_redir(vars->pipes->pipe_root, target_cmd, current);
	}
	return (1);
}

/*
Finds the head (first) redirection node in a chain of redirections.
- Scans backward from current node to find first redirection
- All redirections in the chain must target the same command
Returns:
- The first (earliest) redirection node in the chain
- The original node if no earlier nodes found
Works with link_redirs_pipes().
*/
t_node	*find_redir_chain_head(t_node *current, t_node *last_cmd)
{
	t_node	*chain_head;
	t_node	*prev_redir;
	t_node	*current_target;
	t_node	*prev_target;
	int		same_target;

	chain_head = current;
	prev_redir = current->prev;
	current_target = get_redir_target(current, last_cmd);
	while (prev_redir)
	{
		prev_target = get_redir_target(prev_redir, last_cmd);
		same_target = (prev_target == current_target);
		if (!is_redirection(prev_redir->type) || !same_target)
			break ;
		chain_head = prev_redir;
		prev_redir = prev_redir->prev;
	}
	return (chain_head);
}

/*
Integrates redirection nodes with the pipe structure.
- Updates pipe node references to point to redirection nodes.
- Ensures pipes use redirection nodes instead of direct commands.
- Makes pipe commands output to redirections correctly.
Works with proc_redir() when pipe nodes exist.
*/
// void	link_redirs_pipes(t_vars *vars)
// {
// 	t_node	*current;

// 	if (!vars || !vars->pipes || !vars->pipes->pipe_root || !vars->head)
// 		return ;
// 	vars->pipes->last_cmd = NULL;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 		{
// 			vars->pipes->last_cmd = current;
// 		}
// 		else if (is_redirection(current->type) && current->next)
// 		{
// 			if (!chk_redir_nodes(vars, current))
// 			{
// 				current = current->next;
// 				continue ;
// 			}
// 		}
// 		current = current->next;
// 	}
// }
// void	link_redirs_pipes(t_vars *vars)
// {
// 	t_node	*redir_node;
// 	t_node	*current_pipe;
	
//     if (!vars || !vars->pipes || !vars->pipes->pipe_root || !vars->head || !vars->pipes->redir_root)
//         return ;
//     if (vars->pipes->pipe_root->left && vars->pipes->pipe_root->left->type == TYPE_CMD)
//     {
//         redir_node = find_cmd_redirection(vars->pipes->redir_root, 
//                                                 vars->pipes->pipe_root->left, vars);
//         if (redir_node)
//             vars->pipes->pipe_root->left = redir_node;
//     }
//     if (vars->pipes->pipe_root->right)
//     {
//         if (vars->pipes->pipe_root->right->type == TYPE_CMD)
//         {
//             redir_node = find_cmd_redirection(vars->pipes->redir_root, 
//                                                     vars->pipes->pipe_root->right, vars);
//             if (redir_node)
//                 vars->pipes->pipe_root->right = redir_node;
//         }
//         else if (vars->pipes->pipe_root->right->type == TYPE_PIPE)
//         {
//             current_pipe = vars->pipes->pipe_root->right;
//             while (current_pipe)
//             {
//                 if (current_pipe->left && current_pipe->left->type == TYPE_CMD)
//                 {
//                     redir_node = find_cmd_redirection(vars->pipes->redir_root, 
//                                                             current_pipe->left, vars);
//                     if (redir_node)
//                         current_pipe->left = redir_node;
//                 }
//                 if (current_pipe->right && current_pipe->right->type == TYPE_CMD)
//                 {
//                     redir_node = find_cmd_redirection(vars->pipes->redir_root,
//                                                             current_pipe->right, vars);
//                     if (redir_node)
//                         current_pipe->right = redir_node;
//                 }
//                 if (current_pipe->right && current_pipe->right->type == TYPE_PIPE)
//                     current_pipe = current_pipe->right;
//                 else
//                     break ;
//             }
//         }
//     }
// }
/*
 * Process a command node by finding and applying any redirections targeting it
 * Updates the node pointer if a redirection is found
 */
static void process_cmd_node(t_node **node_ptr, t_node *redir_root, t_vars *vars)
{
    t_node *redir_node;
    
    if (*node_ptr && (*node_ptr)->type == TYPE_CMD)
    {
        redir_node = find_cmd_redirection(redir_root, *node_ptr, vars);
        if (redir_node)
            *node_ptr = redir_node;
    }
}

/*
 * Recursively process a pipe node and all its nested pipes
 * Replaces command nodes with their corresponding redirection nodes
 */
static void process_pipe_node(t_node *pipe_node, t_node *redir_root, t_vars *vars)
{
    if (!pipe_node)
        return;
        
    // Process left side (always a command in valid pipes)
    process_cmd_node(&pipe_node->left, redir_root, vars);
    
    // Process right side (can be command or another pipe)
    if (pipe_node->right)
    {
        if (pipe_node->right->type == TYPE_CMD)
        {
            process_cmd_node(&pipe_node->right, redir_root, vars);
        }
        else if (pipe_node->right->type == TYPE_PIPE)
        {
            process_pipe_node(pipe_node->right, redir_root, vars);
        }
    }
}

/*
 * Main function to connect redirections to commands in the pipe structure
 * Identifies commands in the pipe tree and replaces them with redirection nodes
 */
void link_redirs_pipes(t_vars *vars)
{
    // Validation
    if (!vars || !vars->pipes || !vars->pipes->pipe_root || 
        !vars->head || !vars->pipes->redir_root)
        return;
        
    // Process the entire pipe structure recursively
    process_pipe_node(vars->pipes->pipe_root, vars->pipes->redir_root, vars);
}

/*
Configures a redirection node with source and target commands.
- Sets left child to source command node.
- Sets right child to target command/filename node.
- Establishes the redirection relationship in the AST.
Works with proc_redir().
*/
void	set_redir_node(t_node *redir, t_node *cmd, t_node *target)
{
	if (!redir || !cmd || !target)
		return ;
	redir->left = cmd;
	redir->right = target;
}
