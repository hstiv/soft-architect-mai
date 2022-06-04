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

std::map<string, string> argv2map(int argc, char *argv[], const string &desc)
{
    int i;
    std::map<string, string> args;
    for(i = 1; i < argc; i++)
    {
        int j;
        string arg(argv[i]);
        j = std::find(arg.begin(), arg.end(), '=') - arg.begin();
        if(j == arg.size())
        {
            cout << "ERROR in argc[" + STR(i) + "]" << endl;
            cout << desc << endl;
            return args;
        }
        args[arg.substr(0, j)] = arg.substr(j + 1, arg.size() - j);
    }
    return args;
}

std::map<string, string> argv2map(int argc, char *argv[])
{
    string desc = "";
    return argv2map(argc, argv, desc);
}
