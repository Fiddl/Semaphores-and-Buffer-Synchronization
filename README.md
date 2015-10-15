# Semaphores and Buffer Synchronization

## Description
This project exlpores the use of semaphores to solve the bounded buffer problem utilizing pthreads in C, and
compiles into three executables:

rw: a simple single-threaded program that repeadetdly reads a line of text from an input file, writes the line
into a buffer, then writes the line from the buffer to an output file.

transfer1: Comprised of a "fill thread" and "drain thread". The fill thread reads lines of text from
an input file and writes it to a buffer. The drain thread reads lines of text from a buffer then writes it
to an output file. The assignment is implemented using a mutex and a "spin wait" approach. Aside from providing
an input file and output file, the program takes two additional parameters that specify the length the fill
thread and drain thread should sleep, to explore different thread execution rates.

transfer2: This program is the same as transfer1 except that it eliminates needless busy waits through the use of
condition variables to put the threads to sleep.

## Motivation
Implemented for a concurrent programming homework assignment.

## Installation
make will compile the source code into the three executables: rw, transfer1, and transfer2.

## Usage
```
./rw  <input file path>
      <output file path>
      
./transfer1 <input file path>
            <output file path>
            <fill thread sleep time>
            <drain thread sleep time>
            
./transfer2 <input file path>
            <output file path>
            <fill thread sleep time>
            <drain thread sleep time>
```
