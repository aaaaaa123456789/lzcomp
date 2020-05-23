#include <stdio.h>

int main (void) {
  unsigned short count;
  unsigned char input, command;
  unsigned short address = 0;
  while (!feof(stdin)) {
    input = getchar();
    if (input == 255) break;
    if (input < 224) {
      command = input >> 5;
      count = (input & 31) + 1;
    } else {
      command = (input & 31) >> 2;
      count = ((((unsigned short) (input & 3)) << 8) | (unsigned char) (getchar())) + 1;
    }
    printf("%04hx: ", address);
    address += count;
    switch (command) {
      case 0:
        printf("data ");
        while (count --) printf("$%02hhx%s", (unsigned char) getchar(), count ? ", " : "\n");
        break;
      case 1:
        printf("repeat %hu, $%02hhx\n", count, (unsigned char) getchar());
        break;
      case 2:
        input = getchar();
        printf("repeat %hu, $%02hhx, $%02hhx\n", count, input, (unsigned char) getchar());
        break;
      case 3:
        printf("zero %hu\n", count);
        break;
      default:
        printf("copy %c, %hu, ", "NFR"[command - 4], count);
        input = getchar();
        if (input < 128)
          printf("$%02hhx%02hhx\n", input, (unsigned char) getchar());
        else
          printf("-%hhu\n", input - 127);
    }
  }
  return 0;
}
