#include "services.h"

uint8_t sample_char_value;
volatile int connected = FALSE;
volatile uint8_t set_connectable = 1;
volatile uint16_t connection_handle = 0;
volatile uint8_t notification_enabled = FALSE;
volatile AxesRaw_t axes_data = {0, 0, 0};

uint16_t sampleServHandle, TXCharHandle, RXCharHandle;
uint16_t sampleServHandle, sampleCharHandle;
uint16_t wsampleServHandle, wsampleCharHandle;
uint16_t accServHandle, accCharHandle, acc_x_CharHandle, acc_y_CharHandle, acc_z_CharHandle, acc_aws_CharHandle;
uint16_t buttonServHandle, buttonCharHandle;
uint16_t pdataServHandle, pdataCharHandle;

extern uint8_t bnrg_expansion_board;

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)


//UUID Definitions
#define COPY_SAMPLE_SERVICE_UUID(uuid_struct)			COPY_UUID_128(uuid_struct,0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_SAMPLE_CHAR_UUID(uuid_struct)				COPY_UUID_128(uuid_struct,0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define COPY_ACC_SERVICE_UUID(uuid_struct)  			COPY_UUID_128(uuid_struct,0x03,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
//#define COPY_ACC_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe3,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ACC_X_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe4,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ACC_Y_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe5,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ACC_Z_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe6,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ACC_AWS_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xf3,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)


#define COPY_BUTTON_SERVICE_UUID(uuid_struct)  			COPY_UUID_128(uuid_struct,0x04,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_BUTTON_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe7,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define COPY_W_SAMPLE_SERVICE_UUID(uuid_struct)  			COPY_UUID_128(uuid_struct,0x05,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_W_SAMPLE_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe8,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

//#define COPY_PDATA_SERVICE_UUID(uuid_struct)  			COPY_UUID_128(uuid_struct,0x06,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
//#define COPY_PDATA_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xe9,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
//#define COPY_PDATA_X_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xf0,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
//#define COPY_PDATA_Y_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xf1,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
//#define COPY_PDATA_Z_CHAR_UUID(uuid_struct)         	COPY_UUID_128(uuid_struct,0xf2,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)




/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/**
 * @brief  Add a Sample service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_Sample_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];
  
  COPY_SAMPLE_SERVICE_UUID(uuid);
	//Adding Service 
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 33, &sampleServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;    
  
  COPY_SAMPLE_CHAR_UUID(uuid);
	//Adding Characteristics 
  ret =  aci_gatt_add_char(sampleServHandle, UUID_TYPE_128, uuid, 1,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &sampleCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
  
  PRINTF("Sample Service Added. Handle 0x%04X, Sample Characteristic Handle: 0x%04X\n",sampleServHandle, sampleCharHandle);	
  return BLE_STATUS_SUCCESS; 
  
fail:
  PRINTF("Error while adding Sample Service.\n");
  return BLE_STATUS_ERROR ;
    
}

/**
 * @brief  Update sample characteristic value.
 *
 * @param  Value to write in the characteristic
 * @retval Status
 */
tBleStatus Sample_Characteristic_Update(uint8_t value)
{  
  tBleStatus ret;
	uint8_t buf[0];
	//buf[0] = value;
	STORE_LE_16(buf, value);
	
  ret = aci_gatt_update_char_value(sampleServHandle, sampleCharHandle, 0, 1, buf);
	
	/*
	0x00: Success
	0x47: Error
	0x60: Invalid handle
	0x61: Invalid parameter
	0x64: Insufficient resources
	*/
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating sample characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	
	printf ("Sample Success Update\n");
  return BLE_STATUS_SUCCESS;	
}

/**
 * @brief  Add a Write Sample service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_W_Sample_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];
  
  COPY_W_SAMPLE_SERVICE_UUID(uuid);
	//Adding Service 
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 7, &wsampleServHandle);
  if (ret != BLE_STATUS_SUCCESS) {
		printf ("aci_gatt_add_serv: Fail\n");
		goto fail; 
	}   
  
  COPY_W_SAMPLE_CHAR_UUID(uuid);
	//Adding Characteristics 
//  ret =  aci_gatt_add_char(wsampleServHandle, UUID_TYPE_128, uuid, 1,
//                           CHAR_PROP_WRITE|CHAR_PROP_NOTIFY,
//                           ATTR_PERMISSION_NONE,
//                           GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
//                           16, 0, &wsampleCharHandle);
	
	ret =  aci_gatt_add_char(wsampleServHandle, UUID_TYPE_128, uuid, 1,
                           CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP | CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE,
                           16, 0, &wsampleCharHandle);
	
	
	if (ret != BLE_STATUS_SUCCESS) {
		printf ("aci_gatt_add_char: Fail\n");
		goto fail; 
	}   
  
  PRINTF("Sample Write Service Added. Handle 0x%04X, Write Sample Characteristic Handle: 0x%04X\n",wsampleServHandle, wsampleCharHandle);	
  return BLE_STATUS_SUCCESS; 
  
fail:
  PRINTF("Error while adding Write Sample Service.\n");
  return BLE_STATUS_ERROR ;
    
}

/**
 * @brief  Update write sample characteristic value.
 *
 * @param  Value to write in the characteristic
 * @retval Status
 */
