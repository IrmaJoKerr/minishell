/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 07:58:59 by bleow             #+#    #+#             */
/*   Updated: 2025/03/01 07:59:53 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Handles Ctrl+C (SIGINT)
*/
void sigint_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

/*
Handles Ctrl+\ (SIGQUIT)
*/
void sigquit_handler(int sig)
{
    (void)sig;
    rl_on_new_line();
    rl_redisplay();
}
