#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>


/*
14+(2+13)+4*2
tokenize
parse
calculate
*/

typedef enum {
    TK_RESERVED,  // Symbol
    TK_NUM,       // Integer token
    TK_EOF,       // End-of-File token
} TokenKind;

typedef enum {
    ND_ADD,       // +
    ND_SUB,       // -
    ND_MUL,       // *
    ND_DIV,       // /
    ND_NUM,       // number
} NodeKind;

typedef struct Token Token;
typedef struct Node Node;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

char *user_input;
Token *token;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int position = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", position, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

Token *new_token(TokenKind kind, Token *current_token, char *str) {
    Token *new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    current_token->next = new_token;
    return new_token;
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *current_token = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (strchr("+-*/()", *p)) {
            current_token = new_token(TK_RESERVED, current_token, p);
            p++;
            continue;
        }
        if (isdigit(*p)) {
            current_token = new_token(TK_NUM, current_token, p);
            current_token->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "Invalid token");
    }
    new_token(TK_EOF, current_token, p);
    return head.next;
}


bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        error_at(token->str, "expected '%c'", op);
    }
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "expected a number");
    }
    int num = token->val;
    token = token->next;
    return num;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int num) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = num;
    return node;
}

Node *expr();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
    Node *node = mul();
    for (;;) {
        if (consume('+')) {
            node = new_node(ND_ADD, node, mul());
            continue;
        }
        if (consume('-')) {
            node = new_node(ND_SUB, node, mul());
            continue;
        }
        return node;
    }
}

Node *mul() {
    Node *node = unary();
    for (;;) {
        if (consume('*')) {
            node = new_node(ND_MUL, node, unary());
            continue;
        }
        if (consume('/')) {
            node = new_node(ND_DIV, node, unary());
            continue;
        }
        return node;
    }
}

Node *unary() {
    if (consume('-')) {
        return new_node(ND_DIV, new_node_num(0), primary());
    }
    return primary();
}

Node *primary() {
    if (consume('(')) {
        Node *node = expr();
        consume(')');
        return node;
    }
    return new_node_num(expect_number());
}

void gen(Node* node) {
}


int main(int argc, char* argv[]) {
    int answer;
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    //answer = evaluator(node);

    //printf("buf = %d\n", answer);

    return 0;
}