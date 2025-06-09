unsigned long hashdjb2(const char *str)
{
	const unsigned char	*s = (const unsigned char *)str;
    unsigned long hash = 5381;
    int c;

    while ((c = *s++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}