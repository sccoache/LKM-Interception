#ifndef ANCESTRY_GUARD
#define ANCESTRY_GUARD

struct ancestry{
	pid_t target[10];
    pid_t ancestors[10];
    //int a;

    pid_t siblings[100];
    pid_t children[100];
};

#endif