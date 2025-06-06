/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 07:58:59 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 15:28:13 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Global signal flag variable to indicate when a signal is received.
volatile sig_atomic_t is set to ensure atomic access across signal handlers
and the main program, preventing race conditions.
Used by signal handlers to communicate signal reception to the main program.
*/
volatile sig_atomic_t	g_signal_received = 0;

/*
Handles Ctrl+C (SIGINT) signal in interactive shell.
- Writes a newline to move to a new prompt line
- Uses readline utilities to:
   - Tell readline that user is on a new line.
   - Clear current input line with empty string.
   - Redisplay the prompt with cleared input.
- Sets g_signal_received to indicate interrupt occurred.
This on-screen cancels the current input and shows a fresh
prompt when Ctrl+C is pressed.
*/
void	sigint_handler(int sig)
{
	(void)sig;
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*
Handles Ctrl+C (SIGINT) in heredoc child processes.
- Sets global signal flag to special value (-2)
- Used specifically in forked heredoc processes
- Allows parent process to detect heredoc interruption
- Doesn't manipulate terminal display (handled by parent)
Works with interactive_hd_mode() and process_heredoc().
*/
void	hd_child_sigint_handler(int signo)
{
	(void)signo;
	g_signal_received = -2;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}

/*
Handles Ctrl+C (SIGINT) in main interactive shell.
- Writes a newline for visual feedback
- Uses readline utilities to:
  - Mark current line as completed
  - Clear input line content
  - Redisplay the prompt cleanly
- Sets global signal flag to 130 (standard SIGINT exit code)
Main signal handler for interactive shell operation.
*/
void	signal_handler(int sigint)
{
	(void)sigint;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_signal_received = 130;
}

/*
Sets up signal handlers for the shell.
- Registers signal_handler() for SIGINT (Ctrl+C)
- Ignores SIGQUIT (Ctrl+\) signals
- Ensures consistent signal behavior across shell operation
Called during shell initialization in init_shell().
*/
void	load_signals(void)
{
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
}
