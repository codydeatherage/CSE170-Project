# pragma once

# include <sigogl/ui_button.h>
# include <sigogl/ui_check_button.h>
# include <sigogl/ws_viewer.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvAdd, EvAnimate, EvInfo, EvExit };
	UiCheckButton* mcolorbut;
	SnTransform* face;
	SnTransform* secHand;
	SnTransform* minHand;
	bool isAnimated;

   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void add_ui ();
	void add_mynode ( int n );
	void build_scene();
	void add_model(SnShape* s, GsVec p);
	void run_animation();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};
