#include <mud/mud.h>
#include <14_live_gfx_visual/14_live_gfx_visual.h>

using namespace mud;

static float g_time = 0.f;

void define_visual_script(VisualScript& script)
{
	script.lock();

	using Var = Valve;

	Valve& groot = script.input("groot");

	//static float g_time = 0.f;
	Var& time = script.reference(&g_time);
	//Var& speed = script.value(0.01f);
	//script.function(add<float>, { &time, &speed });

	Var& grid_interval = script.value(vec3(3.f, 0.f, 3.f));
	Var& grid_size = script.value(uvec3(11U, 1U, 11U));

	Var& coords = *script.function(grid, { &grid_size });

	Var& c = script.value(vec3(15.f, 0.f, 15.f));
	Var& grid_position = *script.function(multiply<vec3>, { &coords, &grid_interval });
	Var& position = *script.function(subtract<vec3>, { &grid_position, &c });

	Var& offsets = script.value(vec3{ 0.21f, 0.f, 0.37f });
	Var& f = *script.function(multiply<vec3>, { &coords, &offsets });
	Var& angles = *script.function(add<vec3>, { &f, &time });
	Var& rotation = script.create<quat>({ &angles });

	Var& t = *script.function(add<vec3>, { &f, &time });
	Var& tx = script.get(&vec3::x, { &t });
	Var& ty = script.get(&vec3::z, { &t });

	Var& r = *script.function(ncosf, { &tx });
	Var& b = *script.function(nsinf, { &ty });
	Var& g = *script.function(ncosf, { &time });

	Var& s0 = script.value(Unit3);
	Var& s1 = script.value(1.f);
	Var& scale = *script.function(multiply<vec3>, { &s0, &s1 });

	Var& colour = script.create<Colour>({ &r, &g, &b, &script.value(1.f) });

	Var& node = *script.function(gfx::node, { &groot, &script.node<ProcessValue>(Ref()).output(), &position, &rotation, &scale });

	Var& fill_colour = script.value(Colour::None);
	Var& symbol = script.create<Symbol>({ &colour, &fill_colour });
	Var& shape = script.value(Cube());

	script.function(gfx::draw, { &node, &shape, &symbol });
}

VisualScript& create_visual_script()
{
	Signature signature = { { Param{ "groot", Ref(type<Gnode>()) } } };
	static VisualScript script = { "Example Script", signature };
	define_visual_script(script);
	return script;
}

void ex_14_live_gfx_visual(Shell& app, Widget& parent, Dockbar& dockbar)
{
	UNUSED(app);
#ifdef MUD_PLATFORM_EMSCRIPTEN
	// speed it up a bit on Emscripten cause it's clamped to 60fps
	g_time += 0.02f;
#else
	g_time += 0.01f;
#endif

	SceneViewer& viewer = ui::scene_viewer(parent);
	ui::orbit_controller(viewer);

	static VisualScript& script = create_visual_script();

	if(Widget* dock = ui::dockitem(dockbar, "Game", carray<uint16_t, 1>{ 1U }))
		visual_script_edit(*dock, script);

	Gnode& groot = viewer.m_scene->begin();

	static Var result;
	static std::vector<Var> args = { Ref(&groot) };
	script(args, result);
}

#ifdef _14_LIVE_GFX_VISUAL_EXE
void pump(Shell& app)
{
	edit_context(app.m_ui->begin(), app.m_editor, true);
	ex_14_live_gfx_visual(app, *app.m_editor.m_screen, *app.m_editor.m_dockbar);
}

int main(int argc, char *argv[])
{
	Shell app(cstrarray(MUD_RESOURCE_PATH), argc, argv);
	app.run(pump);
}
#endif

