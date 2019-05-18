
#include "WindowClass.hpp"
#include "GameTimer.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <dxgi1_3.h>
#include <d3d11_2.h>

#include <d2d1_2.h>
#include <d2d1_2helper.h>
using namespace D2D1;

#include <dcomp.h>


#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")


#include <string>
#include <comdef.h>
inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

    std::wstring ToString()const
	{

		// Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}

GameTimer g_timer;

ComPtr<ID2D1Factory2> g_d2dfactory;
ComPtr<ID2D1Device1> g_d2ddevice;
ComPtr<ID2D1DeviceContext> g_d2d_dc;

ComPtr<ID3D11Device> g_d3ddevice;
ComPtr<IDXGIDevice> g_dxgidevice;
ComPtr<IDXGIFactory2> g_dxgifactory;
ComPtr<IDXGISwapChain1> g_swapchain;
ComPtr<IDXGISurface2> surface;
ComPtr<ID2D1Bitmap1> bitmap;
ComPtr<IDCompositionDevice> dcompDevice;
ComPtr<IDCompositionVisual> visual;
ComPtr<IDCompositionTarget> target;
void InitDirect2D(Window& win){
    ThrowIfFailed(
        D3D11CreateDevice(nullptr,
                            D3D_DRIVER_TYPE_HARDWARE,
                            nullptr,
                            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                            nullptr,0,
                            D3D11_SDK_VERSION,
                            &g_d3ddevice,
                            nullptr,
                            nullptr

    ));

    ThrowIfFailed(g_d3ddevice.As(&g_dxgidevice));
    ThrowIfFailed(CreateDXGIFactory2(
    DXGI_CREATE_FACTORY_DEBUG,
    __uuidof(g_dxgifactory),
    reinterpret_cast<void **>(g_dxgifactory.GetAddressOf())
    ));

    DXGI_SWAP_CHAIN_DESC1 description = {};
    description.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;     
    description.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    description.BufferCount      = 2;                              
    description.SampleDesc.Count = 1;                              
    description.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;
    description.Width = win.GetClientWidth();
    description.Height = win.GetClientHeight();

    OutputDebugString((L"Buffer --- Width: " + std::to_wstring(description.Width) +
     L" | Height: " + std::to_wstring(description.Height) + L"\n").c_str());
    
    //创建交换链
    ThrowIfFailed(g_dxgifactory->
    CreateSwapChainForComposition(  g_dxgidevice.Get(),
                                    &description,
                                    nullptr,
                                    g_swapchain.GetAddressOf()
    ));


    D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                        options,
                                        g_d2dfactory.GetAddressOf()
    ));

    ThrowIfFailed(g_d2dfactory->CreateDevice(g_dxgidevice.Get(),g_d2ddevice.GetAddressOf()));

    ThrowIfFailed(g_d2ddevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,g_d2d_dc.GetAddressOf()));

    
    ThrowIfFailed(g_swapchain->GetBuffer(0,__uuidof(surface),reinterpret_cast<void **>(surface.GetAddressOf())));

    D2D1_BITMAP_PROPERTIES1 properties = {};
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.bitmapOptions         = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    
    ThrowIfFailed(g_d2d_dc->CreateBitmapFromDxgiSurface(surface.Get(),properties,bitmap.GetAddressOf()));


    g_d2d_dc->SetTarget(bitmap.Get());

    g_d2d_dc->BeginDraw();
    g_d2d_dc->Clear();


    ComPtr<ID2D1SolidColorBrush> brush;
    D2D1_COLOR_F const brushColor = D2D1::ColorF(0.18f,  // red
                                                0.55f,  // green
                                                0.34f,  // blue
                                                0.75f); // alpha
    ThrowIfFailed(g_d2d_dc->CreateSolidColorBrush(brushColor,
                                brush.GetAddressOf()));
    D2D1_POINT_2F const ellipseCenter = D2D1::Point2F(150.0f,  // x
                                                    150.0f); // y
    D2D1_ELLIPSE const ellipse = D2D1::Ellipse(ellipseCenter,
                                            100.0f,  // x radius
                                            100.0f); // y radius
    g_d2d_dc->FillEllipse(ellipse,
                    brush.Get());
    ThrowIfFailed(g_d2d_dc->EndDraw());

    ThrowIfFailed(g_swapchain->Present(1,0));




   
    ThrowIfFailed(DCompositionCreateDevice(
    g_dxgidevice.Get(),
    __uuidof(dcompDevice),
    reinterpret_cast<void **>(dcompDevice.GetAddressOf())));


    ThrowIfFailed(dcompDevice->CreateTargetForHwnd(win.GetHwnd(),
                                        true, // Top most
                                        target.GetAddressOf()));


    ThrowIfFailed(dcompDevice->CreateVisual(visual.GetAddressOf()));



    ThrowIfFailed(visual->SetContent(g_swapchain.Get()));


    ThrowIfFailed(target->SetRoot(visual.Get()));


	ThrowIfFailed(dcompDevice->Commit());

}

void DestroyResource(){
	//g_d2dfactory->Release();
}
void RenderDirect2D(){
    g_d2d_dc->SetTarget(bitmap.Get());

    g_d2d_dc->BeginDraw();
    g_d2d_dc->Clear();

	static float a = 1.0f;
	static float sum = 0.1f;
	
	a += (sum * g_timer.DeltaTime());
	if (a > 1.0f) {
		a = 1.0f;
		sum = -0.5f;
	}
	else if (a < 0.0f) {
		sum = 0.5f;
		a = 0.0f;
	}
    ComPtr<ID2D1SolidColorBrush> brush;
    D2D1_COLOR_F const brushColor = D2D1::ColorF(0.18f,  // red
                                                0.55f,  // green
                                                0.34f,  // blue
                                                a); // alpha
    ThrowIfFailed(g_d2d_dc->CreateSolidColorBrush(brushColor,
                                brush.GetAddressOf()));
    D2D1_POINT_2F const ellipseCenter = D2D1::Point2F(250.0f,  // x
                                                    250.0f); // y
    D2D1_ELLIPSE const ellipse = D2D1::Ellipse(ellipseCenter,
                                            200.0f,  // x radius
                                            200.0f); // y radius
    g_d2d_dc->FillEllipse(ellipse,
                    brush.Get());
    ThrowIfFailed(g_d2d_dc->EndDraw());

    ThrowIfFailed(g_swapchain->Present(1,0));


}
