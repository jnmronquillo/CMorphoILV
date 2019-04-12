/************************************************************/
/**
 * @file MORPHO_Types.h
 * @brief Definitions of types
 *
 * Copyright (c) SAGEM Sécurité 2007
 *
 * - PROJET      : MorphoSmart
 * - MODULE      : Interfaces
 *
 */
/***********************************************************
 FILE NAME   : $Workfile:   MORPHO_Types.h  $
 REVISION    : $Revision:   1.58  $
 DATE        : $Date:   May 03 2007 11:47:40  $
************************************************************/

#ifndef MORPHO_TYPES_H
#define MORPHO_TYPES_H

#ifdef WIN32
#include "windows.h"
#endif

// constants
/** @brief User unique identifier maximum size in local database */
#define MORPHO_USER_ID_MAXLEN			24
/** @brief The timeout error of the callback receive function */
#define COMERR_TIMEOUT_RECEIVE			-10000
/** @brief Concatenation of MorphoSmart™ product ID and MorphoSmart™ serial number
           Real length is 21, but KT authentication requires this length to be multiple of 8 */
#define MSO_SERIAL_NUMBER_LEN			24
/** @brief The maximum number of template authorized */
#define	MORPHO_LIST_CANDIDATE_PK		2

#define	LIC_MKIT_MAX_SIZE			   256	
#define	LIC_MKIT_ALEA_SIZE				 4

#define LOCK_KEY_NUMBER				5
#define	LOCK_ID_KEY_SIZE			8
#define	LOCK_KEY_SIZE				24
#define	LOCK_ALEA_SIZE				8

/** @brief If this bit is set, the tunneling protocol is activated */
#define SECU_TUNNELING				0x01	// default = no secure pipe
/** @brief If this bit is set, the offered Security protocol is activated. */
#define SECU_OFFERED_SECURITY		0x02	// default = no secure pipe
/** @brief If this bit is set, only templates with a digital signature are accepted. */
#define SECU_PK_ONLY_SIGNED			0x04	// default = accept all PK
/** @brief If this bit is set, the firmware must be digitally signed by SAGEM Défense 
           Sécurité. Note: For a secure MorphoSmart™, this option is always activated
           and can not be modified. */
#define SECU_SIGNED_DOWNLOAD		0x08	// default = downloaded modules are not signed. Once set, it is not anymore possible to discard this flag.
/** @brief If this bit is set, the score can not be exported */
#define SECU_NEVER_EXPORT_SCORE		0x10	// default = score is exported
/** @brief  */
#define SECU_FFD					0x20
/** @brief  */
#define SECU_LOCK					0x40	// Lock secret loaded

// FFD
/** @brief Low False Finger Detection security level */
#define FFD_SECURITY_LEVEL_LOW_HOST			0
/** @brief Medium False Finger Detection security level */
#define FFD_SECURITY_LEVEL_MEDIUM_HOST		1
/** @brief High False Finger Detection security level */
#define FFD_SECURITY_LEVEL_HIGH_HOST		2

/** @brief Default False Finger Detection security level */
#define FFD_SECURITY_LEVEL_DEFAULT_HOST	FFD_SECURITY_LEVEL_LOW_HOST

//CODER CHOICE
/** @brief Standard coder selection (this is the default) */
#define	MORPHO_MSO_V9_CODER		3
/** @brief Standard Juvenile coder selection */
#define MORPHO_MSO_V9_JUV_CODER	7
/** @brief Default coder */
#define	MORPHO_DEFAULT_CODER	MORPHO_MSO_V9_CODER

//DETECT MODE CHOICE
/** @brief Standard detection mode (this is the default) */
#define	MORPHO_VERIF_DETECT_MODE					1
/** @brief stronger detection mode */
#define	MORPHO_ENROLL_DETECT_MODE					2
/** @brief uses a 'led off' presence detection (only on xx1 devices) */
#define MORPHO_WAKEUP_LED_OFF						4

//MATCHING STRATEGY CHOICE
/** @brief Standard matching strategy selection (this is the default) */
#define	MORPHO_STANDARD_MATCHING_STRATEGY		0
/** @brief Advanced matching strategy selection - less FRR but more processing time */
#define MORPHO_ADVANCED_MATCHING_STRATEGY	1

