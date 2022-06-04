bool starts_with(const string &str, const string &prefix)
{
    if(prefix.size() > str.size())
        return false;

    int i;
    for(i = 0; i < prefix.size(); i++)
    {
        if(str[i] != prefix[i])
        {
            return false;
        }
    }
    return true;
}
