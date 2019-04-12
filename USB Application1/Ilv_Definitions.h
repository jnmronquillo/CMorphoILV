/*--------------------------------------------------------------------------+
| File name       : Ilv_Definitions.h
+---------------------------------------------------------------------------+
| Description     : Project MSO_ILVSample
+---------------------------------------------------------------------------+
| Copyright (c) 2004, SAGEM SA 
+--------------------------------------------------------------------------*/

// General commands

#define ILV_GET_DESCRIPTOR				0x05
#define ILV_PING         				0x08
#define ILV_GET_BASE_CONGIG				0x07

// Biometric commands

#define ILV_VERIFY		 				0x20
#define ILV_ENROLL		 				0x21
#define ILV_IDENTIFY					0x22
#define ILV_VERIFY_MATCH				0x23
#define ILV_IDENTIFY_MATCH				0x24

// DataBase commands

#define ILV_CREATE_DB					0x30
#define ILV_ERASE_BASE					0x32
#define ILV_DESTROY_ALL_BASE			0x33
#define ILV_ERASE_ALL_BASE				0x34
#define ILV_ADD_RECORD					0x35
#define ILV_REMOVE_RECORD				0x36
#define ILV_FIND_USER_DB				0x38
#define ILV_DESTROY_DB					0x3B
#define ILV_LIST_PUBLIC_FIELDS			0x3E
#define ILV_GET_DATA_DB					0x3F

// Miscellaneous

#define ILV_CONFIG_UART					0xEE
#define ILV_ASYNC_MESSAGE				0x71

// Invalid ILV

#define ILV_INVALID						0x50

// Error codes

#define ILV_OK							0x00	//  0	Successful result
#define ILVERR_ERROR					0xFF	// -1	An error occurred
#define ILVERR_BADPARAMETER				0xFE	// -2	Input parameters are not valid
#define ILVERR_INVALID_MINUTIAE			0xFD	// -3	The minutiae is not valid
#define ILVERR_INVALID_USER_ID			0xFC	// -4	The record identifier does not exist in the database	
#define ILVERR_INVALID_USER_DATA		0xFB	// -5	The user data are not valid
#define ILVERR_TIMEOUT					0xFA	// -6	No response after defined time.
#define ILVERR_ALREADY_ENROLLED			0xF8	// -8	The person is already in the base
#define ILVERR_BASE_NOT_FOUND			0xF7	// -9	The specified base does not exist
#define ILVERR_BASE_ALREADY_EXISTS		0xF6	//-10	The specified base already exist
#define ILVERR_BIO_IN_PROGRESS			0xF5	//-11	Command received during biometric processing
#define ILVERR_FLASH_INVALID			0xF3	//-13	Flash type invalid
#define ILVERR_NO_SPACE_LEFT			0xF2	//-14	Not Enough memory for the creation of a database
#define ILVERR_BAD_SIGNATURE			0xF0	//-16	Invalid digital signature
#define ILVERR_OUT_OF_FIELD				0xEB	//-21
#define	ILVERR_FIELD_NOT_FOUND			0xE9	//-23	Field does not exist.
#define	ILVERR_FIELD_INVALID			0xE8	//-24	Field size or field name is invalid.
#define ILVERR_USER_NOT_FOUND			0xE6	//-26	The searched user is not found.
#define ILVERR_CMDE_ABORTED				0xE5	//-27	Commanded has been aborted by the user.
#define ILVERR_SAME_FINGER				0xE4	//-28	There are two templates of the same finger
#define ILVERR_NO_HIT					0xE3	//-29	Presented finger does not match
#define ILVERR_SECU_CERTIF_NOT_EXIST	0xE2	//-30	The required certificate does not exist
#define ILVERR_SECU_BAD_STATE			0xE1	//-31	Invalid security state
#define ILVERR_SECU_ANTIREPLAY			0xE0	//-32	An antireplay error occured
#define ILVERR_SECU_ASN1				0xDF	//-33	Error while decoding an ASN1 object
#define ILVERR_SECU						0xDE	//-34	Cryptographic error
#define ILVERR_SECU_AUTHENTICATION		0xDD	//-35	Mutual authentication error

#define ILVERR_NOT_IMPLEMENTED			0x9D	//-99	The request is not yet implemented.

// Status codes

#define ILVSTS_OK							0	//0x00	Successful	
#define ILVSTS_HIT							1	//0x01	Authentication or Identification succeed
#define ILVSTS_NO_HIT						2	//0x02	Authentication or Identification failed	
#define ILVSTS_LATENT						3	//0x03	Security Protection Triggered
#define ILVSTS_DB_FULL						4	//0x04	The database is full.
#define ILVSTS_DB_EMPTY						5	//0x05	The database is empty.
#define ILVSTS_DB_OK						7	//0x07	The database is right.	
#define ILVSTS_DB_KO						10	//0x0A	The flash can not be accessed

// Constants

#define ID_PKCOMP							2	//0x02	// PKCOMP (minutiae)
#define ID_PKMAT							3	//0x03	// PKMAT  (minutiae)
#define ID_USER_ID							4	//0x04	// User ID
#define ID_USER_DATA						5	//0x05	// User data
#define ID_COM1								6	//0x06	// Identify the first serial link of the terminal
#define ID_FIELD							15	//0x0F
#define ID_FIELD_SIZE						16	//0x10
#define ID_TIME_STAMP						17	//0x11
#define ID_PUC_DATA							20	//0x14	// General data
#define ID_DESC_PRODUCT						41	//0x29	// Product descriptor
#define ID_DESC_SOFTWARE					42	//0x2A	// Software descriptor
#define ID_DESC_SENSOR						43	//0x2B	// Sensor descriptor
#define ID_COMPRESSION_NULL					44	//0x2C	// No compression is used
#define ID_FORMAT_TEXT						47	//0x2F	// text format for descriptor
#define	ID_PRIVATE_FIELD					49	//0x31	// Database private field
#define	ID_FIELD_CONTENT					50	//0x32
#define ID_ASYNCHRONOUS_EVENT				52	//0x34	// Reception of asynchronous events during live finger acquisition
#define	ID_PKMAT_NORM						53	//0x35
#define	ID_USER_INDEX						54  //0x36	
#define	ID_PKCOMP_NORM						55	//0x37	
#define ID_BIO_ALGO_PARAM					56	//0x38	// Used to define specific biometric parameter like coder rev., normalization, matcher rev.
#define ID_FIND_USER_DATA					57	//0x39	// Identifies a pattern to find
#define ID_PKBASE							58	//0x3A
#define ID_EXPORT_IMAGE						61	//0x3D
#define	ID_COMPRESSION						62	//0x3E				
#define ID_X509_CERTIFICATE					80	//0x50	// X509 certificate
#define ID_PKCS12_TOKEN						81	//0x51	// PKCS#12 token
#define ID_CRYPTOGRAM_MUTUAL_AUTH			82	//0x52	// Cryptogram exchanged during mutual authentication (secure protocol option) 
#define ID_TKB_SETTINGS						83	//0x53
#define	ID_X984_PARAM						84	//0x54
#define	ID_X984								85	//0x55
#define ID_MATCHING_SCORE					86	//0x56
#define	ID_TKB								87	//0x57