//LATENT DETECT CHOICE
/** @disable the latency detection (this is the default) */
#define LATENT_DETECT_DISABLE		0
/** @enable the latency detection*/
#define LATENT_DETECT_ENABLE		1

// simple types
#ifndef WIN32
#define VOID		void
#endif
#define B			BOOLEAN
#define UC			unsigned char
#define C			char
#define US			unsigned short
#define S			short
#define UL			unsigned long
#define L			long
#define I			int
#define UI			unsigned int
#define PUI			unsigned int*
#ifndef WIN32
#define PVOID		void*
#endif
#define PUC			unsigned char*
#define PC			char*
#define PUS			unsigned short*
#define PS			short*
#define PUL			unsigned long*
#define PL			long*
#define PI			int*

/** @brief This type is used to identify the connection to a specific MorphoSmart™ */
typedef void *	MORPHO_HANDLE;

#ifdef LINUX
#include "../MSO_linux/include/CompatLinux.h"
#endif

// constructed types
/** @brief Those values are used to define how tight the matching threshold is.
           The table below defines the available FAR values. For more information on False
		   Acceptance Rate, please refer to the MorphoSmart Overview.pdf documentation. */
typedef enum { 
	MORPHO_FAR_0 = 0,	/**< Very low threshold for test purpose only */
	MORPHO_FAR_1,		/**< FAR < 1 % */ 
	MORPHO_FAR_2,		/**< FAR < 0.3 % */ 
	MORPHO_FAR_3,		/**< FAR < 0.1 % */ 
	MORPHO_FAR_4,		/**< FAR < 0.03 % */ 
	MORPHO_FAR_5,		/**< Recommended value: FAR < 0.01 % */ 
	MORPHO_FAR_6,		/**< FAR < 0.001 % */ 
	MORPHO_FAR_7,		/**< FAR < 0.0001 % */ 
	MORPHO_FAR_8,		/**< FAR < 0.00001 % */
	MORPHO_FAR_9,		/**< FAR < 0.000001 % */ 
	MORPHO_FAR_10		/**< Very high threshold for test purpose only */
} T_MORPHO_FAR,*PT_MORPHO_FAR;

/** @brief This structure contains information about the image format. */
typedef struct 
{
    UC					m_uc_HeaderRevision;	/**< Header revision (0 with current release) */
	UC					m_uc_HeaderSize;		/**< Header size (10 bytes) */
	US					m_us_NbRow;				/**< Image row number in pixels, */
	US					m_us_NbCol;				/**< Image column number in pixels */
	US					m_us_ResY;				/**< Vertical resolution in dpi */
	US					m_us_ResX;				/**< Horizontal resolution in dpi */
	UC					m_uc_CompressionType;	/**< Compression algorithm used, ID_COMPRESSION_NULL = No compression */
	UC					m_uc_NbBitsPerPixel;	/**< Number of bits per pixel */
}T_MORPHO_IMAGE_HEADER;

/** @brief Those values are used to define the asynchronous status events returned by the
    T_MORPHO_CALLBACK_ FUNCTIONS. There are also used to create the binary mask that describes
	the asynchronous status events that will trig the callback function.
 */
typedef enum { 
	MORPHO_CALLBACK_COMMAND_CMD				=1,		/**< This asynchronous status event identifies a command status information (see T_MORPHO_CALLBACK_COMMAND_STATUS) */
	MORPHO_CALLBACK_IMAGE_CMD				=2,		/**< This asynchronous status event identifies a low-resolution image (see T_MORPHO_CALLBACK_IMAGE_STATUS) */
	MORPHO_CALLBACK_ENROLLMENT_CMD			=4,		/**< This asynchronous status event identifies an enrollment status (see T_ MORPHO_CALLBACK_ENROLLMENT_STATUS) */
	MORPHO_CALLBACK_LAST_IMAGE_FULL_RES_CMD	=8,		/**< This asynchronous status event identifies the last image from a live acquisition which is returned in full resolution (see T_MORPHO_ CALLBACK_IMAGE_STATUS) */
	MORPHO_CALLBACK_RESERVED1				=16,	/**< NOT USE */	
	MORPHO_CALLBACK_RESERVED2				=32,	/**< NOT USE */
	MORPHO_CALLBACK_CODEQUALITY				=64,	/**< This asynchronous status event identifies the status of the quality note of the image detained to be coded. (please see the MorphoSmartHostSystemInterfaceSpecification document for the details) */
	MORPHO_CALLBACK_DETECTQUALITY			=128,	/**< This asynchronous status event identifies the status of the quality note calculated by the "presence detection" function. (please see the MorphoSmartHostSystemInterfaceSpecification document for the details) */
	MORPHO_CALLBACK_RESERVED3				=256,	/**<  */
	MORPHO_CALLBACK_RESERVED4				=512,	/**<  */
	MORPHO_CALLBACK_RESERVED5				=1024	/**<  */
} T_MORPHO_CALLBACK_COMMAND;

