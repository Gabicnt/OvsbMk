#include "fat32.h"
#include "../drivers/ata.h"
#include "../kernel/memory.h"
#include <stdint.h>
#include "../kernel/colors.h"

extern void set_vga_color(uint8_t color);
extern void vga_putchar(char c);
extern void vga_puts(const char *s);

static uint32_t current_dir_cluster = 0;
static char current_path[256] = "/";
static uint32_t next_cluster = 3;

static fat32_boot_t boot;
static uint32_t first_data_sector;
static uint32_t sectors_per_cluster;

static char toupper(char c) {
    return (c >= 'a' && c <= 'z') ? (char)(c - 32) : c;
}

static void clear_bytes(uint8_t *buf, int count) {
    for (int i = 0; i < count; i++) buf[i] = 0;
}

static void set_root_path(void) {
    current_path[0] = '/';
    current_path[1] = '\0';
}

static void append_path_segment(const char *name) {
    int len = 0;
    while (current_path[len]) len++;

    if (len > 1) {
        current_path[len++] = '/';
    } else if (current_path[0] != '/') {
        current_path[0] = '/';
        len = 1;
    }

    for (int i = 0; name[i] && i < 11 && len < 255; i++) {
        if (name[i] != ' ') {
            current_path[len++] = name[i];
        }
    }
    current_path[len] = '\0';
}

static void pop_path_segment(void) {
    int last_slash = 0;
    for (int i = 1; current_path[i]; i++) {
        if (current_path[i] == '/') last_slash = i;
    }
    if (last_slash <= 0) set_root_path();
    else current_path[last_slash] = '\0';
}

static int is_valid_entry_name(const uint8_t *entry_name) {
    if (!entry_name) return 0;
    if (entry_name[0] == 0x00 || entry_name[0] == 0xE5) return 0;
    if (entry_name[0] == '.') return 0;

    for (int i = 0; i < 11; i++) {
        unsigned char c = entry_name[i];
        if (c == ' ' || c == 0) continue;
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-') continue;
        return 0;
    }
    return 1;
}

static int match_name(const char *input, const uint8_t *entry_name) {
    uint8_t expected[11];
    int i;

    if (!input || !entry_name) return 0;
    for (i = 0; i < 11; i++) expected[i] = ' ';

    const char *p = input;
    const char *dot = 0;
    while (*p) {
        if (*p == '.') {
            dot = p;
            break;
        }
        p++;
    }

    int base_len = 0;
    p = input;
    while (*p && *p != '.' && base_len < 8) {
        expected[base_len++] = (uint8_t)toupper(*p++);
    }

    if (dot) {
        int ext_len = 0;
        p = dot + 1;
        while (*p && ext_len < 3) {
            expected[8 + ext_len++] = (uint8_t)toupper(*p++);
        }
    }

    for (i = 0; i < 11; i++) {
        if (expected[i] != entry_name[i]) return 0;
    }
    return 1;
}

static void name_to_fat32(const char *input, uint8_t *output) {
    int i;
    for (i = 0; i < 11; i++) output[i] = ' ';

    const char *p = input;
    const char *dot = 0;
    while (*p) {
        if (*p == '.') {
            dot = p;
            break;
        }
        p++;
    }

    int base_len = 0;
    p = input;
    while (*p && *p != '.' && base_len < 8) {
        output[base_len++] = (uint8_t)toupper(*p++);
    }

    if (dot) {
        int ext_len = 0;
        p = dot + 1;
        while (*p && ext_len < 3) {
            output[8 + ext_len++] = (uint8_t)toupper(*p++);
        }
    }
}

static uint32_t cluster_to_sector(uint32_t cluster) {
    return first_data_sector + (cluster - 2) * sectors_per_cluster;
}

