# PennOS Kernal/Scheduler API Spec

PennOS is designed
around subsystems that model those of standard UNIX. The kernel part of PennOS has a basic priority scheduler and user shell interactions.

## Built-ins
PennOS has the following built-ins and run as independently scheduled PennOS processes.
Built-in | Lib | Usage
| --- | --- | ---
`cat` | `cat.h` | reads data from the file and gives their content as output