/** @brief Those values are used to define a finger status */
typedef enum { 
	MORPHO_MOVE_NO_FINGER,			/**< The terminal waits for the user's finger. */
	MORPHO_MOVE_FINGER_UP,			/**< The user must move his/her finger up. */
	MORPHO_MOVE_FINGER_DOWN,		/**< The user must move his/her finger down. */
	MORPHO_MOVE_FINGER_LEFT,		/**< The user must move his/her finger to the left. */
	MORPHO_MOVE_FINGER_RIGHT, 		/**< The user must move his/her finger to the right. */
	MORPHO_PRESS_FINGER_HARDER,		/**< The user must press his/her finger harder for the device to acquire a larger fingerprint image. */
	MORPHO_LATENT,					/**< The system has detected a latent fingerprint in the input fingerprint. Please change finger position. */
	MORPHO_REMOVE_FINGER,			/**< User must remove his finger. */
	MORPHO_FINGER_OK				/**< The finger acquisition was correctly completed. */
} T_MORPHO_COMMAND_STATUS;

/** @brief This parameter defines a type of compression to apply to the image. It is reserved for future use. */
typedef enum {
	MORPHO_NO_COMPRESS,		/**< No image compression is applied */
	MORPHO_COMPRESS_V1		/**< Image compression is applied */
} T_MORPHO_COMPRESS_ALGO;

/** @brief This structure is used to define a status or a step returned during the enrollment process. */
typedef struct {
	UC				m_uc_nbFinger;			/**< Number of the finger currently being enrolled (starts from 1). */
	UC				m_uc_nbFingerTotal;		/**< Total number of fingers to enroll (1 or 2). */
	UC				m_uc_nbCapture;			/**< Current acquisition number of the finger being currently enrolled (starts from 1). */
	UC				m_uc_nbCaptureTotal;	/**< Total number of acquisitions for one finger (3). */
} T_MORPHO_CALLBACK_ENROLLMENT_STATUS;

/** @brief Structure defining an image. This structure is used to display the image or
           returned by the MorphoSmart™ in the callback function. */
typedef struct {
	T_MORPHO_IMAGE_HEADER 	m_x_ImageHeader;		/**< Image header */
	PUC						m_puc_Image;			/**< Image buffer */
} T_MORPHO_CALLBACK_IMAGE_STATUS, T_MORPHO_IMAGE;