int fat32_init(void) {
    uint8_t sector[512];
    if (ata_read_sector(0, sector) != 0) return -1;
    for (int i = 0; i < (int)sizeof(fat32_boot_t); i++) ((uint8_t *)&boot)[i] = sector[i];
    if (boot.boot_sector_signature != 0xAA55) return -1;
    sectors_per_cluster = boot.sectors_per_cluster;
    first_data_sector = boot.reserved_sectors + boot.num_fats * boot.sectors_per_fat_32;
    current_dir_cluster = boot.root_cluster;
    next_cluster = boot.root_cluster + 1;
    set_root_path();
    return 0;
}

int fat32_create_file(const char *name) {
    uint8_t sec[512];
    uint32_t sector;

    if (!name || !*name) return -1;
    sector = cluster_to_sector(current_dir_cluster);
    if (ata_read_sector(sector, sec) != 0) return -1;

    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    for (int i = 0; i < 16; i++) {
        if (e[i].name[0] == 0x00 || e[i].name[0] == 0xE5) {
            clear_bytes((uint8_t *)&e[i], sizeof(fat32_dir_entry_t));
            name_to_fat32(name, e[i].name);
            e[i].attr = 0x20;
            e[i].size = 0;
            return ata_write_sector(sector, sec);
        }
    }
    return -1;
}

int fat32_create_dir(const char *name) {
    uint8_t sec[512];
    uint32_t sector;
    int free_idx = -1;

    if (!name || !*name) return -1;
    sector = cluster_to_sector(current_dir_cluster);
    if (ata_read_sector(sector, sec) != 0) return -1;

    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    for (int i = 0; i < 16; i++) {
        if (e[i].name[0] == 0x00 || e[i].name[0] == 0xE5) {
            free_idx = i;
            break;
        }
    }
    if (free_idx < 0) return -1;

    uint32_t new_cluster = next_cluster++;
    clear_bytes((uint8_t *)&e[free_idx], sizeof(fat32_dir_entry_t));
    name_to_fat32(name, e[free_idx].name);
    e[free_idx].attr = 0x10;
    e[free_idx].size = 0;
    e[free_idx].first_cluster_low = new_cluster & 0xFFFF;
    e[free_idx].first_cluster_high = (new_cluster >> 16) & 0xFFFF;

    if (ata_write_sector(sector, sec) != 0) return -1;

    uint8_t new_sec[512];
    clear_bytes(new_sec, sizeof(new_sec));
    fat32_dir_entry_t *ne = (fat32_dir_entry_t *)new_sec;

    ne[0].name[0] = '.';
    ne[0].attr = 0x10;
    ne[0].size = 0;
    ne[0].first_cluster_low = new_cluster & 0xFFFF;
    ne[0].first_cluster_high = (new_cluster >> 16) & 0xFFFF;

    ne[1].name[0] = '.';
    ne[1].name[1] = '.';
    ne[1].attr = 0x10;
    ne[1].size = 0;
    ne[1].first_cluster_low = current_dir_cluster & 0xFFFF;
    ne[1].first_cluster_high = (current_dir_cluster >> 16) & 0xFFFF;

    return ata_write_sector(cluster_to_sector(new_cluster), new_sec);
}

int fat32_delete_file(const char *name) {
    uint8_t sec[512];
    uint32_t sector;

    if (!name || !*name) return -1;
    sector = cluster_to_sector(current_dir_cluster);
    if (ata_read_sector(sector, sec) != 0) return -1;

    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    for (int i = 0; i < 16; i++) {
        if (match_name(name, e[i].name) && e[i].name[0] != 0xE5 && e[i].name[0] != 0x00) {
            e[i].name[0] = 0xE5;
            return ata_write_sector(sector, sec);
        }
    }
    return -1;
}

