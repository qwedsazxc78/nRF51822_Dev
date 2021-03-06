/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef APP_TWI_H__
#define APP_TWI_H__

#include <stdint.h>
#include "nrf_drv_twi.h"
#include "sdk_errors.h"

/**
 * @defgroup app_twi TWI transaction manager
 * @{
 * @ingroup app_common
 *
 * @brief Module for scheduling TWI transactions.
 */

/**
 * @brief Flag indicating that a given transfer should not be ended
 *        with a stop condition.
 *
 * Use this flag when a stop condition is undesirable between two transfers,
 * for example, when the first transfer is a write that sets an address in the slave
 * device and the second one is a read that fetches certain data using this
 * address. In this case, the second transfer should follow directly after the 
 * first transfer, with a repeated start condition instead of a stop and then
 * a new start condition.
 */
#define APP_TWI_NO_STOP     0x01

/**
 * @brief Macro for creating a write transfer.
 *
 * @param     address Slave address.
 * @param[in] p_data  Pointer to the data to be sent.
 * @param     length  Number of bytes to transfer.
 * @param     flags   Transfer flags (see @ref APP_TWI_NO_STOP).
 */
#define APP_TWI_WRITE(address, p_data, length, flags) \
    APP_TWI_TRANSFER(APP_TWI_WRITE_OP(address), p_data, length, flags)

/**
 * @brief Macro for creating a read transfer.
 *
 * @param     address Slave address.
 * @param[in] p_data  Pointer to the buffer where received data should be placed.
 * @param     length  Number of bytes to transfer.
 * @param     flags   Transfer flags (see @ref APP_TWI_NO_STOP).
 */
#define APP_TWI_READ(address, p_data, length, flags) \
    APP_TWI_TRANSFER(APP_TWI_READ_OP(address), p_data, length, flags)

/**
 * @brief Helper macro, should not be used directly.
 */
#define APP_TWI_TRANSFER(_operation, _p_data, _length, _flags) \
{                                      \
    .p_data    = (uint8_t *)(_p_data), \
    .length    = _length,              \
    .operation = _operation,           \
    .flags     = _flags                \
}
/**
 * @brief Helper macro, should not be used directly.
 */
#define APP_TWI_WRITE_OP(address)      (((address) << 1) | 0)
/**
 * @brief Helper macro, should not be used directly.
 */
#define APP_TWI_READ_OP(address)       (((address) << 1) | 1)
/**
 * @brief Helper macro, should not be used directly.
 */
#define APP_TWI_IS_READ_OP(operation)  ((operation) & 1)
/**
 * @brief Helper macro, should not be used directly.
 */
#define APP_TWI_OP_ADDRESS(operation)  ((operation) >> 1)

/**
 * @brief TWI transaction callback prototype.
 *
 * @param     result      Result of operation (NRF_SUCCESS on success,
 *                        otherwise a relevant error code).
 * @param[in] p_user_data Pointer to user data defined in transaction
 *                        descriptor.
 */
typedef void (* app_twi_callback_t)(ret_code_t result, void * p_user_data);

/**
 * @brief TWI transfer descriptor.
 */
typedef struct {
    uint8_t * p_data;     ///< Pointer to the buffer holding the data.
    uint8_t   length;     ///< Number of bytes to transfer.
    uint8_t   operation;  ///< Device address combined with transfer direction.
    uint8_t   flags;      ///< Transfer flags (see @ref APP_TWI_NO_STOP).
} app_twi_transfer_t;

/**
 * @brief TWI transaction descriptor.
 */
typedef struct {
    app_twi_callback_t         callback;
    ///< User-specified function to be called after the transaction is finished.

    void *                     p_user_data;
    ///< Pointer to user data to be passed to the callback.

    app_twi_transfer_t const * p_transfers;
    ///< Pointer to the array of transfers that make up the transaction.

    uint8_t                    number_of_transfers;
    ///< Number of transfers that make up the transaction.
} app_twi_transaction_t;

/**
 * @brief TWI transaction queue.
 */
typedef struct {
    app_twi_transaction_t const * volatile * p_buffer;
    uint8_t          size;
    uint8_t volatile read_idx;
    uint8_t volatile write_idx;
} app_twi_queue_t;

/**
 * @brief TWI transaction manager instance.
 */
typedef struct {
    app_twi_queue_t  queue;
    ///< Transaction queue.

    uint8_t volatile current_transfer_idx;
    ///< Index of currently performed transfer (within current transaction).

    bool    volatile internal_transaction_in_progress;
    ///< Informs that an internal transaction is being performed (by app_twi_perform()).

    uint8_t volatile internal_transaction_result;
    ///< Used to pass the result of the internal transaction realized by app_twi_perform().

    app_twi_transaction_t const * volatile p_current_transaction;
    ///< Currently realized transaction.

    nrf_drv_twi_t const twi;
    ///< TWI master driver instance.
} app_twi_t;

/**
 * @brief Macro for creating an instance of the TWI transaction manager.
 *
 * @param[in] twi_idx Index of the TWI master driver instance to be utilized
 *                    by this manager instance.
 */
