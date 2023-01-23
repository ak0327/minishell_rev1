/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 17:00:09 by takira            #+#    #+#             */
/*   Updated: 2023/01/23 18:48:37 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clear_exec_list(t_exec_list **exec_list)
{
	t_exec_list	*next;
	t_exec_list	*node;

	if (!exec_list || !*exec_list)
		return ;
	node = *exec_list;
	while (node)
	{
		next = node->next;

		ft_lstclear(&node->token_list_head, free_token_elem);
		ft_lstclear(&node->pipeline, free_command_list_elem);

		node = next;
	}
}

void	**free_2d_alloc(void **alloc)
{
	size_t	i;

	if (!alloc)
		return (NULL);
	i = 0;
	while (alloc[i])
	{
		free_1d_alloc(alloc[i]);
		i++;
	}
	free(alloc);
	return (NULL);
}


void	*free_1d_alloc(void *alloc)
{
	free(alloc);
	return (NULL);
}

void	*free_info(t_info **info)
{
	if (!info || !*info)
		return (NULL);

	ft_lstclear(&(*info)->envlist_head, free_env_elem);

	clear_input(info);
	free(*info);
	*info = NULL;
	return (NULL);
}

void	free_env_elem(void *content)
{
	t_env_elem	*elem;

	if (!content)
		return ;
	elem = content;
	elem->key = free_1d_alloc(elem->key);
	elem->value = free_1d_alloc(elem->value);
	free_1d_alloc(elem);
}

void	free_command_list_elem(void *content)
{
	t_command_list	*elem;

	if (!content)
		return ;
	elem = content;
	elem->commands = (char **)free_2d_alloc((void **)elem->commands);
	ft_lstclear(&elem->pipeline_token_list, free_env_elem);
	ft_lstclear(&elem->subshell_token_list, free_env_elem);
	//TODO: delete redirect_list
}

void	free_token_elem(void *content)
{
	t_token_elem	*elem;

	if (!content)
		return ;
	elem = content;
	elem->word = free_1d_alloc(elem->word);
	free_1d_alloc(elem);
}


