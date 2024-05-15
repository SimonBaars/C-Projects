#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include "ast.h"

void initialize(void);

struct tree_node;
void run_command(struct tree_node* n);

int handlePipeCommand(command_t cmd, int writePntr, bool doWait);
int parseNode(node_t* node, int writePntr, bool doWait);
void handleResult(int processId);
void showFD(int writePntr);
void addPIDToCurrentChilds(int pid);

#endif
