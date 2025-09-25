#include "stdafx.h"

#define ResolutionWidth 800.0f
#define	ResolutionHeigh 600.0f

// New
// Change : SimpleVertex vertices[] �븻���� ���� Cube���ؽ��� 24���� �����ϰ� ���ؽ����� Normal ����. + UV, tangent�� �̸� �߰�.
// Change : 24���� ���ؽ��� ���� �ε��� ����.
// Change : ConstantBuffer. - lightVariables
// Change : SimpleVertex. - normal
// Change : Inputlayout. - normal, uv, tangent
// Change : VertexShader
// Change : PixelShader
// Change : PSSetting - PixelShader���� Constant�� ����ϱ� ���ؼ��� PSSetConstantBuffer�� ����ؼ� ���� �������.

// Init
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr; // Device Context
IDXGISwapChain* g_pSwapChain = nullptr; // ���� ü��
ID3D11RenderTargetView* g_pRenderTargetView = nullptr; // ���� Ÿ�� ��
ID3D11DepthStencilView* g_pDepthStencilView = nullptr; // ���� ���ٽ� ��

// Render
ID3D11InputLayout* g_pInputLayout = nullptr;
ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;
ID3D11Buffer* g_pConstantBuffer;
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
// **Change**
struct SimpleVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 tangent;
};
// **Change**
struct ConstantBuffer
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;

	//Light
	// 4Byte ������ Padding �����.
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT3 spotPosition;
	float spotRange;
	DirectX::XMFLOAT3 spotDirection;
	float spotAngle;

};
// **Change**
SimpleVertex vertices[] = {
	// �Ʒ� (-Z)
	{DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,-1), DirectX::XMFLOAT2(0,1), DirectX::XMFLOAT4(-1,0,0,1)},
	{DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,-1), DirectX::XMFLOAT2(0,0), DirectX::XMFLOAT4(-1,0,0,1)},
	{DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,-1), DirectX::XMFLOAT2(1,0), DirectX::XMFLOAT4(-1,0,0,1)},
	{DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,-1), DirectX::XMFLOAT2(1,1), DirectX::XMFLOAT4(-1,0,0,1)},
	// �� (+Z)
	{DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,0,0,1)},
	{DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,0,0,1)},
	{DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,0,0,1)},
	{DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,0,0,1)},
	// �� (-X)
	{DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(-1,0,0), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(0,0,1,1)},
	{DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(-1,0,0), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(0,0,1,1)},
	{DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(-1,0,0), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(0,0,1,1)},
	{DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(-1,0,0), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(0,0,1,1)},
	// �� (+X)
	{DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(1,0,0), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(0,0,-1,1)},
	{DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(1,0,0), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(0,0,-1,1)},
	{DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(1,0,0), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(0,0,-1,1)},
	{DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT3(1,0,0), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(0,0,-1,1)},
	// ������ (+Y)
	{DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,1,0), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,0,0,1)},
	{DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,1,0), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,0,0,1)},
	{DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,1,0), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,0,0,1)},
	{DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,1,0), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,0,0,1)},
	// ���� (-Y)
	{DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,-1,0), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(-1,0,1,1)},
	{DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,-1,0), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(-1,0,1,1)},
	{DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,-1,0), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(-1,0,1,1)},
	{DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT3(0,-1,0), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(-1,0,1,1)},
};
// **Change**
WORD indices[] = {
	0,1,2, 0,2,3,       // Front
	4,5,6, 4,6,7,       // Back
	8,9,10, 8,10,11,    // Left
	12,13,14, 12,14,15, // Right
	16,17,18, 16,18,19, // Top
	20,21,22, 20,22,23  // Bottom
};

// Misc
float g_fRotaionAngle = 0.0f;

// ------------------------- Functions ------------------------------------- //
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

