#include "get_next_line.h"
#include "strings_functions.h"
#include <unistd.h>

int		test_nl(const char *buffer, int read_len)
{
	int i;

	i = 0;
	if (buffer)
	{
		while (i < read_len)
		{
			if (buffer[i] == '\n')
				return (i);
			i++;
		}
	}
	return (-1);
}

/*
 * Teste la chaine de caractere stockant les caracteres apres le '\n'
 * de la ligne precedente : si un '\n' est present, test_stock_nl fait appel a ft_strunjoin.
 */

unsigned int	test_stock_nl(char **line, char **stock)
{
	unsigned int	indice_nl_stock;

	indice_nl_stock = 0;
	if (*stock)
	{
		while ((*stock)[indice_nl_stock])
		{
			if ((*stock)[indice_nl_stock] == '\n')
			{
				*stock = ft_strunjoin(line, *stock, indice_nl_stock);
				if (!*stock)
					return (666);
				return (1);
			}
			indice_nl_stock++;
		}
	}
	return (0);
}

/*
 * test_buffer_nl teste la presence d'un '\n' dans les derniers caracteres lus par read.
 * Si un '\n' est present, line contiendra les caracteres jusqu'au '\n' (une ligne),
 * et les caracteres depassant le '\n' seront stockes.
 */
unsigned int	test_buffer_nl(char **line, char *buffer, char **stock, int read_len)
{
	int	indice_nl_buffer;
	if ((indice_nl_buffer = test_nl(buffer, read_len)) >= 0)
	{
		free (*line);
		*line = ft_strnjoin(*stock, buffer, indice_nl_buffer);
		*stock = ft_strcpyfromn(buffer, indice_nl_buffer, read_len);
		return (1);
	}
	else
		*stock = ft_strnjoin(*stock, buffer, read_len);
	return (0);
}

t_string	*initialize_t_string(int fd, t_string *stock)
{
	if (!(stock = malloc(sizeof(t_string))))
		return (NULL);
	stock->str = NULL;
	stock->fd = fd;
	stock->next = NULL;
	return (stock);
}

t_string	*adapt_stock_for_fd(int fd, t_string **begin_list)
{
	t_string	**current;

	if (!*begin_list)
		if (!(*begin_list = initialize_t_string(fd, *begin_list)))
			return (NULL);
	current = begin_list;
	while(*current)
	{
		if ((*current)->fd == fd)
			return (*current);
		current = &(*current)->next;
	}
	if (!(*current = malloc(sizeof(t_string))))
		return (NULL);
	(*current)->str = NULL;
	(*current)->fd = fd;
	(*current)->next = NULL;
	return (*current);
}

char		*free_and_nullify(char *stock)
{
	free(stock);
	return (NULL);
}

int				ft_file_read(int fd, char **line, char **stock)
{
	char			buffer[BUFF_SIZE];
	int				read_len;
	int				is_nl_in_stock;

	if (((is_nl_in_stock = test_stock_nl(line, stock))) == 0)
	{
		while((read_len = read(fd, buffer, BUFF_SIZE)) > 0)
		{
			if (test_buffer_nl(line, buffer, stock, read_len) == 1)
				break;
		}
	}
	if (is_nl_in_stock == 666 || read_len == -1)
		return (-1);
	if (read_len == 0)
		return (0);
	return (1);
}


int				get_next_line(const int fd, char **line)
{
	static t_string		*stock;
	int		ret;
	t_string			*temp;

	if(!(temp = adapt_stock_for_fd(fd, &stock)))
		return (-1);
	ret = ft_file_read(fd, line, &temp->str);
	if (ret == -1)
		return (-1);
	if (!*temp->str && ret == 0) //stock est vide et fichier est vide : plus rien a afficher
	{
		stock->str = free_and_nullify(temp->str);
		return (0);
	}
	if (*temp->str && ret == 0) //stock n'est pas vide mais fichier est vide
	{
		*line = ft_strcpy(*line, temp->str);
		*(temp->str) = '\0';
	}
	return (1);
}