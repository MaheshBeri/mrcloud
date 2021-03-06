/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file subscribe_publish_sample.c
 * @brief simple MQTT publish and subscribe on the same topic
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <signal.h>
#include <memory.h>
#include <sys/time.h>
#include <limits.h>
#include <wiringPi.h>

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_interface.h"
#include "aws_iot_config.h"
#include "cJSON.h"

int32_t threshold_temperature = 35;
int32_t new_threshold_temperature = 0;
int32_t temperature = 0;
int32_t old_temperature = 0;
int MQTTcallbackHandler(MQTTCallbackParams params) {

	INFO("Subscribe callback");
	/*INFO("%.*s\t%.*s",
			(int)params.TopicNameLen, params.pTopicName,
			(int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);*/
	threshold_temperature=atoi((char*)params.MessageParams.pPayload);	
	INFO("Setting new threshold temperature as  %d",threshold_temperature );
	INFO("temperature... %d  threshold %d",temperature,threshold_temperature);

	
/*	cJSON * root = cJSON_Parse((char*)params.MessageParams.pPayload);
	cJSON *state_item = cJSON_GetObjectItem(root, "state");	
	cJSON *desired_item = cJSON_GetObjectItem(state_item, "desired");
	if(cJSON_IsNull(desired_item)) {
		INFO("Processing new threshold  %s",desired_item);		
		cJSON *temp_item = cJSON_GetObjectItem(desired_item, "temp");
		new_threshold_temperature=atoi(temp_item->valuestring);	
		if(new_threshold_temperature!=threshold_temperature){
			INFO("Setting new threshold temperature as  %d",threshold_temperature );
			INFO("temperature... %d  threshold %d",temperature,threshold_temperature);
			threshold_temperature=new_threshold_temperature;
		}
	}*/
	return 0;
}

void disconnectCallbackHandler(void) {
	WARN("MQTT Disconnect");
	IoT_Error_t rc = NONE_ERROR;
	if(aws_iot_is_autoreconnect_enabled()){
		INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
	}else{
		WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect();
		if(RECONNECT_SUCCESSFUL == rc){
			WARN("Manual Reconnect Successful");
		}else{
			WARN("Manual Reconnect Failed - %d", rc);
		}
	}
}
	

/**
 * @brief Default cert location
 */
char certDirectory[PATH_MAX + 1] = "../../certs";

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t port = AWS_IOT_MQTT_PORT;

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
uint32_t publishCount = 0;

void parseInputArgsForConnectParams(int argc, char** argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h:p:c:x:"))) {
		switch (opt) {
		case 'h':
			strcpy(HostAddress, optarg);
			DEBUG("Host %s", optarg);
			break;
		case 'p':
			port = atoi(optarg);
			DEBUG("arg %s", optarg);
			break;
		case 'c':
			strcpy(certDirectory, optarg);
			DEBUG("cert root directory %s", optarg);
			break;
		case 'x':
			publishCount = atoi(optarg);
			DEBUG("publish %s times\n", optarg);
			break;
		case '?':
			if (optopt == 'c') {
				ERROR("Option -%c requires an argument.", optopt);
			} else if (isprint(optopt)) {
				WARN("Unknown option `-%c'.", optopt);
			} else {
				WARN("Unknown option character `\\x%x'.", optopt);
			}
			break;
		default:
			ERROR("Error in command line argument parsing");
			break;
		}
	}

}

