#include "ParameterContainer.hh"

#include <sstream>

ParameterContainer* ParameterContainer::fInstance = 0;

ParameterContainer* ParameterContainer::GetInstance()
{
	if(fInstance == 0)
		fInstance = new ParameterContainer();
	return fInstance;
}

ParameterContainer::ParameterContainer()
{
  par_Name = "Parameters.conf";
  ReadParameters();
	if(this -> GetParInt("UserVerbose") > 0)
	G4cout << "Constructor of ParameterContainer" << G4endl;
}

ParameterContainer::ParameterContainer(G4String fileName)
{
    par_Name = fileName;
    ReadParameters();
}

ParameterContainer::~ParameterContainer()
{
	if(this -> GetParInt("UserVerbose") > 0)
		G4cout << "Destructor of ParameterContainer" << G4endl;
}

void ParameterContainer::ReadParameters()
{
    ifstream par_file(par_Name);
    string line;
    while(getline(par_file,line))
    {
        G4String val_name, val_type;
        istringstream ss(line);
        ss >> val_name >> val_type;

        if(val_name[0] == '#') continue;

        if(val_type.contains("b") || val_type.contains("bool"))
        {
            G4bool val = 0;
            ss >> val;
            par_bool.insert(make_pair(val_name,val));
        }
        if(val_type.contains("i") || val_type.contains("int"))
        {
            G4int val = -1;
            ss >> val;
            par_int.insert(make_pair(val_name,val));
		}
        if(val_type.contains("d") || val_type.contains("double"))
        {
            G4double val = 0;
            ss >> val;
            par_double.insert(make_pair(val_name,val));
        }
        if(val_type.contains("s") || val_type.contains("string"))
        {
            G4String val;
            ss >> val;
            par_string.insert(make_pair(val_name,val));
        }
    }
}

const ParameterContainer& ParameterContainer::operator=(const ParameterContainer& right)
{
    par_bool = right.par_bool;
    par_int = right.par_int;
    par_double = right.par_double;
    par_string = right.par_string;
    return *this;
}

void ParameterContainer::PrintParameter(G4String par)
{
	if(par == "All" || par == "ALL")
	{
		G4cout << "-----Boolean-----" << G4endl;
		for(auto iter = par_bool.begin(); iter != par_bool.end(); iter++)
		{
			G4cout << iter->first << "  b  " << iter->second << G4endl;
		}
		G4cout << "-----Integer-----" << G4endl;
		for(auto iter = par_int.begin(); iter != par_int.end(); iter++)
		{
			G4cout << iter->first << "  i  " << iter->second << G4endl;
		}
		G4cout << "-----Double-----" << G4endl;
		for(auto iter = par_double.begin(); iter != par_double.end(); iter++)
		{
			G4cout << iter->first << "  d  " << iter->second << G4endl;
		}
		G4cout << "-----String-----" << G4endl;
		for(auto iter = par_string.begin(); iter != par_string.end(); iter++)
		{
			G4cout << iter->first << "  s  " << iter->second << G4endl;
		}
	}
	G4bool parb = par_bool[par];
	G4int pari = par_int[par];
	G4double pard = par_double[par];
	G4String pars = par_string[par];
	if(parb)
		G4cout << par << " " << parb << G4endl;
	else if(pari)
		G4cout << par << " " << pari << G4endl;
	else if(pard)
		G4cout << par << " " << pard << G4endl;
	else if(pars)
		G4cout << par << " " << pars << G4endl;
	else
    G4cout << "yjkim in Parameter Container" << G4endl;
}
