/*--------------------------------------------------------------------------+
| File name       : SpIlv.h
+---------------------------------------------------------------------------+
| Description     : Project MSO_ILVSample
+---------------------------------------------------------------------------+
| Copyright (c) 2004, SAGEM SA 
+--------------------------------------------------------------------------*/

#include "Ilv_Definitions.h"

I ILV_Init		(	UC * i_puc_ILV , 
					UL * io_ul_ILVSize , 
					US   i_us_I );

I ILV_AddValue	(	UC * i_puc_ILV , 
					UL * io_ul_ILVSize , 
					UC * i_puc_Value , 
					UL   i_ul_ValueSize	);

#define ILV_GetI(i_puc_ILV)	((UC)*i_puc_ILV)

UL 	ILV_GetL	(	UC *i_puc_ILV	);

void ILV_SetL	(	UC * i_puc_ILV ,
					UL * io_ul_ILVSize ,
					UL   i_ul_LValue	);

UC	* ILV_GetV	(	UC * i_puc_ILV	);

UL	ILV_GetSize	(	UC * i_puc_ILV	);
