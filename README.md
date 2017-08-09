# process-network

# What is this

College project from course of operating systems.
It was a project made to practice inter-process communication and system call usage. Most of the default system calls were used, such as forking, reading, writing, executing and piping (both unnamed and named pipes).

# What does it do

The project allows the user to create a network of processes in such a way as to process data in a continuous fashion (Apache Storm does this same thing).
Having an interface that allows for mapping of processes that work in relation to each other allows for a more readable and scalable manipulation of data.
Below are listed 3 pratical examples in which the network is used. Note that everything listed is included in the repository.

# Example 1 : filtering statistical data

Consider a list of names tagged by their gender (saved on data/names.txt),like such:

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

which has the following:

    node 0 const Phase1
    node 1 isMale
    node 2 isFemale
    connect 0 1 2

<img src=http://i.imgur.com/ON180x6.png height="250">

Inject as such:

    inject 0 cat data/names.txt

The result is two files, output/1out and output/2out, with the filtered results.

# Example 2 : Filtering by keywords in tweets

Consider the list of possible tweets (saved on data/tweets.txt),like such:

    I like mugs
    I like rock in rio
    im was a fan of prince in rock in rio
    i enjoy all types of ice cream
    i am slowly running out of tweet examples
    today i got a manicure and it was okay
    do presidents poop
    I am a big fan of concert shows, my favourite is not rock in rio
    who wants to go to panama next year?
    I love rock in rio!

Say we wish to process all tweets by some process phase1, then filter into 3 possible keywords. Finally, process only one with a special process.

Start the program:

    bin/controller

Select the config file:

    configs/tweetFilter.txt

which has the following:

    node 0 const phase 1
    node 1 containsWord rock in rio
    node 2 containsWord like
    node 3 containsWord love
    connect 0 1 2 3
    node 4 const music!
    connect 1 4

<img src=http://i.imgur.com/rrZTZtF.png height="250">

Inject:

    inject 0 cat data/tweets.txt

The result is 3 files, output/2out, output/3out and output/4out. The tweets were filtered by 3 keywords and the function const music! was only applied to the nodes that were filtered by node 1.

# Example 3 : Manipulating numerical data

Consider the list of possible machine data, with the following structure:

<TIME>:<TEMPERATURE>

As such (saved on data/temperatures.txt):

    00,01:123
    00,02:29
    00,03:23
    00,04:49
    00,05:33
    00,06:43
    00,07:12
    00,08:11
    00,09:00
    00,10:34
    00,11:3
    00,12:49
    00,13:0
    00,14:39
    00,15:39
    00,16:03
    00,17:93
    00,18:93
    00,19:33
    00,20:69
    00,21:94
    00,22:96

Once again, we'll filter into two slots: those above 90 units and those below. From them on, calculate the average temperature of the 100 most recent temperatures.

The config file is as follows:

    node 1 const Phase1
    node 2 filter 2 >= 90
    node 3 filter 2 < 90
    node 4 window 2 avg 100
    node 5 window 2 avg 100
    connect 1 2 3
    connect 2 4
    connect 3 5

The results are as expected.
