#pragma config(Sensor, port2,  ArmBottomBumper, sensorVexIQ_Touch)
#pragma config(Sensor, port3,  Main_Gyro,      sensorVexIQ_Gyro)
#pragma config(Sensor, port4,  LED,            sensorVexIQ_LED)
#pragma config(Sensor, port7,  CenterColor,    sensorVexIQ_ColorGrayscale)
#pragma config(Motor,  motor1,          Left,          tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor5,          Intake,        tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor6,          Right,         tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor9,          CubeClaw,      tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor10,         ArmRight,      tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor11,         ArmLeft,       tmotorVexIQ, PIDControl, reversed, encoder)
//#pragma config(Sensor, port8,  CrossColor,     sensorVexIQ_ColorGrayscale)
//#pragma config(Sensor, port12, BallDetect,     sensorVexIQ_Touch)
//*!!Code automatically generated by 'ROBOTC' configuration wizard on 13/07/2019, 1448 hours           !!*//

//VIQC_SquaredAway_CompCode
/*************************************PROGRAMMER NOTES*****************************************************/
//
/**********************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																			 				Variables																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ProgramPersmissionToStart = true;

int IntakeSpeed = 100;

bool IndexArmPressed; // defines the variable that check whether the controller lift buttons have been pressed during a sequence

long gyroValue;
long gyroError;
//int CorrectionRatioforDrivingSequences = 600/225;
//int gyrodriftrate = 37;
int PickupBonusSequenceState; // defines the variable that is used to tell what state the Pickup Bonus Sequence is in
int PlaceBonusSequenceState; // defines the variable that is used to tell what state the Place Bonus Sequence is in

// Global variables used by the sample code
int   global_1 = 0;
int   global_2 = 0;
int   global_3 = 0;
int   global_4 = 0;
int   global_5 = 0;
int   global_6 = 0;
int   global_7 = 0;
int   global_8 = 0;

// We have 8 possible places to store data, we call each one a datalog series
// This example uses the first four to store values.
#define   DATALOG_SERIES_0    0
#define   DATALOG_SERIES_1    1
#define   DATALOG_SERIES_2    2
#define   DATALOG_SERIES_3    3
#define   DATALOG_SERIES_4    4
#define   DATALOG_SERIES_5    5
#define   DATALOG_SERIES_6    6
#define   DATALOG_SERIES_7    7
#define   DATALOG_SERIES_8    8

int ticksPerRev = 959.9999999999999999999999999999999999999999999;
float x;
float y;
float LeftDriveTraveled;
float RightDriveTraveled;
float diameter = 63.661977236758134307553505349006;
float DriveWidth = 19.5; //cm  B (base line distance)

bool intakeStarted; // defines the variable that waits until the intake button is pressed before starting Intake

float Height0 = 0; //Floor 2", 8/16
float Height1 = -300; //Position for Cube Raise 5", 8/16
float Height2 = -500; // 9", 15/16
float Height3 = -700; //Ride Height 9", 15/16  //11" 8/16//
float Height4 = -1000; //Place Cube on Low Score Platform 12" 14/16
//float Height5 = -1000; //Height4 = 850;
int ArmPresetValue = 0; //The preset number that tells the preset code how high to move the arm

/*
int SpeedLeft = 0;
int SpeedRight = 0;
int Heading = 0;
int HeadingStraight;
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																						Bool Functions																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BatteryWarning(int MinimumVoltage = 6500, int WarningVoltage = 7000, int SafeVoltage = 8500, int WarningDelay = 0) {
	if (nImmediateBatteryLevel<MinimumVoltage) {
		delay(WarningDelay);
		playRepetitiveSound(soundCarAlarm4, 100);
		setTouchLEDColor(LED, colorRed);
		return true;
		if (nImmediateBatteryLevel<5500) {
			ProgramPersmissionToStart = false;
			stopAllTasks();
			stopAllMotors();
		};
		} else if (nImmediateBatteryLevel<WarningVoltage) {
		delay(WarningDelay);
		playSound(soundCarAlarm2);
		setTouchLEDColor(LED, colorOrange);
		return true;
		}else if (nImmediateBatteryLevel>SafeVoltage) {
		delay(WarningDelay);
		playRepetitiveSound(soundWrongWay, 100);
		setTouchLEDColor(LED, colorOrange);
		return true;
		} else {
		return false;
	};
};

bool Turn(int goHere, float speed){
	return false;
	if(goHere < 0){
		goHere += 360;
	}
	int currentLoc = getGyroHeading(Main_Gyro);
	int toGo = currentLoc - goHere;
	if((toGo <= 180 && toGo >=0) || (toGo < 0 && toGo <= -180)){
		repeatUntil(getGyroHeading(Main_Gyro) == goHere || getGyroHeading(Main_Gyro) == goHere - 360){
			setMotor(Left, speed);
			setMotor(Right, -speed);
			return false;
		}
	}
	else if((toGo > 180 && toGo >= 0) || (toGo < 0 && toGo > -180)){
		repeatUntil(getGyroHeading(Main_Gyro) == goHere || getGyroHeading(Main_Gyro) == goHere - 360){
			setMotor(Left, -speed);
			setMotor(Right, speed);
			return false;
		}
	}
	stopMultipleMotors(Left, Right);
	return true;
}

bool TurnDegrees (float varTurnDegrees) { // turn PID function that returns true or false once finished
	static bool InProgressTask; // defines the static bool that keeps the function looping until it has completed the turn
	if (!InProgressTask) { // starts code while it hasn't completed the turn
		resetGyro(Main_Gyro); // resets the gyro
		if (varTurnDegrees>0) { // checks whether the setpoint (where we want to be) is greater than 0 so we turn right or if its less than 0 it turns Left
			setMotorSpeed(Left, 50); // turns Right
			setMotorSpeed(Right, -50);
			} else {
			setMotorSpeed(Left, -50); // turns Left
			setMotorSpeed(Right, 50);
		};
	};
	InProgressTask = true; // stops the above code
	if ((getGyroDegrees(Main_Gyro)>varTurnDegrees && varTurnDegrees>0) || (getGyroDegrees(Main_Gyro)<varTurnDegrees && varTurnDegrees<0)) { // if the robot has turned to the setpoint then stop the Motors
		InProgressTask = false; // starts above code again if it has overshot
		setMotorSpeed(Left, 0); // stops both motors
		setMotorSpeed(Right, 0);
		return true; // returns true if it has finished so that the sequence can move to the next command

	};
	return false; // returns false if it has not finished so that the sequence waits until it is finished
};

void driveDistance(float distance) { // function that converts mm into rotations / degrees so the robot can then use the built in PIDcontroller to turn that ammount
	moveMotorTarget(Left, (distance/200*360*2.6666666666666666666666666666666666666666666666666666666666666666666666666666666666 /*CorrectionRatioforDrivingSequences*/), 80); // moves the motors according to the output variable from 'MoveDistanceRotations'
	moveMotorTarget(Right, (distance/200*360*2.6666666666666666666666666666666666666666666666666666666666666666666666666666666666 /*CorrectionRatioforDrivingSequences*/), 80);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																			 				General Functions																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ArmHeightMove() { //moves the arm to the defined positions (height 0, Height1, Height2, etc) based on the variable ArmPresetValue
	switch(ArmPresetValue) {  //moves the arm by reading the variable (ArmPresetValue)
	case 0://Floor
		setMotorTarget(ArmLeft, Height0, 100);
		setMotorTarget(ArmRight, Height0, 100);
		setTouchLEDColor(LED,colorOrange);
		break;

	case 1://
		setMotorTarget(ArmLeft, Height1, 100);
		setMotorTarget(ArmRight, Height1, 100);
		setTouchLEDColor(LED,colorOrange);
		break;

	case 2://
		setMotorTarget(ArmLeft, Height2, 100);
		setMotorTarget(ArmRight, Height2, 100);
		setTouchLEDColor(LED,colorOrange);
		break;

	case 3://Static Height / Moving height for balance
		setMotorTarget(ArmLeft, Height3, 100);
		setMotorTarget(ArmRight, Height3, 100);
		setTouchLEDColor(LED,colorOrange);
		break;

	case 4://
		setMotorTarget(ArmLeft, Height4, 100);
		setMotorTarget(ArmRight, Height4, 100);
		setTouchLEDColor(LED,colorOrange);
		break;

	};
};

