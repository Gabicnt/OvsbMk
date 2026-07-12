#include "terminal.h"
#include "../../drivers/keyboard.h"
#include "../../commands/shell_cmds.h"

void execute_command(const char *cmd);

#define TERM_ROWS 32
#define TERM_COLS 80
#define TERM_LINE_HEIGHT 16
#define TERM_CHAR_WIDTH 8

static int term_x = 60;
static int term_y = 40;
static int term_w = 900;
static int term_h = 620;
static uint32_t bg_color = 0xFF202020;
static uint32_t border_color = 0xFFFFFFFF;
static uint32_t title_color = 0xFFFFFFFF;
static uint32_t text_color = 0xFFCCCCCC;
static uint32_t prompt_color = 0xFF88FF88;
static char term_lines[TERM_ROWS][TERM_COLS + 1];
static int term_line_count = 0;
static int term_cursor_row = 0;
static char input_buf[TERM_COLS + 1];
static int input_pos = 0;

static void terminal_clear_content_area(void) {
    vesa_draw_rect(term_x + 4, term_y + 24, term_w - 8, term_h - 32, bg_color);
}

static void terminal_draw_window(void) {
    vesa_draw_rect(term_x - 2, term_y - 2, term_w + 4, term_h + 4, border_color);
    vesa_draw_rect(term_x, term_y, term_w, term_h, bg_color);
    vesa_draw_text(term_x + 8, term_y + 4, "Ovsb Terminal", title_color);
}

static void terminal_scroll_lines(void) {
    for (int row = 1; row < TERM_ROWS; row++) {
        for (int col = 0; col <= TERM_COLS; col++) {
            term_lines[row - 1][col] = term_lines[row][col];
        }
    }
    term_lines[TERM_ROWS - 1][0] = '\0';
}

static void terminal_advance_line(void) {
    if (term_cursor_row + 1 >= TERM_ROWS) {
        terminal_scroll_lines();
        term_cursor_row = TERM_ROWS - 1;
    } else {
        term_cursor_row++;
    }
    if (term_line_count < TERM_ROWS) {
        term_line_count = term_cursor_row + 1;
    }
    term_lines[term_cursor_row][0] = '\0';
}

static void terminal_render_buffer(void) {
    for (int row = 0; row < term_line_count; row++) {
        int y = term_y + 24 + row * TERM_LINE_HEIGHT;
        vesa_draw_rect(term_x + 4, y, term_w - 8, TERM_LINE_HEIGHT, bg_color);
        vesa_draw_text(term_x + 8, y, term_lines[row], text_color);
    }
}

static void terminal_append_text(const char *text) {
    int len = 0;
    while (text[len] != '\0') {
        len++;
    }
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (c == '\n') {
            terminal_advance_line();
        } else if (c == '\r') {
            term_lines[term_cursor_row][0] = '\0';
        } else if (term_lines[term_cursor_row][0] != '\0' || c != ' ') {
            int pos = 0;
            while (term_lines[term_cursor_row][pos] != '\0') {
                pos++;
            }
            if (pos < TERM_COLS) {
                term_lines[term_cursor_row][pos] = c;
                term_lines[term_cursor_row][pos + 1] = '\0';
            }
        }
    }
    terminal_render_buffer();
}

static void terminal_draw_prompt(void) {
    int y = term_y + 24 + term_cursor_row * TERM_LINE_HEIGHT;
    vesa_draw_rect(term_x + 4, y, term_w - 8, TERM_LINE_HEIGHT, bg_color);
    vesa_draw_text(term_x + 8, y, "ovsb:/$ ", prompt_color);
    if (input_pos > 0) {
        vesa_draw_text(term_x + 8 + 8 * 8, y, input_buf, text_color);
    }
}

static void terminal_handle_enter(void) {
    input_buf[input_pos] = '\0';
    if (input_pos > 0) {
        char line[TERM_COLS + 1];
        int pos = 0;
        const char *prompt = "ovsb:/$ ";
        while (prompt[pos] != '\0' && pos < TERM_COLS) {
            line[pos] = prompt[pos];
            pos++;
        }
        int i = 0;
        while (input_buf[i] != '\0' && pos < TERM_COLS) {
            line[pos++] = input_buf[i++];
        }
        line[pos] = '\0';
        terminal_append_text(line);
        execute_command(input_buf);
    }
    input_pos = 0;
    input_buf[0] = '\0';
    terminal_advance_line();
    terminal_render_buffer();
    terminal_draw_prompt();
}

void terminal_app_init(void) {
    vesa_fill_screen(0xFF002233);
    terminal_draw_window();
    terminal_clear_content_area();
    for (int row = 0; row < TERM_ROWS; row++) {
        term_lines[row][0] = '\0';
    }
    term_line_count = 0;
    term_cursor_row = 0;
    input_buf[0] = '\0';
    input_pos = 0;
    terminal_draw_prompt();
}

void terminal_app_run(void) {
    while (1) {
        char c = keyboard_read();
        if (c == '\n') {
            terminal_handle_enter();
        } else if (c == '\b') {
            if (input_pos > 0) {
                input_pos--;
                input_buf[input_pos] = '\0';
                terminal_draw_prompt();
            }
        } else if (c >= ' ' && c <= '~' && input_pos < TERM_COLS - 1) {
            input_buf[input_pos++] = c;
            input_buf[input_pos] = '\0';
            terminal_draw_prompt();
        }
    }
}

void terminal_write(const char *text) {
    if (!text) return;
    terminal_append_text(text);
    terminal_draw_prompt();
}
