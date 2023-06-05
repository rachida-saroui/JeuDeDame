/**
 ******************************************************************************
 * @file    STemWin/STemWin_HelloWorld/Src/main.c
 * @author  MCD Application Team
 * @brief   This file provides main program functions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "WM.h"
#include "stm32746g_discovery_ts.h"
#include "GUI.h"
#include "DIALOG.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t GUI_Initialized = 0;
TIM_HandleTypeDef TimHandle;
uint32_t uwPrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
static void MPU_Config(void);
static void SystemClock_Config(void);
void BSP_Background(void);

extern void MainTask(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
typedef struct {
	int board[8][8]; // 0 : case vide, 1 : pion blanc, 2 : pion noir,
//3 : dame blanche, 4 : dame noire
} board_t;
void MainTask(void);
void draw_board(board_t *board);
void init_board(board_t *board);
int is_valid_move(board_t *board, int x1, int y1, int x2, int y2);
int decide(board_t *board);
void move(board_t *board, int x1, int y1, int x2, int y2);
void update_score(board_t *board, int departureX, int departureY, int arriveeX, int arriveeY);
void check_win(int blackScore, int whiteScore);
int R = 15;
int X_SIZE = 60;
int Y_SIZE = 34;
int variable;
int departureX = -1; // Initialize to -1
int departureY = -1; // Initialize to -1
int arriveeX = -1;
int arriveeY = -1;
int cellWidth = 60;
int cellHeight = 34;
int whitePieces=12;
int blackPieces=12;
int main(void) {
	/* Configure the MPU attributes */
	MPU_Config();
	static TS_StateTypeDef  TS_State;
	/* Enable the CPU Cache */
	CPU_CACHE_Enable();

	/* STM32F7xx HAL library initialization:
	 - Configure the Flash ART accelerator on ITCM interface
	 - Configure the Systick to generate an interrupt each 1 msec
	 - Set NVIC Group Priority to 4
	 - Global MSP (MCU Support Package) initialization
	 */
	HAL_Init();

	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	/* Configure LED1 */
	BSP_LED_Init(LED1);

	/***********************************************************/

	/* Compute the prescaler value to have TIM3 counter clock equal to 10 KHz */
	uwPrescalerValue = (uint32_t) ((SystemCoreClock / 2) / 10000) - 1;

	/* Set TIMx instance */
	TimHandle.Instance = TIM3;

	/* Initialize TIM3 peripheral as follows:
	 + Period = 500 - 1
	 + Prescaler = ((SystemCoreClock/2)/10000) - 1
	 + ClockDivision = 0
	 + Counter direction = Up
	 */
	TimHandle.Init.Period = 500 - 1;
	TimHandle.Init.Prescaler = uwPrescalerValue;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		while (1) {
		}
	}

	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */
	if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
		while (1) {
		}
	}

	/***********************************************************/

	/* Init the STemWin GUI Library */
	BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */

	/* Initialize the Touch screen */
	BSP_TS_Init(480, 272);

	GUI_Init();

	GUI_DispStringAt("Starting...", 0, 0);

	GUI_Initialized = 1;

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

	board_t board;
	init_board(&board);
	GUI_Init();
	WM_SetDesktopColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font8x16);
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	draw_board(&board);
	int isDepartureCellSelected = 0;
	int touchX;
	int touchY;
	int valid;
	int departureX = -1; // Initialize to -1
	int departureY = -1; // Initialize to -1
	int arriveeX = -1;
	int arriveeY = -1;
	int player = 1;
	while (1) {
		if (player == 1) { // les pions blancs (l'humain)
			BSP_TS_GetState(&TS_State);
			if (TS_State.touchDetected) {
				if (isDepartureCellSelected) {
					touchX = TS_State.touchX[0];
					touchY = TS_State.touchY[0];
					arriveeX = round(touchX / cellWidth);
					arriveeY = round(touchY / cellHeight);
					valid = is_valid_move(&board, departureX, departureY, arriveeX,
							arriveeY);
					if (valid) {
						// Move player
						board.board[departureX][departureY] = 0;
						board.board[arriveeX][arriveeY] = 2; //pion blanc
						draw_board(&board);
						isDepartureCellSelected = 0;
						player = 2;
						update_score(&board, departureX, departureY, arriveeX,
								 arriveeY);

						departureX = -1; // Reset departure position
						departureY = -1;
						arriveeX = -1; // Reset arrival position
						arriveeY = -1;
						// Check if the game is over
						//if (is_game_over(&board, player)) {
						//  printf("Game over!\n");
						//  break;
						//}

					}

					HAL_Delay(100); // Add delay to avoid registering multiple touches
				} else {
					// Select departure cell
					touchX = TS_State.touchX[0];
					touchY = TS_State.touchY[0];
					departureX = round(touchX / cellWidth);
					departureY = round(touchY / cellHeight);
					//board.board[departureX][departureY] = 0;
					//draw_board(&board);
					isDepartureCellSelected = 1;
					HAL_Delay(200); // Add delay to avoid registering multiple touches
				}

			}


		}
		else if (player == 2) {
			decide(&board); //stm32 decides and moves
			draw_board(&board);
			HAL_Delay(200);
			player = 1;
		}

		check_win(blackPieces,  whitePieces);
	}

}
/**
 * @brief  Period elapsed callback in non blocking mode
 * @param  htim: TIM handle
 * @retval None
 */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//BSP_Background();