tBleStatus WSample_Characteristic_Read(void)
{  
  tBleStatus ret;
	uint8_t buf;
	uint16_t data_len_out_p = 10;
	
  //ret = aci_gatt_read_handle_value(wsampleCharHandle, 10, &data_len_out_p, &buf);
	
	ret = aci_gatt_read_charac_val (connection_handle, wsampleCharHandle);
	
	/*
	0x00: Success
	0x47: Error
	0x60: Invalid handle
	0x61: Invalid parameter
	0x64: Insufficient resources
	*/
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating Read Sample characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	
	printf ("Sample Read Success(0x%02x)\n", ret);
	
	//printf ("Read Value: %d\n", buf);
	
  return BLE_STATUS_SUCCESS;	
}


/**
 * @brief  Add an accelerometer service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_Acc_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];
  
  COPY_ACC_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 16,
                          &accServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;    
	
	// x-component
	COPY_ACC_X_CHAR_UUID(uuid);  
  ret =  aci_gatt_add_char(accServHandle, UUID_TYPE_128, uuid, 6,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &acc_x_CharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	// y-component
	COPY_ACC_Y_CHAR_UUID(uuid);  
  ret =  aci_gatt_add_char(accServHandle, UUID_TYPE_128, uuid, 6,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &acc_y_CharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
		// z-component
	COPY_ACC_Z_CHAR_UUID(uuid);  
  ret =  aci_gatt_add_char(accServHandle, UUID_TYPE_128, uuid, 6,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &acc_z_CharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	// z-component
	COPY_ACC_AWS_CHAR_UUID(uuid);  
  ret =  aci_gatt_add_char(accServHandle, UUID_TYPE_128, uuid, 6,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &acc_aws_CharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
  
  PRINTF("Service ACC added. Handle 0x%04X, Acc Charac handle: 0x%04X, Acc X Charac handle: 0x%04X, Acc Y Charac handle: 0x%04X, Acc Z Charac handle: 0x%04X\n",accServHandle, accCharHandle, acc_x_CharHandle, acc_y_CharHandle, acc_z_CharHandle);	
  return BLE_STATUS_SUCCESS; 
  
fail:
  PRINTF("Error while adding ACC service.\n");
  return BLE_STATUS_ERROR ;
    
}

/**
 * @brief  Update acceleration characteristic value.
 *
 * @param  Structure containing acceleration value in mg
 * @retval Status
 */
tBleStatus Acc_Update(AxesRaw_t *data)
{  
  tBleStatus ret;    

	uint8_t buff_x[2];
	STORE_LE_16(buff_x,data->AXIS_X);
	
	uint8_t buff_y[2];
	STORE_LE_16(buff_y,data->AXIS_Y);
	
	uint8_t buff_z[2];
	STORE_LE_16(buff_z,data->AXIS_Z);
	
	uint8_t buff_aws[2];
	STORE_LE_16(buff_aws,data->AWS);
	
	ret = aci_gatt_update_char_value(accServHandle, acc_x_CharHandle, 0, 2, buff_x);
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating ACC X characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	printf ("Success ACC X Update\n");
	
	ret = aci_gatt_update_char_value(accServHandle, acc_y_CharHandle, 0, 2, buff_y);
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating ACC Y characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	printf ("Success ACC Y Update\n");
	
	ret = aci_gatt_update_char_value(accServHandle, acc_z_CharHandle, 0, 2, buff_z);
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating ACC Z characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	printf ("Success ACC Z Update\n");
	
	ret = aci_gatt_update_char_value(accServHandle, acc_aws_CharHandle, 0, 2, buff_aws);
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating ACC AWS characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	printf ("Success ACC AWS Update\n");
	
  return BLE_STATUS_SUCCESS;	
}


/**
 * @brief  Add an button service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */

