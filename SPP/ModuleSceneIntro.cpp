#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = sensed = false;
	bg = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	bg = App->textures->Load("pinball/pinball_sonic_spritesheet.png");
	rect_bg.h = SCREEN_HEIGHT;
	rect_bg.w = SCREEN_WIDTH;
	rect_bg.x = 0;
	rect_bg.y = 0;

	rect_ball.h = 28;
	rect_ball.w = 28;
	rect_ball.x = 0;
	rect_ball.y = 1418;

	//divide limits maybe

	int pinball_sonic_spritesheet[160] = {
		440, 401,
		438, 407,
		430, 412,
		416, 412,
		416, 403,
		423, 383,
		440, 384,
		440, 380,
		462, 290,
		464, 290,
		464, 266,
		483, 227,
		492, 189,
		497, 165,
		497, 133,
		488, 107,
		475, 88,
		461, 75,
		440, 66,
		419, 58,
		385, 58,
		360, 67,
		337, 82,
		320, 104,
		314, 121,
		310, 128,
		310, 142,
		308, 143,
		308, 182,
		314, 198,
		321, 220,
		329, 237,
		334, 240,
		341, 248,
		341, 260,
		332, 260,
		328, 255,
		328, 246,
		316, 236,
		298, 236,
		291, 244,
		284, 244,
		280, 240,
		282, 229,
		284, 226,
		284, 215,
		286, 214,
		286, 187,
		284, 186,
		282, 175,
		278, 158,
		278, 67,
		268, 56,
		252, 56,
		243, 66,
		242, 94,
		237, 96,
		216, 85,
		175, 77,
		155, 77,
		125, 82,
		95, 90,
		74, 99,
		53, 118,
		37, 140,
		31, 151,
		28, 159,
		26, 167,
		24, 177,
		23, 196,
		23, 234,
		29, 268,
		38, 294,
		54, 327,
		67, 357,
		59, 366,
		74, 411,
		68, 412,
		64, 410,
		62, 401
	};

	PhysBody* chain_tmp;
	chain_tmp = App->physics->CreateChain(0, 0, pinball_sonic_spritesheet, 160);
	chain_tmp->body->SetType(b2_staticBody);
	chain_tmp->body->GetFixtureList()->SetDensity(0.1f);

	//TODO: we need this to check ball lost
	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	//Blitting background
	App->renderer->Blit(bg, 0, 0, &rect_bg, 1.0f);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		balls.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 14));
		balls.getLast()->data->listener = this;
	}
	


	//Look example for pinball structure down below
	/*if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}*/

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = balls.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(bg, x, y, &rect_ball, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	/*c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}*/

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	//App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
