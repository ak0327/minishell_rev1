/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 15:03:45 by takira            #+#    #+#             */
/*   Updated: 2023/01/28 18:18:19 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command_execution.h"

/*
 * pipeline_node
 * pipeline_node &&
 * pipeline_node ||
 *
 * pipelie1 || pipeline2 || .. || pipeline_n; pipeline_n+1 &&
 *  ^ false     ^ not execu        ^not exec   ^ EXEC
 *
 * exec to next ; or NULL
 * if next_operator == &&
 * if next_operator_ == ||
 *
 *
 * bash 0$ echo hello a1=$a1 && export a1=a1dayo && echo hello a1=$a1
 * hello a1=
 * hello a1=a1dayo
 *
 *
 * pipeline_i = cmd_i1 | cmd_i2 | cmd_i3 .. cmd_in
 * 		pipeline_i : t_list_bdi
 * 		cmd_ij : t_command_info
 *
 * */

void	init_pipefd(int prev_pipefd[2], int now_pipefd[2])
{
	if (!prev_pipefd || !now_pipefd)
		return ;
	prev_pipefd[READ] = STDIN_FILENO;
	prev_pipefd[WRITE] = -1;
	now_pipefd[READ] = -1;
	now_pipefd[WRITE] = -1;
}

void	copy_fd_new_to_prev(int prev_pipefd[2], const int now_pipefd[2])
{
	if (!prev_pipefd || !now_pipefd)
		return ;
	prev_pipefd[READ] = now_pipefd[READ];
	prev_pipefd[WRITE] = now_pipefd[WRITE];
}

char **create_minishell_envp(t_info *info)
{
	char	**minishell_envp;


	return (minishell_envp);
}

int	execute_commands(char **commands, char **minishell_envp)
{

}

// pipelien_commands cmd1 | cmd2 | cmd3
int	execute_pipeline(t_list_bdi *pipeline_commands_head, t_info *info)
{
	int				exit_status;
	t_command_info	*command_info;
	t_list_bdi		*pipeline_commands_node;
	t_list_bdi		*last_node;
	int				now_pipefd[2];
	int				prev_pipefd[2];
	char			**minishell_envp;

	if (!pipeline_commands_head)
		return (FAILURE);

	/* vvvvv debug mode: print command_info vvvvv */
	pipeline_commands_node = pipeline_commands_head;
	while (pipeline_commands_node)
	{
		command_info = pipeline_commands_node->content;
		// tmp print
		debug_print_command_info(command_info);
		pipeline_commands_node = pipeline_commands_node->next;
		if (pipeline_commands_node)
			ft_dprintf(STDERR_FILENO, "       v [pipe:|] v\n");
	}
	/* ^^^^^ debug mode: print command_info ^^^^^ */

	/* execute pipeline commands */
	minishell_envp = create_minishell_envp(info);
	if (!minishell_envp)
		return (FAILURE);

	init_pipefd(prev_pipefd, now_pipefd);
	pipeline_commands_node = pipeline_commands_head;
	while (pipeline_commands_node)
	{
		if (pipe(now_pipefd) < 0)
			return (perror_ret_int("pipe", FAILURE));

		command_info = pipeline_commands_node->content;
		command_info->pid = fork();
		if (command_info->pid < 0)
			return (perror_ret_int("fork", FAILURE));

		/* child */
		if (command_info->pid == 0)
			exit (execute_commands(command_info->commands, minishell_envp));

		/* parent */
		if (prev_pipefd[WRITE] != -1)
			if (close(prev_pipefd[READ]) < 0 || close(prev_pipefd[WRITE] < 0))
				return (perror_ret_int("close", FAILURE));
		copy_fd_new_to_prev(prev_pipefd, now_pipefd);

		pipeline_commands_node = pipeline_commands_node->next;
	}

	/* wait */
	last_node = ft_lstlast_bdi(pipeline_commands_head);
	command_info = last_node->content;
	exit_status = waitpid(command_info->pid, &exit_status, 0);

	pipeline_commands_node = pipeline_commands_head;
	while (pipeline_commands_node)
	{
		command_info = pipeline_commands_node->content;
		waitpid(command_info->pid, NULL, 0);
		pipeline_commands_node = pipeline_commands_node->next;
	}

	minishell_envp = (char **)free_2d_alloc((void **)minishell_envp);
	return (exit_status);
}

// operator -> pipeline ->..
void	move_to_next_exec_node(t_exec_list **exec_list, int exit_status)
{
	t_exec_list	*next_operator_node;
	bool		debug = true;

	if (!exec_list || !*exec_list)
		return ;

	next_operator_node = *exec_list;

	/* vvvvv debug mode: execute all node vvvvv */
	if (debug)
	{
		debug_print_exec_nodetype(next_operator_node);
		(*exec_list) = (*exec_list)->next;
		exit_status++;
		return ;
	}
	/* ^^^^^ debug mode: execute all node ^^^^^ */

	if ((next_operator_node->node_kind == e_node_semicolon)
	|| (next_operator_node->node_kind == e_node_and && exit_status == 0)
	|| (next_operator_node->node_kind == e_node_or && exit_status != 0))
	{
		(*exec_list) = (*exec_list)->next;
		return ;
	}
	while (*exec_list && (*exec_list)->node_kind != e_node_semicolon)
		(*exec_list) = (*exec_list)->next;
}

int	execute_execlist(t_info *info)
{
	int			exit_status;
	t_exec_list	*exec_node;
	t_exec_list	*pipeline_node;

	if (!info || !info->execlist_head)
		return (FAILURE);
	exit_status = EXIT_SUCCESS;

	printf("\n***** execute *****\n");
	printf("\nvvvvvvvvvvvvvvvvvvv\n");
	exec_node = info->execlist_head;
	while (exec_node)
	{
		pipeline_node = exec_node;
		/* expansion */
		if (expand_variable(&pipeline_node, info) == FAILURE)
			return (FAILURE);

		/* execution */
		exit_status = execute_pipeline(pipeline_node->pipeline_commands);
		exec_node = exec_node->next;
		move_to_next_exec_node(&exec_node, exit_status);

	}
	printf("^^^^^^^^^^^^^^^^^^^\n");

//	debug_print_exec_list(info->execlist_head, "expansion");

	return (exit_status);
}