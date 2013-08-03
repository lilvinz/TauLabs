#
# Rules to add FreeRTOS+TRACE to a PiOS target
#
# Note that the PIOS target-specific makefile will detect that FREERTOS_TRACE_DIR
# has been defined and add in the target-specific pieces separately.
#

FREERTOS_TRACE_DIR  := $(dir $(lastword $(MAKEFILE_LIST)))
SRC                 += $(wildcard $(FREERTOS_TRACE_DIR)/*.c)
EXTRAINCDIRS        += $(FREERTOS_TRACE_DIR)/Include

