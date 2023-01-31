/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 20:18:47 by takira            #+#    #+#             */
/*   Updated: 2023/01/30 22:58:50 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command_execution.h"

static int	execute_pipeline_iter(t_list_bdi *pipeline_cmds_head, char **envp, t_info *info);
static int	get_last_status_and_wait_children(t_list_bdi *pipeline_cmds_head);

int	execute_pipeline(t_list_bdi *pipeline_cmds_head, t_info *info)
{
	int				exit_status;
	t_command_info	*command_info;
	char			**minishell_envp;
	pid_t			pid;

	if (!pipeline_cmds_head)
		return (PROCESS_ERROR);
	command_info = pipeline_cmds_head->content;
	if (is_single_builtin(pipeline_cmds_head))
		return (execute_builtin(info, command_info->commands));
	minishell_envp = create_minishell_envp(info->envlist_head);
	if (!minishell_envp)
		return (PROCESS_ERROR);
	pid = fork();
	if (pid < 0)
		return (perror_ret_int("fork", PROCESS_ERROR));
	if (is_child_process(pid))
		exit (execute_pipeline_iter(pipeline_cmds_head, minishell_envp, info));
	if (waitpid(pid, &exit_status, 0) < 0)
		return (perror_ret_int("waitpid", PROCESS_ERROR));
	exit_status = WEXITSTATUS(exit_status);
	free_2d_alloc((void **)minishell_envp);
	return (exit_status);
}

static int	execute_pipeline_iter(t_list_bdi *pipeline_cmds_head, char **envp, t_info *info)
{
	int				now_pipefd[2];
	int				prev_pipefd[2];
	t_command_info	*command_info;
	t_list_bdi		*pipeline_cmds_node;
	int				exit_status;

	init_pipefd(prev_pipefd, now_pipefd);
	pipeline_cmds_node = pipeline_cmds_head;
	while (pipeline_cmds_node)
	{
		if (pipe(now_pipefd) < 0)
			return (perror_ret_int("pipe", PROCESS_ERROR));
		command_info = pipeline_cmds_node->content;
		command_info->pid = fork();
		if (command_info->pid < 0)
			return (perror_ret_int("fork", PROCESS_ERROR));
		if (is_child_process(command_info->pid))
		{
			if (dup2_fds(now_pipefd, prev_pipefd, pipeline_cmds_node->next) < 0)
				return (PROCESS_ERROR);
			if (close_fds(now_pipefd, prev_pipefd, pipeline_cmds_node->next) < 0)
				return (PROCESS_ERROR);
			exit (ft_execve(command_info, envp, info));
		}
		if (is_parent_process(command_info->pid))
		{
			if (close_fds(NULL, prev_pipefd, NULL) < 0)
				return (PROCESS_ERROR);
			copy_prevfd_to_newfd(prev_pipefd, now_pipefd);
		}
		pipeline_cmds_node = pipeline_cmds_node->next;
	}
	if (close(now_pipefd[READ]) < 0 || close(now_pipefd[WRITE]) < 0)
		return (perror_ret_int("close", PROCESS_ERROR));
	exit_status = get_last_status_and_wait_children(pipeline_cmds_head);
	return (exit_status);
}

static int	get_last_status_and_wait_children(t_list_bdi *pipeline_cmds_head)
{
	t_list_bdi		*last_node;
	t_list_bdi		*pipeline_cmds_node;
	t_command_info	*command_info;
	int				exit_status;

	/* wait */
	last_node = ft_lstlast_bdi(pipeline_cmds_head);
	command_info = last_node->content;
	waitpid(command_info->pid, &exit_status, 0);
	pipeline_cmds_node = pipeline_cmds_head;
	while (pipeline_cmds_node)
	{
		command_info = pipeline_cmds_node->content;
		waitpid(command_info->pid, NULL, 0);
		pipeline_cmds_node = pipeline_cmds_node->next;
	}
	return (WEXITSTATUS(exit_status));
}