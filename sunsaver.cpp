/*
 *  sunsaver.cpp - This program reads all the RAM and EEPROM registers on a Morningstar SunSaver MPPT and prints the results.
 *  

This software was derived from work done by Tom Rinehart.  Thank you.

Copyright 2012 Tom Rinehart.
Updated 2018 by Andrew Schoewe.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/* Compile with: g++ `pkg-config --cflags --libs libmodbus` sunsaver.cpp -o sunsaver */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>
#include <unistd.h>
#include <iostream>
using namespace std;

#define SUNSAVERMPPT    0x01	/* MODBUS Address of the SunSaver MPPT */

modbus_t* connect(string *devicePath);
int disconnect(modbus_t *ctx);
void read(modbus_t *ctx);
void writeRegister(modbus_t *ctx);
void writeCoil(modbus_t *ctx);
void logs(modbus_t *ctx);
int _writeRegister(modbus_t *ctx, int addr, float rawInput);
int _writeCoil(modbus_t *ctx, int addr, int status);

int main(void)
{
	modbus_t *ctx = NULL;
	short Ths, menuChoice;
	string devicePath = "/dev/tty.usbserial-DO001BL8";  //TODO

	cout << "Morningstar SunSaver MPPT Charge Controller Settings" << endl << endl;

	cout << "Choose mode..." << endl;
	cout << "1. Read" << endl;
	cout << "2. Write" << endl;
	cout << "3. Logs" << endl;
	cout << "Mode: ";

	cin >> menuChoice;
	cout << endl;

	//sleep(5);
	
	/* Set up a new MODBUS context */
	/* Make sure to set the appropriate path for your computer and device */
	ctx = connect(&devicePath);
	if (ctx == NULL) {
		return -1;
	}
	
	if(menuChoice == 1) {
		read(ctx);
	} else if(menuChoice == 2) {
		writeRegister(ctx);
	} else if(menuChoice == 3) {
		logs(ctx);
	} else if(menuChoice == 3) {
		writeCoil(ctx);
	} else {
		printf("Invalid choice.");
		return -1;
	}

	return 0;

	/* Close the MODBUS connection */
	return disconnect(ctx);
}

modbus_t* connect(string *devicePath) {

	modbus_t *ctx = NULL;

	/* Set up a new MODBUS context */
	/* Make sure to set the appropriate path for your computer and device */
	cout << "Attempting to connect to " << *devicePath << endl << endl;
	ctx = modbus_new_rtu((*devicePath).c_str(), 9600, 'N', 8, 2);
	if (ctx == NULL) {
		fprintf(stderr, "Unable to create the libmodbus context\n");
		return ctx;
	}

	/* Define a new timeout of 750ms */
	modbus_set_response_timeout(ctx, 0, 750000);
	
	/* Set the slave id to the SunSaverMPPT MODBUS id */
	modbus_set_slave(ctx, SUNSAVERMPPT);
	
	/* Open the MODBUS connection to the SunSaverMPPT */
	if (modbus_connect(ctx) == -1) {
        	fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        	modbus_free(ctx);
        	return NULL;
	}

	return ctx;
}

int disconnect(modbus_t *ctx) {
	modbus_close(ctx);
	modbus_free(ctx);
	
	return(0);

}

