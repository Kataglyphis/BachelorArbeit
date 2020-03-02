// dear imgui - standalone example application for DirectX 11
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"


#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

//for calculating texture while displaying stuff
#include <thread>
#include <atlstr.h>

/**
our own created files.
*/
#include "CoolDownTester.h"
#include "helpers.h"
#include "SimulatedAnnealingTest.h"
#include "SimulatedAnnealing.h"
#include "SimulatedAnnealingVisualizer.h"
#include "RandomnessStrategy.h"
#include "WangHash.h"
#include "MersenneTwister.h"
#include "TemporalReprojection.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// task we run when normal retarget texture needs to be calculated!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calculate_retarget_texture() {

    const char* filename = "LDR_RGBA_0_64.png";
    int image_width = 64;
    int image_height = 64;
    SimulatedAnnealingTest testing = SimulatedAnnealingTest(filename, image_width, image_height);
    testing.testPermutation();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// task we run when retarget textures with additional temporal reprojection need to be calculated!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calculate_temporal_rep_textures() {
    
    TemporalReprojection tr;
    tr.generateRetargetTextureSet(4,4);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// task we run when cool down schedules need to be compared
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compare_cool_downs_schedules() {

    //test different cool down functions
    const char* filename = "LDR_RGBA_0_64.png";
    int image_width = 64;
    int image_height = 64;
     CoolDownTester test(1000, image_width, image_height, filename);
     test.compareDifferentCoolDownSchedules();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// task we run when seed texture needs to be calculated!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calculate_seed_texture(helpers* helper) {

    uint64_t seed_texture_width = 1920;
    uint64_t seed_texture_height = 1080;
    uint64_t seed_texture_resolution = 32;
    helper->generate_seed_png(seed_texture_width, seed_texture_height, seed_texture_resolution, new MersenneTwister());

}

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Retarget Seeds!"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Retargeting pixel seeds"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_main_window = true;
    bool show_another_window = false;

    //our self-defined variables we need for our tasks 
    uint64_t seed_texture_width = 1920;
    uint64_t seed_texture_height = 1080;
    uint64_t seed_texture_resolution = 32;
    int image_width = 64;
    int image_height = 64;

    bool started_calculation = false;
    bool started_calculation_temporal_reprojection = false;
    bool started_calculation_cool_downs = false;
    bool started_calculation_seed_texture = false;
    
    //threads  we run  for not stopping our main loop!
    std::thread calc_perm;
    std::thread compare_cool_downs;
    std::thread calc_temporal_reprojection_textures;
    std::thread calc_seed_texture;

    //look up the available #threads
    uint32_t available_threads;

    const char* filename_small = "LDR_RGBA_0_16.png";
    const char* filename = "LDR_RGBA_0_64.png";
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //Load the textures as images in our window
    helpers* helper = new helpers();
    int my_image_width = 0;
    int my_image_height = 0;
    ID3D11ShaderResourceView* my_texture = NULL;
    bool ret = helper->LoadTextureFromFile(
        "LDR_RGBA_0_64.png", 
        &my_texture, g_pd3dDevice, &my_image_width, &my_image_height);
	if (!ret) return 1;

    //BOOL generated_seed_texture = helper->generate_seed_png(seed_texture_width, seed_texture_height, seed_texture_resolution, new MersenneTwister());
    //if (!generated_seed_texture) cout << "Something went horribly wrong!";
    //calc retargeted texture with temporal annealing!
    //SimulatedAnnealing* retarget = new SimulatedAnnealing();
    //retarget->execute(1000000, filename);

    //retarget image
    /*int my_retarget_width = 0;
    int my_retarget_height = 0;
    ID3D11ShaderResourceView* my_retarget_texture = NULL;
    bool erstellt = helper->LoadTextureFromFile(
        "retargeted_texture.png",
        &my_retarget_texture, g_pd3dDevice, &my_retarget_width, &my_retarget_height);
    if (!erstellt) return 1;
    */

    //testing the simulated annealing
    //SimulatedAnnealingTest testing = SimulatedAnnealingTest(filename, image_width, image_height);
    //testing.testPermutation();

    //TemporalReprojection tr;
    //tr.generateRetargetTextureSet(4,4);

    //test different cool down functions
    //CoolDownTester test(100000, 64, 64, filename);
    //test.compareDifferentCoolDownSchedules();

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big main window (Most of the sample code is in ImGui::ShowmainWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_main_window)
            ImGui::ShowDemoWindow(&show_main_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            //update number available threads 
            available_threads = std::thread::hardware_concurrency();
            CString str_number_threads;
            str_number_threads.Format("%d", available_threads);

            ImGui::Begin("Retargeting");                          // Create a window for displaying blue noise and retargeting texture!!

   
            ImGui::Text("Number available concurrent Threads = "+ str_number_threads);
            ImGui::Text("Our Blue noise texture we are performing our simulated annealing on");
			//show image
			ImGui::Image((void*)my_texture, ImVec2((float)my_image_width, (float)my_image_height));
            ImGui::Text("Calculation results are stored in \pictures subfolder!");
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////// from here we can launch the task for calculating the retarget texture with its corresponding visualizations
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (ImGui::Button("Calculate retarget texture with correspondig visualization!")) {
                if (!started_calculation) {
                    started_calculation = true;
                    calc_perm = thread(calculate_retarget_texture); 
                }
            }
            if(started_calculation) ImGui::Text("Started Calculation of retarget texture!");

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////// from here we can launch the task for calculating the retarget textures with additional temporal reprojection
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (ImGui::Button("Calculate retarget texture with additional tempral Reprojection support!")) {
                if (!started_calculation_temporal_reprojection) {
                    started_calculation_temporal_reprojection = true;
                    calc_temporal_reprojection_textures = std::thread(calculate_temporal_rep_textures);
                }
            }
            if (started_calculation_temporal_reprojection) ImGui::Text("Started Calculation of retarget textures with additional tempral Reprojection support!");

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////// compare different cool down functions
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (ImGui::Button("Comparing Cool Down Schedules now!")) {
                if (!started_calculation_cool_downs) {
                    started_calculation_cool_downs = true;
                    compare_cool_downs = std::thread(compare_cool_downs_schedules);
                }
            }
            if (started_calculation_cool_downs) ImGui::Text("Started Calculation of CoolDown Comparison!");

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////// calculate the seed texture
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (ImGui::Button("Calculate seed texture with Marsenne Twister!")) {
                if (!started_calculation_seed_texture) {
                    started_calculation_seed_texture = true;
                    calc_seed_texture = std::thread(calculate_seed_texture, helper);
                }
            }
            if (started_calculation_seed_texture) ImGui::Text("Started Calculation of seed texture!");

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////// calculate the seed texture
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(started_calculation) calc_perm.join();
    if(started_calculation_cool_downs) compare_cool_downs.join();
    if(started_calculation_temporal_reprojection) calc_temporal_reprojection_textures.join();
    if(started_calculation_seed_texture) calc_seed_texture.join();

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