/** @brief T_MORPHO_CALLBACK_FUNCTION */
/**
 * This callback function is called each time the MorphoSmart™ send an asynchronous
 * message while processing a biometric function.
 *
 * This callback function is not reentered for a same C_MORPHO_Device object.
 * As a result, if a callback function is associated to a C_MORPHO_Device object,
 * it is forbidden to call a biometric function (like Verify, Capture, Identify …)
 * on this same C_MORPHO_Device object within this callback function. 
 * To call a biometric function (like Verify, Capture, Identify …) within this callback
 * function:
 * -	First, you need to create and open a second C_MORPHO_Device object
 *(outside of this callback function and before using this callback function)
 * -	Then, call the biometric function on this second C_MORPHO_Device object
 * within this same callback function.
 *
 * Example:
 * @code 
 * I BiometricCallback(	
 * 	PVOID									i_pv_context,
 * 	T_MORPHO_CALLBACK_COMMAND	i_i_command,
 * 	PVOID									i_pv_param	
 * )
 * {
 * 	T_MORPHO_IMAGE_HEADER* l_px_headerImage;
 * 
 * 	if (i_i_command == MORPHO_CALLBACK_COMMAND_CMD) 	
 * 	{
 * 		switch(*(PI)i_pv_param)
 * 		{
 * 			case MORPHO_REMOVE_FINGER:
 * 			... 
 * 		}
 * 	}
 * 	else if (i_i_command == MORPHO_CALLBACK_ENROLLMENT_CMD) 
 * 	{
 * 		l_px_headerImage = (T_MORPHO_IMAGE_HEADER*)((I)i_pv_param);
 * 		...
 * 	}
 * 	else if (i_i_command == MORPHO_CALLBACK_IMAGE_CMD) {
 * 		...
 * 	}
 * 	return 0;
 * }
 * @endcode
 *
 * @param[in] i_pv_context
 * - context pointer.
 * @param[in] i_i_command
 * - asynchronous received status.
 * @param[in] i_pv_param
 * - structure depending on the I_I_command value. 
 * 
 * The following table gives the relation between the I_I_command and the type of
 * this structure:
 * - MORPHO_CALLBACK_COMMAND_CMD		T_MORPHO_CALLBACK_COMMAND_STATUS
 * - MORPHO_CALLBACK_IMAGE_CMD			T_MORPHO_CALLBACK_IMAGE_STATUS
 * - MORPHO_CALLBACK_ENROLLMENT_CMD		T_MORPHO_CALLBACK_ENROLLMENT_STATUS
 * - MORPHO_CALLBACK_CODEQUALITY		UC.The recovered value is a PUC that contains a UC. 
 * - MORPHO_CALLBACK_DETECTQUALITY		UC.The recovered value is a PUC that contains a UC.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 */
typedef I	(*T_MORPHO_CALLBACK_FUNCTION)( 
					PVOID						i_pv_context,
					T_MORPHO_CALLBACK_COMMAND	i_i_command,
					PVOID						i_pv_param	);

/** @brief Those values identify a biometric template format. For more information on template format,
           please refer to the MorphoSmart Overview documentation. */
typedef enum {
		MORPHO_PK_COMP,				/**< compressed standard template format. This is the template format that should be used in most of the developments. This is the template that gives the best results. */
		MORPHO_PK_MAT_NORM,			/**< Specific template format reserved for compatibility with existing systems or specific usage. */
		MORPHO_PK_COMP_NORM,		/**< Specific template format reserved for compatibility with existing systems or specific usage. */	
		MORPHO_PK_MAT,				/**< Specific template format reserved for compatibility with existing systems or specific usage. */	
		MORPHO_PK_ANSI_378,			/**< Normalized template format defined as ANSI INCITS 378 standard. */
		MORPHO_PK_MINEX_A,			/**< Normalized format according to MINEX specification. */	
		MORPHO_PK_ISO_FMR,			/**< Normalized format defined as ISO/IEC 19794-2 standard, at section Finger Minutiae Record. */	
		MORPHO_PK_ISO_FMC_NS,		/**< Normalized format defined as ISO/IEC 19794-2 standard, at section Finger Minutiae Card Record Normal Size. */	
		MORPHO_PK_ISO_FMC_CS,		/**< Normalized format defined as ISO/IEC 19794-2 standard, at section Finger Minutiae Card Record Compact Size. */
		MORPHO_PK_ILO_FMR,			/**<  */
		MORPHO_PK_FROM_SMART_CARD,	/**<  */
		MORPHO_PK_V10
} T_MORPHO_TYPE_TEMPLATE;

/** @brief This parameter defines a biometric template envelope. */
typedef enum {
		MORPHO_RAW_TEMPLATE,			/**< raw template without a signature. */
		MORPHO_X984_SIGNED_TEMPLATE		/**< template with a digital signature and an X9.84 envelope. */	
}T_MORPHO_TEMPLATE_ENVELOP;

// Workflow For SMARTCARD
typedef enum {
		MORPHO_WORFLOW_SMARTCARD_SAGEM_DEMO,
		MORPHO_WORFLOW_SMARTCARD_CLIENT_GB
} T_MORPHO_SMARTCARD_WORKFLOW_TYPE;

