#include "spiSlave.h"
#include "usart.h"
#include "stm32f10x.h"

void spiGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ʱ������ */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* MISO����Ϊ����������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SCK��MOSI����Ϊ�������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* NSS����Ϊ�������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void spiModeInit(void)
{
	SPI_InitTypeDef SPI_InitStructure;

	/* ʱ������ */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO| RCC_APB2Periph_SPI1, ENABLE);  

	/* ����SPIģʽ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* �򿪽����ж� */
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE); 

	/* ��SPI1 */
	SPI_Cmd(SPI1, ENABLE);

}

void nvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* ѡ���жϷ���2 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* ����SPI1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void SPI1_IRQHandler(void) 
{
	volatile uint8_t i;

	/* �����ж� */
	if(SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET)
	{  
		/* ���յ����� */
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		i = SPI_I2S_ReceiveData(SPI1);
		printf("%d\r\n",i);
		/* ��������ж� */
		SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
	}  
}


void spiSlaveInit(void)
{
	spiGpioInit();
	spiModeInit();
	nvicConfig();
}

