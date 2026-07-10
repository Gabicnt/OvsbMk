#include "fat32.h"
#include "../drivers/ata.h"
#include "../kernel/memory.h"
#include <stdint.h>

extern void vga_putchar(char c);
extern void vga_puts(const char *s);

static fat32_boot_t boot;
static uint32_t first_data_sector;
static uint32_t sectors_per_cluster;

int fat32_init(void) {
    uint8_t sector[512];
    if (ata_read_sector(0, sector) != 0) return -1;
    
    for (int i = 0; i < sizeof(fat32_boot_t); i++) {
        ((uint8_t*)&boot)[i] = sector[i];
    }
    
    if (boot.boot_sector_signature != 0xAA55) return -1;
    
    sectors_per_cluster = boot.sectors_per_cluster;
    first_data_sector = boot.reserved_sectors + boot.num_fats * boot.sectors_per_fat_32;
    
    return 0;
}

int fat32_create_file(const char *name) {
    uint32_t root_cluster = boot.root_cluster;
    uint32_t root_sector = first_data_sector + (root_cluster - 2) * sectors_per_cluster;
    
    uint8_t dir_sector[512];
    if (ata_read_sector(root_sector, dir_sector) != 0) return -1;
    
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)dir_sector;
    
    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5) {
            for (int j = 0; j < 11; j++) {
                entries[i].name[j] = (name[j]) ? name[j] : ' ';
            }
            entries[i].attr = 0x20;
            entries[i].size = 0;
            entries[i].first_cluster_low = 0;
            entries[i].first_cluster_high = 0;
            
            if (ata_write_sector(root_sector, dir_sector) != 0) return -1;
            return 0;
        }
    }
    return -1;
}

int fat32_delete_file(const char *name) {
    uint32_t root_cluster = boot.root_cluster;
    uint32_t root_sector = first_data_sector + (root_cluster - 2) * sectors_per_cluster;
    
    uint8_t dir_sector[512];
    if (ata_read_sector(root_sector, dir_sector) != 0) return -1;
    
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)dir_sector;
    
    for (int i = 0; i < 16; i++) {
        int match = 1;
        for (int j = 0; j < 11; j++) {
            char c = (name[j]) ? name[j] : ' ';
            if (entries[i].name[j] != c) { match = 0; break; }
        }
        if (match && entries[i].name[0] != 0xE5 && entries[i].name[0] != 0x00) {
            entries[i].name[0] = 0xE5;
            if (ata_write_sector(root_sector, dir_sector) != 0) return -1;
            return 0;
        }
    }
    return -1;
}

int fat32_read_file(const char *name, uint8_t *buffer, uint32_t size) {
    uint32_t root_cluster = boot.root_cluster;
    uint32_t root_sector = first_data_sector + (root_cluster - 2) * sectors_per_cluster;
    
    uint8_t dir_sector[512];
    if (ata_read_sector(root_sector, dir_sector) != 0) return -1;
    
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)dir_sector;
    
    for (int i = 0; i < 16; i++) {
        int match = 1;
        for (int j = 0; j < 11; j++) {
            char c = (name[j]) ? name[j] : ' ';
            if (entries[i].name[j] != c) { match = 0; break; }
        }
        if (match) {
            uint32_t file_size = entries[i].size;
            uint32_t bytes_to_read = (size < file_size) ? size : file_size;
            uint32_t cluster = entries[i].first_cluster_low | (entries[i].first_cluster_high << 16);
            uint32_t sector = first_data_sector + (cluster - 2) * sectors_per_cluster;
            uint8_t file_sector[512];
            if (ata_read_sector(sector, file_sector) != 0) return -1;
            for (uint32_t k = 0; k < bytes_to_read && k < 512; k++) {
                buffer[k] = file_sector[k];
            }
            return bytes_to_read;
        }
    }
    return -1;
}

int fat32_write_file(const char *name, const uint8_t *buffer, uint32_t size) {
    uint32_t root_cluster = boot.root_cluster;
    uint32_t root_sector = first_data_sector + (root_cluster - 2) * sectors_per_cluster;
    
    uint8_t dir_sector[512];
    if (ata_read_sector(root_sector, dir_sector) != 0) return -1;
    
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)dir_sector;
    
    for (int i = 0; i < 16; i++) {
        int match = 1;
        for (int j = 0; j < 11; j++) {
            char c = (name[j]) ? name[j] : ' ';
            if (entries[i].name[j] != c) { match = 0; break; }
        }
        if (match) {
            entries[i].size = size;
            entries[i].first_cluster_low = 3;
            entries[i].first_cluster_high = 0;
            uint32_t data_sector = first_data_sector + (3 - 2) * sectors_per_cluster;
            uint8_t file_sector[512] = {0};
            uint32_t bytes_to_write = (size < 512) ? size : 512;
            for (uint32_t k = 0; k < bytes_to_write; k++) {
                file_sector[k] = buffer[k];
            }
            ata_write_sector(data_sector, file_sector);
            ata_write_sector(root_sector, dir_sector);
            return bytes_to_write;
        }
    }
    return -1;
}

int fat32_list_dir(void) {
    uint32_t root_cluster = boot.root_cluster;
    uint32_t root_sector = first_data_sector + (root_cluster - 2) * sectors_per_cluster;
    
    uint8_t dir_sector[512];
    if (ata_read_sector(root_sector, dir_sector) != 0) return -1;
    
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)dir_sector;
    int count = 0;
    
    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        if (entries[i].attr == 0x0F) continue;
        
        for (int j = 0; j < 11; j++) {
            char c = entries[i].name[j];
            if (c != ' ') vga_putchar(c);
        }
        vga_putchar('\n');
        count++;
    }
    return count;
}