int fat32_read_file(const char *name, uint8_t *buf, uint32_t size) {
    uint8_t sec[512];
    uint32_t sector;

    if (!name || !*name || !buf) return -1;
    sector = cluster_to_sector(current_dir_cluster);
    if (ata_read_sector(sector, sec) != 0) return -1;

    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    for (int i = 0; i < 16; i++) {
        if (match_name(name, e[i].name)) {
            uint32_t fs = e[i].size;
            if (fs == 0) return 0;
            uint32_t br = (size < fs) ? size : fs;
            uint32_t c = e[i].first_cluster_low | (e[i].first_cluster_high << 16);
            if (c < 2) return 0;

            uint8_t fsec[512];
            if (ata_read_sector(cluster_to_sector(c), fsec) != 0) return -1;
            if (br > 512) br = 512;
            for (uint32_t k = 0; k < br; k++) buf[k] = fsec[k];
            return (int)br;
        }
    }
    return -1;
}

int fat32_write_file(const char *name, const uint8_t *buf, uint32_t size) {
    uint8_t sec[512];
    uint32_t sector;

    if (!name || !*name || !buf) return -1;
    sector = cluster_to_sector(current_dir_cluster);
    if (ata_read_sector(sector, sec) != 0) return -1;

    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    for (int i = 0; i < 16; i++) {
        if (match_name(name, e[i].name)) {
            uint32_t c = next_cluster++;
            uint32_t bw = (size < 512) ? size : 512;
            uint8_t fsec[512];
            clear_bytes(fsec, sizeof(fsec));
            for (uint32_t k = 0; k < bw; k++) fsec[k] = buf[k];

            e[i].size = bw;
            e[i].first_cluster_low = c & 0xFFFF;
            e[i].first_cluster_high = (c >> 16) & 0xFFFF;
            if (ata_write_sector(cluster_to_sector(c), fsec) != 0) return -1;
            return ata_write_sector(sector, sec) == 0 ? (int)bw : -1;
        }
    }
    return -1;
}

int fat32_list_dir(void) {
    uint8_t sec[512];
    uint32_t sector;

    sector = cluster_to_sector(current_dir_cluster);
    if (ata_read_sector(sector, sec) != 0) return -1;

    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    int count = 0;
    for (int i = 0; i < 16; i++) {
        if (!is_valid_entry_name(e[i].name)) {
            if (e[i].name[0] == 0x00) break;
            continue;
        }

        if (e[i].attr & 0x10) {
            set_vga_color(C_DIR);
            vga_putchar('[');
        } else {
            set_vga_color(C_FILE);
        }

        for (int j = 0; j < 11; j++) {
            char c = e[i].name[j];
            if (c != ' ' && c != 0) vga_putchar(c);
        }

        if (e[i].attr & 0x10) vga_putchar(']');
        vga_putchar('\n');
        count++;
    }
    return count;
}

int fat32_change_dir(const char *name) {
    uint8_t sec[512];

    if (!name || !*name) return -1;
    if (name[0] == '/' && name[1] == '\0') {
        current_dir_cluster = boot.root_cluster;
        set_root_path();
        return 0;
    }
    if (name[0] == '.' && name[1] == '\0') return 0;
    if (name[0] == '.' && name[1] == '.' && name[2] == '\0') {
        if (ata_read_sector(cluster_to_sector(current_dir_cluster), sec) != 0) return -1;
        fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
        current_dir_cluster = e[1].first_cluster_low | (e[1].first_cluster_high << 16);
        pop_path_segment();
        return 0;
    }

    if (ata_read_sector(cluster_to_sector(current_dir_cluster), sec) != 0) return -1;
    fat32_dir_entry_t *e = (fat32_dir_entry_t *)sec;
    for (int i = 0; i < 16; i++) {
        if (match_name(name, e[i].name) && (e[i].attr & 0x10)) {
            current_dir_cluster = e[i].first_cluster_low | (e[i].first_cluster_high << 16);
            append_path_segment(name);
            return 0;
        }
    }
    return -1;
}

void fat32_get_path(char *buffer) {
    int i = 0;
    while (current_path[i]) {
        buffer[i] = current_path[i];
        i++;
    }
    buffer[i] = '\0';
}