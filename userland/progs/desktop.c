static long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2), "d"(a3) : "memory", "rcx", "r11");
    return ret;
}
static void write_str(const char *s) {
    long len = 0;
    while (s[len]) len++;
    syscall3(2, 1, (long)s, len);
}
int main(void) {
    write_str("[desktop] ring3 iniciou\n");
    for (;;) __asm__ volatile("pause");
    return 0;
}
