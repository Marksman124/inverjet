/**
******************************************************************************
* @file				ntc_3950.c
* @brief			ntc_3950 �¶ȼ�� (��������)
*
* @author			WQG
* @versions		v1.0
* @date				2024-7-23
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "ntc_3950.h"
#include "adc.h"
#include <math.h>
#include "debug_protocol.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef* p_hadc_ntc = &hadc2;

static uint16_t Temperature_Fault_Cnt = 0;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

//��� δ����
#define NTC_ADC_MAX 181
const uint16_t NTC_ADC_Tab[NTC_ADC_MAX] = {
54,	58,	62,	66,	70,	75,	80,	86,	91,	97,//-55~-46
104,	110,	117,	125,	133,	141,	150,	159,	168,	178,//-45~-36
189,	200,	212,	224,	236,	250,	263,	278,	293,	309,//-35~-26
325,	342,	360,	378,	397,	417,	437,	459,	481,	504,//-25~-16
527,	551,	577,	603,	629,	657,	685,	715,	745,	775,//-15~-6
807,	840,	873,	907,	942,	974,	1014,	1051,	1089,	1127,//-5~4
1167,	1207,	1247,	1288,	1330,	1372,	1415,	1458,	1501,	1545,//15~4
1589,	1634,	1678,	1723,	1768,	1814,	1859,	1904,	1949,	1994,//15~24
2048,	2084,	2128,	2173,	2217,	2260,	2303,	2346,	2388,	2430,//25~34
2471,	2512,	2553,	2592,	2631,	2669,	2707,	2744,	2780,	2816,//35~44
2851,	2885,	2918,	2951,	2983,	3014,	3045,	3074,	3103,	3132,//45~54
3159,	3186,	3212,	3238,	3262,	3287,	3310,	3333,	3355,	3376,//55~64
3397,	3418,	3437,	3457,	3475,	3493,	3510,	3527,	3544,	3560,//65~74
3575,	3590,	3605,	3619,	3632,	3646,	3659,	3671,	3683,	3695,//75~84
3706,	3717,	3728,	3738,	3748,	3758,	3767,	3777,	3786,	3794,//-85~94
3802,	3811,	3818,	3826,	3833,	3840,	3847,	3854,	3861,	3867,//-95~104
3873,	3879,	3885,	3891,	3896,	3901,	3907,	3912,	3917,	3921,//-105~114
3926,	3931,	3935,	3939,	3943,	3947,	3951,	3955,	3959,	3962,//-115~124
3966,
};
/*************************************************           
[��������]u16Check_NTC_TabTemp                                                  
[��������]�������¶�    ���ַ����                                               
[��    ��]NTC_ADvalue �ɼ��õ���ADֵ                                                   
[ȫ�ֱ���]                                                  
[�� �� ֵ]��ǰ���¶�ֵ+55                                                  
[��    ע]                                                  
*************************************************/             
uint16_t u16Check_NTC_TabTemp(uint16_t NTC_ADvalue)
{
 uint16_t end = NTC_ADC_MAX-1;//�������һ����
 uint16_t Front = 0;//�����һ����
 uint8_t half = 0;
 uint16_t TempValue = 0;
 uint16_t TempAdcValue = 0;
 TempAdcValue = NTC_ADvalue;
 if((TempAdcValue>NTC_ADC_Tab[0])&&(TempAdcValue<NTC_ADC_Tab[NTC_ADC_MAX-1]))
 {
     for(half=90;end-Front!=1;)
	 {
	  if(TempAdcValue>NTC_ADC_Tab[half])//��������м�ֵ�������
	  {
		Front = half;
		half = (Front+end)/2;
	  }
	  else if(TempAdcValue<NTC_ADC_Tab[half])//���С���м�ֵ����ǰ��
	  {
	   end = half;
	   half = (Front+end)/2;
	  }
	  else//���õ��ڱ���е�ֵ
	  {
	   TempValue = half;
	   break;
	  }
	 }
	 if(end-Front==1)
	 {
	  if(NTC_ADC_Tab[end]-TempAdcValue >TempAdcValue-NTC_ADC_Tab[Front])
	  {
	   TempValue = half;
	  }
	  else
	  {
	   TempValue = half+1;
	  }
	 }
 }
 else//������Χ
 {
  if(TempAdcValue<=NTC_ADC_Tab[0]) 
      TempValue = 0;
  else if(TempAdcValue>=NTC_ADC_Tab[NTC_ADC_MAX-1])
  {
      TempValue = NTC_ADC_MAX;
  }
 }	
 return TempValue;
}

