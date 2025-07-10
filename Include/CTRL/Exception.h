/**
 * @file Exception.h
 * @brief Exception handling utilities.
 */
#ifndef _CTRL_EXCEPTION_H
#define _CTRL_EXCEPTION_H

#include <CTRL/Defs.h>

/**
 * @brief Max number of handlers.
 * Handlers are set into TLS starting from word index 19, thus applications
 * shall not write to such range when exception handling is enabled.
 */
#define CTRL_MAX_EX_HANDLERS 13

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
 * @note This is a no-op if exception handling is not supported.
 */
void ctrlEnableExceptionHandling(void);

/**
 * @brief Disable exception handling.
 * @note This is a no-op if exception handling is not supported.
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
 * @brief Get the exception handler at a given index.
 * @param[in] index Exception handler index.
 * @return The exception handler on success, NULL otherwise.
 */
CTRLExHandlerFn ctrlGetExceptionHandler(size_t index);

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