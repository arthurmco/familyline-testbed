#include "Game.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Input;
using namespace Tribalia::Graphics;
using namespace Tribalia::Graphics::GUI;

Game::Game(Window* w, Framebuffer* fb3D, Framebuffer* fbGUI,
	   GUIRenderer* gr)
    : win(w), fbGUI(fbGUI), fb3D(fb3D), gr(gr)
{
    int winW, winH;
    w->GetSize(winW, winH);
    
    try {
    	/* Initialise things */
        om = new ObjectManager{};
        
        rndr = new Renderer{};

        gctx.om = om;

        hp = new HumanPlayer{"Arthur"};

    	terrFile = new TerrainFile(ASSET_FILE_DIR "terrain_test.trtb");
    	terr = terrFile->GetTerrain();

    	tc = new TeamCoordinator();
    	auto tteam = tc->CreateTeam("test");
    	printf("%s -- %#x\n", tteam->name.c_str(), tteam->id);
        

    	scenemng = new SceneManager(terr->GetWidth() * SEC_SIZE, terr->GetHeight() * SEC_SIZE);

    	cam = new Camera{glm::vec3(6.0f, 36.0f, 6.0f), (float)winW/(float)winH, glm::vec3(0,0,0)};
        scenemng->SetCamera(cam);
        hp->SetCamera(cam);

        rndr->SetSceneManager(scenemng);

        if (!am->ReadFromFile(ASSET_FILE_DIR "assets.taif")) {
            throw asset_exception(nullptr, "Invalid asset file!");
        }

    } catch (window_exception& we) {
    	Log::GetLog()->Fatal("game", "Window creation error: %s (%d)", we.what(), we.code);
    	exit(EXIT_FAILURE);
    } catch (renderer_exception& re) {
        Log::GetLog()->Fatal("game", "Rendering error: %s [%d]",
            re.what(), re.code);
        exit(EXIT_FAILURE);
    }  catch (mesh_exception& se) {
        Log::GetLog()->Fatal("game", "Mesh error: %s", se.what());
        Log::GetLog()->Fatal("game", "Mesh file: %s", se.file.c_str());
        exit(EXIT_FAILURE);
    }  catch (material_exception& se) {
        Log::GetLog()->Fatal("game", "Material error: %s ", se.what());
        Log::GetLog()->Fatal("game", "Material file: %s", se.file.c_str());
        exit(EXIT_FAILURE);
    } catch (shader_exception& se) {
        Log::GetLog()->Fatal("game", "Shader error: %s [%d]", se.what(), se.code);
        Log::GetLog()->Fatal("game", "Shader file: %s, type %d", se.file.c_str(), se.type);
        exit(EXIT_FAILURE);
    } catch (asset_exception& ae) {
        Log::GetLog()->Fatal("game", "Asset file error: %s", ae.what());
        if (ae.assetptr) {
            AssetFileItem* a = (AssetFileItem*)ae.assetptr;
            Log::GetLog()->Fatal("game", "Asset %s, file: %s", a->name.c_str(), a->path.c_str());
        }
        exit(EXIT_FAILURE);
    } catch (terrain_file_exception& te) {
    	Log::GetLog()->Fatal("game", "Terrain file error: %s on file %s", te.what(), te.file.c_str());
    	if (te.code != 0) {
    	    Log::GetLog()->Fatal("game", "Error code: %d (%s)", te.code, strerror(te.code));
    	}
    	exit(EXIT_FAILURE);
    }
   
    Light* l = new Light{ "mainLight", glm::vec3(16, 30, 16), 0xff, 0xff, 0xff, 10 };
    scenemng->AddObject(l);

    terr_rend = new TerrainRenderer{rndr};
    terr_rend->SetTerrain(terr);
    terr_rend->SetCamera(cam);

    objrend = new ObjectRenderer(om, scenemng);
    hp->objr = objrend;

    InputManager::GetInstance()->Initialize();

    ip = new InputPicker{ terr_rend, win, scenemng, cam, om};
    hp->SetPicker(ip);

    pathf = new PathFinder(om);
    pathf->InitPathmap(terr->GetWidth(), terr->GetHeight());
    pathf->UpdatePathmap(terr->GetWidth(), terr->GetHeight());
    hp->SetPathfinder(pathf);

    widgets.p = new Panel{0, 0, 320, 240};
    widgets.p->SetBackColor(255, 0, 0, 25);
    gr->AddPanel(widgets.p);

    widgets.lbl = new Label(120, 460, "This is a true label");
    widgets.lbl->SetForeColor(255, 128, 0, 255);

    widgets.lblVersion = new Label(10, 10, "Tribalia " VERSION " commit " COMMIT);
    widgets.lblVersion->SetForeColor(255, 255, 255, 255);

    widgets.pnl = new Panel(0, 0, 630, 100);
    widgets.pnl->AddPanel(widgets.lblVersion, 10, 10);
    widgets.pnl->SetBackColor(0, 0, 0, 128);

    gr->AddPanel(widgets.pnl);
    gr->AddPanel(widgets.lbl);

    widgets.btn = new Button(300, 200, 150, 40, "Test");
    widgets.btn->SetOnClickListener([&](GUI::IControl* c) {
    	    static int times = 0;
    	    times++;

    	    char ss[8];
    	    snprintf(ss, 7, "%d", times);
    	    widgets.lbl->SetText(ss);
    	});
    gr->AddPanel(widgets.btn);

    /* Adds the objects to the factory */
    ObjectFactory::GetInstance()->AddObject(new WatchTower);
    ObjectFactory::GetInstance()->AddObject(new Tent);

    ObjectPathManager::getInstance()->SetTerrain(terr);

}

