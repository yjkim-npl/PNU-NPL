#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ParameterContainer;

class DetectorConstruction : public G4VUserDetectorConstruction
{
	public:
		DetectorConstruction();

		virtual ~DetectorConstruction();

		virtual G4VPhysicalVolume* Construct();

	protected:
		ParameterContainer* PC;
};

#endif
