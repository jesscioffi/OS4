// Esmeralda Cervantes
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

void error(string message){
    cout << "ERROR: " << message << endl;
    exit(1);
}

// function to return vector of str from site and search files
vector<string> get(char const* ofile){
    string info;
    vector<string> vstrings;
    ifstream ifs;
    ifs.open(ofile);
    if (!ifs.is_open()){
        error("Unable to open file");
    }
    while (ifs >> info){
        vstrings.push_back(info);
    //    cout << info << endl;
    }
    ifs.close();
    return vstrings;
}

int main (int argc, char *argv[]){
    if (argc == 1){ // not enough arguments given in command line
        cout << "ERROR: Configuration file needed" << endl;
        exit(1);
    }
    // parsing config file
    int PERIOD_FETCH = 180;
    int NUM_FETCH = 1;
    int NUM_PARSE = 1;
    string SEARCH_FILE = "";
    string SITE_FILE = "";
    string stream; // line from file
    string token; // broken down string
    string t[2]; //array of words
    ifstream infile;
    infile.open(argv[1]);
    if (!infile.is_open()){
        error("Config file not opened");
    }
    while (infile >> stream){
        istringstream ss(stream);
        int i = 0;
        while(getline(ss, token, '=')){
            t[i] = token;
            i++;
        }
        if (t[0] == "PERIOD_FETCH"){
            int re;
            stringstream(t[1]) >> re; // converts str to int
            PERIOD_FETCH = re;
        }
        else if (t[0] == "NUM_FETCH"){
            int re;
            stringstream(t[1]) >> re; // str to int
            NUM_FETCH = re;
            if (NUM_FETCH < 1 || NUM_FETCH > 8){
                cout << "ERROR: NUM_FETCH out of range!" << endl;
                exit(1);
            }

        }
        else if (t[0] == "NUM_PARSE"){
            int re;
            stringstream(t[1]) >> re; // str to int
            NUM_PARSE = re;
            if (NUM_PARSE < 1 || NUM_PARSE > 8){
                cout << "ERROR: NUM_PARSE out of range!" << endl;
                exit(1);
            }   
        }
        else if (t[0] == "SEARCH_FILE"){
            SEARCH_FILE = t[1];
        } 
        else if (t[0] == "SITE_FILE"){
            SITE_FILE = t[1];
        }
        else{
            cout << "WARNING: Unkown parameter " << t[0] << endl;
        }

    }   
    infile.close(); // closes config file
    if (SEARCH_FILE == "" || SITE_FILE == ""){
        error("No Search or Site files found");
    }
    char const* cstr = SEARCH_FILE.c_str();
    vector<string> searches = get(cstr);
//    cout << "here" << endl;
    for (unsigned j = 0; j < searches.size(); j++){
        cout << searches[j] << endl;
    }
    char const* sstr = SITE_FILE.c_str();
    vector<string> sites = get(sstr);
    for (unsigned j = 0; j < sites.size(); j++){
        cout << sites[j] << endl;
    }
//    cout << PERIOD_FETCH << endl;
//    cout << NUM_FETCH << endl;
//    cout << NUM_PARSE << endl;
//    cout << SEARCH_FILE << endl;
//    cout << SITE_FILE << endl;

    return 0;
}
