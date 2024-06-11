#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

#define error(msg, ...) do {                    \
    fprintf(stderr, "Fatal Error: ");           \
    fprintf(stderr, msg, ##__VA_ARGS__);        \
    fprintf(stderr, ": (%zu, %zu) at %s:%d\n", tok->line_num, tok->line_off, __FUNCTION__, __LINE__); \
    fprintf(stderr, "\n"); \
    tok_report_line_error();\
    exit(1); \
  } while(0)

#define TOK_TYPE_EOF  0
#define TOK_TYPE_NUM  1
#define TOK_TYPE_SYM  2
#define TOK_TYPE_FUNC 3

#define TOKENS(_)\
  /* enum-symbol          token-literal  token-value */\
  _( TOK_INT,         "int",         6388       )\
  _( TOK_VOID,        "void",        11386      )\
  _( TOK_ASM,         "asm",         5631       )\
  _( TOK_START,       "_start()",    33977      )\
  _( TOK_SEMI,        ";",           11         )\
  _( TOK_DEREF,       "*(int*)",     64653      )\
  _( TOK_WHILE_BEGIN, "while(",      55810      )\
  _( TOK_IF_BEGIN,    "if(",         6232       )\
  _( TOK_BODY_BEGIN,  "){",          5          )\
  _( TOK_LPAREN,      "(",           65528      )\
  _( TOK_RPAREN,      ")",           65529      )\
  _( TOK_BLK_BEGIN,   "{",           75         )\
  _( TOK_BLK_END,     "}",           77         )\
  _( TOK_ASSIGN,      "=",           13         )\
  _( TOK_ADDR,        "&",           65526      )\
  _( TOK_SUB,         "-",           65533      )\
  _( TOK_ADD,         "+",           65531      )\
  _( TOK_MUL,         "*",           65530      )\
  _( TOK_OR,          "|",           76         )\
  _( TOK_XOR,         "^",           46         )\
  _( TOK_SHL,         "<<",          132        )\
  _( TOK_SHR,         ">>",          154        )\
  _( TOK_EQ,          "==",          143        )\
  _( TOK_NE,          "!=",          65399      )\
  _( TOK_LT,          "<",           12         )\
  _( TOK_GT,          ">",           14         )\
  _( TOK_LE,          "<=",          133        )\
  _( TOK_GE,          ">=",          153        )\

enum {
#define ELT(enum_sym, _2, token_val) enum_sym = ((u16)token_val),
  TOKENS(ELT)
#undef ELT
};

static const char *token_str(int val)
{
  switch (val) {
#define ELT(enum_sym, str, _3) case enum_sym: return str;
    TOKENS(ELT)
#undef ELT
  }
  return NULL;
}

static bool token_is_kw(int val)
{
  switch (val) {
#define ELT(enum_sym, str, _3) case enum_sym: return true;
    TOKENS(ELT)
#undef ELT
  }
  return false;
}

typedef struct token token_t;
struct token
{
  int type;
  u16 val;

  const char * text;
  size_t       len;

  const char * line_start;
  size_t       line_num;
  size_t       line_off;
};

#define TOK_FMT      "'%.*s'"
#define TOK_ARG(t)   (int)((t)->len), (t)->text

static char *  input_buf = NULL;
static char *  input_ptr = NULL;
static size_t  input_len = 0;
static token_t tok[1];

