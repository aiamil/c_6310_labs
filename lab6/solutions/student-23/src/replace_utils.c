#include "../inc/txtfile.h"

size_t	count_occurrences(const char *str, const char *sub)
{
	size_t	count;
	size_t	len;

	if (!str || !sub || !sub[0])
		return (0);
	count = 0;
	len = ft_strlen(sub);
	while (*str)
	{
		if (ft_strncmp(str, sub, len) == 0)
		{
			count++;
			str += len;
		}
		else
			str++;
	}
	return (count);
}

int	count_lines_array(char **lines)
{
	int	count;

	if (!lines)
		return (0);
	count = 0;
	while (lines[count])
		count++;
	return (count);
}

char	*replace_in_line(const char *line,
		const char *old,
		const char *new_str)
{
	char	*result;
	size_t	old_len;
	size_t	new_len;
	size_t	count;
	size_t	i;
	size_t	j;

	if (!line || !old || !new_str)
		return (NULL);
	
	old_len = ft_strlen(old);
	new_len = ft_strlen(new_str);
	
	// Защита от пустой old (чтобы не было бесконечного цикла)
	if (old_len == 0)
		return (ft_strdup(line));
	
	count = count_occurrences(line, old);
	
	// Выделяем память сразу под всю строку
	result = (char *)malloc(ft_strlen(line) + count * (new_len - old_len) + 1);
	if (!result)
		return (NULL);
	
	i = 0;
	j = 0;
	while (line[i])
	{
		if (ft_strncmp(&line[i], old, old_len) == 0)
		{
			ft_memcpy(&result[j], new_str, new_len);
			j += new_len;
			i += old_len;
		}
		else
		{
			result[j++] = line[i++];
		}
	}
	result[j] = '\0';
	return (result);
}