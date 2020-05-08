/**
 * \file
 * \brief Configuration header file for FAT_FileSystem
 *
 * This header file is used to configure settings of the Generic I2C module.
 */

#ifndef __FAT1_CONFIG_H
#define __FAT1_CONFIG_H

#if !defined(FAT1_CONFIG_CARD_INSERT_DELAY_TIME_MS)
  #define FAT1_CONFIG_CARD_INSERT_DELAY_TIME_MS    (100)
    /*!< Delay time in milliseconds after insertion of the card detected */
#endif

#if !defined(FAT1_CONFIG_SHELL_ENABLED)
  #define FAT1_CONFIG_SHELL_ENABLED    (1)
    /*!< 1: Shell support is enabled; 0: no shell support enabled */
#endif


#endif /* __FAT1_CONFIG_H */