static void input_append_source_file(const char *path)
{
  FILE *fp = fopen(path, "r");
  if (!fp) {
    fprintf(stderr, "Failed to open file: %s\n", path);
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size_t file_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  size_t old_sz = input_len;
  size_t new_sz = old_sz + file_sz;

  input_buf = realloc(input_buf, new_sz);
  if (!input_buf) {
    fprintf(stderr, "Failed to allocate buffer for source text\n");
    exit(2);
  }

  size_t n = fread(input_buf + input_len, 1, file_sz, fp);
  if (n != file_sz) {
    fprintf(stderr, "Failed to read source file completely\n");
    exit(3);
  }

  fclose(fp);

  input_ptr = input_buf;
  input_len = new_sz;
  tok->line_start = input_buf;
}

static void tok_report_line_error(void)
{
  size_t line_len;
  char *s = strchr(tok->line_start, '\n');
  if (s) {
    line_len = s - tok->line_start;
  } else {
    line_len = strlen(tok->line_start);
  }

  fprintf(stderr, "%.*s\n", (int)line_len, tok->line_start);
  fprintf(stderr, "%*s^\n", (int)(tok->line_off - tok->len), "");
}

__attribute__((unused)) void tok_print()
{
  fprintf(stderr, "tok (%zu:%zu) | type %d | text '%.*s'\n", tok->line_num, tok->line_off, tok->type, (int)tok->len, tok->text);
}

int next_is_semi = 0;

char tok_char_get()
{
  if (next_is_semi) return ' ';
  else return *input_ptr;
}

void tok_char_next()
{
  if (next_is_semi) {
    next_is_semi = 0;
    return;
  }

  char last = *input_ptr;
  if (last == '\n') {
    tok->line_start = input_ptr + 1;
    tok->line_num++;
    tok->line_off = 0;
  } else {
    tok->line_off++;
  }
  input_ptr++;

  if (*input_ptr == ';') {
    next_is_semi = 1;
  }
}

static bool tok_lint_number(const char *text, size_t len)
{
  for (size_t i = 0; i < len; i++) {
    char c = text[i];
    if (!('0' <= c && c <= '9')) {
      return false;
    }
  }
  return true;
}

static bool tok_lint_ident(const char *text, size_t len)
{
  for (size_t i = 0; i < len; i++) {
    char c = text[i];
    if (!(c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (i != 0 && '0' <= c && c <= '9'))) {
      return false;
    }
  }
  return true;
}

static bool tok_lint_func_name(const char *text, size_t len)
{
  if (len <= 2 || 0 != memcmp(&text[len-2], "()", 2)) {
    return false;
  }
  return tok_lint_ident(text, len-2);
}

static void tok_lint_matches(const char *str)
{
  size_t len = strlen(str);
  if (tok->len != len || 0 != memcmp(tok->text, str, len)) {
    error("token (%u) collision between " TOK_FMT " and '%s'", tok->val, TOK_ARG(tok), str);
  }
}

static void tok_lint()
{
  switch (tok->type) {
    case TOK_TYPE_EOF: break;
    case TOK_TYPE_NUM: {
      if (!tok_lint_number(tok->text, tok->len)) {
        error("token (%u) parsed as int, but it is not: " TOK_FMT, tok->val, TOK_ARG(tok));
      }
    } break;
    case TOK_TYPE_SYM: {
      switch (tok->val) {
#define ELT(enum_sym, str, _3) case enum_sym: tok_lint_matches(str); return;
        TOKENS(ELT)
#undef ELT
      }
      // If we've reached this point, it's not a special token so it must be an ordinary variable identifier
      if (!tok_lint_ident(tok->text, tok->len)) {
        error("token (%u) parsed as variable identifier, but it is not: " TOK_FMT, tok->val, TOK_ARG(tok));
      }
    } break;
    case TOK_TYPE_FUNC: {
      if (!tok_lint_func_name(tok->text, tok->len)) {
        error("token (%u) parsed as func name, but it is not: " TOK_FMT, tok->val, TOK_ARG(tok));
      }
    } break;
  }
}

static void tok_next(void)
{
  while (1) {
    char c = tok_char_get();

    // Handle EOF
    if (c == 0) {
      tok->type = TOK_TYPE_EOF;
      tok->len = 0;
      return;
    }

    // Skip whitespace
    if (c <= ' ') {
      tok_char_next();
      continue;
    }

    tok->type = ('0' <= c && c <= '9') ? TOK_TYPE_NUM : TOK_TYPE_SYM;
    tok->text = input_ptr;
    tok->len = 1;
    tok->val = c - '0';

    bool slash = c == '/';
    tok_char_next();
    c = tok_char_get();

    // single-line comment?
    if (slash && c == '/') {
      tok_char_next();
      c = tok_char_get();
      if (c > ' ') error("expected space after // comment");
      while (1) {
        if (c == '\n') break;
        tok_char_next();
        c = tok_char_get();
      }
      continue; // try tok_next() again
    }

    // multi-line comment?
    if (slash && c == '*') {
      tok_char_next();
      c = tok_char_get();
      if (c > ' ') error("expected space after /* comment");

      // ending "*/" must be proceeded by a space
      int last_space_off = 0;
      bool last_is_asterisk = false;
      while (1) {
        tok_char_next();
        c = tok_char_get();
        if (last_is_asterisk && c == '/') {
          if (last_space_off != 1) error("expected space before */ comment terminator");
          /* Found, done! */
          break;
        }
        last_space_off++;
        last_is_asterisk = c == '*';
        if (c <= ' ') {
          last_space_off = 0;
        }
      }
      tok_char_next();
      continue;
    }

    // normal token
    while (c > ' ') {
      tok->len++;
      tok->val = 10 * tok->val + c - '0';
      tok_char_next();
      c = tok_char_get();
    }

    // special logic to detect function names
    if (tok->len > 2 && 0 == memcmp(&tok->text[tok->len-2], "()", 2)) {
      tok->type = TOK_TYPE_FUNC;
    }

    // lint the token to detect errors
    tok_lint();
    return;
  }
}

static bool tok_num_is(void)
{
  return tok->type == TOK_TYPE_NUM;
}

static void tok_num_expect(void)
{
  if (!tok_num_is()) {
    error("expected num token");
  }
  tok_next();
}

static bool tok_kw_is(u16 val)
{
  return
    tok->type == TOK_TYPE_SYM &&
    tok->val == val &&
    token_is_kw(val);
}

static void tok_kw_expect(u16 val)
{
  if (!tok_kw_is(val)) {
    error("expected keyword token '%s' (%u)", token_str(val), val);
  }
  tok_next();
}

static bool tok_ident_is(void)
{
  return
    tok->type == TOK_TYPE_SYM &&
    !token_is_kw(tok->val) &&
    tok_lint_ident(tok->text, tok->len);
}

static void tok_ident_expect(void)
{
  if (!tok_ident_is()) {
    error("expected identifier token");
  }
  tok_next();
}

static bool tok_func_is(void)
{
  return tok->type == TOK_TYPE_FUNC;
}

static void tok_func_expect(void)
{
  if (!tok_func_is()) {
    error("expected func token");
  }
  tok_next();
}

static bool tok_oper_is(void)
{
  return
    tok_kw_is(TOK_ADD) ||
    tok_kw_is(TOK_SUB) ||
    tok_kw_is(TOK_MUL) ||
    tok_kw_is(TOK_ADDR) ||  // "AND" in this context
    tok_kw_is(TOK_OR) ||
    tok_kw_is(TOK_XOR) ||
    tok_kw_is(TOK_SHL) ||
    tok_kw_is(TOK_SHR) ||
    tok_kw_is(TOK_EQ) ||
    tok_kw_is(TOK_NE) ||
    tok_kw_is(TOK_LT) ||
    tok_kw_is(TOK_GT) ||
    tok_kw_is(TOK_LE) ||
    tok_kw_is(TOK_GE);
}

// fwd decl needed for paren grouping recursion
static void parse_expr(void);

// unary = deref identifier
//       | "&" identifier
//       | "(" expr ")"
//       | identifier
//       | integer
static void parse_unary(void)
{
  if (tok_kw_is(TOK_DEREF)) {
    tok_next(); // consume TOK_DEREF
    tok_ident_expect();
  }

  else if (tok_kw_is(TOK_ADDR)) {
    tok_next(); // consume TOK_ADDR
    tok_ident_expect();
  }

  else if (tok_kw_is(TOK_LPAREN)) {
    tok_next(); // consume TOK_LPAREN
    parse_expr();
    tok_kw_expect(TOK_RPAREN);
  }

  else if (tok_ident_is()) {
    tok_next(); // consume identifier
  }

  else if (tok_num_is()) {
    tok_next(); // consume number
  }

  else {
    error("expected unary expression");
  }
}

// expr = unary (op unary)?
static void parse_expr(void)
{
  parse_unary();
  if (tok_oper_is()) {
    tok_next(); // consume the operator
    parse_unary();
  }
}

// assign_expr = deref? identifier "=" expr
static void parse_assign_expr(void)
{
  // optionally we have a deref token
  if (tok_kw_is(TOK_DEREF)) {
    tok_next();
  }

  tok_ident_expect();
  tok_kw_expect(TOK_ASSIGN);
  parse_expr();
}

// statement = "if(" expr "){" statement* "}"
//           | "while(" expr "){" statement* "}"
//           | "asm" integer ";"
//           | func_name ";"
//           | assign_expr ";"
static void parse_statement(void)
{
  if (tok_kw_is(TOK_IF_BEGIN)) {
    tok_next(); // consume TOK_IF_BEGIN
    parse_expr();
    tok_kw_expect(TOK_BODY_BEGIN);
    while (!tok_kw_is(TOK_BLK_END)) {
      parse_statement();
    }
    tok_next(); // consume TOK_BODY_BEGIN
  }

  else if (tok_kw_is(TOK_WHILE_BEGIN)) {
    tok_next(); // consume TOK_WHILE_BEGIN
    parse_expr();
    tok_kw_expect(TOK_BODY_BEGIN);
    while (!tok_kw_is(TOK_BLK_END)) {
      parse_statement();
    }
    tok_next(); // consume TOK_BODY_BEGIN
  }

  else if (tok_kw_is(TOK_ASM)) {
    tok_next(); // consume TOK_ASM
    tok_num_expect();
    tok_kw_expect(TOK_SEMI);
  }

  else if (tok_func_is()) {
    tok_next(); // consume func name
    // TODO: validate against a symbol table
    tok_kw_expect(TOK_SEMI);
  }

  else { // default case: an assignment expression
    parse_assign_expr();
    tok_kw_expect(TOK_SEMI);
  }
}

// var_decl = "int" identifier ";"
static void parse_var_decl(void)
{
  tok_kw_expect(TOK_INT);
  tok_ident_expect();
  // TODO: Build and check a symbol table of globals
  tok_kw_expect(TOK_SEMI);
}

// func_decl = "void" func_name "{" statement* "}"
static void parse_func_decl(void)
{
  tok_kw_expect(TOK_VOID);
  tok_func_expect();
  // TODO: Build and check a symbol table of functions
  tok_kw_expect(TOK_BLK_BEGIN);
  while (!tok_kw_is(TOK_BLK_END)) {
    parse_statement();
  }
  tok_kw_expect(TOK_BLK_END);
}

// program = (var_decl | func_decl)+
static void parse_program(void)
{
  while (tok->type != TOK_TYPE_EOF) {
    if (tok_kw_is(TOK_INT)) {
      parse_var_decl();
    }
    else if (tok_kw_is(TOK_VOID)) {
      parse_func_decl();
    }
    else {
      error("expected var decl or func decl");
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "usage: %s <source-file-1> <source-file-2> ...\n", argv[0]);
    return 1;
  }

  for (size_t i = 1; i < (size_t)argc; i++) {
    input_append_source_file(argv[i]);
  }

  tok_next();
  parse_program();

  free(input_buf);
  return 0;
}
