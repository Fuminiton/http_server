#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int calculate(char *buf) {
    int num1, num2;
    char op;

    sscanf(buf, "%d%c%d", &num1, &op, &num2);
    switch (op) {
    case '+':
        return num1 + num2;
    case '-':
        return num1 - num2;
    case '*':
        return num1 * num2;
    case '/':
        if (num2 == 0) {
            puts("error: you can't use 0 for division.");
            break;
        }
        return num1 / num2;
    default:
        break;
    }

    return 0;
}

int main() {
    char buf[1024];

    strcpy(buf, "1+1");
    int answer = calculate(buf);
    printf("%s is %d\n", buf, answer);

    strcpy(buf, "5-2");
    answer = calculate(buf);
    printf("%s is %d\n", buf, answer);

    strcpy(buf, "2*6");
    answer = calculate(buf);
    printf("%s is %d\n", buf, answer);

    strcpy(buf, "12/3");
    answer = calculate(buf);
    printf("%s is %d\n", buf, answer);

    return 0;
}