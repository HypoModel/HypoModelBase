

#include "wx/wx.h"
#include <hypomodel.h>
#include "wx/grid.h"


CellBox::CellBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size)
: ParamBox(mod, title, pos, size, "cellbox")
{
	int datwidth;

	diagbox = mod->diagbox;

	datwidth = 50;
	spikes = NumPanel(datwidth, wxALIGN_RIGHT);
	mean = NumPanel(datwidth, wxALIGN_RIGHT);
	freq = NumPanel(datwidth, wxALIGN_RIGHT);
	sd = NumPanel(datwidth, wxALIGN_RIGHT);

	wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
	datagrid->Add(new wxStaticText(panel, -1, "Spikes"), 0, wxALIGN_CENTRE);
	datagrid->Add(spikes);
	datagrid->Add(new wxStaticText(panel, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(freq);
	datagrid->Add(new wxStaticText(panel, -1, "Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(mean);
	datagrid->Add(new wxStaticText(panel, -1, "Std Dev"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(sd);
	
	datneuron = new wxTextCtrl(panel, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
	datspin = new wxSpinButton(panel, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
	datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datbox->AddSpacer(5);
	
	wxBoxSizer *neurobox = new wxBoxSizer(wxHORIZONTAL);
	neurobox->Add(new wxStaticText(panel, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	neurobox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, panel, "");
	databox->AddSpacer(2);
	databox->Add(neurobox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
	databox->AddSpacer(5);
	databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	mainbox->AddStretchSpacer();
	mainbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	mainbox->AddStretchSpacer();

	panel->Layout();

	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CellBox::OnEnter));
	Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(CellBox::OnNext));
	Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(CellBox::OnPrev));
}


void CellBox::PanelData(NeuroDat *data)
{
	if(data->netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	mean->SetLabel(snum.Format("%.1f", data->meanisi));
	sd->SetLabel(snum.Format("%.2f", data->isivar));
}


void CellBox::NeuroData()
{	
	currcell->neurocalc(&(cells[neuroindex]));
	currcell->id = neuroindex;
	PanelData(&(cells[neuroindex]));
	mainwin->scalebox->GraphUpdate();	
}


void CellBox::OnPrev(wxSpinEvent& WXUNUSED(event))
{
	if(neuroindex > 0) neuroindex--;
	else neuroindex = cellcount-1;
	NeuroData();
}


void CellBox::OnNext(wxSpinEvent& WXUNUSED(event))
{
	if(neuroindex < cellcount-1) neuroindex++;
	else neuroindex = 0;
	NeuroData();
}


void CellBox::OnEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	long data;

	// Enter pressed for neuron selection
	if(id == ID_Neuron) {
		datneuron->GetValue().ToLong(&data);
		if(data >= 0 && data < cellcount) {
			neuroindex = data;
			NeuroData();
		}
		return;
	}
}


OutBox::OutBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols)
: ParamBox(mod, title, pos, size, "outbox")
{
	int gridrows, gridcols;

	redtag = "";
	gridrows = rows;
	gridcols = cols;
	delete parambox;

	diagbox = mod->diagbox;

	textgrid = new TextGrid(panel, wxSize(gridrows, gridcols));
	for(i=0; i<gridrows; i++) textgrid->SetRowSize(i, 25);

	wxBoxSizer *controlbox = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *storebox = StoreBox();

	buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Undo, "Undo", 40, buttonbox);
	buttonbox->AddSpacer(2);
	AddButton(ID_Copy, "Copy", 40, buttonbox);

	controlbox->Add(storebox, 0);
	controlbox->AddSpacer(10);
	controlbox->Add(buttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	mainbox->Add(textgrid, 1, wxEXPAND);
	mainbox->Add(controlbox, 0);

	//GridDefault();

	//histmode = 1;
	//HistLoad();
	//paramstoretag->SetLabel(initparams);

	panel->Layout();

	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnGridStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnGridLoad));
	Connect(ID_Undo, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnUndo));
	Connect(ID_Copy, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnCopy));
	Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(OutBox::OnRightClick));
};


void OutBox::OnUndo(wxCommandEvent& event)
{
	textgrid->Undo();
}


void OutBox::OnButton(wxCommandEvent& event)
{
	wxString text;

	mainwin->diagbox->Write("Button: Neuro data scan\n");
}


void OutBox::OnCopy(wxCommandEvent& event)
{
	textgrid->Copy();
}