void displayControl (int delayforscroll = 2000) {
	//Display Code
	displayTextLine(0, "Arm Height = %d", ((getMotorEncoder(ArmLeft)) + (getMotorEncoder(ArmRight)))/2); //Displays Average of motor encoders position
	displayVariableValues(1,ArmPresetValue); // displays the preset value for the height of the arm
	displayTextLine(2, "Gyro = %d", getGyroDegrees(Main_Gyro)); // displays the gyro value in degrees
	displayVariableValues(4, PickupBonusSequenceState);
	displayVariableValues(5, PlaceBonusSequenceState);
	displayTextLine(3,"Battery (MV) = %d", nImmediateBatteryLevel);
	delay(delayforscroll);
	displayTextLine(0, "Arm Height = %d", ((getMotorEncoder(ArmLeft)) + (getMotorEncoder(ArmRight)))/2); //Displays Average of motor encoders position
	displayVariableValues(line1,x);
	displayVariableValues(line2,y);
	displayVariableValues(line3,LeftDriveTraveled);
	displayVariableValues(line4,RightDriveTraveled);
	displayVariableValues(line5, getGyroDegreesFloat(Main_Gyro));
	delay(delayforscroll);
	displayTextLine(0, "Arm Height = %d", ((getMotorEncoder(ArmLeft)) + (getMotorEncoder(ArmRight)))/2); //Displays Average of motor encoders position
	displayVariableValues(line1, DriveWidth);
	displayVariableValues(line2, diameter);
	displayVariableValues(line3, PI);
	displayVariableValues(line4, getGyroRate(Main_Gyro));
	displayVariableValues(line5, getGyroDegreesFloat(Main_Gyro));
	delay(delayforscroll);
}
void GyroCustomCalibration(int count = 30) {
	startGyroCalibration( Main_Gyro, gyroCalibrateSamples512 );
	// delay so calibrate flag can be set internally to the gyro
	wait1Msec(100);

	// wait for calibration to finish or 2 seconds, whichever is longer
	while( getGyroCalibrationFlag(Main_Gyro) || (count-- > 0) ) {
		wait1Msec(100);
	} resetGyro(Main_Gyro);
};

