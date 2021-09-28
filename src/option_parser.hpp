#ifndef OPTION_PARSER_HPP_
#define OPTION_PARSER_HPP_

/*  Command line option parsing is done according to
    the POSIX recommended convention that can be found here:
    https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html
*/

#include <algorithm>
#include <exception>
#include <map>
#include <string>
#include <vector>

enum class OptionArg
{
    OPT,
    REQ,
    NONE,
};

class OptionBase
{
    friend class OptionParser;

   private:
    bool IsValidOptName(std::string name);
    std::vector<std::string> names;
    const char* description;

   protected:
    OptionBase(std::vector<std::string> nmes, OptionArg arg,
               const char* desc = "");
    virtual void ParseValue(std::string strVal) = 0;
    std::string GetLongestName();

    OptionArg arg = OptionArg::OPT;
    bool is_set = false;
};

inline OptionBase::OptionBase(std::vector<std::string> nmes, OptionArg arg,
                              const char* desc)
    : names(nmes), arg(arg), description(desc)
{
    if (names.size() == 0)
        throw std::invalid_argument("An option must have at least one name.");

    std::vector<std::string>::iterator it;
    for (it = names.begin(); it != names.end(); it++)
    {
        if (!this->IsValidOptName(*it))
            throw std::invalid_argument("Option: " + std::string(*it) +
                                        " -> invalid option name.");
    }
}

inline bool OptionBase::IsValidOptName(std::string name)
{
    for (char c : name)
    {
        if (!iswalnum(c) && c != '-') return false;
    }
    return true;
}

inline std::string OptionBase::GetLongestName()
{
    std::vector<std::string>::iterator it;
    std::string longest = "";
    for (it = names.begin(); it != names.end(); it++)
    {
        if ((*it).size() > longest.size()) longest = *it;
    }
    return longest;
}

template <class T>
class Option : public OptionBase
{
   public:
    Option(std::vector<std::string> nmes, OptionArg arg, const char* desc = "");
    Option<T> Default(T defaultVal);
    Option<T> Check(bool (*func)(T val));
    Option<T> Bind(T* ptr);

    T Value();

   private:
    void ParseValue(std::string strVal);
    T value;
    T* var_ptr = nullptr;

    bool (*check)(T val) = nullptr;
};

template <class T>
inline Option<T>::Option(std::vector<std::string> nmes, OptionArg arg,
                         const char* desc)
    : OptionBase(nmes, arg, desc)
{
}

template <class T>
inline Option<T> Option<T>::Default(T defaultVal)
{
    if (this->arg == OptionArg::REQ)
        throw std::logic_error(
            "Option: " + this->GetLongestName() +
            " option that requires a value can't have a default value. ");
    else if(this->arg == OptionArg::NONE)
        throw std::logic_error(
            "Option: " + this->GetLongestName() +
            " option that takes no value can't have a default value. ");

    this->value = defaultVal;
    return *this;
}

template <class T>
inline Option<T> Option<T>::Bind(T* ptr)
{
    this->var_ptr = ptr;
    return *this;
}

template <class T>
inline Option<T> Option<T>::Check(bool (*func)(T val))
{
    this->check = func;
    return *this;
}

template <class T>
inline T Option<T>::Value()
{
    return this->value;
}

template <typename T>
inline void Option<T>::ParseValue(std::string strVal)
{
    T val;
    std::stringstream ss(strVal);
    ss >> val;

    if (ss.fail())
        throw std::invalid_argument("Option: " + this->GetLongestName() +
                                    " failed to parse option argument. (" +
                                    strVal + ")");

    if (this->check != nullptr && !this->check(val))
        throw std::invalid_argument(
            "Option: " + this->GetLongestName() +
            " invalid argument provided, check failed. (" + strVal + ")");

    this->value = val;
    this->is_set = true;

    if (this->var_ptr != nullptr)
    {
        *(this->var_ptr) = this->value;
    }
}

class OptionParser
{
   public:
    void Parse(int argc, char* argv[]);

    template <class T>
    void AddOption(Option<T>* opt);

   private:
    OptionBase* GetOptionByName(std::string name);
    std::vector<OptionBase*> options;
};

template <class T>
inline void OptionParser::AddOption(Option<T>* opt)
{
    options.push_back(opt);
}

inline void OptionParser::Parse(int argc, char* argv[])
{
    // skip executable name
    for (int i = 1; i < argc; i++)
    {
        std::string key(argv[i]);

        // skip invalid options with no key
        if (key.size() == 1) continue;
        // incase of "--" stop reading arguments
        else if (key == "--")
            break;

        if (key[0] == '-')
        {
            if (key[1] == '-')
            {
                // two hyphens, multi-letter options
                std::string val;
                key.erase(0, 2);

                int index;
                if (index = key.find('=') != std::string::npos)
                {
                    // in case of "key=val" syntax
                    val = key.substr(index, key.size());
                    key = key.substr(0, index);
                }
                else
                {
                    // in case of "key val" syntax
                    OptionBase* opt = GetOptionByName(key);
                    if (opt->arg != OptionArg::NONE)
                    {
                        val = argv[++i];
                        opt->ParseValue(val);
                    }
                    else
                    {
                        opt->ParseValue("");
                    }
                }

                GetOptionByName(key)->ParseValue(val);
            }
            else
            {
                // one hyphen delimiter, one-letter option/s
                key.erase(0, 1);

                if (key.size() > 1)
                {
                    // multiple one-letter options that do not take values
                    // e.g (-xyz -> -x -y -z)
                    // or one letter option with value without a separator
                    // e.g (-xval -> x = val)

                    OptionBase* opt = GetOptionByName(std::string(1, key[0]));
                    if (opt->arg == OptionArg::REQ)
                    {
                        key.erase(0, 1);
                        opt->ParseValue(key);
                    }
                    else
                    {
                        for (int j = 0; j < key.size(); j++)
                        {
                            opt = GetOptionByName(std::string(1, key[j]));
                            opt->ParseValue("");
                        }
                    }
                }
                else
                {
                    // single one-letter option
                    GetOptionByName(key)->ParseValue(argv[++i]);
                }
            }
        }
    }
}

inline OptionBase* OptionParser::GetOptionByName(std::string name)
{
    for (OptionBase* ob : options)
    {
        for (std::string n : ob->names)
        {
            if (n == name) return ob;
        }
    }

    throw std::invalid_argument("Option: " + name + " -> non existent.");
    return nullptr;
}

#endif