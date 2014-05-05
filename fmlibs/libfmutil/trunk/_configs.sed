s/^#undef  *\([ABCDEFGHIJKLMNOPQRSTUVWXYZ_]\)/#undef FMUTIL_\1/
s/^#undef  *\([abcdefghijklmnopqrstuvwxyz]\)/#undef _fmutil_\1/
s/^#define  *\([ABCDEFGHIJKLMNOPQRSTUVWXYZ_][abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]*\)\(.*\)/#ifndef FMUTIL_\1 \
#define FMUTIL_\1 \2 \
#endif/
s/^#define  *\([abcdefghijklmnopqrstuvwxyz][abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]*\)\(.*\)/#ifndef _fmutil_\1 \
#define _fmutil_\1 \2 \
#endif/
