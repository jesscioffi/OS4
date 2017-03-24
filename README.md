Project 4: System for Verifying Web Placement

BY: Esmeralda Cervantes (ecervant) and Jessica Cioffi (jcioffi)

How Our System Works: (how to build, run, and config code)


Parameters:
The config.txt file contains all the parameters needed to run our project. 
This file is also the only argument passed in to the code. The following 
arguments are parsed from the config file: PERIOD_FETCH, NUM_FETCH, NUM_PARSE, SEARCH_FILE, SITE_FILE.
PERIOD_FETCH is the time between fetches of the various sites and it will default to 180 if the parameter is not found in the config file. 
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
