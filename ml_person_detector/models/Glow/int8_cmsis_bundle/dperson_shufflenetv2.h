// Bundle API auto-generated header file. Do not edit!
// Glow Tools version: 2022-05-19 (2ee55ec50) (Glow_Release_MCUX_SDK_2.12.0)

#ifndef _GLOW_BUNDLE_DPERSON_SHUFFLENETV2_H
#define _GLOW_BUNDLE_DPERSON_SHUFFLENETV2_H

#include <stdint.h>

// ---------------------------------------------------------------
//                       Common definitions
// ---------------------------------------------------------------
#ifndef _GLOW_BUNDLE_COMMON_DEFS
#define _GLOW_BUNDLE_COMMON_DEFS

// Glow bundle error code for correct execution.
#define GLOW_SUCCESS 0

// Memory alignment definition with given alignment size
// for static allocation of memory.
#define GLOW_MEM_ALIGN(size)  __attribute__((aligned(size)))

// Macro function to get the absolute address of a
// placeholder using the base address of the mutable
// weight buffer and placeholder offset definition.
#define GLOW_GET_ADDR(mutableBaseAddr, placeholderOff)  (((uint8_t*)(mutableBaseAddr)) + placeholderOff)

#endif

// ---------------------------------------------------------------
//                          Bundle API
// ---------------------------------------------------------------
// Model name: "dperson_shufflenetv2"
// Total data size: 1635008 (bytes)
// Activations allocation efficiency: 1.0000
// Placeholders:
//
//   Name: "input_1"
//   Type: float<1 x 3 x 192 x 320>
//   Size: 184320 (elements)
//   Size: 737280 (bytes)
//   Offset: 0 (bytes)
//
//   Name: "A758"
//   Type: float<1 x 6 x 12 x 20>
//   Size: 1440 (elements)
//   Size: 5760 (bytes)
//   Offset: 737280 (bytes)
//
// NOTE: Placeholders are allocated within the "mutableWeight"
// buffer and are identified using an offset relative to base.
// ---------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// Placeholder address offsets within mutable buffer (bytes).
#define DPERSON_SHUFFLENETV2_input_1  0
#define DPERSON_SHUFFLENETV2_A758     737280

// Memory sizes (bytes).
#define DPERSON_SHUFFLENETV2_CONSTANT_MEM_SIZE     246848
#define DPERSON_SHUFFLENETV2_MUTABLE_MEM_SIZE      743040
#define DPERSON_SHUFFLENETV2_ACTIVATIONS_MEM_SIZE  645120

// Memory alignment (bytes).
#define DPERSON_SHUFFLENETV2_MEM_ALIGN  64

// Bundle entry point (inference function). Returns 0
// for correct execution or some error code otherwise.
int dperson_shufflenetv2(uint8_t *constantWeight, uint8_t *mutableWeight, uint8_t *activations);

#ifdef __cplusplus
}
#endif
#endif