//}

/**
 * @brief TIM MSP Initialization 
 *        This function configures the hardware resources used in this application: 
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration  
 * @param htim: TIM handle pointer
 * @retval None
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* TIMx Peripheral clock enable */
	__HAL_RCC_TIM3_CLK_ENABLE();

	/*##-2- Configure the NVIC for TIMx ########################################*/
	/* Set the TIMx priority */
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);

	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

/**
 * @brief  BSP_Background.
 * @param  None
 * @retval None
 */

//void BSP_Background(void) {
	//BSP_LED_Toggle(LED1);
//}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow : 
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 200000000
 *            HCLK(Hz)                       = 200000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 400
 *            PLL_P                          = 2
 *            PLL_Q                          = 8
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 6
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 400;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	/* Activate the OverDrive to reach the 200 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}
}

void draw_board(board_t *board) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			// Tracer le rectangle et le remplir en blanc si il le faut
			GUI_SetColor(GUI_WHITE);
			GUI_DrawRect(i * X_SIZE, j * Y_SIZE, (i + 1) * X_SIZE,
					(j + 1) * Y_SIZE);
			if ((i + j) % 2 != 0) {
				GUI_FillRect(i * X_SIZE, j * Y_SIZE, (i + 1) * X_SIZE,
						(j + 1) * Y_SIZE);
			}
			else{
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(i * X_SIZE, j * Y_SIZE, (i + 1) * X_SIZE,
					(j + 1) * Y_SIZE);
			}
			// tracer les cercles
			GUI_SetColor(GUI_WHITE);
			if (board->board[i][j] == 1) //pion noir
					{
				GUI_DrawCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);
			} else if (board->board[i][j] == 2) // pion blanc
					{
				GUI_SetColor(GUI_WHITE);
                GUI_DrawCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);
				GUI_FillCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);

			} else if (board->board[i][j] == 3) //pion blanc qui est devenu une dame
					{
				GUI_SetColor(GUI_BLACK);
				GUI_DrawCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);
				GUI_SetColor(GUI_WHITE);
				GUI_FillCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);
				GUI_SetColor(GUI_BLACK);
			} else if (board->board[i][j] == 4) // dame noire
					{
				GUI_SetColor(GUI_BLACK);
				GUI_DrawCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);
				GUI_SetColor(GUI_WHITE);
				GUI_FillCircle((i + 0.5) * X_SIZE, (j + 0.5) * Y_SIZE, R);
				GUI_SetColor(GUI_BLACK);
			}
		}
	}
}

void init_board(board_t *board) {
	// Initialiser toutes les cases à 0 (vide)
	memset(board, 0, sizeof(board_t));

	// Initialiser les pions noirs
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2 == 0) {
				board->board[i][j] = 1;
			}
		}
	}

	// Initialiser les pions blancs
	for (int i = 5; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2 == 0) {
				board->board[i][j] = 2;
			}
		}
	}
}
int is_valid_move(board_t *board, int x1, int y1, int x2, int y2) {
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (board->board[x2][y2] == 0) {
		if (dx == 1 && dy == 1) {
			// Déplacement simple d'un pion
			if (board->board[x1][y1] == 2 && x2 > x1) {
				// Les pions blancs ne peuvent pas reculer
				return 0;
			} else if (board->board[x1][y1] == 1 && x1 > x2) {
				// Les pions noirs ne peuvent pas reculer
				return 0;
			} else {
				return 1;
			}
		} else if (dx == 2 && dy == 2) {
			// Capture d'un pion
			int capturedX = (x1 + x2) / 2;
			int capturedY = (y1 + y2) / 2;
			if (board->board[x1][y1] == 2 && x1 > x2
					&& board->board[capturedX][capturedY] == 1) {
				// Déplacement vers l'avant et capture d'un pion noir pour les pions blancs
				board->board[capturedX][capturedY] = 0;
				return 1;
			} else if (board->board[x1][y1] == 1 && x2 > x1
					&& board->board[capturedX][capturedY] == 2) {
				// Déplacement vers l'avant et capture d'un pion blanc pour les pions noirs
				board->board[capturedX][capturedY] = 0;
				return 1;
			} else {
				return 0;
			}
		}
		// Default case
		return 0;
	} else
		return 0;
}

void move(board_t *board, int x1, int y1, int x2, int y2) {
	board->board[x2][y2] = 1;
	board->board[x1][y1] = 0;
}

void update_score(board_t *board, int departureX, int departureY, int arriveeX, int arriveeY) {
    if ((abs(arriveeX - departureX) == 2)& (abs(arriveeY - departureY) == 2)){
        int capturedX = round((departureX + arriveeX) / 2);
        int capturedY = round((departureY + arriveeY) / 2);
        if (board->board[capturedX][capturedY] == 1) {
            // White pawn captured a black pawn
            blackPieces--;
        } else if (board->board[capturedX][capturedY] == 2) {
            // Black pawn captured a white pawn
            whitePieces--;
        }
    }
}

int decide(board_t *board) {
    int departX = -1, departY = -1, arriveeX = -1, arriveeY = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++){
            if (board->board[i][j] == 1){
                for (int k = -1; k <= 1; k += 2){
                    for (int l = -1; l <= 1; l += 2) {
                        int x = i + k;
                        int y = j + l;
                        while (x >= 0 && x < 8 && y >= 0 && y < 8) {
                            // Déplacement possible
                            int valid = is_valid_move(board, i, j, x, y);
                            if (valid) {
                                departX = i;
                                departY = j;
                                arriveeX = x;
                                arriveeY = y;
                                break;
                            }
                            x += k;
                            y += l;
                        }
                    }
                }
            }
        }
    }
    if (departX == -1 && departY == -1 && arriveeX == -1 && arriveeY == -1) {
        // Aucun déplacement possible
    	//move(board, 1, 1, 2, 2);
        return 0;
    } else {
        // Déplacement trouvé
        // Déplacer le pion noir sur le plateau
        move(board, departX, departY, arriveeX, arriveeY);
    	update_score(board, departureX, departureY, arriveeX,
    								 arriveeY);
        return 1;
    }
}

void check_win(int blackPieces, int whitePieces) {
    if (blackPieces == 0) {
        GUI_DispString("White wins!\n");
        // Stop the game
        while(1);
    } else if (whitePieces == 0) {
        GUI_DispString("Black wins!\n");
        // Stop the game
        while(1);
    }
}

/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
static void CPU_CACHE_Enable(void) {
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

/**
 * @brief  Configure the MPU attributes
 * @param  None
 * @retval None
 */
static void MPU_Config(void) {
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* Disable the MPU */
	HAL_MPU_Disable();

	/* Configure the MPU as Strongly ordered for not defined regions */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x00;
	MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x87;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes as WT for SDRAM */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0xC0000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes FMC control registers */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0xA0000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER2;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x0;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

