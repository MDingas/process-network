# process-network

# What is this

College project from course of operating systems.
It was a project made to practice inter-process communication and system call usage. Most of the default system calls were used, such as forking, reading, writing, executing and piping (both unnamed and named pipes).

# What does it do

The project allows the user to create a network of processes in such a way as to process data in a continuous fashion (Apache Storm does this same thing).
Having an interface that allows for mapping of processes that work in relation to each other allows for a more readable and scalable manipulation of data.
Below are listed 3 pratical examples in which the network is used. Note that everything listed is included in the repository.

# Example 1 : filtering data

Consider a list of names tagged by their gender (saved on data/names.txt), like such:

    M,Edgar
    M,Jack
    M,John
    F,Cecil
    F,Jackeline
    F,Sandra
    F,Susan
    M,Carl
    F,Anna
    M,Jack
    M,Marley
    F,Aisha
    F,Bulma

We wish to separate those tagged with the M character from those tagged with the F character.

First we'll need a program that filters females, and another that filters males, those are isFemale and isMale, in the bin directory.

Then, go to the main repository directory and start the program:

    bin/controller

and select the config file:

    configs/genderFilter.txt

which has the following

    node 0 const Phase1
    node 1 isMale
    node 2 isFemale
    connect 0 1 2

And creates the following map:

<img src=http://i.imgur.com/ON180x6.png height="250">

Inject as such:
  inject 0 cat data/names.txt

The result is two files, output/1out and output/2out, with the filtered results.
