#ifndef NM_MACHINE_H_
#define NM_MACHINE_H_

typedef struct {
     nm_str_t arch;
     nm_vect_t *list;
} nm_mach_t;

void nm_mach_free(void);
void nm_mach_vect_ins_mlist_cb(const void *unit_p, const void *ctx);
void nm_mach_vect_free_mlist_cb(const void *unit_p);
const char **nm_mach_get(const nm_str_t *arch);

#endif /* NM_MACHINE_H_ */
/* vim:set ts=4 sw=4 fdm=marker: */