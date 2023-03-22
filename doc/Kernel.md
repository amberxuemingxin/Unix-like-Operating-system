# PennOS Kernel/Scheduler API Spec

PennOS is designed
around subsystems that model those of standard UNIX. The kernel part of PennOS has a basic priority scheduler and user shell interactions.

## Built-ins
PennOS has the following built-ins and run as independently scheduled PennOS processes.
Built-in | Lib | Usage
| --- | --- | ---
`cat` | `cat.h` | reads data from the file and gives their content as output
`sleep` | `sleep.h` | causes the calling thread to sleep
`busy` | `busy.h` | waits indefinitely
`echo` | `echo.h` | displays a line of text
`ls` | `ls.h` | lists all files in the directory
`touch file` | `touch.h` | creates an empty file if it does not exist, or update its timestamp otherwise
`mv src dest` | `mv.h` | rename `src` to `dest`
`cp src dest` | `cp.h` | copy `src` to `dest`
`rm file` | `rm.h` | remove files
`chmod` | `chmod.h` | changes the file mode bits of each given file according to mode
`ps` | `ps.h` | lists all processes on PennOS
`kill [SIG] pid` | `kill.h` | sends a specified signal to the specified process
`zombify` | `zombie_orphan.h` | spawn a zombie process for the testing purpose
`orphanify` | `zombie_orphan.h` | spwan an orphan process for the testing purpose
`nice priority command` | `nice.h` | set the priority of the `command` to `priority` and execute the command
`nice_pid priority pid` | `nice.h` | adjust the nice level of process `pid` to priority `priority`
`man` | `man.h` | lists all available commands
`bg [job_id]` | `job.h` | continues the last stopped `job` or the specified `job`
`fg [job_id]` | `job.h` | bring the last stopped/bg job to the foreground, or the specified `job`
`jobs` | `job.h` | lists all jobs
`logout` | `logout.h` | exit the shell and shutdown PennOS