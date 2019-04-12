/*--------------------------------------------------------------------------+
| File name       : Ilv.c
+---------------------------------------------------------------------------+
| Description     : Project MSO_ILVSample
+---------------------------------------------------------------------------+
| Copyright (c) 2004, SAGEM SA 
+--------------------------------------------------------------------------*/

#include <string.h>
#include "MORPHO_Types.h"
#include "Errors.h"
#include "SPIlv.h"

#define GET_US(i_p_UC)  ( (US) (((US)*( (PUC)(i_p_UC)    +1))<<8  ) | (*((PUC)(i_p_UC))) )
#define GET_UL(i_puc_US)( (UL) (((UL) (GET_US((i_puc_US)+2))<<16 ) | (UL)(GET_US(i_puc_US) ) ) )
#define SET_US(i_p_UC,i_US)( (*(i_p_UC)=(UC)(i_US)),(*(i_p_UC+1)=(UC)((i_US)>>8)) )
#define SET_UL(i_p_UC,i_UL)( SET_US(i_p_UC,(US)i_UL), SET_US(i_p_UC+2,(US)((i_UL)>>16)))

/*--------------------------------------------------------------------------+
| Function         : SizeOfL
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

UL SizeOfL( UC *i_puc_ILV ) 
{
	UL l_ul_L_Value = 0;

	l_ul_L_Value = GET_US( i_puc_ILV + sizeof(UC) );

	if ( l_ul_L_Value == (UL)0xFFFF )
		return sizeof(US)+sizeof(UL);
	else
		return sizeof(US);
}

/*--------------------------------------------------------------------------+
| Function         : SizeOfIandL
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

UL SizeOfIandL( UC *i_puc_ILV ) 
{
	UL l_ul_L_Value;

	l_ul_L_Value = sizeof(UC) + SizeOfL(i_puc_ILV);

	return l_ul_L_Value;
}

/*--------------------------------------------------------------------------+
| Function         : ILV_Init
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

I ILV_Init(UC *i_puc_ILV, UL *io_ul_ILVSize, US i_us_I)
{
 	*io_ul_ILVSize = 0;

	i_puc_ILV[0] = (UC)(i_us_I & 0xFF);
	i_puc_ILV[1] = (UC)(*io_ul_ILVSize);
	i_puc_ILV[2] = 0x00;

	*io_ul_ILVSize += 3;
	
	return RETURN_NO_ERROR;
}

/*--------------------------------------------------------------------------+
| Function         : ILV_Check
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

I ILV_Check(UC *i_puc_ILV, UL i_ul_ILVSize)
{
	UL		l_ul_L_Value;
	I		l_s_Ret;

	l_s_Ret = RETURN_NO_ERROR;

	if(i_ul_ILVSize < 3 || i_ul_ILVSize < ( sizeof(UC)+2 ) || i_ul_ILVSize < SizeOfIandL(i_puc_ILV))
		return ILV_ERROR_BAD_ILV_SIZE;

	l_ul_L_Value = ILV_GetL(i_puc_ILV) + SizeOfIandL(i_puc_ILV);

	if ( i_ul_ILVSize != l_ul_L_Value )
		return ILV_ERROR_BAD_ILV_SIZE;

	return RETURN_NO_ERROR;
}

/*--------------------------------------------------------------------------+
| Function         : ILV_AddValue
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

I ILV_AddValue(UC *i_puc_ILV, UL *io_ul_ILVSize, UC* i_puc_Value, UL i_ul_ValueSize)
{
	UL l_ul_L_Value=0;
		
	// Get the old size of V
	l_ul_L_Value = ILV_GetL(i_puc_ILV);

	// Add value at the end
	memcpy(i_puc_ILV+*io_ul_ILVSize,i_puc_Value,(I)i_ul_ValueSize);

	// Update ILV size
	*io_ul_ILVSize += i_ul_ValueSize;

	// Set new L value
	ILV_SetL(i_puc_ILV,io_ul_ILVSize,l_ul_L_Value+i_ul_ValueSize);
	
	return RETURN_NO_ERROR;
}

/*--------------------------------------------------------------------------+
| Function         : ILV_GetValue
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

I ILV_GetValue(UC *i_puc_ILV, UL *io_ul_Pos, UC* i_puc_Value, UL i_ul_ValueSize)
{
	UL l_ul_L_Value=0;
	PUC	l_puc_V;
		
	l_ul_L_Value = ILV_GetL(i_puc_ILV);

	if ( i_ul_ValueSize > (l_ul_L_Value-*io_ul_Pos) )
		return SPILVERR_BAD_PARAMETER;
	
	l_puc_V = ILV_GetV(i_puc_ILV);
	//l_puc_V = (PUC)((UL)l_puc_V + *io_ul_Pos);
    l_puc_V = (PUC)((uintptr_t)l_puc_V + *io_ul_Pos);

	memcpy(i_puc_Value,l_puc_V,(I)i_ul_ValueSize);
	
	*io_ul_Pos += +i_ul_ValueSize;

	return RETURN_NO_ERROR;
}

/*--------------------------------------------------------------------------+
| Function         : ILV_GetL
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

UL ILV_GetL(UC *i_puc_ILV)
{
	UL	l_ul_L_Value=0;
	PUC	l_puc_Ptr;

	l_puc_Ptr = i_puc_ILV+sizeof(UC);

	l_ul_L_Value = (UL)GET_US( l_puc_Ptr );

	if ( l_ul_L_Value == (UL)0xFFFF )
		l_ul_L_Value = GET_UL( l_puc_Ptr+sizeof(US) );

	return l_ul_L_Value;
}

/*--------------------------------------------------------------------------+
| Function         : ILV_GetSize
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

UL	ILV_GetSize(UC *i_puc_ILV)
{	
	return (ILV_GetL(i_puc_ILV)+SizeOfIandL(i_puc_ILV));
}

/*--------------------------------------------------------------------------+
| Function         : ILV_GetV
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

UC	*ILV_GetV(UC *i_puc_ILV)
{
	return (i_puc_ILV+SizeOfIandL(i_puc_ILV));
}

/*--------------------------------------------------------------------------+
| Function         : ILV_SetL
| Description      : 
| Arguments        : 
|
| Copyright (c) 2004, SAGEM SA
+--------------------------------------------------------------------------*/