//////////////////////////////////////////////////////
//							 Place cube Sequence								//
//////////////////////////////////////////////////////

void PickupBonusSequence () {
	static int LastState;
	bool P1;

	if (PickupBonusSequenceState !=LastState) {
		P1 = true;
		resetTimer(T1);
		LastState = PickupBonusSequenceState;
	}
	else {
		P1 = false;
	};
	switch (PickupBonusSequenceState){
	case 1:
		if (getJoystickValue(BtnEUp)==1) {
			PickupBonusSequenceState = 2;
		};
		break;

	case 2:
		if (P1) {
			ArmPresetValue=3;
			ArmHeightMove();
			delay(800);
			driveDistance(600);
			//debugging for accuracy
			//delay(10000);
			//
			delay(500);
		};
		if (getMotorZeroVelocity(Left)) {
			PickupBonusSequenceState = 3;
		};
		break;

	case 3:
		if (P1) {
			driveDistance(-100);
			delay(400);
			ArmPresetValue=2;
			ArmHeightMove();
			delay(800);

		};
		if (getMotorZeroVelocity(ArmLeft) || (getTimerValue(T1)>3000)) {
			PickupBonusSequenceState = 4;
		};
		break;

	case 4:
		if (P1) {
			driveDistance(-600);
			delay(800);
			ArmPresetValue=0;
			ArmHeightMove();
			delay(100);
		};
		if(getTimerValue(T1)>1500) {
			ArmHeightMove();
		};
		if (getMotorZeroVelocity(Left) || (getTimerValue(T1)>3000)) {
			PickupBonusSequenceState = 1;
		};
		break;

		/*	case 5:
		if (TurnDegrees(90.0)) {
		PickupBonusSequenceState =1;
		};
		break;
		*/
	default: PickupBonusSequenceState = 1;
	};
};

//////////////////////////////////////////////////////
//							 Pickup cube Sequence								//
//////////////////////////////////////////////////////

void PlaceBonusSequence () {
	static int LastState2;
	bool P2;

	if (PlaceBonusSequenceState !=LastState2) {
		P2 = true;
		resetTimer(T1);
		LastState2 = PlaceBonusSequenceState;
	}
	else {
		P2 = false;
	};
	switch (PlaceBonusSequenceState){
	case 1:
		if (getJoystickValue(BtnFUp)==1) {
			PlaceBonusSequenceState = 2;
		};
		break;

	case 2:
		if (P2) {
			ArmPresetValue=3;
			ArmHeightMove();
			delay(800);
			driveDistance(600);
			//debugging for accuracy
			//delay(10000);
			//
			delay(500);
		};
		if (getMotorZeroVelocity(Left)) {
			PlaceBonusSequenceState = 3;
		};
		break;

	case 3:
		if (P2) {
			driveDistance(-100);
			delay(400);
			ArmPresetValue=2;
			ArmHeightMove();
			delay(800);

		};
		if (getMotorZeroVelocity(ArmLeft) || (getTimerValue(T1)>3000)) {
			PlaceBonusSequenceState = 4;
		};
		break;

	case 4:
		if (P2) {
			driveDistance(-600);
			delay(800);
			ArmPresetValue=0;
			ArmHeightMove();
			delay(100);
		};
		if(getTimerValue(T1)>1500) {
			ArmHeightMove();
		};
		if (getMotorZeroVelocity(Left) || (getTimerValue(T1)>3000)) {
			PlaceBonusSequenceState = 1;
		};
		break;


	default: PlaceBonusSequenceState = 1;
	};
};

