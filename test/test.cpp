
#include <stdio.h>

#include "getoptpp.h"

using namespace std;
using namespace getoptpp;



int main(int argc, char **argv)
{

    const char *opt_str = "h[help] v[loglevel]:";

    parser R(opt_str, argc, argv);

    if(!R)
    {
	printf("ERROR: %s\n", R().error.c_str());
	return -1;
    };

    auto &r = R();
    printf("h: %s\n", r['h'].c_str());
    printf("v: %s\n", r['v'].c_str());
    printf("\n");

    for(auto &x: r.argv)
	printf("Arg: '%s'\n", x.c_str());

    return 0;
};
