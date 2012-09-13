/*
x-session - xinitrc reaper for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */


#include <unistd.h>
#include <sys/wait.h>

int main(void) { for (;;) { wait(NULL); sleep(1); } }