void read(modbus_t *ctx) {
	int rc;
	float EV_reg, EV_float, EV_floatlb_trip, EV_float_cancel, EV_eq;
	unsigned short Et_float, Et_floatlb, Et_float_exit_cum, Et_eqcalendar, Et_eq_above, Et_eq_reg;
	float EV_reg2, EV_float2, EV_floatlb_trip2, EV_float_cancel2, EV_eq2;
	float Adc_vb_f, Adc_va_f, Adc_vl_f, Adc_ic_f, Adc_il_f, Power_out, Vb_f, Vb_ref;
	short T_hs, T_batt;
	unsigned short Et_float2, Et_floatlb2, Et_float_exit_cum2, Et_eqcalendar2, Et_eq_above2, Et_eq_reg2;
	float EV_tempcomp, EV_hvd, EV_hvr, Evb_ref_lim;
	short ETb_max, ETb_min;
	float EV_lvd, EV_lvr, EV_lhvd, EV_lhvr, ER_icomp, Et_lvd_warn;
	float EV_soc_y2g, EV_soc_g2y, EV_soc_y2r0, EV_soc_r2y;
	unsigned short Emodbus_id, Emeter_id;
	float Eic_lim;
	unsigned int Ehourmeter;
	short Etmr_eqcalendar;
	float EAhl_r, EAhl_t, EAhc_r, EAhc_t, EkWhc, EVb_min, EVb_max, EVa_max;
	float Vb_min_daily, Vb_max_daily, Ahc_daily, Ahl_daily;
	short charge_state;
	uint16_t data[50];
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE000, 11, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("EEPROM Registers\n\n");
	
	printf("Charge Settings (bank 1)\n");
	
	EV_reg=data[0]*100.0/32768.0;
	printf("EV_reg = %.2f V\n",EV_reg);
	
	EV_float=data[1]*100.0/32768.0;
	printf("EV_float = %.2f V\n",EV_float);
	
	Et_float=data[2];
	printf("Et_float = %d s\n",Et_float);
	
	Et_floatlb=data[3];
	printf("Et_floatlb = %d s\n",Et_floatlb);
	
	EV_floatlb_trip=data[4]*100.0/32768.0;
	printf("EV_floatlb_trip = %.2f V\n",EV_floatlb_trip);
	
	EV_float_cancel=data[5]*100.0/32768.0;
	printf("EV_float_cancel = %.2f V\n",EV_float_cancel);
	
	Et_float_exit_cum=data[6];
	printf("Et_float_exit_cum = %d s\n",Et_float_exit_cum);
	
	EV_eq=data[7]*100.0/32768.0;
	printf("EV_eq = %.2f V\n",EV_eq);
	
	Et_eqcalendar=data[8];
	printf("Et_eqcalendar = %d days\n",Et_eqcalendar);
	
	Et_eq_above=data[9];
	printf("Et_eq_above = %d s\n",Et_eq_above);
	
	Et_eq_reg=data[10];
	printf("Et_eq_reg = %d s\n",Et_eq_reg);
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE00D, 11, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nCharge Settings (bank 2)\n");
	
	EV_reg2=data[0]*100.0/32768.0;
	printf("EV_reg2 = %.2f V\n",EV_reg2);
	
	EV_float2=data[1]*100.0/32768.0;
	printf("EV_float2 = %.2f V\n",EV_float2);
	
	Et_float2=data[2];
	printf("Et_float2 = %d s\n",Et_float2);
	
	Et_floatlb2=data[3];
	printf("Et_floatlb2 = %d s\n",Et_floatlb2);
	
	EV_floatlb_trip2=data[4]*100.0/32768.0;
	printf("EV_floatlb_trip2 = %.2f V\n",EV_floatlb_trip2);
	
	EV_float_cancel2=data[5]*100.0/32768.0;
	printf("EV_float_cancel2 = %.2f V\n",EV_float_cancel2);
	
	Et_float_exit_cum2=data[6];
	printf("Et_float_exit_cum2 = %d s\n",Et_float_exit_cum2);
	
	EV_eq2=data[7]*100.0/32768.0;
	printf("EV_eq2 = %.2f V\n",EV_eq2);
	
	Et_eqcalendar2=data[8];
	printf("Et_eqcalendar2 = %d days\n",Et_eqcalendar2);
	
	Et_eq_above2=data[9];
	printf("Et_eq_above2 = %d s\n",Et_eq_above2);
	
	Et_eq_reg2=data[10];
	printf("Et_eq_reg2 = %d s\n",Et_eq_reg2);
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE01A, 6, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nCharge Settings (shared)\n");
	
	EV_tempcomp=data[0]*100.0/65536.0;
	printf("EV_tempcomp = %.2f V\n",EV_tempcomp);
	
	EV_hvd=data[1]*100.0/32768.0;
	printf("EV_hvd = %.2f V\n",EV_hvd);
	
	EV_hvr=data[2]*100.0/32768.0;
	printf("EV_hvr = %.2f V\n",EV_hvr);
	
	Evb_ref_lim=data[3]*100.0/32768.0;
	printf("Evb_ref_lim = %.2f V\n",Evb_ref_lim);
	
	ETb_max=data[4];
	printf("ETb_max = %d °C\n",ETb_max);
	
	ETb_min=data[5];
	printf("ETb_min = %d °C\n",ETb_min);
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE022, 6, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nLoad Settings\n");
	
	EV_lvd=data[0]*100.0/32768.0;
	printf("EV_lvd = %.2f V\n",EV_lvd);
	
	EV_lvr=data[1]*100.0/32768.0;
	printf("EV_lvr = %.2f V\n",EV_lvr);
	
	EV_lhvd=data[2]*100.0/32768.0;
	printf("EV_lhvd = %.2f V\n",EV_lhvd);
	
	EV_lhvr=data[3]*100.0/32768.0;
	printf("EV_lhvr = %.2f V\n",EV_lhvr);
	
	ER_icomp=data[4]*1.263/65536.0;
	printf("ER_icomp = %.2f ohms\n",ER_icomp);
	
	Et_lvd_warn=data[5]*0.1;
	printf("Et_lvd_warn = %.2f s\n",Et_lvd_warn);
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE030, 6, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nMisc Settings\n");
	
	EV_soc_y2g=data[0]*100.0/32768.0;
	printf("EV_soc_y2g = %.2f V\n",EV_soc_y2g);
	
	EV_soc_g2y=data[1]*100.0/32768.0;
	printf("EV_soc_g2y = %.2f V\n",EV_soc_g2y);
	
	EV_soc_y2r0=data[2]*100.0/32768.0;
	printf("EV_soc_y2r0 = %.2f V\n",EV_soc_y2r0);
	
	EV_soc_r2y=data[3]*100.0/32768.0;
	printf("EV_soc_r2y = %.2f V\n",EV_soc_r2y);
	
	Emodbus_id=data[4];
	printf("Emodbus_id = %d\n",Emodbus_id);
	
	Emeter_id=data[5];
	printf("Emeter_id = %d\n",Emeter_id);
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE038, 1, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nPPT Settings\n");
	
	Eic_lim=data[0]*79.16/32768.0;
	printf("Eic_lim = %.2f A\n",Eic_lim);
	
	/* Read the EEPROM Registers and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0xE040, 15, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nRead only section of EEPROM\n");
	
	Ehourmeter=(data[1] << 16) + data[0];
	printf("Ehourmeter = %d h\n",Ehourmeter);
	
	EAhl_r=((data[3] << 16) + data[2])*0.1;
	printf("EAhl_r = %.2f Ah\n",EAhl_r);
	
	EAhl_t=((data[5] << 16) + data[4])*0.1;
	printf("EAhl_t = %.2f Ah\n",EAhl_t);
	
	EAhc_r=((data[7] << 16) + data[6])*0.1;
	printf("EAhc_r = %.2f Ah\n",EAhc_r);
	
	EAhc_t=((data[9] << 16) + data[8])*0.1;
	printf("EAhc_t = %.2f Ah\n",EAhc_t);
	
	EkWhc=data[10]*0.1;
	printf("EkWhc = %.2f kWh\n",EkWhc);
	
	EVb_min=data[11]*100.0/32768.0;
	printf("EVb_min = %.2f V\n",EVb_min);
	
	EVb_max=data[12]*100.0/32768.0;
	printf("EVb_max = %.2f V\n",EVb_max);
	
	EVa_max=data[13]*100.0/32768.0;
	printf("EVa_max = %.2f V\n",EVa_max);
	
	Etmr_eqcalendar=data[14];
	printf("Etmr_eqcalendar = %d days\n",Etmr_eqcalendar);

	/* Read the RAM and convert the results to their proper values */
	rc = modbus_read_registers(ctx, 0x0008, 7, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}
	
	printf("\nRAM\n");

	Adc_vb_f=data[0]*100.0/32768.0;
	printf("Adc_vb_f = %.2f V (1sec avg)\n",Adc_vb_f);

	Adc_va_f=data[1]*100.0/32768.0;
	printf("Adc_va_f = %.2f V (1sec avg)\n",Adc_va_f);

	Adc_vl_f=data[2]*100.0/32768.0;
	printf("Adc_vl_f = %.2f V (1sec avg)\n",Adc_vl_f);

	Adc_ic_f=data[3]*79.16/32768.0;
	printf("Adc_ic_f = %.2f A (1sec avg)\n",Adc_ic_f);

	Adc_il_f=data[4]*79.16/32768.0;
	printf("Adc_il_f = %.2f A (1sec avg)\n",Adc_il_f);

	T_hs=data[5];
	printf("T_hs = %d °C\n",T_hs);

	T_batt=data[6];
	printf("T_batt = %d °C\n",T_batt);

	/* Read charge power from RAM and convert the results to proper value */
	rc = modbus_read_registers(ctx, 0x0027, 8, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}

	Power_out=data[0]*989.5/65536.0;
	printf("Power_out = %.2f W\n",Power_out);

	Vb_min_daily=data[4]*100.0/32768.0;
	printf("Vb_min_daily (resets after dark) = %.2f V\n",Vb_min_daily);

	Vb_max_daily=data[5]*100.0/32768.0;
	printf("Vb_max_daily (resets after dark) = %.2f V\n",Vb_max_daily);

	Ahc_daily=data[6]*0.1;
	printf("Ahc_daily (resets after dark) = %.2f Ah\n",Ahc_daily);
		
	Ahl_daily=data[7]*0.1;
	printf("Ahl_daily (resets after dark) = %.2f Ah\n",Ahl_daily);

	rc = modbus_read_registers(ctx, 0x0011, 4, data);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return;
	}

	charge_state=data[0];
	printf("charge_state = %d (0=start,1=night_check,2=disconnect,3=night,4=fault,5=bulk_charge,6=absorption,7=float,8=equalize)\n",charge_state);

	Vb_f=data[2]*100.0/32768.0;
	printf("Vb_f = %.2f V (25sec avg)\n",Vb_f);

	Vb_ref=data[2]*96.667/32768.0;
	printf("Vb_ref = %.2f V\n",Vb_ref);
	
		
}

