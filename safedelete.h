// safedelete.h
#ifndef SAFEDELETE_H
#define SAFEDELETE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define TRASH_DIR ".safedelete_trash"
#define MAX_PATH 1024
#define KEY 0x55 // Simple XOR key

// Core functions
void secure_delete(const char *file_path);
void list_trash();
void recover_file(int index);
void empty_trash(int days_old);

// Helpers
void encrypt_file(const char *path);
const char* get_filename(const char *path);
int create_trash_dir();

#endif
