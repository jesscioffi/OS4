Project 4: System for Verifying Web Placement

BY: Esmeralda Cervantes (ecervant) and Jessica Cioffi (jcioffi)

How Our System Works: (how to build, run, and config code)
To build our project, a user just has to run make. Our default compiler is the /usr/bin/g++
compiler from the student machines. After the executable is made, the user runs it with 
the config.txt file to pass in the necessary variables. Our code immediately parses the 
config file and then also parses the URLs and the Search terms from the denoted files. If no
file names given in the config.txt, then the program does not execute.
Once the executable is running, the user can exit the program with Ctrl+C. This tells the program
to stop as soon as it finishes the file it is currently working on so it exits "gracefully".

Parameters:
The config.txt file contains all the parameters needed to run our project. 
This file is also the only argument passed in to the code. The following 
arguments are parsed from the config file: PERIOD_FETCH, NUM_FETCH, NUM_PARSE, SEARCH_FILE, SITE_FILE.
PERIOD_FETCH is the time between fetches of the various sites and it will default to 
180 if the parameter is not found in the config file. 
NUM_FETCH is the number of fetch threads to create and it will default to 1.
NUM_PARSE is the number of parse threads to create and it will default to 1.
SEARCH_FILE is the file containins all the search strings.
SITE_FILE is the file containing the sites to query. If the SEARCH_FILE or
the SITE_FILE is not passed in, then the program does not run. Our confg.txt file 
contains the following values for the parameters:
PERIOD_FETCH=10
NUM_FETCH=3
NUM_PARSE=2
SEARCH_FILE=search.txt
SITE_FILE=sites.txt