#define APP_TWI_INSTANCE(twi_idx) \
{                                        \
    .twi = NRF_DRV_TWI_INSTANCE(twi_idx) \
}

/**
 * @brief Macro that simplifies the initialization of a TWI transaction manager
 *        instance.
 *
 * This macro allocates a static buffer for the transaction queue.
 * Therefore, it should be used in only one place in the code for a given
 * instance.
 *
 * @param[in]  p_app_twi    Pointer to the instance to be initialized.
 * @param[in]  p_twi_config Pointer to the TWI master driver configuration.
 * @param      queue_size   Size of the transaction queue (maximum number
 *                          of pending transactions).
 *                          See @ref app_twi_init_note "this note".
 * @param[out] err_code     The result of the app_twi_init() function call
 *                          is written to this parameter.
 */
#define APP_TWI_INIT(p_app_twi, p_twi_config, queue_size, err_code) \
    do {                                                                   \
        static app_twi_transaction_t const * queue_buffer[queue_size + 1]; \
        err_code = app_twi_init(p_app_twi, p_twi_config,                   \
                                queue_size, queue_buffer);                 \
    } while (0)

/**
 * @brief Function for initializing a TWI transaction manager instance.
 *
 * This function initializes the utilized TWI master driver instance and
 * prepares the transaction queue.
 *
 * @anchor app_twi_init_note
 * @note The queue size is the maximum number of pending transactions
 *       not counting the one that is currently realized. This means that
 *       for an empty queue with size of, for example, 4 elements, it is 
 *       possible to schedule up to 5 transactions.
 *
 * @param[in] p_app_twi      Pointer to the instance to be initialized.
 * @param[in] p_twi_config   Pointer to the TWI master driver configuration.
 * @param     queue_size     Size of the transaction queue (maximum number
 *                           of pending transactions).
 * @param[in] p_queue_buffer Pointer to a buffer for queued transactions
 *                           storage. Due to the queue implementation, the buffer must
 *                           be big enough to hold queue_size + 1 entries
 *                           (pointers to transaction descriptors).
 *
 * @retval NRF_SUCCESS If initialization was successful. Otherwise, the error code
 *         returned by the nrf_drv_twi_init() function is returned.
 */
ret_code_t app_twi_init(app_twi_t *                     p_app_twi,
                        nrf_drv_twi_config_t const *    p_twi_config,
                        uint8_t                         queue_size,
                        app_twi_transaction_t const * * p_queue_buffer);

/**
 * @brief Function for uninitializing a TWI transaction manager instance.
 *
 * @param[in] p_app_twi Pointer to the instance to be uninitialized.
 */
void       app_twi_uninit(app_twi_t * p_app_twi);

/**
 * @brief Function for scheduling a TWI transaction.
 *
 * The transaction is enqueued and started as soon as the TWI bus is
 * available, thus when all previously scheduled transactions have been
 * finished (possibly immediately).
 *
 * @param[in] p_app_twi     Pointer to the TWI transaction manager instance.
 * @param[in] p_transaction Pointer to the descriptor of the transaction to be
 *                          scheduled.
 *
 * @retval NRF_SUCCESS    If the transaction has been successfully scheduled.
 * @retval NRF_ERROR_BUSY If the limit of pending transactions has been reached
 *                        (the transaction queue is full).
 */
ret_code_t app_twi_schedule(app_twi_t *                   p_app_twi,
                            app_twi_transaction_t const * p_transaction);

/**
 * @brief Function for scheduling a transaction and waiting until it is finished.
 *
 * This function schedules a transaction that consists of one or more transfers 
 * and waits until it is finished.
 *
 * @param[in] p_app_twi           Pointer to the TWI transaction manager instance.
 * @param[in] p_transfers         Pointer to an array of transfers to be performed.
 * @param     number_of_transfers Number of transfers to be performed.
 * @param     user_function       User-specified function to be called while
 *                                waiting. NULL if such functionality
 *                                is not needed.
 *
 * @retval NRF_SUCCESS    If the transfers have been successfully realized.
 * @retval NRF_ERROR_BUSY If some transfers are already performed (if this function
 *                        was called from another context).
 * @retval -              Other error codes mean that the transaction has ended
 *                        with the error that is specified in the error code.
 */
ret_code_t app_twi_perform(app_twi_t *                p_app_twi,
                           app_twi_transfer_t const * p_transfers,
                           uint8_t                    number_of_transfers,
                           void (* user_function)(void));

/**
 * @brief Function for getting the current state of a TWI transaction manager
 *        instance.
 *
 * @param[in] p_app_twi Pointer to the TWI transaction manager instance.
 *
 * @retval true  If all scheduled transactions have been finished.
 * @retval false Otherwise.
 */
__STATIC_INLINE bool app_twi_is_idle(app_twi_t * p_app_twi)
{
    return (p_app_twi->p_current_transaction == NULL);
}

/**
 *@}
 **/

#endif // APP_TWI_H__
