#ifndef LOGGER_UTILS_H
#define LOGGER_UTILS_H

#include <string>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

static int rmDir(const std::string &path) {
    DIR *d = opendir(path.c_str());
    size_t path_len = strlen(path.c_str());
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = (char *) malloc(len);

            if (buf) {
                struct stat statbuf{};

                snprintf(buf, len, "%s/%s", path.c_str(), p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = rmDir(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path.c_str());

    return r;
}

#endif //LOGGER_UTILS_H