void writeRegister(modbus_t *ctx) {
	int rc;
	float rawInput;

	/***************
	*
	* Write EV_reg2
	*
	****************/
	
	// Regulation Charge Voltage - EV_reg2
	// valueInBase10 = (12.4V * 65535) / 16.92 = 48028.014184397163121
	// valueInBase16 = 0xBB9C
	cout << "Regulation Charge Voltage: ";
	cin >> rawInput;
	cout << endl;
	// bank1
	rc = _writeRegister(ctx, 0xE000, rawInput);
	if(rc == 1) {
		printf("Successfully updated Regulation Charge Voltage for bank1\n");
	} else {
		printf("Update of Regulation Charge Voltage for bank1 Failed");
	}

	//bank2
	rc = _writeRegister(ctx, 0xE00D, rawInput);
	if(rc == 1) {
		printf("Successfully updated Regulation Charge Voltage for bank2\n");
	} else {
		printf("Update of Regulation Charge Voltagefor bank2 Failed");
	}
}

void writeCoil(modbus_t *ctx) {
	int rc;
	int status;

	/***************
	*
	* Reset control (send '1')
	*
	****************/
	
	cout << "System Reset (enter 1): ";
	cin >> status;
	cout << endl;
	rc = _writeCoil(ctx, 0x00FF, status);
	if(rc == 1) {
		printf("Successful system reset\n");
	} else {
		printf("System reset Failed");
	}
}