typedef struct _T_MORPHO_TEMPLATE_IN_SMARTCARD
{
	T_MORPHO_SMARTCARD_WORKFLOW_TYPE	m_x_WorkFlowCardType;	// Work Flow type to read the templates in the Smart Card
	T_MORPHO_TYPE_TEMPLATE				m_x_typTemplate;		// template type stored in the Smart Card
	UC									m_uc_TemplateNbr;		// Number of Templates to read in the Smart Card
} T_MORPHO_TEMPLATE_IN_SMARTCARD, *PT_MORPHO_TEMPLATE_IN_SMARTCARD;


//////////////////////////////////////////////////////////////////
//		CALLBACK COM
//////////////////////////////////////////////////////////////////

/** @brief T_MORPHO_CALLBACK_COM_OPEN */
/**
 * The function initializes/opens a specific customer communication link.
 *
 * @param[in] o_ph_MsoHandle
 * - Pointer to the MORPHO_HANDLE of the initialized connection to the specific MSO.
 * This parameter is filled after a correct processing of this function.
 * If the processing failed, the value of this parameter is NULL.
 * @param[in] i_pc_String
 * - String parameter (for example, "COM1" if using the serial link).
 * @param[in] i_ul_Param
 * - Long integer parameter, (for example, "115200" if using the serial link).
 * @param[in,out] io_pv_Param
 * - Any further specific parameter.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_COM_OPEN)( 
					MORPHO_HANDLE	*o_ph_MsoHandle,
					PC				i_pc_String,
					UL				i_ul_Param,
					PVOID			io_pv_Param	);

typedef F_MORPHO_CALLBACK_COM_OPEN *T_MORPHO_CALLBACK_COM_OPEN;

/** @brief T_MORPHO_CALLBACK_COM_SEND */
/**
 * The function sends an ILV frame, using a specific customer communication link.
 *
 * @param[in] i_h_MsoHandle
 * - identifier of the connection to a specific MSO.
 * @param[in] i_ul_Timeout
 * - timeout in milliseconds.
 * @param[in] i_puc_Data
 * - buffer to transmit.
 * @param[in] i_ul_DataSize
 * - buffer size.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors.
 */
typedef I	(F_MORPHO_CALLBACK_COM_SEND)( 
					MORPHO_HANDLE	i_h_MsoHandle,
					UL				i_ul_Timeout,
					PUC				i_puc_Data,
					UL				i_ul_DataSize);

typedef F_MORPHO_CALLBACK_COM_SEND *T_MORPHO_CALLBACK_COM_SEND;

/** @brief T_MORPHO_CALLBACK_COM_RECEIVE */
/**
 * The function receives an ILV frame using a specific customer link.
 * Allocation shall be performed by the callback function.
 *
 * @param[in] i_h_MsoHandle
 * - identifier of the connection to a specific MSO.
 * @param[in] i_ul_Timeout
 * - timeout in milliseconds.
 * @param[in,out] io_ppuc_Data
 * - reception buffer. This buffer shall be allocated by the callback function.
 * @param[out] o_pul_DataSize
 * - buffer size.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors.
 * If the timeout expired, the function must return COMERR_TIMEOUT_RECEIVE.
 */
typedef I	(F_MORPHO_CALLBACK_COM_RECEIVE)( 
					MORPHO_HANDLE	i_h_MsoHandle,
					UL				i_ul_Timeout,
					PUC*			io_ppuc_Data,
					PUL				o_pul_DataSize);

typedef F_MORPHO_CALLBACK_COM_RECEIVE *T_MORPHO_CALLBACK_COM_RECEIVE;