HRESULT InitDeviceAndSwapChain(HWND hWnd, IDXGIAdapter* pBestAdapter)
{
	// ���� ü�� ����ü�� �ʱ�ȭ �ؾ� ��
	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0x00, sizeof(sd));
	sd.BufferCount = 1; // �� ������ ��
	sd.BufferDesc.Width = ResolutionWidth; // �� ������ �ʺ�
	sd.BufferDesc.Height = ResolutionHeigh; // �� ������ ����
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �� ������ ����
	sd.BufferDesc.RefreshRate.Numerator = 60; // ȭ�� ���� ��ħ �� ( ���� )
	sd.BufferDesc.RefreshRate.Denominator = 1; // ȭ�� ���� ��ħ �� ( �и� )
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �� ������ �뵵
	sd.OutputWindow = hWnd; // ������ �� ������ �ڵ�
	sd.SampleDesc.Count = 1; // ��Ƽ���ø� ��
	sd.SampleDesc.Quality = 0; // ��Ƽ���ø� ǰ��
	sd.Windowed = TRUE; // â ��� ���� �ƴ���.

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
		DEBUG_BREAK();
		return hr;
	}

	return S_OK;
}

HRESULT InitRenderTargetView()
{
	// �� ������ ���� Ÿ�� �並 ���;��Ѵ�.
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	return S_OK;
}

HRESULT InitDepthStencilBuffer()
{
	// Depth Stencil Buffer
	D3D11_TEXTURE2D_DESC Desc;
	Desc.Width = 800;
	Desc.Height = 600;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT; // ���ҽ��� ������ ���� ��
	/*
		D3D11_USAGE_DEFAULT	= 0, // GPU ���� �ַ� ���Ǹ�, CPU�� ���� �������� ����.
		D3D11_USAGE_IMMUTABLE	= 1, // ���� �� ������� �ʴ� ���ҽ�
		D3D11_USAGE_DYNAMIC	= 2,     // CPU���� ���� ������Ʈ �Ǹ�, GPU ���� �б� �������� ���
		D3D11_USAGE_STAGING	= 3 // CPU�� GPU ���� ������ ���ۿ� ����
	*/

	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer = nullptr;
	HRESULT hr = g_pd3dDevice->CreateTexture2D(&Desc, nullptr, &pDepthStencilBuffer);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	hr = g_pd3dDevice->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &g_pDepthStencilView);
	pDepthStencilBuffer->Release();
	if (FAILED(hr)) 
	{
		DEBUG_BREAK();
		return hr;
	}

	return S_OK;
}

HRESULT InitD3D(HWND hWnd)
{
	IDXGIAdapter* pBestAdapter = GetBestAdapter();
	if (nullptr == pBestAdapter)
	{
		DEBUG_BREAK();
		return E_FAIL;
	}

	InitDeviceAndSwapChain(hWnd, pBestAdapter);

	InitRenderTargetView();

	InitDepthStencilBuffer();
	
	return S_OK;
}

HRESULT InitVertexBuffer()
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0x00, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(vertices);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	memset(&InitData, 0x00, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = vertices;

	HRESULT hr = g_pd3dDevice->CreateBuffer(&desc, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}
	return S_OK;
}

HRESULT InitIndexBuffer()
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0x00, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(indices);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	memset(&InitData, 0x00, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = indices;

	HRESULT hr = g_pd3dDevice->CreateBuffer(&desc, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}
	return S_OK;
}

HRESULT InitConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0x00, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(ConstantBuffer);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;

	HRESULT hr = g_pd3dDevice->CreateBuffer(&desc, nullptr, &g_pConstantBuffer);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	return S_OK;
}

// **Change**
HRESULT InitInputLayout(ID3DBlob* pVSBlob)
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT hr = g_pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pInputLayout);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	return S_OK;
}

HRESULT InitVertexShader()
{
	ID3DBlob* pVSBlob = nullptr;
	HRESULT hr = D3DReadFileToBlob(L"VertexShader.cso", &pVSBlob);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		pVSBlob->Release();
		return hr;
	}

	InitInputLayout(pVSBlob);

	pVSBlob->Release();
	return S_OK;
}

HRESULT InitPixelShader()
{
	ID3DBlob* pPSBlob = nullptr;
	HRESULT hr = D3DReadFileToBlob(L"PixelShader.cso", &pPSBlob);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}

	return S_OK;
}

