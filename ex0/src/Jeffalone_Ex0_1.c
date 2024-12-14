#include <stdio.h>

int main(int argc, char *argv[]) {
  char name[80];
  if (argc == 2) {
    printf("Hello %s!\n", argv[1]);
    return 0;
  }
  printf("Please enter your name: ");
  scanf("%s", name);
  printf("Hello %s!\n", name);
  return 0;
}