void OutBox::OnRightClick(wxMouseEvent& event)
{
	int i;
	//int id = event.GetId();
	//wxWindow *pos = FindWindowById(id, toolpanel);
	//wxPoint point = this->GetPosition();
	textgrid->SetCellValue(10, 0, "right");

	wxPoint pos = event.GetPosition();
	//wxSize size = this->GetSize();
	//menuPlot->Check(1000 + gpos->GetFront()->gindex, true);
	//PopupMenu(rightmenu, pos.x + 20, pos.y);
}


void OutBox::HistLoad()
{
	wxString filename, readline;
	TextFile infile;

	// Output Grid History
	filename =  "outgridhist.ini";
	if(infile.Open(filename)) {
		readline = infile.ReadLine();
		//tofp.WriteLine(readline);
		while(!readline.IsEmpty()) {
			readline = readline.AfterFirst(' ');
			readline.Trim();
			initparams = readline;
		  paramstoretag->Insert(initparams, 0);
			readline = infile.ReadLine();
			//tofp.WriteLine(readline);
		}
		infile.Close();	
	}
	paramstoretag->SetValue(initparams);
}



void OutBox::TestGrid()
{
	int i;
	wxString text;

	for(i=0; i<1000; i++) textgrid->SetCellValue(i, 0, text.Format("%.1f", i*0.1 + 10));
}


void OutBox::GridDefault()
{
	textgrid->SetCellValue(0, 0, "date");
	textgrid->SetCellValue(1, 0, "breath rhy");
	textgrid->SetCellValue(2, 0, "odour");
	textgrid->SetCellValue(3, 0, "bedding");
	textgrid->SetCellValue(4, 0, "val");
	textgrid->SetCellValue(5, 0, "hex");
	textgrid->SetCellValue(6, 0, "air");
	textgrid->SetCellValue(7, 0, "other");

	textgrid->SetCellValue(9, 0, "phasic");
	textgrid->SetCellValue(10, 0, "other");
	textgrid->SetCellValue(11, 0, "vas");

	textgrid->SetCellValue(13, 0, "data");
	textgrid->SetCellValue(14, 0, "spikes");
	textgrid->SetCellValue(15, 0, "freq");
	textgrid->SetCellValue(16, 0, "mean isi");
	textgrid->SetCellValue(17, 0, "isi SD");
}


void OutBox::OnGridStore(wxCommandEvent& event)
{
	GridStore();
}


void OutBox::OnGridLoad(wxCommandEvent& event)
{
	textgrid->CopyUndo();
	GridLoad();
}


int OutBox::ColumnData(int col, datdouble *data)
{
	int row, count;
	double value;
	wxString celltext;

	count = 0;
	for(row=0; row<textgrid->GetNumberRows(); row++) {
		celltext = textgrid->GetCellValue(row, col);
		if(celltext != "") {
			celltext.ToDouble(&value);
			count++;
		}
		else value = 0;
		(*data)[row] = value;
	}
	return count;
}


void OutBox::GridStore()
{
	TextFile ofp;
	int row, col;
	wxString celltext, text, filename, filetag;
	wxColour redpen("#dd0000"), blackpen("#000000");

	filetag = paramstoretag->GetValue();
	filename = filetag + "-grid.txt";

	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	if(ofp.Exists(filename) && redtag != filetag) {
		paramstoretag->SetForegroundColour(redpen);
		paramstoretag->SetValue("");
		paramstoretag->SetValue(filetag);
		redtag = filetag;
		return;
	}
	
	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	ofp.New(filename);

	for(row=0; row<textgrid->GetNumberRows(); row++)
		for(col=0; col<textgrid->GetNumberCols(); col++) {
			celltext = textgrid->GetCellValue(row, col);
			celltext.Trim();                                                                     // Fixes odd line endings in pasted data  23/4/15
			if(!celltext.IsEmpty()) ofp.WriteLine(text.Format("%d %d %s", row, col, celltext));
		}

	ofp.Close();
}


void OutBox::GridLoad()
{
	TextFile ifp;

	int row, col;
	long numdat;
	wxString text, filename, filetag, cell;
	wxString readline, datstring;
	wxColour redpen("#dd0000"), blackpen("#000000");

	filetag = paramstoretag->GetValue();
	filename = filetag + "-grid.txt";
	if(!ifp.Open(filename)) {
		paramstoretag->SetValue("Not found");
		return;
	}

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);
	
	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	textgrid->ClearGrid();

	readline = ifp.ReadLine();
	while(!readline.IsEmpty()) {
		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		cell = readline;
		textgrid->SetCellValue(row, col, cell);
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
	}
	ifp.Close();
}

