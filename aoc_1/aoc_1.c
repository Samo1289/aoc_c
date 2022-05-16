#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// I assume all the console printing functions always work (if not for some
/// special occasions) and I will check return value of other functions

/// TODO C returns

int32_t safe_add(int32_t *const a, int32_t const b) {
  assert(a);

  int32_t prev = *a;
  *a += b;
  if (prev > *a) {
    fprintf(stderr, "Calculation overflowed...\n");
    return -1; // overflow occurred
  }
  return 0;
}

typedef struct {
  int32_t *buffer_ptr;
  uint32_t buffer_size;
  uint32_t buffer_current_index;
  int32_t buffer_sum;
  int8_t buffer_full;
} SlidingSumBuffer;

int32_t create_buffer(SlidingSumBuffer *buffer_inst,
                      uint32_t const buffer_size) {

  /// allocate buffer for the sliding window
  /// NOTE: I am aware that allocating buffer for 1 or 3 elements is not a great
  /// idea (even in computer world, not just embedded), but this is just to
  /// demonstrate a "theoretical" solution

  if (buffer_size == 0) {
    return -1;
  }

  buffer_inst->buffer_ptr = malloc(sizeof(uint32_t) * buffer_size);
  if (!buffer_inst->buffer_ptr) {
    return -1;
  }

  buffer_inst->buffer_size = buffer_size;
  buffer_inst->buffer_current_index = 0;
  buffer_inst->buffer_sum = 0;
  buffer_inst->buffer_full = 0;

  return 0;
}

int32_t
add_to_buffer(SlidingSumBuffer *buffer_inst, int32_t const value) {
  assert(buffer_inst->buffer_ptr);

  // if not buffer full
  if (!buffer_inst->buffer_full) {
    buffer_inst->buffer_ptr[buffer_inst->buffer_current_index++] = value;
    if (safe_add(&buffer_inst->buffer_sum, value)) {
      return -1;
    }
    if (buffer_inst->buffer_current_index >= (buffer_inst->buffer_size - 1)) {
      buffer_inst->buffer_full = 1;
    }
  } else {
    if (++buffer_inst->buffer_current_index > (buffer_inst->buffer_size - 1)) {
      buffer_inst->buffer_current_index = 0;
    }
    buffer_inst->buffer_sum -=
        buffer_inst->buffer_ptr[buffer_inst->buffer_current_index];
    if (safe_add(&buffer_inst->buffer_sum, value)) {
      return -1;
    }
    buffer_inst->buffer_ptr[buffer_inst->buffer_current_index] = value;
  }
  return 0;
}

void reset_buffer(SlidingSumBuffer *buffer_inst) {
  free(buffer_inst->buffer_ptr);
  buffer_inst->buffer_size = 0;
  buffer_inst->buffer_sum = 0;
  buffer_inst->buffer_full = 0;
  buffer_inst->buffer_current_index = 0;
}

int32_t solution_with_buffer(FILE *file_ptr, int32_t const window_size) {

  SlidingSumBuffer buffer;

  if (0 != create_buffer(&buffer, window_size)) {
    fprintf(stderr, "Could not create buffer for sliding window\n");
    return -1;
  }

  int32_t increases = 0;

  // TODO explain ensure no underflow on empty file will occur
  int32_t previous = INT32_MIN + 1;

  char line[16] = {0};

  // get number line by line
  while (fgets(line, sizeof(line), file_ptr)) {

    // trim out newline
    line[strcspn(line, "\n")] = 0;
    int32_t number = (int32_t)strtol(line, NULL, 10);
// todo readout strtol
    if (!add_to_buffer(&buffer, number)) {
      if (buffer.buffer_full) {
        if (previous < buffer.buffer_sum) {
          if (safe_add(&increases, 1)) {
            goto ERR;
          }
        }
        previous = buffer.buffer_sum;
      }
    } else {
      fprintf(stderr, "Buffer sum overflowed\n");
      goto ERR;
    }
  }

  // cleanup
  reset_buffer(&buffer);

  // this is to remove first change from the first value.
  // also, as this happens everytime, INT32_MIN + 1 is selected as initial value
  // to still be in negative number if no increase occurs or the file is empty.
  --increases;

  // we should be returning only positive number in regard to problem nature
  return ((increases >= 0) ? increases : -1);

ERR:
  reset_buffer(&buffer);
  return -1;
}

int main() {

  FILE *file_ptr = fopen("input.txt", "r"); // open file for reading

  if (errno) { // error occurred during opening the file
    perror("Error during fopen");
    goto ERR;
  }

  int32_t const question_1_window_size = 1;
  int32_t result = solution_with_buffer(file_ptr, question_1_window_size);

  if (0 > result) {
    fprintf(stderr, "Program failed...\n");
    goto ERR;
  }
  printf("result to question 1 is: %d\n", result);

  rewind(file_ptr); // let's read the file again

  int32_t const question_2_window_size = 3;
  result = solution_with_buffer(file_ptr, question_2_window_size);

  if (0 > result) {
    fprintf(stderr, "Program failed...\n");
    goto ERR;
  }
  printf("result to question 2 is: %d\n", result);

  if (fclose(file_ptr)) { // close the file
    perror("Error during fclose");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;

ERR:
  if (fclose(file_ptr)) { // close the file
    perror("Error during fclose");
  }
  return EXIT_FAILURE;
}
