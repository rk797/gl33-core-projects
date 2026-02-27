//
// Created by mohammad on 29/01/25.
// Updated by chris on 13/02/25
//

#include "Game.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "../usr/LocalPlayer.h"
#include "../usr/EnemyShip.h"

extern LocalPlayer* pLocalPlayer;
//======================================================================================================================

Game::Game() {}

//======================================================================================================================

void Game::update(float deltaTime)
{
	if (enemiesSpawned < number_of_pirateships)
	{
		spawnTimer += deltaTime;

		if (spawnTimer >= 1.0f)
		{
			spawnTimer = 0.f;

			float rnd = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float spawnX = (rnd > 0.5) ? 1.5f : -1.5f; // spawn either on the left or right side of the map
			float spwanY = (rnd * 2.6f) - 1.5f;


			glm::vec3 spawnPos(spawnX, spwanY, 0.0f);


			float dy = pLocalPlayer->position.y - spawnPos.y;
			float dx = pLocalPlayer->position.x - spawnPos.x;
			float ang2player = atan2(dy, dx);


			enemies.emplace_back(spawnPos, ang2player);
			enemiesSpawned++;

		}
	}
}

void Game::RenderImGui() {
    {
        // Three functions that must be called each new frame and before
        // other ImGui calls!
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Putting the text-containing window in the top-left of the screen.
        ImGui::SetNextWindowPos(ImVec2(5, 5));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Green text
        // Setting flags
        ImGuiWindowFlags textWindowFlags = ImGuiWindowFlags_NoMove | // text "window" should not move
            ImGuiWindowFlags_NoResize | // should not resize
            ImGuiWindowFlags_NoCollapse | // should not collapse
            ImGuiWindowFlags_NoSavedSettings | // don't want saved settings mucking things up
            ImGuiWindowFlags_AlwaysAutoResize | // window should auto-resize to fit the text
            ImGuiWindowFlags_NoBackground | // window should be transparent; only the text should be visible
            ImGuiWindowFlags_NoDecoration | // no decoration; only the text should be visible
            ImGuiWindowFlags_NoTitleBar; // no title; only the text should be visible

        // Begin a new window with these flags. (bool *)0 is the "default" value for its argument.
        ImGui::Begin("scoreText", (bool*)0, textWindowFlags);

        // Scale up text a little, and set its value
        ImGui::SetWindowFontScale(1.5f);
    }
    //===========================================================================
    // TODO: Print correct information to screen --------------------------------

    ImGui::Text("Score: %d", score); // Second parameter (int) gets passed into "%d"
    ImGui::Text("Health: %d", health); // Second parameter (int) gets passed into "%d"

    // The below is something you may uncomment for debugging purposes, if you
    // feel it would help to see your program's current fps:
    // ImGui::Text("Average %.1f ms/frame (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    //===========================================================================
    {
        // End the window.
        ImGui::End();

        ImGui::PopStyleColor();
    }

    // Our ImGui::Render() call does not actually display the ImGui items to the
    // screen yet. It just prepares and sets ImGui's draw data.
    ImGui::Render();
    // Now we actually render the set ImGui data to screen. 
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//======================================================================================================================
