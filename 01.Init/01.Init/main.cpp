#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxgi")

ID3D11Device* g_pd3dDevice = nullptr;

ID3D11DeviceContext* g_pImmediateContext = nullptr; // Device Context

IDXGISwapChain* g_pSwapChain = nullptr; // 스왑 체인

ID3D11RenderTargetView* g_pRenderTargetView = nullptr; // 렌더 타켓 뷰

IDXGIAdapter* GetBestAdapter()
{
	IDXGIFactory* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(hr))
	{
		return nullptr;
	}

	IDXGIAdapter* pBestAdapter = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	size_t maxDedicatedVedioMemory = 0;

	for (UINT n = 0; pFactory->EnumAdapters(n, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++n)
	{
		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		if (maxDedicatedVedioMemory < desc.DedicatedVideoMemory)
		{
			if (pBestAdapter)
			{
				pBestAdapter->Release();
			}
			pBestAdapter = pAdapter;
			maxDedicatedVedioMemory = desc.DedicatedVideoMemory;
		}
		else
		{
			pAdapter->Release();
		}
	}

	pFactory->Release();
	return pBestAdapter;
}

HRESULT InitD3D(HWND hWnd)
{
	IDXGIAdapter* pBestAdapter = GetBestAdapter();
	if (nullptr == pBestAdapter)
	{
		return E_FAIL;
	}

	// 스왑 체인 구조체를 초기화 해야 함
	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0x00, sizeof(sd));
	sd.BufferCount = 1; // 백 버퍼의 수
	sd.BufferDesc.Width = 800; // 백 버퍼의 너비
	sd.BufferDesc.Height = 600; // 백 버퍼의 높이
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 백 버퍼의 포맷
	sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 새로 고침 빈도 ( 분자 )
	sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 새로 고침 빈도 ( 분모 )
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 백 버퍼의 용도
	sd.OutputWindow = hWnd; // 렌더링 할 윈도우 핸들
	sd.SampleDesc.Count = 1; // 멀티샘플링 수
	sd.SampleDesc.Quality = 0; // 멀티샘플링 품질
	sd.Windowed = TRUE; // 창 모드 인지 아닌지.

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		pBestAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&g_pSwapChain,
		&g_pd3dDevice,
		nullptr,
		&g_pImmediateContext
	);

	pBestAdapter->Release();

	if (FAILED(hr))
	{
		return hr;
	}

	// 백 버퍼의 렌더 타켓 뷰를 얻어와야한다.
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

	// 뷰 포트 설정
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

void Cleanup()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 윈도우 클래스 등록
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = L"WindowClass";
	RegisterClassEx(&wc);

	// 윈도우 생성
	HWND hWnd = CreateWindowEx(
		0,
		L"WindowClass",
		L"Direct3D 11 윈도우",
		WS_OVERLAPPEDWINDOW,
		100, 100,
		800, 600,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	ShowWindow(hWnd, nCmdShow);

	if (FAILED(InitD3D(hWnd)))
	{
		Cleanup();
		return 1;
	}

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
			g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);

			g_pSwapChain->Present(0, 0);
		}
	}

	Cleanup();

	return (int)msg.wParam;
}