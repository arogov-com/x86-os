// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include "task.h"
#include "syscounters.h"

extern scheduler_t scheduler_perf;

gdt_tss_t *tss_list, *current_tss;
process_t *proc_list, *current;
int proc_max;

void init_scheduler(void *process_list_addr, unsigned int maxproc) {
    proc_max = maxproc;
    proc_list = (process_t *) process_list_addr;

    memset(proc_list, 0, sizeof(process_t) * proc_max);
    proc_list->left = proc_list;
    proc_list->right = proc_list;
    proc_list->quantum = QUANTUM;
    proc_list->id = 0;
    proc_list->vtty = 0;
    proc_list->state = STATE_ACTIVE;
    current = proc_list;
}

void scheduler(void *context) {
    ++scheduler_perf.interrupts;
    --current->quantum;
    ++current->times;

    if(!current->quantum) {
        process_t *next = current;
        while(1) {
            next = next->right;
            if(current == next) {
                next->quantum = QUANTUM;
                return;
            }
            if(next->state == STATE_READY && next->id) {                                        // Switch task if task ID != 0 and next task is ready
                memcpy((void *)(next->context.esp - 0x20), &next->context, sizeof(context_t));  // Copy context into next task stack
                memcpy(&current->context, context, sizeof(context_t));                          // Copy current context to process's structure
                memcpy(context, &next->context, sizeof(context_t));                             // Copy from next process structure to current context

                next->state = STATE_ACTIVE;
                next->quantum = QUANTUM;

                current->state = STATE_READY;
                current->quantum = QUANTUM;
                current = next;

                ++scheduler_perf.switches;
                return;
            }
        }
    }
 }

int kthread(void *entry_point, void *stack, int vttyn) {
    asm volatile("cli");
    process_t *last = proc_list->left;                      //Last item in proc list place behind proc_list

    last->right = (process_t *)(last + sizeof(process_t));  //Create new item
    last->right->left = last;
    last->right->right = proc_list;
    proc_list->left = last->right;

    last->right->context.edi = 0;
    last->right->context.esi = 0;
    last->right->context.ebp = stack;
    last->right->context.esp = stack;
    last->right->context.ebx = 0;
    last->right->context.edx = 0;
    last->right->context.ecx = 0;
    last->right->context.eax = 0;
    last->right->context.eip = entry_point;
    last->right->context.cs = TASK_SEGMENT;
    last->right->context.eflags = proc_list->context.eflags | 1 << 9;

    last->right->id = last->id + 1;
    last->right->state = STATE_READY;
    last->right->quantum = QUANTUM;
    last->right->vtty = vttyn;
    last->right->times = 0;
    asm volatile("sti");
    return last->right->id;
}

process_t *get_process_table() {
    return proc_list;
}

int get_current_proc_vtty() {
    return current->vtty;
}

int get_proc_struct(int pid, process_t *store) {
    process_t *tmp = proc_list;
    while(tmp->id != pid) {
        tmp = tmp->right;
        if(tmp == proc_list) return -1;
    }
    memcpy(store, tmp, sizeof(process_t));
    return 1;
}

int get_proc_times(unsigned int pid) {
    process_t *tmp = proc_list;
    while(tmp->id != pid) {
        tmp = tmp->right;
        if(tmp == proc_list) return -1;
    }
    return tmp->times;
 }

int kill_kthread(int pid) {
    asm volatile("cli");
    process_t *tmp = proc_list;
    while (tmp->id != pid) {
        tmp = tmp->right;
        if(tmp == proc_list) return -1;
    }
    if(tmp != proc_list) {
        memcpy(tmp, tmp->right, sizeof(process_t));
        asm volatile("sti");
        return 0;
    }
    else {
        asm volatile("sti");
        return 1;
    }
}