tBleStatus Add_Button_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];
  
  COPY_BUTTON_SERVICE_UUID(uuid);
	//Adding Service 
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 7, &buttonServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;    
  
  COPY_BUTTON_CHAR_UUID(uuid);
	//Adding Characteristics 
  ret =  aci_gatt_add_char(buttonServHandle, UUID_TYPE_128, uuid, 1,
                           CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &buttonCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
  
  PRINTF("Button Service Added. Handle 0x%04X, Button Characteristic Handle: 0x%04X\n",buttonServHandle, buttonCharHandle);	
  return BLE_STATUS_SUCCESS; 
  
fail:
  PRINTF("Error while adding Button Service.\n");
  return BLE_STATUS_ERROR ;
    
}

/**
 * @brief  Update button characteristic value.
 *
 * @param  Value to write in the characteristic
 * @retval Status
 */
tBleStatus Button_Characteristic_Update(uint8_t value)
{  
  tBleStatus ret;
	uint8_t buf[0];
	//buf[0] = value;
	buf[0] = 10;
	
  ret = aci_gatt_update_char_value(buttonServHandle, buttonCharHandle, 0, 1, buf);
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating button characteristic.(0x%02x)\n", ret) ;
    return BLE_STATUS_ERROR ;
  }
	
	printf ("Button Success Update\n");
  return BLE_STATUS_SUCCESS;	
}

/**
 * @brief  Add an ProcessedData (PData) service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */

tBleStatus Add_PData_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];
  
  COPY_BUTTON_SERVICE_UUID(uuid);
	//Adding Service 
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 7, &pdataServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;    
  
  COPY_BUTTON_CHAR_UUID(uuid);
	//Adding Characteristics 
  ret =  aci_gatt_add_char(buttonServHandle, UUID_TYPE_128, uuid, 1,
                           CHAR_PROP_WRITE,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &pdataCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
  
  PRINTF("PData Service Added. Handle 0x%04X, PData Characteristic Handle: 0x%04X\n",pdataServHandle, pdataCharHandle);	
  return BLE_STATUS_SUCCESS; 
  
fail:
  PRINTF("Error while adding PData Service.\n");
  return BLE_STATUS_ERROR ;
    
}

/**
 * @brief  Read PData characteristic value.
 *
 * @param  Value to write in the characteristic
 * @retval Status
 */
tBleStatus PData_Characteristic_read(void)
{  
//  tBleStatus ret;
//	tBleStatus ret_value;
//	uint8_t buf[0];
//	//buf[0] = value;
//	buf[0] = 10;
//	
//  ret = aci_gatt_update_char_value(pdataServHandle, buttonCharHandle, 0, 1, buf);
//	
//  if (ret != BLE_STATUS_SUCCESS){
//    PRINTF("Error while updating button characteristic.(0x%02x)\n", ret) ;
//    return BLE_STATUS_ERROR ;
//  }
//	
//	printf ("Button Success Update\n");
  return BLE_STATUS_SUCCESS;	
}

/**
 * @brief  Puts the device in connectable mode.
 *         If you want to specify a UUID list in the advertising data, those data can
 *         be specified as a parameter in aci_gap_set_discoverable().
 *         For manufacture data, aci_gap_update_adv_data must be called.
 * @param  None 
 * @retval None
 */
/* Ex.:
 *
 *  tBleStatus ret;    
 *  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'G','X','X'};    
 *  const uint8_t serviceUUIDList[] = {AD_TYPE_16_BIT_SERV_UUID,0x34,0x12};    
 *  const uint8_t manuf_data[] = {4, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x05, 0x02, 0x01};
 *  
 *  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
 *                                 8, local_name, 3, serviceUUIDList, 0, 0);    
 *  ret = aci_gap_update_adv_data(5, manuf_data);
 *
 */
void setConnectable(void)
{  
  tBleStatus ret;
  
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'S','P','I','C','Y'};
  
  /* disable scan response */
  hci_le_set_scan_resp_data(0,NULL);
  PRINTF("General Discoverable Mode.\n");
  
  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while setting discoverable mode (%d)\n", ret);    
  }  
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{  
  connected = TRUE;
  connection_handle = handle;
  
  PRINTF("Connected to device:");
  for(int i = 5; i > 0; i--){
    PRINTF("%02X-", addr[i]);
  }
  PRINTF("%02X\n", addr[0]);
}

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None 
 * @retval None
 */
void GAP_DisconnectionComplete_CB(void)
{
  connected = FALSE;
  PRINTF("Disconnected\n");
  /* Make the device connectable again. */
  set_connectable = TRUE;
  notification_enabled = FALSE;
}

/**
 * @brief  Read request callback.
 * @param  uint16_t Handle of the attribute
 * @retval None
 */
