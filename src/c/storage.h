#pragma once
#include <pebble.h>

#define STORAGE_KEY_HISTORY 1
#define MAX_HISTORY_ENTRIES 10

typedef struct {
  int total_spent;
  int entries[MAX_HISTORY_ENTRIES];
  int entry_count;
} SpendingHistory;