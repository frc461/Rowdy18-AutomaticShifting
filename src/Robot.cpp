#include <iostream>
#include <memory>
#include <string>

#include <WPILib.h>
#include "Ports.h"
#include "XboxJoystickMap.h"

#define SHIFTER_HIGH DoubleSolenoid::kReverse
#define SHIFTER_LOW DoubleSolenoid::kForward

class Robot: public frc::IterativeRobot {
public:
	Joystick* stickLeft = new Joystick(0);
	Joystick* stickRight = new Joystick(1);
	RobotDrive* driveTrain;
	Victor *frontLeft = new Victor(frontLeftPWM);
	Victor *backLeft = new Victor(backLeftPWM);
	Victor *frontRight = new Victor(frontRightPWM);
	Victor *backRight = new Victor(backRightPWM);
	DoubleSolenoid* shifter = new DoubleSolenoid(shifterForwardPCM, shifterReversePCM);

	Encoder* leftDriveEncoder = new Encoder(leftDriveEncoderA, leftDriveEncoderB);
	Encoder* rightDriveEncoder = new Encoder(rightDriveEncoderA, rightDriveEncoderB);
	bool highEnough;
	int count = 0, gearState, lowGearSpeedMaximum, highGearSpeedMinimum;
	bool changingGears = false;
	double maxOutput;
	enum {
		lowGear = 0,
		highGear,
		lowToHigh,
		highToLow
	};


	void RobotInit() {
		driveTrain = new RobotDrive(frontLeft, backLeft, frontRight, backRight);
	}

	void AutonomousInit() override {

	}

	void AutonomousPeriodic() {

	}

	void TeleopInit() {
		count = 0;
		gearState = 0;
		shifter->Set(SHIFTER_LOW);
		SmartDashboard::PutNumber("MaxOutput", 0.4);
		SmartDashboard::PutNumber("lowGearSpeedMaximum", 800);
		SmartDashboard::PutNumber("highGearSpeedMinimum", 2000);
	}

	void TeleopPeriodic() {

		double left  = stickLeft->GetRawAxis(1);
		double right = stickRight->GetRawAxis(1);
		driveTrain->TankDrive(left, right);
		double leftEncoderSpeed = leftDriveEncoder->GetRate(), rightEncoderSpeed = rightDriveEncoder->GetRate();
		SmartDashboard::PutNumber("Left encoder:", leftEncoderSpeed);
		SmartDashboard::PutNumber("Right encoder:", rightEncoderSpeed);
		maxOutput = SmartDashboard::GetNumber("MaxOutput", maxOutput);
		lowGearSpeedMaximum = SmartDashboard::GetNumber("lowGearSpeedMaximum", lowGearSpeedMaximum);
		highGearSpeedMinimum = SmartDashboard::GetNumber("highGearSpeedMinimum", highGearSpeedMinimum);

		if (leftEncoderSpeed > 2500 && rightEncoderSpeed > 2500){
			highEnough = true;
		}

		switch(gearState){
			case lowGear:
				if(leftEncoderSpeed > lowGearSpeedMaximum && rightEncoderSpeed > lowGearSpeedMaximum)
					gearState = lowToHigh;
				else break;
			case lowToHigh:
				LowToHigh();
				break;
			case highGear:
				if(highEnough && (leftEncoderSpeed < highGearSpeedMinimum && rightEncoderSpeed < highGearSpeedMinimum))
					gearState = highToLow;
				else break;
			case highToLow:
				HighToLow();
			break;
			default:
				gearState = lowGear;
		}


	}

	void LowToHigh(){
		if(count < 5){
			count++;
			driveTrain->SetMaxOutput(maxOutput);
			shifter->Set(SHIFTER_HIGH);
			highEnough = false;
		} else {
			driveTrain->SetMaxOutput(1.0);
			gearState = highGear;
			count = 0;
		}
	}

	void HighToLow(){
		if(count < 5){
			count++;
			driveTrain->SetMaxOutput(maxOutput);
			shifter->Set(SHIFTER_LOW);
		} else {
			driveTrain->SetMaxOutput(1.0);
			gearState = lowGear;
			count = 0;
		}
	}
	void TestPeriodic() {
	}

private:

};

START_ROBOT_CLASS(Robot)
