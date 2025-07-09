/**
 * @file Exception.h
 * @brief Exception utilities.
 */
#ifndef _CTRL_EXCEPTION_H
#define _CTRL_EXCEPTION_H

#include <CTRL/Defs.h>

#define CTRL_MAX_EX_HANDLERS 13 ///< Maximum number of handlers

/**
 * @brief Exception handler.
 * @param[in, out] ex Exception data.
 * @return True if the exception has been handled, false otherwise.
 */
typedef bool(*CTRLExHandlerFn)(ERRF_ExceptionData* ex);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Check if exception handling is supported.
 * @return True if exception handling is supported, false otherwise.
 */
bool ctrlExceptionHandlingIsSupported(void);

/**
 * @brief Enable exception handling.
 * @return True on success, false otherwise.
 * @note At least 1 exception handler must be set.
 */
bool ctrlEnableExceptionHandling(void);

/**
 * @brief Disable exception handling.
 */
void ctrlDisableExceptionHandling(void);

/**
 * @brief Set an exception handler.
 * @param[in] fn Exception handler.
 * @param[in] index Exception handler index.
 * @return True on success, false otherwise.
 */
bool ctrlSetExceptionHandler(CTRLExHandlerFn fn, size_t index);

/**
 * @brief Clear an exception handler.
 * @param[in] index Exception handler index.
 * @return True on success, false otherwise.
 */
bool ctrlClearExceptionHandler(size_t index);

/**
 * @brief Check if an exception handler is set.
 * @param[in] index Exception handler index.
 * @return True if the exception handler is set, false otherwise.
 */
bool ctrlExceptionHandlerIsSet(size_t index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _CTRL_EXCEPTION_H */