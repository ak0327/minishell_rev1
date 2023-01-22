/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchen <wchen@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 15:03:45 by takira            #+#    #+#             */
/*   Updated: 2023/01/22 20:37:55 by wchen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int command_execution(t_info *info)
{
	int	exit_status;

	//commad_list
	char	*cmds[20] = {"export", "b+=append_abc", NULL};
	exit_status = execute_builtin(info, cmds);
	//ft_execve();
	return (exit_status);
}
