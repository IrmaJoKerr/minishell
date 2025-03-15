/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 07:58:59 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:51:16 by bleow            ###   ########.fr       */
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
Sets up signal handlers for terminal control signals using sigaction.
1) Initialize sigaction structs for SIGINT (Ctrl+C) and SIGQUIT (Ctrl+\)
2) Assigns matching handler functions to each signal
3) Sets flags to 0 for standard behavior
4) Clears the signal masks to prevent blocking of other signals
5) Registers the handlers using sigaction() system call
Sigaction() is more modern and portable than signal()
Called during shell startup. Used with init_shell().
*/
void	load_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_memset(&sa_int, 0, sizeof(sa_int));
	ft_memset(&sa_quit, 0, sizeof(sa_quit));
	sa_int.sa_handler = sigint_handler;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sa_quit.sa_handler = sigquit_handler;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/*
Handles Ctrl+C (SIGINT) signal in interactive shell.
1) Writes a newline to move to a new prompt line
2) Uses readline utilities to:
   - Tell readline that user is on a new line
   - Clear current input line with empty string
   - Redisplay the prompt with cleared input
3) Sets g_signal_received to indicate interrupt occurred
This on-screen cancels the current input and shows a fresh
prompt when Ctrl+C is pressed.
*/
void	sigint_handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*
Handles Ctrl+\ (SIGQUIT) signal in interactive shell.
1) Uses readline utilities to:
   - Tell readline that user is on a new line
   - Redisplay the prompt with cleared input
This custom Ctrl+\ doesn't terminate the process with a core dump.
It does nothing else.
*/
void	sigquit_handler(int sig)
{
	(void)sig;
	rl_on_new_line();
	rl_redisplay();
}