void Read_Request_CB(uint16_t handle)
{  
  if(handle == sampleCharHandle + 1){
		Sample_Characteristic_Update(sample_char_value);
		PRINTF("Reading Sample Characteristic\n");
	}  
  
  //EXIT:
  if(connection_handle != 0)
    aci_gatt_allow_read(connection_handle);
}

/**
 * @brief  This function is called attribute value corresponding to 
 *         characteristic gets modified.
 * @param  Handle of the attribute
 * @param  Size of the modified attribute data
 * @param  Pointer to the modified attribute data
 * @retval None
 */
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  /* If GATT client has modified characteristic value, read values */
  if(handle == wsampleCharHandle + 1){ 
			printf("new value of wsampleChar\n");
			WSample_Characteristic_Read ();
      //BSP_LED_Toggle(LED2);
  }
}

/**
 * @brief  This function is called attribute value corresponding to 
 *         characteristic gets modified.
 * @param  Handle of the attribute
 * @param  Size of the modified attribute data
 * @param  Pointer to the modified attribute data
 * @retval None
 */
void Char_By_UUID_Rep(uint8_t *att_data)
{
  /* If GATT client has modified characteristic value, read values */
  //if(handle == wsampleCharHandle + 1){ 
			printf("Reponse value of wsampleChar\n");
			//WSample_Characteristic_Read ();
      //BSP_LED_Toggle(LED2);
		printf("Value: %d", &att_data);
  //}
}

/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  /* obtain event packet */
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  if(hci_pckt->type != HCI_EVENT_PKT)
    return;
  
  switch(event_pckt->evt){
    
  case EVT_DISCONN_COMPLETE:
    {
      GAP_DisconnectionComplete_CB();
    }
    break;
    
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
        }
        break;
      }
    }
    break;
    
  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
			PRINTF("EVT_VENDOR %d\n", blue_evt->ecode);
      switch(blue_evt->ecode){
				
			case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:         
			{
				/* this callback is invoked when a GATT attribute is modified
				extract callback data and pass to suitable handler function */
				if (bnrg_expansion_board == IDB05A1) {
					evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
					Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data); 
				}
				else {
					evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
					Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data); 
				}                       
			}
			break;
			
			case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
          {
            //PRINTF("EVT_BLUE_GATT_PROCEDURE_COMPLETE\n\r");
            evt_gatt_procedure_complete *evt = (evt_gatt_procedure_complete*)blue_evt->data;
            PRINTF("EVT_BLUE_GATT_PROCEDURE_COMPLETE error_code=%d\n\r", evt->error_code);
					}
					break;

      case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          evt_gatt_read_permit_req *pr = (void*)blue_evt->data;                    
          Read_Request_CB(pr->attr_handle);                    
        }
        break;
			case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
				{
					PRINTF("EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP\n\r");
					evt_att_read_by_group_resp *evt = (evt_att_read_by_group_resp*)blue_evt->data;
				}
				break;
			case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
				{
					PRINTF("EVT_BLUE_ATT_READ_BY_TYPE_RESP\n\r");
					evt_att_read_by_type_resp *evt = (evt_att_read_by_type_resp*)blue_evt->data;
				}
				break;
			case EVT_BLUE_ATT_READ_RESP:
				{
					PRINTF("EVT_BLUE_ATT_READ_RESP\n\r");
					evt_att_read_resp *evt = (evt_att_read_resp*)blue_evt->data;
					Char_By_UUID_Rep (evt -> attribute_value);
				}
				break;
			case EVT_BLUE_ATT_EXEC_WRITE_RESP:
				{
					PRINTF("EVT_BLUE_ATT_EXEC_WRITE_RESP\n\r");
					evt_att_prepare_write_resp *evt = (evt_att_prepare_write_resp*)blue_evt->data;
				}
				break;
			case EVT_BLUE_ATT_PREPARE_WRITE_RESP:
				{
					PRINTF("EVT_BLUE_ATT_PREPARE_WRITE_RESP\n\r");
					evt_att_prepare_write_resp *evt = (evt_att_prepare_write_resp*)blue_evt->data;
				}
				break;
			case EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP:
				{
					PRINTF("EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP\n\r");
					evt_gatt_disc_read_char_by_uuid_resp *evt = (evt_gatt_disc_read_char_by_uuid_resp*)blue_evt->data;
				}
				break;
			case EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP:
				{
					PRINTF("EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP\n\r");
					evt_att_find_by_type_val_resp *evt = (evt_att_find_by_type_val_resp*)blue_evt->data;
				}
				break;

      }
    }
    break;
  }    
}
