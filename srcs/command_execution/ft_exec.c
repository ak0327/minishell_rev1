/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exec.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 19:34:52 by takira            #+#    #+#             */
/*   Updated: 2023/02/03 18:54:55 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command_execution.h"

static int	ft_execvp(char **commands, char **minishell_envp, t_list *envlist);
static bool	is_path(const char *commands_head);
static char	*get_execute_path(char *path, char *file);

int	ft_execve(t_command_info *command_info, char **minishell_envp, t_info *info)
{
	if (!command_info || !minishell_envp || !info)
		exit (PROCESS_ERROR);

//	debug_print_2d_arr(command_info->commands, "<DEBUG>execute command");

	/* exec redirect */
	if (execute_redirect(command_info, info) == FAILURE)
		exit (PROCESS_ERROR);

	/* is_builtin -> execute_builtin */
	if (is_builtin(command_info->commands))
		exit (execute_builtin(info, command_info->commands));

	/* is_subshell */
	if (command_info->subshell_token_list)
		exit (execute_subshell(&command_info->subshell_token_list, info));
	/* execute commands (other than builtin) */
	if (is_path(command_info->commands[0]))
		execve(command_info->commands[0], command_info->commands, minishell_envp);
	else
		if (ft_execvp(command_info->commands, minishell_envp, info->envlist_head) == PROCESS_ERROR)
			exit (PROCESS_ERROR);

	/* if command not fount */
	ft_dprintf(STDERR_FILENO, "command not found: %s\n", command_info->commands[0]);
	exit (CMD_NOT_FOUND);
}

static int	ft_execvp(char **commands, char **minishell_envp, t_list *envlist)
{
	char		**splitted_paths;
	char		*env_paths;
	size_t		idx;
	char 		*path;

	if (!commands)
		return (PROCESS_ERROR);
	errno = 0;

	env_paths = get_env_value(PATH, envlist);
	splitted_paths = ft_split(env_paths, CHA_PATH_DELIM);
	if (!splitted_paths)
		return (perror_ret_int("malloc", PROCESS_ERROR));
	idx = 0;
	while (splitted_paths[idx])
	{
		path = get_execute_path(splitted_paths[idx], commands[0]);
		if (!path)
		{
			free_2d_alloc((void **)splitted_paths);
			return (perror_ret_int("malloc", PROCESS_ERROR));
		}
		execve(path, commands, minishell_envp);
		free_1d_alloc(path);
		idx++;
	}
	free_2d_alloc((void **)splitted_paths);
	return (CMD_NOT_FOUND);//意味ない
}

static char	*get_execute_path(char *path, char *file)
{
	char			*execute_path;
	size_t			len;
	const size_t	path_len = ft_strlen_ns(path);
	const size_t	file_len = ft_strlen_ns(file);

	errno = 0;
	len = path_len + file_len;
	if (path_len > 0 && path[path_len - 1] != '/')
		len++;
	execute_path = (char *)ft_calloc(sizeof(char), len + 1);
	if (!execute_path)
		return (perror_ret_nullptr("malloc"));
	ft_strlcat_ns(execute_path, path, len + 1);
	if (path_len > 0 && path[path_len - 1] != '/')
		ft_strlcat_ns(execute_path, "/", len + 1);
	ft_strlcat_ns(execute_path, file, len + 1);
	return (execute_path);
}

static bool	is_path(const char *commands_head)
{
	if (commands_head && (commands_head[0] == '/' || commands_head[0] == '.'))
		return (true);
	return (false);
}
