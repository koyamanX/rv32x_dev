/* environ is defined as external variable at unistd.h */
char *__env[1] = {0};
char **environ = __env;
