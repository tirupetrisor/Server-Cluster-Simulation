#ifndef SERVER_H
#define SERVER_H

void log_message(const char *message);
void main_server(int num_workers, const char *filename);

#endif