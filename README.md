# C-shell_simulator
To test this project
```
gcc cshell.c -o cshell
./cshell
```
There are several example instructions shown below. 
```
# These are some regular linux instructions. Feel free to try out some other instructions
ls
cd .
pwd
# These are some non-built-in instructions:
log
print hello
theme blue
exit
```
print command will print the following string. 
note that exit command will exit the cshell environment. theme command can change the color of terminal. Three colors avaliable (blue, red and green). You can also reset the color by typing theme reset

You can also create environment variables by following command:
```
$var=123
```
every time you want to access the environment variables, you can simply use $var. Example is as below
```
print $var //output will be 123
```
You can pass the whole script file to cshell by using below command: 
```
./cshell script.txt
```
A example script file is included in this repository named Myscript.txt. Please feel free to create your own and test it. 
