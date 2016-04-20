#include "CubeMapChooser.h"
#include "../scene/cubeMap.h"
#include "../scene/material.h"
#include "../ui/GraphicalUI.h"
#include <iostream>

#ifdef WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

CubeMapChooser::CubeMapChooser() {

	w = new Fl_Menu_Window(395, 355, "Load Cubemap");
	w->user_data((void*)(this));

	fi[0] = new Fl_File_Input(20, 25, 310, 35);
	fi[1] = new Fl_File_Input(20, 71, 310, 35);
	fi[2] = new Fl_File_Input(20, 117, 310, 35);
	fi[3] = new Fl_File_Input(20, 163, 310, 35);
	fi[4] = new Fl_File_Input(20, 209, 310, 35);
	fi[5] = new Fl_File_Input(20, 255, 310, 35);

	cb_fi[0] =  &cb_xpi;
	cb_fi[1] =  &cb_xni;
	cb_fi[2] =  &cb_ypi;
	cb_fi[3] =  &cb_yni;
	cb_fi[4] =  &cb_zpi;
	cb_fi[5] =  &cb_zni;

	char curPath[FILENAME_MAX];
	if (!GetCurrentDir(curPath, sizeof(curPath))) curPath[0] = 0;
	string fileName = std::string(curPath);
	for (int i = 0; i < 6; i++) {
		fi[i]->user_data((void*)(this));
		fi[i]->callback(cb_fi[i]);
		fi[i]->value(curPath);
		fn[i] = fileName;
	}

	cb_fb[0] =  &cb_xpb;
	cb_fb[1] =  &cb_xnb;
	cb_fb[2] =  &cb_ypb;
	cb_fb[3] =  &cb_ynb;
	cb_fb[4] =  &cb_zpb;
	cb_fb[5] =  &cb_znb;

	btnMsg[0] = "Positive X Texture Map";
	btnMsg[1] = "Negative X Texture Map";
	btnMsg[2] = "Positive Y Texture Map";
	btnMsg[3] = "Negative Y Texture Map";
	btnMsg[4] = "Positive Z Texture Map";
	btnMsg[5] = "Negative Z Texture Map";

	fb[0] = new Fl_Light_Button(340, 35, 40, 25, "+X");
	fb[1] = new Fl_Light_Button(340, 81, 40, 25, "-X");
	fb[2] = new Fl_Light_Button(340, 127, 40, 25, "+Y");
	fb[3] = new Fl_Light_Button(340, 173, 40, 25, "-Y");
	fb[4] = new Fl_Light_Button(340, 219, 40, 25, "+Z");
	fb[5] = new Fl_Light_Button(340, 265, 40, 25, "-Z");

	for (int i = 0; i < 6; i++) {
		fb[i]->user_data((void*)(this));
		fb[i]->callback(cb_fb[i]);
	}

	ok = new Fl_Return_Button(250, 310, 60, 25, "OK");
	ok->user_data((void*)(this));
	ok->callback(cb_ok);

	cancel = new Fl_Button(320, 310, 60, 25, "Cancel");
	cancel->user_data((void*)(this));
	cancel->callback(cb_cancel);

	w->end();
	w->set_modal();
}

void CubeMapChooser::show() {
	w->show();
}

void CubeMapChooser::hide() {
	w->hide();
}

void CubeMapChooser::cb_cancel(Fl_Widget* o, void* v) {
	((CubeMapChooser*)(o->parent()->user_data()))->hide();
}

void CubeMapChooser::cb_ok(Fl_Widget* o, void* v) {
	CubeMapChooser* ch = (CubeMapChooser*)(o->parent()->user_data());
	bool allGreen = true;
	for (int i = 0; i < 6; i++)
        if (ch->fb[i]->selection_color() != FL_GREEN) allGreen = false;
	if (allGreen) {
		CubeMap* cm = 0;
		if (ch->caller->getRayTracer()->haveCubeMap())
			cm = ch->caller->getRayTracer()->getCubeMap();
		else {
			cm = new CubeMap();
			ch->caller->getRayTracer()->setCubeMap(cm);
		}
		//NEED TO IMPLEMENT THIS SHIT
		cm->setXposMap(ch->cubeFace[0]);
		cm->setXnegMap(ch->cubeFace[1]);
		cm->setYposMap(ch->cubeFace[2]);
		cm->setYnegMap(ch->cubeFace[3]);
		cm->setZposMap(ch->cubeFace[4]);
		cm->setZnegMap(ch->cubeFace[5]);

		ch->caller->setCubeMap(true);
		ch->caller->useCubeMap(true);
		ch->caller->m_filterSlider->activate();
		ch->caller->m_cubeMapCheckButton->activate();
		ch->caller->m_cubeMapCheckButton->value(1);
	}
	ch->hide();
}

