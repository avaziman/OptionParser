# Option Parser
Note: this project is still work in progress.
## Features
- Argument parsing
- Argument checking
- Default value
- Variable binding

## How to use:
1. Initialize OptionParser:
```c++
    OptionParser op;
```
2. Initialize your options using the following parameters:
        -names -> string vector
        -arg -> possible values are:
            -OptionArg::NONE -> argument won't be taken even if it's provided
            -OptionArg::OPT -> argument is optional
            -OptionArg::REQ -> argument is required
        -description (optional) -> string describing the option
Example:
        
```c++ 
auto xOpt = Option<int>({"x", "set-x"}, OptionArg::OPT, "set variable x");
```
3. Add your options:
```c++
OptionParser op;
auto xOpt = Option<int>({"x", "set-x"}, OptionArg::OPT, "set variable x");
op.AddOption(&xOpt);
```
4. Parse
```c++
OptionParser op;
auto xOpt = Option<int>({"x", "set-x"}, OptionArg::OPT, "set variable x");
op.AddOption(&xOpt);
op.Parse();
```
5. Retrieve argument
```c++
OptionParser op;
auto xOpt = Option<int>({"x", "set-x"}, OptionArg::OPT, "set variable x");
op.AddOption(&xOpt);
op.Parse();

int x = xOpt.Value();
```

## Testing
GoogleTest is required for building tests, it shuold be located in the lib folder.
Install:
```
cd build
git clone https://github.com/google/googletest/
```