//��ʽ��
#define B 3950.0//�¶�ϵ��
#define TN 298.15//��¶�(�����¶ȼӳ���:273.15+25)
#define RN 10// ���ֵ(�����¶�ʱ�ĵ���ֵ10k)
#define BaseVol 3.30 //ADC��׼��ѹ
#define Correction 0 //������
/*************************************************           
[��������]fGet_NTC_Temp                                                  
[��������]����ntc�¶�                                                  
[��    ��]Rntc ��ǰ��NTC����ֵ                                                  
[ȫ�ֱ���]                                                  
[�� �� ֵ]��ǰ���¶�ֵ                                                  
[��    ע]                                                  
*************************************************/             
float Get_Tempture(uint32_t adc)
{
	float RV,RT,Tmp;
	RV=BaseVol/4096.0*(float)adc;//ADCΪ12λADC,���NTC��ѹ:RV=ADCValu/4096*BaseVoltag
	RT=RV*10/(BaseVol-RV);//�����ǰ�¶���ֵ (BaseVoltage-RV)/R16=RV/RT;
	Tmp=1/(1/TN+(log(RT/RN)/B))-273.15+Correction;//%RT = RN exp*B(1/T-1/TN)%

	//DEBUG_PRINT("ad:%d\t��ѹ:%0.3f\t����:%0.3f\t�¶ȣ�%0.3f��C\n",adc, RV, RT, Tmp);
	
	return Tmp;
	
}


/**
  * @brief ��ȡ�ڲ��¶ȴ�����
  */
//float Get_Internal_Temp(void)
//{
//    uint32_t Temp;//�¶Ȳ����ֲ�ֵ
//    float Vsense = 0.0;//�¶Ȳ�����ѹֵ
//    float Temperature = 0.0;//�¶�ֵ
    //�����ֲ��¶�ת����ʽ��T = ((V25-Vsense)/Avg_Slope) + 25
//    float V25 = 1.43;//�����ֲ���
//    float Avg_Slope = 0.0043;//4.3mV/���϶�
//    //printf("rnrn------------------MCU�ڲ��¶ȴ���������------------------rnrn");
//    //step1 ����ADC
//    HAL_ADC_Start(&hadc1);
//    //step2 �¶Ȳɼ�ת��
//    HAL_ADC_PollForConversion(&hadc1,5);
//    //step3 ת������
//    Temp = HAL_ADC_GetValue(&hadc1);//��ȡ����ֵ�ֲ�ֵ
//    Vsense = Temp *(3.3/4096);//��������12bit,���ֲ�ֵ4096
//    Temperature = ((V25-Vsense)/Avg_Slope) + 25;//����ʽ�����¶�ֵ
//    //step4 ���ڴ�ӡ

	//printf("�¶ȷֲ�ֵ��%drn�¶ȵ�ѹֵ��%0.3frn�¶Ȳ���ֵ��%0.3frn",Temp,Vsense,Temperature);

//	return Temperature;
//}

void MX_ADC_CHANNEL_Config(uint32_t Channel) {
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = Channel; // ����ͨ��
  sConfig.Rank = 1; // ת��˳��
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // ����ʱ��
  //sConfig.Offset = 0; // ƫ����
  HAL_ADC_ConfigChannel(p_hadc_ntc, &sConfig);
}

/**
  * @brief ��ȡ�ⲿ�¶ȴ�����
  */
float Get_External_Temp(void)
{
    uint32_t adc;//�¶Ȳ����ֲ�ֵ
    float Temperature = 0.0;//�¶�ֵ
	
	  // ����ͨ��
		MX_ADC_CHANNEL_Config(ADC_CHANNEL_9);
		//У׼
		HAL_ADCEx_Calibration_Start(p_hadc_ntc);
    //step1 ����ADC
    HAL_ADC_Start(p_hadc_ntc);
    //step2 �¶Ȳɼ�ת��
    HAL_ADC_PollForConversion(p_hadc_ntc,ADC_SAMPLETIME_239CYCLES_5);
	
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(p_hadc_ntc), HAL_ADC_STATE_REG_EOC))//��ȡADC��ɱ�־λ
		{
			//step3 ת������
			adc = HAL_ADC_GetValue(p_hadc_ntc);//��ȡ����ֵ�ֲ�ֵ
		}
    //Temperature = u16Check_NTC_TabTemp(Temp);

		if((adc == 0) || (adc == 4096))
		{
			if(Temperature_Fault_Cnt ++ > 10)
			{
				return -100;
			}
		}
		else
		{
			Temperature_Fault_Cnt = 0;
		}
		Temperature = Get_Tempture(adc);
	
	return Temperature;
}
	
//------------------- ��ȡwifi״̬�� ----------------------------