void CubeMapChooser::cb_xpi(Fl_Widget* o, void* v) {
	cb_ffi (o, 0);
}

void CubeMapChooser::cb_xpb(Fl_Widget* o, void* v) {
	cb_ffb (o, 0);
}

void CubeMapChooser::cb_xni(Fl_Widget* o, void* v) {
	cb_ffi (o, 1);
}

void CubeMapChooser::cb_xnb(Fl_Widget* o, void* v) {
	cb_ffb (o, 1);
}

void CubeMapChooser::cb_ypi(Fl_Widget* o, void* v) {
	cb_ffi (o, 2);
}

void CubeMapChooser::cb_ypb(Fl_Widget* o, void* v) {
	cb_ffb (o, 2);
}

void CubeMapChooser::cb_yni(Fl_Widget* o, void* v) {
	cb_ffi (o, 3);
}

void CubeMapChooser::cb_ynb(Fl_Widget* o, void* v) {
	cb_ffb (o, 3);
}

void CubeMapChooser::cb_zpi(Fl_Widget* o, void* v) {
	cb_ffi (o, 4);
}

void CubeMapChooser::cb_zpb(Fl_Widget* o, void* v) {
	cb_ffb (o, 4);
}

void CubeMapChooser::cb_zni(Fl_Widget* o, void* v) {
	cb_ffi (o, 5);
}

void CubeMapChooser::cb_znb(Fl_Widget* o, void* v) {
	cb_ffb (o, 5);
}

void CubeMapChooser::cb_ffi(Fl_Widget* o, int i) {
	CubeMapChooser* ch = (CubeMapChooser*)(o->parent()->user_data());
	try { ch->cubeFace[i] = new TextureMap(ch->fi[i]->value()); }
	catch (TextureMapException &xcpt) {
		ch->fb[i]->selection_color(FL_RED);
		ch->fb[i]->value(0);
		ch->fb[i]->value(1);
		std::cerr << xcpt.message() << std::endl;
		std::string msg("Error: could not open file: ");
		msg.append(ch->fi[i]->value());
		ch->caller->alert(msg);
		return;
	}

	std::string fN = std::string(ch->fi[i]->value());
	std::string pN = fN.substr(0,fN.find_last_of("/"));
	for (int j = 0; j < 6; j++) {
		if (ch->fb[j]->selection_color() != FL_GREEN) {
			ch->fi[j]->value(pN.c_str());
			ch->fn[j] = pN;
		}
	}
	ch->fi[i]->value(fN.c_str());
	ch->fn[i] = fN;
	ch->fb[i]->selection_color(FL_GREEN);
	ch->fb[i]->value(0);
	ch->fb[i]->value(1);
}

void CubeMapChooser::cb_ffb(Fl_Widget* o, int i) {
	CubeMapChooser* ch = (CubeMapChooser*)(o->parent()->user_data());
	if (char* curPath = fl_file_chooser(ch->btnMsg[i].c_str(),  ".bmp or .png (*.{bmp,png})", ch->fn[i].c_str(), 0)) {
		try { ch->cubeFace[i] = new TextureMap(curPath); }
		catch (TextureMapException &xcpt) {
			ch->fb[i]->selection_color(FL_RED);
			ch->fb[i]->value(0);
			ch->fb[i]->value(1);
			std::cerr << xcpt.message() << std::endl;
			std::string msg("Error: could not open file: ");
			msg.append(ch->fi[i]->value());
			ch->caller->alert(msg);
			return;
		}
		std::string fN = std::string(curPath);
		std::string pN = fN.substr(0, fN.find_last_of("/"));
		for (int j = 0; j < 6; j++) {
			if (ch->fn[j].substr(0, ch->fn[j].find_last_of("/")) != pN) {
				ch->fi[j]->value(pN.c_str());
				ch->fn[j] = pN;
			}
		}
		ch->fn[i] = fN;
		ch->fi[i]->value(curPath);
		ch->fb[i]->selection_color(FL_GREEN);
	}
	ch->fb[i]->value(0);

	ch->fb[i]->value(1);
}
