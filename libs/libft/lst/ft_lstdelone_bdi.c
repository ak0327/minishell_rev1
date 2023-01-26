/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone_bdi.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/26 09:49:14 by takira            #+#    #+#             */
/*   Updated: 2023/01/26 09:56:37 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./../../include/libft.h"

void	ft_lstdelone_bdi(t_list_bdi **lst, void (*del)(void *))
{
	if (!lst || !*lst)
		return ;
	if (del != NULL)
		(*del)((*lst)->content);
	free(*lst);
	*lst = NULL;
}
