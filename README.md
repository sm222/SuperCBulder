# SuperCBulder
scb `super c builder` or `stupid c builder` is a program that use your files tree to make a makefile or other
build system

## why use it
you don't have too, it mostly a tool i made for myself.  
The idea behind it is to restore old projects and make them more accessible, keeping the build logic simple and stupid.  
just enough setting to be versatile and not too much to include problem.



# how to use
todo:
```
$scb {path} [build system (not added yet)]
```

# variable declaration
the sine `#` is used for comment, but it need to be the first character of the line to be valid.  

simple rule for the config file, to declare value you use that syntax.  

```
name:value
  additional value
```
if you don't add `:` at the end it will throw an error, it needs that to find the end of the name and the beginning of the value.

any text under the variable name will be consider as addionnal value only if there is at least,  
one space, form-feed `\f`, carriage return `\r`, horizontal tab `\t`, or vertical tab `\v`.  
  
otherwise it will be considered a new variable declaration. if any text is found "floating" with no other valid text above it it will throw an error.

```
# valide
foo:

foo:bar

foo:
  bar

foo:bar
  baz

# invalide
foo

  bar

 foo:

foo:
bar
```
you can also reference varible with `%`, to make the file more readable

```
foo:hello

bar: jeff

baz:
  %foo
  %bar

```

now `baz` value is `hello jeff`. variable can only be read from bottom to top  
```
foo:A

bar:B
  %baz

baz:C
  %foo

```
the result it `foo A bar B baz CA`. like in bash if a variable is not resolved the value is empty

now to make the project more modular and have more option we have some keyword that help you make the project more. you can use the keywork `_LINUX` like so to only set a varible if the program is build on linux or the target computer is linux.

```
foo:are we we building for linux? 
  %_LINUX yes
```
we can also use the keywork `_ENV_` to set a variable pr read some information, and use `_SHELL` to see the result.

```
name: %_ENV_USER


%_SHELL echo %name
```


# reserve variable
|  name       | default value         | about                              |
|-------------| --------------------- |------------------------------------|
|**CC**       | cc                    |c compiler                          |
|**CXX**      | c++                   |cpp compiler                        |
|**NAME**     | {directory name}      |output file name                    |
|**NAMEX**    |                       |add an extension to the program     |
|**CFLAGS**   | -Wall -Werror -Wextra |c flags                             |
|**CXXFLAGS** | -Wall -Werror -Wextra |cpp flags                           |
|**ING**      |                       |ignore folder during build          |
|**DEP**      |                       |add a rule that run before compiling|
|**PROG**     |                       |set the build mode to executable    |
|**LIB**      |                       |set the build mode to library       |
|**SHELL**    | sh                    |the shell the system will use |
# keyword

|  name       | action                |
|-------------| --------------------- |
|**_LINUX**   |only read the rest of the line if the system is call on linux **or** targer is linux|
|**_WINDOWS** |only read the rest of the line if the system is call on windows **or** targer is windows|
|**_MACOS**   |only read the rest of the line if the system is call on macos **or** targer is macos|
|**_ENV\_{}** |go read the environmant varible with the same name (**todo**)|
|**_SHELL**   |run line as is.                                    (**todo**)|




# suported build system and scrips language

| name     |support|
|----------|-------|
|**make**  | **X** |
|**cmake** |       |
|**ninja** |       |
|**meson** |       |
|**tup**   |       |
|**______**|   |
|**sh**    |       |
|**pyton** |       |
|**zig**   |       |
