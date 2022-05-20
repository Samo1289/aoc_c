#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// read number as binary
uint16_t
get_number(const char *input) {
  uint16_t number = (int16_t)strtol(input, NULL, 2);
  if (errno) { // EINVAL, ERANGE
    char msg[50];
    if (0 > snprintf(msg, 50, "Cannot convert %s to number", input)) {
      perror("snprintf failed");
    } else {
      perror(msg);
    }
    return INT16_MIN;
  }
  return number;
}

enum { word_width_max = 16 };
typedef struct {
  uint32_t log1_bit_position_count[word_width_max];
  uint32_t size;
  uint32_t allocated;
  uint16_t *data;
  uint16_t significant_digits;
} SolutionStorage;

int32_t
init_solution_storage(SolutionStorage *storage) {
  bzero(storage->log1_bit_position_count,
        sizeof(storage->log1_bit_position_count));
  storage->data = malloc(64 * sizeof(uint16_t));
  storage->size = 0;
  storage->significant_digits = 0;
  storage->allocated = 64;
  return storage->data == NULL;
}

int32_t
add_to_storage(SolutionStorage *storage, uint16_t number) {
  if (storage->size == storage->allocated) {
    const float phi = 1.618f;
    uint16_t *backup = storage->data;
    uint32_t new_allocated = (uint32_t)(((float)storage->allocated * phi));

    storage->data = realloc(storage->data, new_allocated * sizeof(uint16_t));
    if (!storage->data) {
      storage->data = backup;
      return -1;
    }
    storage->allocated = new_allocated;
  }
  storage->data[storage->size++] = number;
  for (uint32_t i = 0; i < word_width_max; i++) {
    storage->log1_bit_position_count[i] += (int)((number & (1 << i)) > 0);
  }
  return 0;
}

int32_t
populate_solution_storage(SolutionStorage *storage, FILE *file_ptr) {
  char line[word_width_max] = {0};
  if (init_solution_storage(storage)) {
    return -1;
  };

  if (fgets(line, sizeof(line), file_ptr)) {
    // trim out newline
    line[strcspn(line, "\n")] = 0;
    uint16_t number = get_number(line);
    if (add_to_storage(storage, number)) {
      abort();
    };

    // do it once
    storage->significant_digits = (uint32_t)strlen(line);

    // get rest
    while (fgets(line, sizeof(line), file_ptr)) {
      // trim out newline
      line[strcspn(line, "\n")] = 0;
      number = get_number(line);
      if (add_to_storage(storage, number)) {
        return -1;
      };
    }
  }
  return 0;
}

uint32_t solution1(SolutionStorage const *storage) {
  uint32_t const threshold = (storage->size / 2) + 1;
  uint16_t gamma = 0;

  for (uint32_t i = 0; i < (storage->significant_digits); i++) {
    if (storage->log1_bit_position_count[i] >= threshold) {
      gamma |= (1 << i);
    }
  }

  uint16_t mask = (uint16_t)((1u << (storage->significant_digits)) - 1);
  return gamma * ((~gamma) & mask);
}

int main() {

  FILE *file_ptr = fopen("input.txt", "r"); // open file for reading

  if (errno) { // error occurred during opening the file
    perror("Error during fopen");
    goto ERR;
  }

  SolutionStorage storage;
  if (init_solution_storage(&storage)) {
    fprintf(stderr, "Cannot create storage for solution");
    goto ERR;
  }

  if (populate_solution_storage(&storage, file_ptr)) {
    fprintf(stderr, "Cannot create storage for solution");
    goto ERR;
  }

  printf("solution for question 1: %d\n", solution1(&storage));
  // TODO solution 2

  free(storage.data);
  if (fclose(file_ptr)) { // close the file
    perror("Error during fclose");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;

ERR:
  free(storage.data);
  if (fclose(file_ptr)) { // close the file
    perror("Error during fclose");
  }
  return EXIT_FAILURE;
}