/** @brief T_MORPHO_CALLBACK_COM_RECEIVE_FREE */
/**
 * The function releases memory allocated by T_MORPHO_CALLBACK_COM_RECEIVE.
 *
 * @param[in] i_h_MsoHandle
 * - identifier of the connection to a specific MSO.
 * @param[in,out] io_ppuc_Data
 * - buffer
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_COM_RECEIVE_FREE)( 
					MORPHO_HANDLE	i_h_MsoHandle,
					PUC*			io_ppuc_Data);

typedef F_MORPHO_CALLBACK_COM_RECEIVE_FREE *T_MORPHO_CALLBACK_COM_RECEIVE_FREE;

/** @brief T_MORPHO_CALLBACK_COM_CLOSE */
/**
 * The function closes a specific customer communication link.
 *
 * @param[in] i_h_MsoHandle
 * - identifier of the connection to a specific MSO. 
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_COM_CLOSE)( 
					MORPHO_HANDLE	*i_ph_MsoHandle);

typedef F_MORPHO_CALLBACK_COM_CLOSE *T_MORPHO_CALLBACK_COM_CLOSE;

//////////////////////////////////////////////////////////////////
//		CALLBACK TUNNELING
//////////////////////////////////////////////////////////////////

/** @brief T_MORPHO_CALLBACK_TUNNELING_INIT1 */
/**
 * The function performs the mutual authentication and key exchange between
 * the Host and the MorphoSmart™ (according to Needham-Schroeder protocol for asymmetric keys).
 *
 * @param[in] i_ul_TokenMso1Size
 * - Token size.
 * @param[in] i_puc_TokenMso1
 * - Token generated by the MorphoSmart™. This token is encrypted with the host public key.
 * This token contains S1 (24 bytes) | C1 (24 bytes) | MSO ident (20 bytes).
 * S1 and C1 are used to compute triple DES signature and encryption keys (see above).
 * @param[in] i_ul_CertifMsoSize
 * - MorphoSmart™ certificate size.
 * @param[in] i_puc_CertifMso
 * - MorphoSmart™ X509 certificate. This certificate is encoded in DER.
 * @param[in] i_ul_HashSizeMso
 * - MSO identifier length.
 * @param[in] i_puc_HashMso
 * - MSO identifier. This identifier is the hash of MSO distinguish name.
 * @param[in] i_ul_HashSizeHost
 * - Host identifier length.
 * @param[in] i_puc_HashHost
 * - Host identifier. This identifier is the hash of Host distinguish name.
 * @param[out] o_pul_TokenHostSize
 * - Token size.
 * @param[out] o_ppuc_TokenHostMSO
 * - Token generated by the Host. This token is encrypted with the MorphoSmart™ public key.
 * This token contains: 
 * MSO ident (20 bytes) | S2 (24 bytes) | C2 (24 bytes) | S1 (24 bytes) | C1 (24 bytes)
 * S2 and C2 are random numbers generated by the host
 * The symmetric signature key is S = S1 xor S2
 * The symmetric encryption key is C = C1 xor C2
 * @param[in,out] io_pv_context
 * - Pointer that can be used for customer convenience.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_TUNNELING_INIT1)(
					UL					i_ul_TokenMso1Size,
					PUC					i_puc_TokenMso1,
					UL					i_ul_CertifMsoSize,
					PUC					i_puc_CertifMso,
					UL					i_ul_HashSizeMso,
					PUC					i_puc_HashMso,
					UL					i_ul_HashSizeHost,
					PUC					i_puc_HashHost,
					PUL					o_pul_TokenHostSize,
					PUC*				o_ppuc_TokenHostMSO,
					PVOID);

typedef F_MORPHO_CALLBACK_TUNNELING_INIT1 *T_MORPHO_CALLBACK_TUNNELING_INIT1;

/** @brief T_MORPHO_CALLBACK_TUNNELING_INIT2 */
/**
 * The function performs the mutual authentication and key exchange between
 * the Host and the MorphoSmart™ (according to Needham-Schroeder protocol
 * for asymmetric keys).
 * * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_TokenMso2Size
 * - Size of the following token.
 * @param[in] i_puc_TokenMso2
 * - Token encrypted with the Host public key. This token contains S2 (24 bytes) | C2 (24 bytes)
 * 
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_TUNNELING_INIT2)(
					UL					i_ul_TokenMso2Size,
					PUC					i_puc_TokenMso2);

typedef F_MORPHO_CALLBACK_TUNNELING_INIT2 *T_MORPHO_CALLBACK_TUNNELING_INIT2;

/** @brief T_MORPHO_CALLBACK_TRIPLE_DES_SIGN */
/**
 * The function signs a buffer under triple DES EDE3 CBC MAC. (IV is 0).
 * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_DataSize
 * - Data length.
 * @param[in] i_puc_Data
 * - Data to sign.
 * @param[out] o_puc_Sign
 * - Triple signature performed on i_puc_Data. Signature key is S = S1 xor S2
 * (see T_MORPHO_CALLBACK_TUNNELING_INIT1).
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_TRIPLE_DES_SIGN)(
					UL		i_ul_DataSize,
					PUC		i_puc_Data,
					PUC		o_puc_Sign);

typedef F_MORPHO_CALLBACK_TRIPLE_DES_SIGN *T_MORPHO_CALLBACK_TRIPLE_DES_SIGN;

/** @brief T_MORPHO_CALLBACK_TRIPLE_DES_CRYPT */
/**
 * The function encrypts a buffer under triple DES EDE3 CBC. (IV is 0)
 * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_DataSize
 * - Data length.
 * @param[in] i_puc_Data
 * -Data to encrypt.
 * @param[out] o_puc_Crypt
 * - Encrypted data. Encryption key is C = C1 xor C2
 * (see T_MORPHO_CALLBACK_TUNNELING_INIT1).
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_TRIPLE_DES_CRYPT)(
					UL		i_ul_DataSize,
					PUC		i_puc_Data,
					PUC		o_puc_Crypt);

typedef F_MORPHO_CALLBACK_TRIPLE_DES_CRYPT *T_MORPHO_CALLBACK_TRIPLE_DES_CRYPT;

/** @brief T_MORPHO_CALLBACK_TRIPLE_DES_VERIF_SIGN */
/**
 * The function verifies a triple DES EDE3 CBC MAC signature. (IV is 0).
 * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_DataSize
 * - Data length.
 * @param[in] i_puc_Data
 * - Signed data.
 * @param[in] i_puc_Sign
 * Triple signature performed on i_puc_Data. Signature key is S = S1 xor S2
 * (see T_MORPHO_CALLBACK_TUNNELING_INIT1).
 * @param[out] o_pi_Result
 * - 1 if signature is correct. 0 if signature is not correct.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_TRIPLE_DES_VERIF_SIGN)(
					UL		i_ul_DataSize,
					PUC		i_puc_Data,
					PUC		i_puc_Sign,
					PI		o_pi_Result);

typedef F_MORPHO_CALLBACK_TRIPLE_DES_VERIF_SIGN *T_MORPHO_CALLBACK_TRIPLE_DES_VERIF_SIGN;

/** @brief T_MORPHO_CALLBACK_TRIPLE_DES_DECRYPT */
/**
 * The function decrypts a buffer under triple DES EDE3 CBC. (IV is 0).
 * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_DataSize
 * - Data length.
 * @param[in] i_puc_Data
 * - Data to encrypt.
 * @param[out] o_puc_Decrypt
 * - Decrypted data. Encryption key is C = C1 xor C2 (see T_MORPHO_CALLBACK_TUNNELING_INIT1).
 * 
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_TRIPLE_DES_DECRYPT)(
					UL		i_ul_DataSize,
					PUC		i_puc_Data,
					PUC		o_puc_Decrypt);

typedef F_MORPHO_CALLBACK_TRIPLE_DES_DECRYPT *T_MORPHO_CALLBACK_TRIPLE_DES_DECRYPT;

//////////////////////////////////////////////////////////////////
//		CALLBACK OFFERED SECURITY
//////////////////////////////////////////////////////////////////
/** @brief T_MORPHO_CALLBACK_VERIF_OFFERED_SECU_SIGN */
/**
 * The function checks the digital signature performed by the MorphoSmart™.
 * This callback only applies to MorphoSmart™ S with "offered security" protocol.
 * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_MsoCertificateSize
 * - MorphoSmart™ X509 certificate size.
 * @param[in] i_puc_MsoCertificate
 * - MorphoSmart™ X509 certificate size, coded in DER.
 * @param[in] i_ul_SignatureSize
 * - digital signature size.
 * @param[in] i_puc_Signature
 * - digital signature.
 * @param[in] i_ul_DataSize
 * - signed data length.
 * @param[in] i_puc_Data
 * - signed data.
 * @param[out] o_pi_Result
 * - 1 if signature is correct, 0 if signature is not correct.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_VERIF_OFFERED_SECU_SIGN)(
					UL		i_ul_MsoCertificateSize,
					PUC		i_puc_MsoCertificate,
					UL		i_ul_SignatureSize,
					PUC		i_puc_Signature,
					UL		i_ul_DataSize,
					PUC		i_puc_Data,
					PI		o_pi_Result);

typedef F_MORPHO_CALLBACK_VERIF_OFFERED_SECU_SIGN *T_MORPHO_CALLBACK_VERIF_OFFERED_SECU_SIGN;

/** @brief T_MORPHO_CALLBACK_RAND */
/**
 * The function generates the random number used to compute the anti-replay integer.
 * This callback only applies to MorphoSmart™ S with "offered security" protocol
 * Note: an implementation example using OpenSSL™ is given with its source code
 * (please refer to the MsoSecu Visual C++ project within the MorphoSmart SDK CD-ROM).
 *
 * @param[in] i_ul_RandomSize
 * - Size of the random number that must be generated.
 * @param[out] o_puc_Random
 * - Generated random number.
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_RAND)(
					UL		i_ul_RandomSize,
					PUC		o_puc_Random);

typedef F_MORPHO_CALLBACK_RAND *T_MORPHO_CALLBACK_RAND;

/** @brief T_MORPHO_CALLBACK_VERIF_CERTIF */
/**
 * 
 *
 * @param[in]  
 *
 * @retval MORPHO_OK	The execution of the function was successful.
 * @retval -10001_to_-10499 Other errors
 */
