// safedelete.c
#include "safedelete.h"

// Extract filename from path
const char* get_filename(const char *path) {
    const char *p = path;
    while(*path) {
        if(*path == '/') p = path + 1;
        path++;
    }
    return p;
}

// Create trash directory if needed
int create_trash_dir() {
    struct stat st = {0};
    if (stat(TRASH_DIR, &st) {
        if (mkdir(TRASH_DIR, 0700) && errno != EEXIST) {
            perror("Error creating trash directory");
            return 0;
        }
    }
    return 1;
}

// Simple XOR encryption
void encrypt_file(const char *path) {
    FILE *fp = fopen(path, "rb+");
    if (!fp) return;

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        fseek(fp, -1, SEEK_CUR);
        fputc(ch ^ KEY, fp);
        fflush(fp);
        fseek(fp, 0, SEEK_CUR); // Fix position for next read
    }
    fclose(fp);
}

void decrypt_file(const char *path) {
    encrypt_file(path);  // XOR is symmetric
}

void secure_delete(const char *file_path) {
    if (!create_trash_dir()) return;
    
    // Generate unique trash filename
    char trash_path[MAX_PATH];
    time_t now = time(NULL);
    const char *filename = get_filename(file_path);
    snprintf(trash_path, MAX_PATH, "%s/%ld_%s", TRASH_DIR, now, filename);
    
    // Move file to trash
    if (rename(file_path, trash_path)) {
        perror("Error moving to trash");
        return;
    }
    
    // Encrypt the file
    encrypt_file(trash_path);
    printf("Securely deleted: %s\n", filename);
}

void list_trash() {
    if (!create_trash_dir()) return;

    DIR *dir = opendir(TRASH_DIR);
    if (!dir) {
        perror("Error opening trash directory");
        return;
    }

    printf("\n=== SafeDelete Trash ===\n");
    struct dirent *entry;
    int count = 0;
    
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", TRASH_DIR, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st)) continue;
        
        // Extract original filename
        const char *orig_name = strchr(entry->d_name, '_');
        if (orig_name) orig_name++;
        else orig_name = entry->d_name;
        
        printf("[%d] %s\n", ++count, orig_name);
        printf("    Deleted: %s", ctime(&st.st_mtime));
        printf("    Size: %ld KB\n", st.st_size / 1024);
    }
    closedir(dir);
}

void recover_file(int index) {
    if (!create_trash_dir() || index < 1) return;

    DIR *dir = opendir(TRASH_DIR);
    if (!dir) return;

    struct dirent *entry;
    int count = 0;
    char target_path[MAX_PATH] = {0};
    const char *orig_name = NULL;
    
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        if (++count == index) {
            snprintf(target_path, MAX_PATH, "%s/%s", TRASH_DIR, entry->d_name);
            
            // Extract original filename
            orig_name = strchr(entry->d_name, '_');
            if (orig_name) orig_name++;
            else orig_name = entry->d_name;
            break;
        }
    }
    closedir(dir);
    
    if (!*target_path) {
        printf("Invalid selection!\n");
        return;
    }
    
    // Decrypt before recovery
    decrypt_file(target_path);
    
    // Restore file to current directory
    char new_path[MAX_PATH];
    snprintf(new_path, MAX_PATH, "./%s", orig_name);
    
    if (rename(target_path, new_path)) {
        perror("Recovery failed");
        encrypt_file(target_path);  // Re-encrypt if failed
    } else {
        printf("Recovered: %s\n", orig_name);
    }
}

void empty_trash(int days_old) {
    if (!create_trash_dir() || days_old < 0) return;

    DIR *dir = opendir(TRASH_DIR);
    if (!dir) return;
    
    time_t now = time(NULL);
    struct dirent *entry;
    
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", TRASH_DIR, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st)) continue;
        
        double days = difftime(now, st.st_mtime) / (60 * 60 * 24);
        if (days > days_old) {
            printf("Permanently deleting: %s\n", entry->d_name);
            unlink(full_path);
        }
    }
    closedir(dir);
}
