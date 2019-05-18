#include <wrl.h>
using namespace Microsoft::WRL;
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")

 struct ComException
{
  HRESULT result;
  ComException(HRESULT const value) :
    result(value)
  {}
};
inline void HR(HRESULT const result)
{
  if (S_OK != result)
  {
    throw ComException(result);
  }
}


ComPtr<ID3D11Device> direct3dDevice;


HR(D3D11CreateDevice(nullptr,    // Adapter
                     D3D_DRIVER_TYPE_HARDWARE,
                     nullptr,    // Module
                     D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                     nullptr, 0, // Highest available feature level
                     D3D11_SDK_VERSION,
                     &direct3dDevice,
                     nullptr,    // Actual feature level
                     nullptr));  // Device context


ComPtr<IDXGIDevice> dxgiDevice;
HR(direct3dDevice.As(&dxgiDevice));


ComPtr<IDXGIFactory2> dxFactory;
HR(CreateDXGIFactory2(
  DXGI_CREATE_FACTORY_DEBUG,
  __uuidof(dxFactory),
  reinterpret_cast<void **>(dxFactory.GetAddressOf())));


DXGI_SWAP_CHAIN_DESC1 description = {};



description.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;     
description.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
description.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
description.BufferCount      = 2;                              
description.SampleDesc.Count = 1;                              
description.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;



RECT rect = {};
GetClientRect(window, &rect);
description.Width  = rect.right - rect.left;  
description.Height = rect.bottom - rect.top;


ComPtr<IDXGISwapChain1> swapChain;
HR(dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(),
                                            &description,
                                            nullptr, // Don’t restrict
                                            swapChain.GetAddressOf()));




 // Create a single-threaded Direct2D factory with debugging information
ComPtr<ID2D1Factory2> d2Factory;
D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                     options,
                     d2Factory.GetAddressOf()));
// Create the Direct2D device that links back to the Direct3D device
ComPtr<ID2D1Device1> d2Device;
HR(d2Factory->CreateDevice(dxgiDevice.Get(),
                           d2Device.GetAddressOf()));
// Create the Direct2D device context that is the actual render target
// and exposes drawing commands
ComPtr<ID2D1DeviceContext> dc;
HR(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                 dc.GetAddressOf()));
// Retrieve the swap chain's back buffer
ComPtr<IDXGISurface2> surface;
HR(swapChain->GetBuffer(
    0, // index
    __uuidof(surface),
    reinterpret_cast<void **>(surface.GetAddressOf())));
// Create a Direct2D bitmap that points to the swap chain surface
D2D1_BITMAP_PROPERTIES1 properties = {};
properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
properties.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
properties.bitmapOptions         = D2D1_BITMAP_OPTIONS_TARGET |
                                   D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
ComPtr<ID2D1Bitmap1> bitmap;
HR(dc->CreateBitmapFromDxgiSurface(surface.Get(),
                                   properties,
                                   bitmap.GetAddressOf()));
// Point the device context to the bitmap for rendering
dc->SetTarget(bitmap.Get());
// Draw something
dc->BeginDraw();
dc->Clear();

ComPtr<ID2D1SolidColorBrush> brush;
D2D1_COLOR_F const brushColor = D2D1::ColorF(0.18f,  // red
                                             0.55f,  // green
                                             0.34f,  // blue
                                             0.75f); // alpha
HR(dc->CreateSolidColorBrush(brushColor,
                             brush.GetAddressOf()));
D2D1_POINT_2F const ellipseCenter = D2D1::Point2F(150.0f,  // x
                                                  150.0f); // y
D2D1_ELLIPSE const ellipse = D2D1::Ellipse(ellipseCenter,
                                           100.0f,  // x radius
                                           100.0f); // y radius
dc->FillEllipse(ellipse,
                brush.Get());
HR(dc->EndDraw());
// Make the swap chain available to the composition engine
HR(swapChain->Present(1,   // sync
                          0)); // flags




ComPtr<IDCompositionDevice> dcompDevice;
HR(DCompositionCreateDevice(
   dxgiDevice.Get(),
   __uuidof(dcompDevice),
   reinterpret_cast<void **>(dcompDevice.GetAddressOf())));



ComPtr<IDCompositionTarget> target;
HR(dcompDevice->CreateTargetForHwnd(window,
                                    true, // Top most
                                    target.GetAddressOf()));


ComPtr<IDCompositionVisual> visual;
HR(dcompDevice->CreateVisual(visual.GetAddressOf()));



HR(visual->SetContent(swapChain.Get()));


HR(target->SetRoot(visual.Get()));


HR(dcompDevice->Commit());