void logs(modbus_t *ctx) {
	int i, rc;
	unsigned int hourmeter, alarm_daily;
	float Vb_min_daily, Vb_max_daily, Ahc_daily, Ahl_daily, Va_max_daily;
	unsigned short array_fault_daily, load_fault_daily, time_ab_daily, time_eq_daily, time_fl_daily;
	unsigned short data[16];

	for(i=0x8000; i<0x81FF; i+=0x0010) {
		
		/* Read the log registers and convert the results to their proper values */
		rc = modbus_read_registers(ctx,  i, 13, data);
		if (rc == -1) {
			fprintf(stderr, "%s\n", modbus_strerror(errno));
			return;
		}
		
		printf("Log Record: 0x%0X\n\n",i);
		
		hourmeter=data[0] + ((data[1] & 0x00FF) << 16);
		printf("hourmeter = %d h\n",hourmeter);
		
		alarm_daily=(data[2] << 8) + (data[1] >> 8);
		printf("alarm_daily = Today's controller self-diagnostic alarms:\n");
		if (alarm_daily == 0) {
			printf("\tNo alarms\n");
		} else {
			if (alarm_daily & 1) printf("\tRTS open\n");
			if ((alarm_daily & (1 << 1)) >> 1) printf("\tRTS shorted\n");
			if ((alarm_daily & (1 << 2)) >> 2) printf("\tRTS disconnected\n");
			if ((alarm_daily & (1 << 3)) >> 3) printf("\tThs open\n");
			if ((alarm_daily & (1 << 4)) >> 4) printf("\tThs shorted\n");
			if ((alarm_daily & (1 << 5)) >> 5) printf("\tSSMPPT hot\n");
			if ((alarm_daily & (1 << 6)) >> 6) printf("\tCurrent limit\n");
			if ((alarm_daily & (1 << 7)) >> 7) printf("\tCurrent offset\n");
			if ((alarm_daily & (1 << 8)) >> 8) printf("\tUndefined\n");
			if ((alarm_daily & (1 << 9)) >> 9) printf("\tUndefined\n");
			if ((alarm_daily & (1 << 10)) >> 10) printf("\tUncalibrated\n");
			if ((alarm_daily & (1 << 11)) >> 11) printf("\tRTS miswire\n");
			if ((alarm_daily & (1 << 12)) >> 12) printf("\tUndefined\n");
			if ((alarm_daily & (1 << 13)) >> 13) printf("\tUndefined\n");
			if ((alarm_daily & (1 << 14)) >> 14) printf("\tMiswire\n");
			if ((alarm_daily & (1 << 15)) >> 15) printf("\tFET open\n");
			if ((alarm_daily & (1 << 16)) >> 16) printf("\tP12\n");
			if ((alarm_daily & (1 << 17)) >> 17) printf("\tHigh Va current limit\n");
			if ((alarm_daily & (1 << 18)) >> 18) printf("\tAlarm 19\n");
			if ((alarm_daily & (1 << 19)) >> 19) printf("\tAlarm 20\n");
			if ((alarm_daily & (1 << 20)) >> 20) printf("\tAlarm 21\n");
			if ((alarm_daily & (1 << 21)) >> 21) printf("\tAlarm 22\n");
			if ((alarm_daily & (1 << 22)) >> 22) printf("\tAlarm 23\n");
			if ((alarm_daily & (1 << 23)) >> 23) printf("\tAlarm 24\n");
		}
		
		Vb_min_daily=data[3]*100.0/32768.0;
		printf("Vb_min_daily = %.2f V\n",Vb_min_daily);
		
		Vb_max_daily=data[4]*100.0/32768.0;
		printf("Vb_max_daily = %.2f V\n",Vb_max_daily);
		
		Ahc_daily=data[5]*0.1;
		printf("Ahc_daily = %.2f Ah\n",Ahc_daily);
		
		Ahl_daily=data[6]*0.1;
		printf("Ahl_daily = %.2f Ah\n",Ahl_daily);
		
		array_fault_daily=data[7];
		printf("array_fault_daily = Today's solar input self-diagnostic faults:\n");
		if (array_fault_daily == 0) {
			printf("\tNo faults\n");
		} else {
			if (array_fault_daily & 1) printf("\tOvercurrent\n");
			if ((array_fault_daily & (1 << 1)) >> 1) printf("\tFETs shorted\n");
			if ((array_fault_daily & (1 << 2)) >> 2) printf("\tSoftware bug\n");
			if ((array_fault_daily & (1 << 3)) >> 3) printf("\tBattery HVD\n");
			if ((array_fault_daily & (1 << 4)) >> 4) printf("\tArray HVD\n");
			if ((array_fault_daily & (1 << 5)) >> 5) printf("\tEEPROM setting edit (reset required)\n");
			if ((array_fault_daily & (1 << 6)) >> 6) printf("\tRTS shorted\n");
			if ((array_fault_daily & (1 << 7)) >> 7) printf("\tRTS was valid, now disconnected\n");
			if ((array_fault_daily & (1 << 8)) >> 8) printf("\tLocal temperature sensor failed\n");
			if ((array_fault_daily & (1 << 9)) >> 9) printf("\tFault 10\n");
			if ((array_fault_daily & (1 << 10)) >> 10) printf("\tFault 11\n");
			if ((array_fault_daily & (1 << 11)) >> 11) printf("\tFault 12\n");
			if ((array_fault_daily & (1 << 12)) >> 12) printf("\tFault 13\n");
			if ((array_fault_daily & (1 << 13)) >> 13) printf("\tFault 14\n");
			if ((array_fault_daily & (1 << 14)) >> 14) printf("\tFault 15\n");
			if ((array_fault_daily & (1 << 15)) >> 15) printf("\tFault 16\n");
		}
		
		load_fault_daily=data[8];
		printf("load_fault_daily = Today's load output self-diagnostic faults:\n");
		if (load_fault_daily == 0) {
			printf("\tNo faults\n");
		} else {
			if (load_fault_daily & 1) printf("\tExternal short circuit\n");
			if ((load_fault_daily & (1 << 1)) >> 1) printf("\tOvercurrent\n");
			if ((load_fault_daily & (1 << 2)) >> 2) printf("\tFETs shorted\n");
			if ((load_fault_daily & (1 << 3)) >> 3) printf("\tSoftware bug\n");
			if ((load_fault_daily & (1 << 4)) >> 4) printf("\tHVD\n");
			if ((load_fault_daily & (1 << 5)) >> 5) printf("\tHeatsink over-temperature\n");
			if ((load_fault_daily & (1 << 6)) >> 6) printf("\tEEPROM setting edit (reset required)\n");
			if ((load_fault_daily & (1 << 7)) >> 7) printf("\tFault 8\n");
		}
		
		Va_max_daily=data[9]*100.0/32768.0;
		printf("Va_max_daily = %.2f V\n",Va_max_daily);
		
		time_ab_daily=data[10];
		printf("time_ab_daily = %d min\n",time_ab_daily);
		
		time_eq_daily=data[11];
		printf("time_eq_daily = %d min\n",time_eq_daily);
		
		time_fl_daily=data[12];
		printf("time_fl_daily = %d min\n\n",time_fl_daily);
	}
}

int _writeRegister(modbus_t *ctx, int addr, float rawInput) {
	float floatInputVal = (rawInput * 32768.0) / 100.0;
	int intInputVal = (int)floatInputVal;
	//cout << intInputVal << endl;
	return modbus_write_register(ctx, addr, intInputVal);
}

int _writeCoil(modbus_t *ctx, int addr, int status) {
	return modbus_write_bit(ctx, addr, status);
}
