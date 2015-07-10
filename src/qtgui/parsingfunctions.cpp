#ifndef PARSING_FUNCTIONS_H
#define PARSINGFUNCTIONS_H
#endif

#include <fstream>
#include <iostream>
#include <cstring>

//Takes wrapper functions.cpp, parses the file and generates ap_list.h, which has the list of all functions
//in a format readable by the text editor widget for auto completion
using namespace std;

int create_apih_from_wrapper()
{
    //fill this with the pathname of wrapper_functions
    ifstream fin("functions.txt", ios::in);

    //change to whatever is required for output file
    ofstream fout("api_list.h", ios::out|ios::trunc);

    char ln[200];
    char * rest;

    if(fin.bad())
    {
        cerr << "input file is bad" << endl;
        return -1;
    }

    if(fout.bad())
    {
        cerr << "output file is bad" << endl;
        return -1;
    }


    while(fin)
    {
        if(fin.bad()||fin.eof())
        {
            cerr << "input file is bad" << endl;
            return -1;
        }

        fin.getline(ln, 200);

        rest = strstr(ln, "Entity");
        if(rest != NULL)
        {
            //cout << "Found Entity" << endl;
            break;
        }
    }

    fout << "//This is the list of functions for auto-complete." << endl;
    fout << endl << "api_names" << endl;

    char* begin_posn;
    int i;

    while(1)
    {
        if(fin.eof())
        {
            break;
        }
        fin.getline(ln, 200);

        begin_posn = strchr(ln, '\"');
        if(begin_posn == NULL)
        {
            continue;
        }
        i=0;
        fout << "<< ";
        while(1)
        {
            fout << begin_posn[i];
            i++;
            if(begin_posn[i] == '\"')
            {
                fout << begin_posn[i] << endl;
                break;
            }
        }
    }

    //adding the python commands for autocomplete as well
    fout <<
    "\n<< \"and\" \n<< \"as\" \n<< \"assert\" \n<< \"break\" \n<< \"class\" \n<< \"continue\" \n<< \"def\" \n<< \"del\" \n<< \"elif\" \n<< \"else\" \n<< \"except\" \n<< \"exec\" \n<< \"finally\" \n<< \"for\" \n<< \"from\" \n<< \"global\" \n<< \"if\" \n<< \"import\" \n<< \"in\" \n<< \"is\" \n<< \"lambda\" \n<< \"not\" \n<< \"or\" \n<< \"pass\" \n<< \"print\" \n<< \"raise\" \n<< \"return\" \n<< \"try\" \n<< \"while\" \n<< \"with\" \n<< \"yield\"\n;"
    ;

    fin.close();
    fout.close();
    return 0;
}
