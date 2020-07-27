#pragma once


#include "hypomods.h"


enum {
	ID_epspsynch = 10000,
	ID_scalemode,
	ID_polymode,
	ID_synthmode,
	ID_transmode,
	ID_VSflag,
	ID_transflag,
	ID_Synth,
	ID_Proto,
	ID_storereset
};



class NeuroMod : public Model
{
public:
	// Data
	SpikeDat *evodata, *evodata2;

	// Cell grid data - November 2019
	vector<SpikeDat> viewcell;
	vector<NeuroDat> celldata;
	int cellcount;

	// Boxes

	NeuroMod(int, wxString, HypoMain *);
	virtual ~NeuroMod();

	//void SpikeBox(int modmode = 0);
	void EvoGraphs();
	void IoDGraph(datdouble *, datdouble *, wxString, wxString, int, int barshift=0, wxString settag="");
	//virtual void DataSwitch();
};
