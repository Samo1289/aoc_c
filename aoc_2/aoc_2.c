#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char const *token;
  int const token_length;
} Token;

Token const forward_token = {.token = "forward", .token_length = 8};

Token const down_token = {.token = "down", .token_length = 5};

Token const up_token = {.token = "up", .token_length = 3};

typedef struct {
  int32_t horizontal;
  int32_t depth;
  int32_t aim;
} Position;

int32_t get_number(const char *input) {
  int32_t number = (int32_t)strtol(input, NULL, 10);
  if (errno) { // EINVAL, ERANGE
    char msg[50];
    if (0 > snprintf(msg, 50, "Cannot convert %s to number", input)) {
      perror("snprintf failed");
    } else {
      perror(msg);
    }
    return INT32_MIN;
  }
  return number;
}

typedef void (*TokenPolicy)(Token, int32_t number, Position *position);

int32_t
compute(char const *line, Position* position, TokenPolicy token_policy) {
  Token const known_tokens[] = {forward_token, down_token, up_token};
  for (int32_t i = 0; i < (int32_t)(sizeof(known_tokens) / sizeof(Token));
       i++) {
    if (strstr(line, known_tokens[i].token)) {
      int32_t number = get_number(line + known_tokens[i].token_length);
      if (INT32_MIN == number) {
        return -1;
      }
      token_policy(known_tokens[i], number, position);
      break;
    }
  }
  return 0;
}

void
sol2_token_policy(Token token,
                  int32_t number,
                  Position *position) {
  if (!strcmp(token.token, forward_token.token)) {
    position->horizontal += number;
    position->depth += position->aim * number;
  } else if (!strcmp(token.token, up_token.token)) {
    position->aim -= number;
  } else if (!strcmp(token.token, down_token.token)) {
    position->aim += number;
  }
}

void
sol1_token_policy(Token token,
                  int32_t number,
                  Position *position) {
  if (!strcmp(token.token, forward_token.token)) {
    position->horizontal += number;
  } else if (!strcmp(token.token, up_token.token)) {
    position->depth -= number;
  } else if (!strcmp(token.token, down_token.token)) {
    position->depth += number;
  }
}

int64_t
solution(FILE *file_ptr, TokenPolicy token_policy) {
  Position position = {0};

  char line[16] = {0};
  // get number line by line
  while (fgets(line, sizeof(line), file_ptr)) {
    // trim out newline
    line[strcspn(line, "\n")] = 0;
    compute(line, &position, token_policy);
  }

  return position.depth * position.horizontal;
}


int main() {

  FILE *file_ptr = fopen("input.txt", "r"); // open file for reading
  if (errno) { // error occurred during opening the file
    perror("Error during fopen");
    goto ERR;
  }

  printf("solution part 1 is: %lld\n", solution(file_ptr, sol1_token_policy));
  rewind(file_ptr);
  printf("solution part 2 is: %lld\n", solution(file_ptr, sol2_token_policy));

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

// TODO commentaries, cleanup, error codes
