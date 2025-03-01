#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char* path){
    static char buf[DIRSIZ + 1]; //DIRSIZ = 14
    char* p;
    
    for(p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;
    
    if(strlen(p) >= DIRSIZ) 
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void find(char *path, char *filename){
    char buf[1024], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    if((fd = open(path, O_RDONLY)) < 0){
        printf("find: cannot open %s\n", path);
        close(fd);
        return;
    }
    
    if (fstat(fd, &st) < 0){
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_DIR){
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            //break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if(st.type == T_FILE && strcmp(de.name, filename) == 0){
                printf("%s\n", buf);
            }
            if(st.type == T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0){
                find(buf, filename);
            }
        }
    }
    close(fd);
}

int main(int argc, char* argv[]){
    if (argc < 3){
        printf("Does not enough input arguments\n");
        exit(1);
    }

    for (int i = 2; i < argc; i++)
        find(argv[1], argv[i]);
    exit(0);
}
