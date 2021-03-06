#ifndef NM_HW_INFO_H_
#define NM_HW_INFO_H_

#include <stdint.h>

uint32_t nm_hw_total_ram(void);
uint32_t nm_hw_ncpus(void);
uint32_t nm_hw_disk_free(void);

#endif /* NM_HW_INFO_H_ */
/* vim:set ts=4 sw=4: */