void GrayscaleDetector () {
	if (getColorGrayscale(CenterColor)<15) {
		setTouchLEDColor(LED,colorRed);
	}

};
void ArmReset() { // resets the arm if the bottom bumper is pressed
	if (getBumperValue(ArmBottomBumper)==1) {
		resetMotorEncoder(ArmLeft);
		resetMotorEncoder(ArmRight);
		setTouchLEDColor(LED,colorBlue);
		setTouchLEDColor(LED,colorNone);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																			 						Tasks																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


task odometry () { // odometry task

	setMotorEncoderUnits(encoderCounts);

	float x = 0;
	float y = 0;
	float LeftDriveTraveled = 0;
	float RightDriveTraveled = 0;

	while(true)
	{
		LeftDriveTraveled = ((PI * diameter) / ticksPerRev) * (getMotorEncoder(Left) * 2.66666666666666666666666666666666666666666666666666666666666666);
		RightDriveTraveled = ((PI * diameter) / ticksPerRev) * (getMotorEncoder(Right) * 2.66666666666666666666666666666666666666666666666666666666666666);
		delay(10);
		x = x + (((LeftDriveTraveled + RightDriveTraveled) / 2) * cos(getGyroDegreesFloat(Main_Gyro)));
		y = y + (((LeftDriveTraveled + RightDriveTraveled) / 2) * sin(getGyroDegreesFloat(Main_Gyro)));
	};
};


task gyroTask()
{
	long rate;
	long angle, lastAngle;
	lastAngle = 0;
	gyroError=0;
	// Change sensitivity, this allows the rate reading to be higher
	setGyroSensitivity(Main_Gyro, gyroNormalSensitivity);
	//Reset the gyro sensor to remove any previous data.
	resetGyro(Main_Gyro);
	wait1Msec(1000);
	repeat (forever) {
		rate = getGyroRate(Main_Gyro);
		angle = getGyroDegrees(Main_Gyro);
		// If big rate then ignore gyro changes
		if( abs( rate ) < 2 )
		{
			if( angle != lastAngle )
				gyroError += lastAngle - angle;
		}
		lastAngle = angle;
		gyroValue = angle + gyroError;
		wait1Msec(10);
	}
}
/*
task keepStraight(){
while(true) {
HeadingStraight=0;
if(gyroValue<-2){HeadingStraight=-6;}
if(gyroValue>2){HeadingStraight=6;}
wait1Msec(100);
setMotorSpeed(Left, SpeedLeft-HeadingStraight);
setMotorSpeed(Right,SpeedRight +HeadingStraight);
}}*/

task datacollection()
/* ignore values on bottom of screen only graph values are valid /
Exp1 ArmPresetValue (fix negs)
Black = ArmPresetValue
Exp2 Motors
Drk-Green = Right
Purple = Left
Lime-Green = Intake
Maroon = CubeClaw

Exp3 Gyro Readings
blue = with drift
yellow = drift
red = Main_Gyro with out Drift
*/
{
	int loops = 0;
	datalogClear();
	while(true)
	{
		global_1 = getGyroDegrees(Main_Gyro); //series 1
		global_2 = gyroValue; // 2
		global_3 = gyroError; // 3
		global_4 = ArmPresetValue; //4
		global_5 = getMotorSpeed(Right); //5
		global_6 = getMotorSpeed(Left); //6
		global_7 = getMotorSpeed(Intake); //7
		global_8 = getMotorSpeed(CubeClaw); //8

		datalogDataGroupStart();
		datalogAddValue( DATALOG_SERIES_0, global_1 );
		datalogAddValue( DATALOG_SERIES_1, global_2 );
		datalogAddValue( DATALOG_SERIES_2, global_3 );
		datalogAddValue( DATALOG_SERIES_3, global_4 );
		datalogAddValue( DATALOG_SERIES_4, global_5 );
		datalogAddValue( DATALOG_SERIES_5, global_6 );
		datalogAddValue( DATALOG_SERIES_6, global_7 );
		datalogAddValue( DATALOG_SERIES_7, global_8 );
		datalogDataGroupEnd();

		wait1Msec(10);
		datalogAddValueWithTimeStamp( DATALOG_SERIES_3, global_3++ );
		wait1Msec(10);
		datalogAddValueWithTimeStamp( DATALOG_SERIES_3, global_3++ );

		// Repeat sequence every 360 loops
		if(loops++ == 360)
			loops = 0;

		// loop delay
		wait1Msec(10);
	}
}
task Functions(){
	while(true){
		BatteryWarning();
		ArmReset();
		GrayscaleDetector();
		displayControl();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																			 				Main Task																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

task main() { // main program code
	resetMotorEncoder(ArmLeft);	 //Resets Left Arm Motor Encoder to 0
	resetMotorEncoder(ArmRight); //Resets Right Arm Motor Encoder to 0
	intakeStarted = false; // sets the variable that starts the intake to false
	resetTimer(timer2);
	GyroCustomCalibration(30);
	delay(10);
	startTask(Functions);
	startTask(datacollection);
	startTask(odometry);
	startTask(gyroTask);

	while(/*timer2 < 90*/ProgramPersmissionToStart) //while the program is running do this:
	{
		PickupBonusSequence();
		PlaceBonusSequence();
		//datalogging
		/*
		datalogDataGroupStart();
		datalogAddValue( 2, nImmediateBatteryLevel);
		datalogAddValue( 6, getGyroHeading(Main_Gyro));
		datalogAddValue( 7, gyroValue);
		datalogAddValue( 8, gyroError);
		datalogDataGroupEnd();
		*/
		setTouchLEDColor(LED,colorNone);
		//Arm Code
		if (getJoystickValue(BtnLDown)==1 && IndexArmPressed==false) {  //gets the reading of LUP & LDOWN and turns it into a variable(ArmPresetValue)
			ArmPresetValue = (ArmPresetValue-1);
			ArmHeightMove();
			IndexArmPressed=true;
		}
		else {
			if (getJoystickValue(BtnLUp)==1 && IndexArmPressed==false) {
				ArmPresetValue = (ArmPresetValue+1);
				ArmHeightMove();
				IndexArmPressed=true;
			}
		};
		if (getJoystickValue(BtnLDown)==0 && getJoystickValue(BtnLUp)==0) {
			IndexArmPressed=false;
		};

		if (0>ArmPresetValue){
			ArmPresetValue = 0;
		}
		else {
			if (4<ArmPresetValue) {
				ArmPresetValue = 4;
			};
		}

		// Intake Code
		if (getMotorEncoder(Intake)>=(990*2.666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666)) {
			resetMotorEncoder(Intake);
	};

	if (!intakeStarted) {
		waitUntil(getJoystickValue(BtnEDown));
		intakeStarted = true;
		resetTimer(timer2);
	}
	else if (!getJoystickValue(BtnEDown)) {
		setMotorSpeed(Intake, IntakeSpeed);
		} else if(getJoystickValue(BtnEDown)){
		setMotorSpeed(Intake, -IntakeSpeed);
	};

	//Claw Code
	if (getJoystickValue(BtnRUp)) {
		setMotorSpeed(CubeClaw, 100);
		} else if (getJoystickValue(BtnRDown)) {
		setMotorSpeed(CubeClaw, -100);
		} else {
		setMotorSpeed(CubeClaw, 0);
	};

	//DriveCode
	if (PickupBonusSequenceState ==1 && PlaceBonusSequenceState ==1) {
		if(abs(getJoystickValue(ChA))>25 || abs(getJoystickValue(ChD))>25) {	//if the absoloute value of ChA is above 20 or the absoloute value of ChD is above 15 then allow the Motors to
			setMotorSpeed(Left, getJoystickValue(ChA)); //set the value of the motor to the value of the controller joystick
			setMotorSpeed(Right, getJoystickValue(ChD)); //set the value of the motor to the value of the controller joystick
		}
		else { setMotorSpeed(Left, 0);	// if nothing is happening on the controller set the motor speed to 0
			setMotorSpeed(Right, 0); // if nothing is happening on the controller set the motor speed to 0
		};
	};
};
setTouchLEDColor(LED,colorRed);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																			 				End of Code																								//
//																		Last Known Compile: 13/07/2019, 1448																//
//																			  		By Joseph Greening																					//
//																				3428B VIQC Squared Away																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
