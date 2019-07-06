#pragma config(Sensor, port2,  ArmBottomBumper, sensorVexIQ_Touch)
#pragma config(Sensor, port3,  Main_Gyro,      sensorVexIQ_Gyro)
#pragma config(Sensor, port4,  LED,            sensorVexIQ_LED)
#pragma config(Sensor, port7,  CenterColor,    sensorVexIQ_ColorGrayscale)
#pragma config(Sensor, port8,  CrossColor,     sensorVexIQ_ColorGrayscale)
#pragma config(Sensor, port12, BallDetect,     sensorVexIQ_Touch)
#pragma config(Motor,  motor1,          Left,          tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor5,          Intake,        tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor6,          Right,         tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor,  motor9,          CubeClaw,      tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor10,         ArmRight,      tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor11,         ArmLeft,       tmotorVexIQ, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//VIQC_SquaredAway_CompCode
/*************************************PROGRAMMER NOTES*****************************************************//*
/*1*//* get odometry working
/*2*//* add special stuff
/*3*//* fix sequences
/*4*//*
/*5*//*
/*6*//*
/*7*//*
/*8*//*
/*9*//*
/*10*//*
/*11*//*
/*12*//*
/*13*//*
/*14*//*
/*15*//*
*//*End of Notes Main Program Code is Below */

int PickupBonusSequenceState; // defines the variable that is used to tell what state the Pickup Bonus Sequence is in
int PlaceBonusSequenceState; // defines the variable that is used to tell what state the Place Bonus Sequence is in

bool intakeStarted; // defines the variable that waits until the intake button is pressed before starting Intake

float Height0 = 0; //Floor
float Height1 = -300; //Place HighScored Hub and Move MultiPusher Over Field Lines
float Height2 = -700; //Position HighScored Hub
float Height3 = -1100; //Static Height / Moving height for balance
float Height4 = -1500; //Bonus Hub Position
int ArmPresetValue = 0; // the preset number that tells the preset code how high to move the arm

void driveDistance(float distance) { // function that converts mm into rotations / degrees so the robot can then use the built in PIDcontroller to turn that ammount
	float MoveDistanceRotations=distance/200*360; //divides the input mm by 100 then times it by 360 to get the rotations / degrees
	moveMotorTarget(Left, MoveDistanceRotations, 100); // moves the motors according to the output variable from 'MoveDistanceRotations'
	moveMotorTarget(Right, MoveDistanceRotations, 100);
};

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
	if ((getGyroDegreesFloat(Main_Gyro)>varTurnDegrees && varTurnDegrees>0) || (getGyroDegreesFloat(Main_Gyro)<varTurnDegrees && varTurnDegrees<0)) { // if the robot has turned to the setpoint then stop the Motors
		InProgressTask = false; // starts above code again if it has overshot
		setMotorSpeed(Left, 0); // stops both motors
		setMotorSpeed(Right, 0);
		return true; // returns true if it has finished so that the sequence can move to the next command

	};
	return false; // returns false if it has not finished so that the sequence waits until it is finished
};

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

void GyroCustomCalibration(int count = 30) {
	startGyroCalibration( Main_Gyro, gyroCalibrateSamples512 );
	// delay so calibrate flag can be set internally to the gyro
	wait1Msec(100);

	// wait for calibration to finish or 2 seconds, whichever is longer
	while( getGyroCalibrationFlag(Main_Gyro) || (count-- > 0) ) {
		wait1Msec(100);
	} resetGyro(Main_Gyro);
};

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
			ArmPresetValue=4;
			ArmHeightMove();
			driveDistance(600);
			delay(100);
		};
		if (getMotorZeroVelocity(Left)) {
			PickupBonusSequenceState = 3;
		};
		break;

	case 3:
		if (P1) {
			ArmPresetValue=2;
			ArmHeightMove();
			delay(100);
		};
		if (getMotorZeroVelocity(ArmLeft) || (getTimerValue(T1)>3000)) {
			PickupBonusSequenceState = 4;
		};
		break;

	case 4:
		if (P1) {
			driveDistance(-600);
			ArmPresetValue=0;
			delay(100);
		};
		if(getTimerValue(T1)>1500) {
			ArmHeightMove();
		};
		if (getMotorZeroVelocity(Left) || (getTimerValue(T1)>3000)) {
			PickupBonusSequenceState = 5;
		};
		break;

	case 5:
		if (TurnDegrees(90.0)) {
			PickupBonusSequenceState =1;
		};
		break;

	default: PickupBonusSequenceState = 1;
	};
};

