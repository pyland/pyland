#include <iostream>
#include <string>
using namespace std;

int main(){
    
    int w = 40;
    int h = 64;
    
    int size = 64;
    
    int x = 0;
    int y = 7;

    for(int i = 0; i<h; i++)
    {
        for(int j = 0; j<w; j++)
        {
            cout << "  " << "<object " << "name=" << "\"vine/vine_" << (i) << "_" << (j) << "\" " << "gid=\"101\" ";
            cout << "x=\"" << ((j+x) * size) << "\" y=\"" << ((i+y) * size) << "\" ";
            cout << "width=\"" << (size)	<< "\" height=\"" << (size) << "\"/>" << endl;
        }
    }
        
    return 0;
}	  


