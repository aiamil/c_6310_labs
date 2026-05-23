#include "../inc/txtfile.h"

char	**replace_substr(char **lines,
		const char *old,
		const char *new_str)
{
	char	**result;
	int		i;
	int		count;

	if (!lines || !old || !new_str)
		return (NULL);
	
	count = count_lines_array(lines);
	result = (char **)ft_calloc(count + 1, sizeof(char *));
	if (!result)
		return (NULL);
	
	i = 0;
	while (i < count)
	{
		result[i] = replace_in_line(lines[i], old, new_str);
		if (!result[i])
		{
			free_lines(result);
			return (NULL);
		}
		i++;
	}
	return (result);
}