// **Change""
void UpdateConstantResource(const Transform& worldTransform)
{
	Float4 worldPosition = worldTransform.GetPosition();
	DirectX::XMMATRIX scale = worldTransform.GetScaleMatrix();
	DirectX::XMMATRIX rotation = worldTransform.GetRotationMatrix();
	DirectX::XMMATRIX position = worldTransform.GetPositionMatrix();

	// ����, ��, �������� ��� ����
	// world�� ������Ʈ���� ������ ���̸�, ������ ������Ʈ�� Transform �� �����ؾ���.
	DirectX::XMMATRIX world = scale * rotation * position;
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(10.0f, 0.0f, 2.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, ResolutionWidth / ResolutionHeigh, 0.01f, 100.0f);

	// Spot Light
	DirectX::XMFLOAT4 lightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	DirectX::XMFLOAT3 spotPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f);
	DirectX::XMFLOAT3 spotDirection = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	float spotRange = 5.0f;
	float spotAngle = cosf(DirectX::XMConvertToRadians(20.0f));

	// ��� ���� ������Ʈ
	ConstantBuffer cb;
	cb.world = DirectX::XMMatrixTranspose(world);
	cb.view = DirectX::XMMatrixTranspose(view);
	cb.projection = DirectX::XMMatrixTranspose(projection);
	cb.lightColor = lightColor;
	cb.ambientColor = ambientColor;
	cb.spotPosition = spotPosition;
	cb.spotDirection = spotDirection;
	cb.spotRange = spotRange;
	cb.spotAngle = spotAngle;

	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
}

void IASetting()
{
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);
	// ���ؽ� ���� ����
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// �ε��� ���� ����
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// ������Ƽ�� ���� ����
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void VSSetting()
{
	g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);

	// StartSlot : Buffer Slot
	// NumBuffers : Buffer�� 2�� �̻��� �迭�� ��� ����.
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
}

void RSSetting()
{
	// �� ��Ʈ ����
	D3D11_VIEWPORT vp;
	vp.Width = ResolutionWidth;
	vp.Height = ResolutionHeigh;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	g_pImmediateContext->RSSetViewports(1, &vp);
}

// **Change""
void PSSetting()
{
	g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
}

void OMSetting()
{
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}

void BeginPlay()
{
	InitDepthStencilBuffer();

	InitVertexBuffer();

	InitIndexBuffer();

	InitConstantBuffer();

	InitVertexShader();

	InitPixelShader();

	IASetting();

	VSSetting();

	RSSetting();

	PSSetting();

	OMSetting();
}

void RenderBegin()
{
	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Render()
{
	g_fRotaionAngle += 0.001f;

	Transform tf1;
	tf1.SetScale({ 2.0f, 2.0f, 1.0f });
	tf1.SetRotation({ 0.0f, 0.0f, g_fRotaionAngle });
	tf1.SetPosition({ 0.0f, 1.0f, 0.0f });
	UpdateConstantResource(tf1);
	g_pImmediateContext->DrawIndexed(36, 0, 0);

	Transform tf2;
	tf2.SetScale({ 2.0f, 2.0f, 1.0f });
	tf2.SetRotation({ 0.0f, 0.0f, g_fRotaionAngle });
	tf2.SetPosition({ 0.0f, -1.0f, 0.0f });
	UpdateConstantResource(tf2);
	g_pImmediateContext->DrawIndexed(36, 0, 0);
}

void RenderEnd()
{
	g_pSwapChain->Present(0, 0);
}

void Cleanup()
{
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
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
	// ������ Ŭ���� ���
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

	// ������ ����
	HWND hWnd = CreateWindowEx(
		0,
		L"WindowClass",
		L"Direct3D 11 ������",
		WS_OVERLAPPEDWINDOW,
		100, 100,
		ResolutionWidth, ResolutionHeigh,
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

	BeginPlay();

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
			RenderBegin();

			Render();

			RenderEnd();
		}
	}

	Cleanup();

	return (int)msg.wParam;
}