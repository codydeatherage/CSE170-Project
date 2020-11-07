
# include "my_viewer.h"
# include "sn_mynode.h"

# include <sigogl/gl_tools.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <sigogl\ws_run.h>


MyViewer::MyViewer(int x, int y, int w, int h, const char* l) : WsViewer(x, y, w, h, l)
{

	add_ui();
//	add_mynode(1);
	build_scene();
	isAnimated = false;
}

void MyViewer::add_ui()
{
	UiPanel* p;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel("", UiPanel::HorizLeft);
	p->add(new UiButton("Add", EvAdd));
	p->add(new UiCheckButton("Animate", EvAnimate));
	p->add(new UiButton("OpenGL", EvInfo));
	p->add(new UiButton("Exit", EvExit));
}

void MyViewer::add_mynode(int n)
{
	SnMyNode* c;

	float ran = 0.15f; // position range
	while (n-- > 0)
	{
		c = new SnMyNode;

		c->multicolor = true;

		c->init.set(0, 0, 0);
		c->width = gs_random(0.001f, ran);
		c->height = gs_random(0.001f, ran * 2);

		// Example how to print/debug your generated data:
		// gsout<<n<<": "<<c->color()<<gsnl;

		rootg()->add(c);
	}
}

void MyViewer::add_model(SnShape* s, GsVec p)
{
	// This method demonstrates how to add some elements to our scene graph: lines,
	// and a shape, and all in a group under a SnManipulator.
	// Therefore we are also demonstrating the use of a manipulator to allow the user to
	// change the position of the object with the mouse. If you do not need mouse interaction,
	// you can just use a SnTransform to apply a transformation instead of a SnManipulator.
	// You would then add the transform as 1st element of the group, and set g->separator(true).
	// Your scene graph should always be carefully designed according to your application needs.

	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation(p);
	manip->initial_mat(m);

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);
	manip->visible(false); // call this to turn off mouse interaction

	rootg()->add(manip);
}

void MyViewer::build_scene()
{
	GsModel* m1 = new GsModel;
	//m1-> (GsModel::load("include/arm/rhand.m");
	GsModel* clockFace = new GsModel;
	clockFace->make_cylinder(GsPnt(0, 0, 0), GsPnt(0, 0.01f, 0), 0.5f, 0.5f, 25, true);
	SnGroup* g1 = new SnGroup;
	g1->add(face = new SnTransform);
	g1->add(new SnModel(clockFace));
	g1->top<SnModel>()->color(GsColor::darkgray);
	face->get().rotx(90.0f * ((float)M_PI / 180.0f));

	GsModel* peg = new GsModel;
	peg->make_cylinder(GsPnt(0, 0, 0), GsPnt(0, 0.1f, 0), 0.01f, 0.01f, 25, true);
	g1->add(new SnModel(peg));
	g1->top<SnModel>()->color(GsColor::yellow);
	
	GsModel* minute = new GsModel;
	minute->make_cylinder(GsPnt(0, 0, 0.03f), GsPnt(0, 0.4f, 0.03f), 0.01f, 0.01f, 25, true);
	SnGroup* g2 = new SnGroup;
	g2->add(minHand = new SnTransform);
	g2->add(new SnModel(minute));

	g2->top<SnModel>()->color(GsColor::red);
	minHand->get().rotx(270.0f * ((float)M_PI / 180.0f));

	GsModel* seconds = new GsModel;
	seconds->make_cylinder(GsPnt(0, 0, 0.06f), GsPnt(0, 0.2f, 0.06f), 0.01f, 0.01f, 25, true);
	//SnGroup* g2 = new SnGroup;
	g2->add(secHand = new SnTransform);
	g2->add(new SnModel(seconds));

	rootg()->add(g1);
	rootg()->add(g2);
 

}

void MyViewer::run_animation()
{
	if (isAnimated) return; // avoid recursive calls
	isAnimated = true;

	GsMat s = secHand->get();
	GsMat m = minHand->get();

	double frdt = 1.0 / 30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t = 0, lt = 0, t0 = gs_time(); 
	float rotAmt = (6.00f * ((float)M_PI / 180.0f)); //6 degree rotation factor
	float currentRot_sec = 0;
	float currentRot_min = 0;
	do // run for a while:
	{
		while (t - lt < frdt) { ws_check(); t = gs_time() - t0; } // wait until it is time for next frame
		double yinc = (t - lt) * v;
		currentRot_sec += rotAmt;
		secHand->get().rotz(-currentRot_sec);
	
		if (t > 1) {
			currentRot_min += rotAmt;
			minHand->get().rotz(-currentRot_min);
		}
		lt = t;
	//	if (t > 1) minHand->get().rotz(-rotAmt);
		

		render(); // notify it needs redraw
		ws_check(); // redraw now
	} while (isAnimated);

	isAnimated = false;
}

int MyViewer::handle_keyboard(const GsEvent& e)
{

	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;
	SnMyNode* c = (SnMyNode*)rootg()->get(0);

	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); return 1;
	case GsEvent::KeyLeft: gsout << "Left\n"; return 1;
/*
	case 'q': c->n += 1.0f;	c->touch();	redraw(); return 1; //q -> n++
	case 'a': c->n -= 1.0f; if (c->n >= 3) { c->touch();	redraw(); }return 1;//a -> n--

	case 'w': if (c->r + 0.01f < c->R) { c->r += 0.01f; }	c->touch();	redraw(); return 1; // w -> r++

	case 's': if (c->r - 0.01f > 0) { c->r -= 0.01f; } 	c->touch();	redraw(); return 1; // w -> r++

	case 'e': c->R += 0.01f; 	c->touch();	redraw(); return 1; // e -> R++

	case 'd': if (c->R - 0.01f > c->r) { c->r -= 0.01f; }	c->touch();	redraw(); return 1;// d -> R--
	*/
	// etc
	default: gsout << "Key pressed: " << e.key << gsnl;

	}



	return 0;
}

int MyViewer::uievent(int e)
{
	switch (e)
	{
	case EvAdd: add_mynode(1); return 1;

	case EvAnimate: run_animation(); return 1;

	case EvInfo:
	{	if (output().len() > 0) { output(""); return 1; }
	output_pos(0, 30);
	activate_ogl_context(); // we need an active context
	GsOutput o; o.init(output()); gl_print_info(&o); // print info to viewer
	return 1;
	}

	case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
