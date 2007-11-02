#include "stdafx.h"

// Rich Salz wildmat function, as he defined it. Will stick to
// standards, so I have copied it from UNIX systems.

static int DoMatch(const char *text, const char *p, const char* end)
{
    int	                matched;
	
    for (; *p && (end?text<end:TRUE); text++, p++)
	{
		if (*text == '\0' && *p != '*')
			return -1;
		switch (*p)
		{
			default:
				if (*text != *p)
					return FALSE;
				continue;
			case '?':
				/* Match anything. */
				continue;
			case '*':
				while (*++p == '*')
					/* Consecutive stars act just like one. */
					continue;
				if (*p == '\0')
					/* Trailing star matches everything. */
					return TRUE;
				while (*text)
					if ((matched = DoMatch(text++, p, end)) != FALSE)
						return matched;
					return -1;
				return -1;

		}
    }
    return *text == '\0';
}

int wildmat(const char *Buffer, const char *Pattern)
{
    if (Pattern[0] == '*' && Pattern[1] == '\0')
		return TRUE;
	
    return DoMatch(Buffer, Pattern, NULL) == TRUE;
}

int wildmats(const char *Buffer, const char *Pattern, char *end)
{
    if (Pattern[0] == '*' && Pattern[1] == '\0')
		return TRUE;
	
    return DoMatch(Buffer, Pattern, end) == TRUE;
}