/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wchen <wchen@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/20 21:07:02 by wchen             #+#    #+#             */
/*   Updated: 2023/01/22 20:40:31 by wchen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	define_key_value(char *cmd, char **key, char **value)
{
	char		*key_sign;
	int			exit_status;
	ssize_t		equal_index;
	ssize_t		cmd_len;

	exit_status = EXIT_SUCCESS;
	equal_index = 0;
	key_sign = ft_strchr(cmd, '=');
	//'='を見つからないときに、何もせずに、exit_status(0)を返す
	if(key_sign == NULL)
		return (EXIT_SUCCESS);
	cmd_len = ft_strlen(cmd);
	while (cmd[equal_index] != '=')
		equal_index ++;
	*key = ft_substr(cmd, 0, equal_index);
	*value = ft_substr(cmd, equal_index + 1, (cmd_len - (equal_index+1)));
	if (!key || !value)
		 perror_and_return_int("malloc", EXIT_FAILURE);
	return (exit_status);
}

static t_key_type	judge_key(char *key)
{
	ssize_t	i;
	ssize_t key_len;

	i = 0;
	if (ft_isdigit(*key) || *key == '\0')
		return (e_error);
	key_len = ft_strlen(key);
	while(*key)
	{
		if ((i + 1) == key_len && *key == '+')
			return (e_append);
		if(!ft_isalnum(*key))
			return (e_error);
		i ++;
		key ++;
	}
	return (e_add);
}

static int	append_env(t_info *info, char *key, char *value)
{
	char	**elem_value;
	ssize_t	key_len;
	char 	*elem_key;
	char	*temp_ptr;

	key_len = ft_strlen(key);
	elem_key = malloc(sizeof(char) * (key_len));
	if (!elem_key)
		perror_and_return_int("malloc", EXIT_FAILURE);
	elem_key = ft_memmove(elem_key, key, key_len - 1);
	free (key);
	printf("elem_key:%s\n", elem_key);
	(void) value;
	elem_value = get_elem(info, elem_key);
	if (elem_value == NULL)
	{
		printf("not get\n");
		set_elem(info, elem_key, value);
		ft_env(info);
	}
	else
	{
		printf("*elem_value: %s\n", *elem_value);
		temp_ptr = *elem_value;
		*elem_value = ft_strjoin(*elem_value, value);
		free (temp_ptr);
		ft_env(info);
	}

	return (EXIT_SUCCESS);
}

int	ft_export(t_info *info, char **cmds)
{
	int			exit_status;
	char		*key;
	char		*value;
	t_key_type	key_type;

	key = NULL;
	value = NULL;
	exit_status = EXIT_SUCCESS;
	if (!info || !cmds)
		return (EXIT_FAILURE);
	cmds ++;
	//export NULL の挙動は未定義（shell変数を表示するが、実装しない！？なんのエラーを表示する？）
	if (*cmds == NULL)
		ft_dprintf(STDERR_FILENO, "minishell: export: invalid option");
	while (*cmds != NULL)
	{
		exit_status = define_key_value(*cmds, &key, &value);
		printf("key:%s, value:%s\n", key, value);
		key_type = judge_key(key);
		if (key_type == e_error)
			ft_dprintf(STDERR_FILENO, "minishell: export: `%s': not a valid identifier\n", *cmds);
		if (key_type == e_append)
			exit_status = append_env(info, key, value);

		// 	return (exit_status);
		// export_env(info, key, value);
		cmds ++;
	}
	return (exit_status);
}
