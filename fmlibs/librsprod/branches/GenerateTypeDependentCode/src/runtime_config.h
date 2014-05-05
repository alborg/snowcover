#ifndef RUNTIME_CONFIG_H
#define RUNTIME_CONFIG_H

#define LIBRSPROD_QUIET   0
#define LIBRSPROD_VERBOSE 1

extern int         librsprod_echo_mode;
extern rsprod_type librsprod_unpack_to;
extern int         librsprod_unpack_datasets;
extern int         librsprod_write_nullchar;
extern char        librsprod_pad_strings_with;

extern void librsprod_dump_config(void);
extern void librsprod_init_fillvalue(rsprod_type, void *);
extern void librsprod_init_fillvalues(void);
extern void librsprod_init_unityvalues(void);
extern void librsprod_init_zerovalues(void);

#endif