int Game::RunLoop()
{
    CombatManager::GetInstance()->SetOnDeath([&](Logic::AttackableObject* at) {
	    gctx.om->UnregisterObject(at);
	});
    unsigned int ticks = SDL_GetTicks();
    unsigned int frame = 0;

    int delta = 1;
    double pms = 0.0;

    double maxdelta = 0, mindelta = 99, sumfps = 0;

    
    do {

        ip->UpdateIntersectedObject();
        ip->UpdateTerrainProjectedPosition();

    	gr->DebugWrite(10, 40, "Press C to create an object at mouse cursor, and R to remove it.");
        player = true;
        gctx.elapsed_seconds = delta / 1000.0;

    	InputEvent ev;
    	gr->ProcessInput(ev);
    	hp->ProcessInput();
        if (!hp->Play(&gctx))
            player = false;
	
    	terr_rend->Update();

        bool objupdate = objrend->Check();
        if (objupdate || hp->HasUpdatedObject()) {
            objrend->Update();
            pathf->UpdatePathmap(terr->GetWidth(), terr->GetHeight());
        }
    	objrend->Update();

        LocatableObject* selected = hp->GetSelectedObject();
        auto locc = ip->GetIntersectedObject();
        if (locc) {
            gr->DebugWrite(10, 100, "Hovering object '%s'", locc->GetName());

    	    if (selected && locc->HasProperty("maxHP")) {
    		AttackableObject* a = (AttackableObject*)locc;
    		gr->DebugWrite(350, 100, a->CheckAttackRange((AttackableObject*)selected) ? "In range" : "Not in range");
    	    }
    	}

        {
            int qx, qy;
            scenemng->GetCameraQuadrant(qx, qy);
            gr->DebugWrite(10, 160, "Camera quadrant: %d x %d", qx, qy);
        }

        if (selected) {
    	    if (selected->HasProperty("maxHP")) {
    		AttackableObject* a = (AttackableObject*)selected;
    		gr->DebugWrite(10, 120, "Selected object: '%s' (%4d/%4d)",
    			      a->GetName(), (int)a->GetHP(), a->GetMaxHP());
	
    	    } else {
    		gr->DebugWrite(10, 120, "Selected object: '%s'",
    			      selected->GetName());
	      
    	    }	    
        }

    	CombatManager::GetInstance()->DoAttacks(gctx.elapsed_seconds);
        glm::vec3 p = ip->GetTerrainProjectedPosition();
        glm::vec2 q = ip->GetGameProjectedPosition();

        AnimationManager::GetInstance()->Iterate();
    	ObjectPathManager::getInstance()->UpdatePaths();

    	fb3D->SetAsBoth();
    	rndr->SetBoundingBox(hp->renderBBs);
    	if (objupdate) rndr->UpdateObjects();
        rndr->UpdateFrames();
        rndr->Render();

    	gr->DebugWrite(10, 140, "Terrain pos: (OpenGL: %.3f,%.3f,%.3f | Game: %.2f, %.2f)",
    		      p.x, p.y, p.z, q.x, q.y);
    	gr->DebugWrite(10, 180, "Camera rotation: %.1fº",
    		      cam->GetRotation() * 180 / M_PI);
        gr->DebugWrite(10, 65, "Bounding box: %s", hp->renderBBs ?
          "Enabled" : "Disabled");

    	fbGUI->SetAsBoth();
    	gr->Render();
    	fbGUI->Unset();
	
    	win->Update();

        frame++;

        unsigned int elapsed = SDL_GetTicks();
        delta = elapsed - ticks;

        ticks = SDL_GetTicks();
        Timer::getInstance()->RunTimers(delta);

    	if (frame % 15 == 0) {
    	    pms = delta * 1.0;
    	}

    	gr->DebugWrite(0, 420, "%.2f ms, %.2f fps", pms, 1000 / pms);

	#define FPS_LOCK 120.0
	
    	//Locked in ~120 fps
        if (delta < 1000/FPS_LOCK) {
	    auto sleepdelta = int((1000/FPS_LOCK) - delta);
    	    SDL_Delay(sleepdelta);
        }

	if (delta < mindelta)
	    mindelta = delta;

	if (delta > maxdelta)
	    maxdelta = delta;

	sumfps += (1000 / delta);

    } while (player);

    double maxfps = 1000/mindelta;// less delta, more fps
    double minfps = 1000/maxdelta;
    double avgfps = sumfps / frame;
    Log::GetLog()->Write("game", "fps max: %.3f (%.3f ms), min: %.3f (%.3f ms), avg: %.3f",
			 maxfps, mindelta, minfps, maxdelta, avgfps);
    Log::GetLog()->Write("game", "Total frames: %d", frame);
    
    return 0;
}
