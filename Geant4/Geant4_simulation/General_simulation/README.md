# General Geant4 simulation for NPL

## Required geant4 version
 - Geant4-10.7 stable version

## Install

### 1. Clone source
```bash
git clone https:://github.com/yjkim-npl/NPL-G4.git
```

### 2. Compile
```bash
mkdir build
cd build
cmake ..
make
```
### 3. Manage parameter.conf
To manage parameter.conf, link of parameter file is required.
and also if you want to make another parameter.conf or want to link , make and link it to the build directory

```bash
cd build
ln -sf ../<Parameter.conf> .
```

### 4. Run
You can run this example with simple command.
```bash
./G4sim
```
The default setting of parameter container is "Parameter.conf" in build directory

### 5. development log

#### 26.Jan 
 - applied random number to X,Y and Z of beam starting point with its unit.
 - renamed function name 'read,'Print' to 'ReadParameters',"PrintParameter".
 - unified the style (int, Int_t, G4int) to G4int.

#### 30.Jan
 - checked Random Seed and output
 - applying seed at G4sim.cc is effective to PrimaryGeneratorAction
 - checked 10000 different outputs of flat distribution of G4UniformRand 
 - /random/setSeed command in run1.mac was removed because it is not effective

#### 31.Jan
 - `G4sim.cc`: batch mode condition was modified (argc > 2 -> argc >=2)

#### 03.Feb
 - `ParameterContainer.xx`: moved from standard class to static class