void PlaceBonusSequence () {
	static int LastState;
	bool P1;

	if (PlaceBonusSequenceState !=LastState) {
		P1 = true;
		resetTimer(T1);
		LastState = PlaceBonusSequenceState;
	}
	else {
		P1 = false;
	};
	switch (PlaceBonusSequenceState){
	case 1:
		if (getJoystickValue(BtnFUp)==1) {
			PlaceBonusSequenceState = 2;
		};
		break;

	case 2:
		if (P1) {
			driveDistance(500);
			ArmPresetValue=3;
			ArmHeightMove();
			delay(100);
		};
		if (getMotorZeroVelocity(Left) || (getTimerValue(T1)>3000)) {
			PlaceBonusSequenceState = 3;
		};
		break;

	case 3:
		if (P1) {
			driveDistance(-70);
			delay(100);
		};
		if (getMotorZeroVelocity(Left) || (getTimerValue(T1)>1500)) {
			PlaceBonusSequenceState = 4;
		};
		break;

	case 4:
		if (P1) {
			ArmPresetValue=1;
			ArmHeightMove();
		};
		if (getMotorZeroVelocity(ArmLeft) || (getTimerValue(T1)>500)) {
			PlaceBonusSequenceState = 5;
		};
		break;

	case 5:
		if (P1) {
			delay(500);
			driveDistance(-380);
			delay(100);

		};
		if (getMotorZeroVelocity(Left) || (getTimerValue(T1)>1500)) {
			PlaceBonusSequenceState = 1;
		};
		if(getTimerValue(T1)>1500) {
			ArmPresetValue = 0;
			ArmHeightMove();
		};
		break;

	default: PlaceBonusSequenceState = 1;
	};
};

void ArmReset() { // resets the arm if the bottom bumper is pressed
	if (getBumperValue(ArmBottomBumper)==1) {
		resetMotorEncoder(ArmLeft);
		resetMotorEncoder(ArmRight);
		setTouchLEDColor(LED,colorBlue);
		setTouchLEDColor(LED,colorNone);
	}
}

float xaxisPos = 0;
float yaxisPos = 0;
float robotAngle = 0;
float robotPosition = 0;
float dL = 0;
float dR = 0;

//int ticksPerRev = 360;
//float diameter = 2.5;
float radius = 1.25;
//float calibration = 3.14159265429*diameter/ticksPerRev; //C our calibration factor 2:11 video
//float avcR = 0.218166156547917;
//float avcL = 0.218166156547917;
float distanceBetweenWheels = 7.25; // B (base line distance)

task odometry () { // odometry task
	while(true)
	{
		setMotorEncoderUnits(encoderCounts);
		dR = /*calibration * */ getMotorEncoder(Right);
		dL = /*calibration * */ getMotorEncoder(Left);
		robotPosition = (dR+dL) / 2;
		robotAngle = (dL-dR) / distanceBetweenWheels * (180/3.14159265429);
		xaxisPos = radius/2 * (dR + dL) * sin(robotAngle);
		yaxisPos = radius/2 * (dR + dL) * cos(robotAngle);

		displayVariableValues(line1,xaxisPos);
		displayVariableValues(line2,yaxisPos);
		displayVariableValues(line3,robotPosition);
		displayVariableValues(line4, robotAngle);
		displayVariableValues(line5, getGyroDegreesFloat(Main_Gyro));
	};
};

task main() { // main program code
	resetMotorEncoder(ArmLeft);	 //Resets Left Arm Motor Encoder to 0
	resetMotorEncoder(ArmRight); //Resets Right Arm Motor Encoder to 0
	bool IndexArmPressed; // defines the variable that check whether the controller lift buttons have been pressed during a sequence
	intakeStarted = false; // sets the variable that starts the intake to false
	resetTimer(timer2);
	GyroCustomCalibration(30);
	delay(10);
	startTask(odometry);
	while(/*timer2 < 90*/true) //while the program is running do this:
	{
		setTouchLEDColor(LED,colorNone);
		ArmReset();
		//Display Code
		displayTextLine(0, "Arm Height=%d", ((getMotorEncoder(ArmLeft)) + (getMotorEncoder(ArmRight)))/2); //Displays Average of motor encoders position
		displayVariableValues(1,ArmPresetValue); // displays the preset value for the height of the arm
		displayTextLine(2, "Gyro=%d", getGyroDegrees(Main_Gyro)); // displays the gyro value in degrees
		displayVariableValues(3, PickupBonusSequenceState);
		displayVariableValues(4, PlaceBonusSequenceState);
		displayVariableValues(5, Height0);
		//Sequences
		PickupBonusSequence();
		PlaceBonusSequence();
		//Preset Functions

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
		if (!intakeStarted) {
			waitUntil(getJoystickValue(BtnEDown));
			intakeStarted = true;
			resetTimer(timer2);
		}
		else if (!getJoystickValue(BtnEDown)) {
			setMotorSpeed(Intake, 100);
			} else if(getJoystickValue(BtnEDown)){
			setMotorSpeed(Intake, -100);
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
		if (PickupBonusSequenceState ==1 && PlaceBonusSequenceState == 1) {
			if(abs(getJoystickValue(ChA))>25 || abs(getJoystickValue(ChD))>25) {	//if the absoloute value of ChA is above 20 or the absoloute value of ChD is above 15 then allow the Motors to
				setMotorSpeed(Left, getJoystickValue(ChA)); //set the value of the motor to the value of the controller joystick
				setMotorSpeed(Right, getJoystickValue(ChD)); //set the value of the motor to the value of the controller joystick
			}
			else { setMotorSpeed(Left, 0);	// if nothing is happening on the controller set the motor speed to 0
				setMotorSpeed(Right, 0); // if nothing is happening on the controller set the motor speed to 0
			};
		};
	};
	/*while (true) { // if competition ended set led to red
	setTouchLEDColor(LED,colorRed);};*/
}

//END
