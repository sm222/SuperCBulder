# SuperCBuilder
scb `super c builder` or `stupid c builder` is a simple makefile generator program, Which aims to create a custom build system from your repo's file tree.

## why use it
You don't have to, as is mostly a tool i made for myself.  
The idea behind it is to help restore old projects and make them more accessible, keeping the build logic simple and stupid.  
It has enough settings to be versatile but tries to avoid overengeneering, Minimize related the problems that that could bring.



# how to use
If you just clone the project you can run `compile.sh` for initial setup. after that you can do `./scb .`  
and it gona build a make file for it self.
```
$ ./compile.sh
$ ./scb .
$ make
```
Now we can simply use
```
$scb {path} [build system name]
```
By default if no build system is provided scb will use makefile.

# variable declaration
the sine `#` is used for comment, but it need to be the first character of the line to be valid.  

simple rule for the config file, to declare value you use that syntax.  

```
name:value
  additional value
```
If you do not add colon `:` between the name and the value, an error will be thrown. The colon is needed for the program to know which is which.  
Value can have space in side of them but if the are on a new line the space will be trim. 
In the case you want the value to start with space just add them after the name declaration.
```
# valid
name:   value
```

any text under the variable name will be consider as addionnal value only if there is at least,  
one space, form-feed `\f`, carriage return `\r`, horizontal tab `\t`, or vertical tab `\v`.  
> If unsure use man isspace.

Otherwise it will be considered a new variable declaration. if any text is found "floating" with no other valid text above it it will throw an error.  

### valid
```
foo:

foo:bar

foo:
  bar

foo:bar
  baz
```
### invalid
```
foo

  bar

 foo:

foo:
bar
```
You can also reference previously declared variable with a `%`, to make the file more readable.

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
The result is `foo A bar B baz CA`. Like in bash if a variable is not resolved/assigned the value is empty.
some [variables are pre-declared](#reserve-variable) by scb, you can overwrite them by adding them to your config file.

## [keywords](#keyword)
now to make the project more modular and have more option we have some keyword that help.  
you can use the keywork `_LINUX` like so to only set a varible if the program is build on linux or the target computer is linux.

```
foo:are we we building for linux?
  %_LINUX yes
  %_WINDOWS no
  %_MACOS no
```
todo: add flag for target  
you can also change the target with the flag `-T=MACOS` or any other valid target.

We can also use the keyword `_ENV_{NAME}` to set a variable to the value of a
 variable of enviroment.

```
name: %_ENV_USER
# set variable to the user name

```
# syntaxe
### valid
```
name:value
  additional value
  %variable
  %_keyword
# comment

# valid

name:
  value

name:%_keyword value

name:

name:%variable
```
### invalide
```
name

name value

  value

name: value #comment
```
In the last case the comment will be in the value,  
scb only take the comment in consideration if the `#` is the first character on the line.

# reserved variable
|  name       | default value         | about                              |
|-------------| --------------------- |------------------------------------|
|**CC**       | cc                    |c compiler                          |
|**CXX**      | c++                   |cpp compiler                        |
|**NAME**     | {directory name}      |output file name                    |
|**NAMEX**    |                       |add an extension to the output      |
|**CFLAGS**   | -Wall -Werror -Wextra |c flags                             |
|**CXXFLAGS** | -Wall -Werror -Wextra |cpp flags                           |
|**ING**      |                       |ignore folder during build          |
|**DEP**      |                       |add a rule that run before compiling|
|**PROG**     |                       |add the value to the compilation    |
|**LIB**      |                       |set the build mode to library       |
|**SHELL**    | sh                    |the shell the system will use       |

> if prog or lib is not in the config file, scb will make a program by default.

# keyword
|  name       | action                |
|-------------| --------------------- |
|**_LINUX**   |only reads the rest of the line if the system is call on linux **or** targer is linux|
|**_WINDOWS** |only reads the rest of the line if the system is call on windows **or** targer is windows|
|**_MACOS**   |only reads the rest of the line if the system is call on macos **or** targer is macos|
|**_ENV\_{NAME}** |reads the environment variable with the same name|
|**_SHELL**   |run line as is |
|**_ROOT**    |get the root of the projet (first argv) |




# suported build system and scrips language

|  name      | type         | support |
| ---------- | ------------ | ------- |
| **make**   | build system |  **X**  |
| **cmake**  | build system |         |
| **ninja**  | build system |         |
| **meson**  | build system |         |
| **sh**     | scripting    |  **\|** |
| **pyton**  | scripting    |         |
| **zig**    | scripting    |         |


## special thanks
  - [lord](https://github.com/loyc12)