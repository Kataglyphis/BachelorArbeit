#define STB_IMAGE_IMPLEMENTATION
#include "helpers.h"

bool helpers::LoadTextureFromFile(const char* filename,ID3D11ShaderResourceView** srv , ID3D11Device* g_pd3dDevice, int* width, int* height) {
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL,4);
	if (image_data == NULL) return false;

	//texture creation
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
		
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, srv);
	pTexture->Release();

	*width = image_width;
	*height = image_height;
	stbi_image_free(image_data);

	return true;
}
using namespace cppOpt;

auto toOptimize = [](OptCalculation<double>& optCalculation) {
	double
		A(10.0),
		n(2.0),
		pi(3.1415926535897932384),
		x1 = optCalculation.get_parameter("x1"),
		x2 = optCalculation.get_parameter("x2");

	double sum = (x1 * x1 - A * cos(2.0 * pi * x1)) + (x2 * x2 - A * cos(2.0 * pi * x2));

	optCalculation.result = A * n + sum;
};

bool helpers::simulatedAnnealingBlueNoiseTexture() {
	
	using namespace std;

	OptBoundaries<double> optBoundaries;
	optBoundaries.add_boundary({ -5.12, 5.12, "x1" });
	optBoundaries.add_boundary({ -5.12, 5.12, "x2" });

	//number of calculations we are allowing
	unsigned int maxcalculations = 3000;

	//we want to find the minimum
	OptTarget optTarget = cppOpt::OptTarget::MINIMIZE;

	//how fast the simulated annealing algorithm slows down
	//http://en.wikipedia.org/wiki/Simulated_annealing
	double coolingFactor = 0.99;

	//the chance in the beginning to follow bad solutions
	double startChance = 0.25;

	//define your coordinator
	OptCoordinator<double, false> coordinator(
		maxcalculations,
		toOptimize,
		optTarget,
		0);

	//add simulated annealing as child
	coordinator.add_child(make_unique<cppOpt::OptSimulatedAnnealing<double>>(
		optBoundaries,
		coolingFactor,
		startChance));

	//let's go
	coordinator.run_optimisation();

	//print result
	OptCalculation<double> best = coordinator.get_best_calculation();

	return true;
	}