typedef I	(F_MORPHO_CALLBACK_VERIF_CERTIF)(
					UL		i_ul_MsoCertificateSize,
					PUC		i_puc_MsoCertificate,
					PI		o_pi_Result);

typedef F_MORPHO_CALLBACK_VERIF_CERTIF *T_MORPHO_CALLBACK_VERIF_CERTIF;

//////////////////////////////////////////////////////////////////
//		CALLBACK VERIFY SIGNED DATA
//////////////////////////////////////////////////////////////////
#define T_MORPHO_CALLBACK_VERIF_SIGN	T_MORPHO_CALLBACK_VERIF_OFFERED_SECU_SIGN


//////////////////////////////////////////////////////////////////
//		OTP
//////////////////////////////////////////////////////////////////

// OTP Options presence
/** @brief If this bit is set, the value of the sequence number N is valid */
#define OTP_VALID_SEQUENCE_NUMBER				0x0001
/** @brief If this bit is set, the value of the OTP password is valid. */
#define OTP_VALID_PASSWORD						0x0002
/** @brief If this bit is set, the value of the re-enrollment code pin is valid. */
#define OTP_VALID_PIN							0x0004
/** @brief If this bit is set, the value of the OTP parameters is valid */
#define OTP_VALID_PARAM							0x0008
/** @brief If this bit is set, the value of the hash algorithm is valid. */
#define OTP_VALID_HASH							0x0010
/** @brief If this bit is set, the value of the User ID is valid. */
#define OTP_VALID_USERID						0x0020
/** @brief If this bit is set, the value of the additional user data field 1 is valid. */
#define OTP_VALID_ADD1							0x0040
/** @brief If this bit is set, the value of the additional user data field 2 is valid. */
#define OTP_VALID_ADD2							0x0080
/** @brief If this bit is set, the value of the seed is valid. */
#define OTP_VALID_SEED							0x0100
/** @brief If this bit is set, the value of the OTP User Data is valid. */
#define OTP_VALID_OTP_USER_DATA					0x0200

// OTP Password Size
/** @brief Size of the OTP password. */
#define OTP_STRING_SIZE							16

// Pin Size
/** @brief Size of the Pin code */
#define OTP_PIN_SIZE							10

// OTP Algorithm parameters				
/** @brief SHA1 hash algorithm */
#define OTP_HASH_ALGO_SHA1						1
/** @brief Default hash algorithm */
#define OTP_HASH_ALGO_DEFAULT					OTP_HASH_ALGO_SHA1
/** @brief Maximum number of hash algorithm */
#define OTP_HASH_ALGO_NB_MAX					OTP_HASH_ALGO_SHA1


// Database
/** @brief Size Max of User additionnal fields */
#define	OTP_DB_FIELD_MAX_SIZE					15
/** @brief Number of Database Fields (User ID + 2 User data fields) */
#define OTP_DB_NB_FIELDS						3 // user ID + 2 Additional fields
/** @brief Number of Reference template */
#define OTP_DB_PK_MAX							2

#endif // MORPHO_TYPES_H