int main(int argc, char** argv) {
	IoT_Error_t rc = NONE_ERROR;
	int32_t i = 0;
	int32_t flagSlack = 0;
	bool infinitePublishFlag = true;

printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );

        if ( wiringPiSetup() == -1 )
                exit( 1 );


	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];
	char cafileName[] = AWS_IOT_ROOT_CA_FILENAME;
	char clientCRTName[] = AWS_IOT_CERTIFICATE_FILENAME;
	char clientKeyName[] = AWS_IOT_PRIVATE_KEY_FILENAME;
	char alertFlag[]="true";

	parseInputArgsForConnectParams(argc, argv);

	INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	getcwd(CurrentWD, sizeof(CurrentWD));
	sprintf(rootCA, "%s/%s/%s", CurrentWD, certDirectory, cafileName);
	sprintf(clientCRT, "%s/%s/%s", CurrentWD, certDirectory, clientCRTName);
	sprintf(clientKey, "%s/%s/%s", CurrentWD, certDirectory, clientKeyName);

	DEBUG("rootCA %s", rootCA);
	DEBUG("clientCRT %s", clientCRT);
	DEBUG("clientKey %s", clientKey);

	MQTTConnectParams connectParams = MQTTConnectParamsDefault;

	connectParams.KeepAliveInterval_sec = 10;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = "CSDK-test-device";
	connectParams.pHostURL = HostAddress;
	connectParams.port = port;
	connectParams.isWillMsgPresent = false;
	connectParams.pRootCALocation = rootCA;
	connectParams.pDeviceCertLocation = clientCRT;
	connectParams.pDevicePrivateKeyLocation = clientKey;
	connectParams.mqttCommandTimeout_ms = 40000;
	connectParams.tlsHandshakeTimeout_ms = 45000;
	connectParams.isSSLHostnameVerify = true; // ensure this is set to true for production
	connectParams.disconnectHandler = disconnectCallbackHandler;

	INFO("Connecting...");
	rc = aws_iot_mqtt_connect(&connectParams);
	if (NONE_ERROR != rc) {
		ERROR("Error(%d) connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
	}
	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_mqtt_autoreconnect_set_status(true);
	if (NONE_ERROR != rc) {
		ERROR("Unable to set Auto Reconnect to true - %d", rc);
		return rc;
	}

	MQTTSubscribeParams subParams = MQTTSubscribeParamsDefault;
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = "sdkTest/config";
	//subParams.pTopic = "$aws/things/HydraulicPump/shadow/update/accepted";
	subParams.qos = QOS_0;

	if (NONE_ERROR == rc) {
		INFO("Subscribing...");
		rc = aws_iot_mqtt_subscribe(&subParams);
		if (NONE_ERROR != rc) {
			ERROR("Error subscribing");
		}
	}

	MQTTMessageParams Msg = MQTTMessageParamsDefault;
	Msg.qos = QOS_0;
	char cPayload[100];

	Msg.pPayload = (void *) cPayload;

	MQTTPublishParams Params = MQTTPublishParamsDefault;
	//Params.pTopic = "sdkTest/alert";
        Params.pTopic = "$aws/things/HydraulicPump/shadow/update";

	if (publishCount != 0) {
		infinitePublishFlag = false;
	}
	initmotor();
	INFO("-->starting");
	while ((NETWORK_ATTEMPTING_RECONNECT == rc || RECONNECT_SUCCESSFUL == rc || NONE_ERROR == rc)
			&& (publishCount > 0 || infinitePublishFlag)) {

		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(100);
		if(NETWORK_ATTEMPTING_RECONNECT == rc){
			INFO("-->sleep");
			sleep(1);
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}
		//INFO("-->sleep");
		//sleep(1);
		delay(100);
		
		temperature=read_dht11_dat();
		if(temperature==0)
			temperature=old_temperature;

		if(temperature>threshold_temperature){
			blink(1);
			startmotor(1);
			strcpy(alertFlag, "true");

		}else{
			blink(0);
			startmotor(0);
			strcpy(alertFlag, "false");

		}
		//sprintf(cPayload, "%s : %d ", "Device KJT7659 , Temperature Alert ", temperature);
		sprintf(cPayload,"{\"state\":{\"reported\":{\"temp\":\"%d\",\"pressure\":\"2\",\"oil_temp\":\"%d\",\"alert\":\"%s\"}}}",threshold_temperature,temperature,alertFlag);
		i++;
		Msg.PayloadLen = strlen(cPayload) + 1;
		Params.MessageParams = Msg;

		if(temperature!=old_temperature ){
			INFO("temperature... %d  threshold %d",temperature,threshold_temperature);

			if(temperature>threshold_temperature || old_temperature==0){
				INFO("-->Publishing ");
				rc = aws_iot_mqtt_publish(&Params);
				if (publishCount > 0) {
					publishCount--;
				}
                        }
		}
			//if(flagSlack==0){
				//publish_to_slack(temperature);
				//flagSlack=1;
			//}
		//}else{
		//	flagSlack=0;
		//}
		old_temperature=temperature;
	}

	if (NONE_ERROR != rc) {
		ERROR("An error occurred in the loop.\n");
		ERROR("Error code %d.\n",rc);
	} else {
		INFO("Publish done\n");
	}

	return rc;
}