void ILV_SetL(UC *i_puc_ILV,UL *io_ul_ILVSize,UL i_ul_LValue)
{
	UL	l_ul_oldL_Value=0;
	UL	l_ul_i;
	UC	*	l_puc_tmp;

	l_ul_oldL_Value = ILV_GetL(i_puc_ILV);
	if (l_ul_oldL_Value>(UL)0xFFFF)
	{
		if ( i_ul_LValue >= (UL)0xFFFF )
		{
			// It was an UL, and it is always an UL
			SET_UL(i_puc_ILV+sizeof(UC)+sizeof(US), i_ul_LValue);
		}
		else
		{
			// It was an UL, and it is an US
			SET_US( i_puc_ILV+sizeof(UC), i_ul_LValue );

			// We move left the V of 4 bytes
			l_puc_tmp = i_puc_ILV+sizeof(UC)+sizeof(US)+sizeof(UL);

			for ( l_ul_i = 0 ; l_ul_i < i_ul_LValue ; l_ul_i++ )
			{
				l_puc_tmp[l_ul_i-sizeof(UL)] = l_puc_tmp[l_ul_i];
			}

			*io_ul_ILVSize=(*io_ul_ILVSize) - sizeof(UL);
		}
	}
	else
	{
		if ( i_ul_LValue < (UL)0xFFFF )
		{
			// It was an US, and it is always an US
			SET_US ( i_puc_ILV+sizeof(UC), i_ul_LValue );
		}
		else
		{
			// It was an US, and it is an UL
			// We move right the V of 4 bytes
			l_puc_tmp = i_puc_ILV+sizeof(UC)+sizeof(US);
			for (l_ul_i=(*io_ul_ILVSize);l_ul_i>0;l_ul_i--) {
				if (l_ul_i == 4)
					l_puc_tmp[l_ul_i-1+sizeof(UL)] = l_puc_tmp[l_ul_i-1];

				l_puc_tmp[l_ul_i-1+sizeof(UL)] = l_puc_tmp[l_ul_i-1];
			}

			l_ul_oldL_Value = (UL)0xFFFF;
			SET_US ( i_puc_ILV+sizeof(UC), l_ul_oldL_Value );
			SET_UL ( i_puc_ILV+sizeof(UC)+sizeof(US), i_ul_LValue );
			*io_ul_ILVSize=(*io_ul_ILVSize) + sizeof(UL);			
		}
	}

	return;
}
