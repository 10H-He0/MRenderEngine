#include "D3D12CommandContext.h"
#include "D3D12Device.h"

MD3D12CommandContext::MD3D12CommandContext(MD3D12Device* IDevice)
	:Device(IDevice)
{
	CreateCommandContext();

	DescriptorCache = std::make_unique<MD3D12DescriptorCache>(Device);
}

MD3D12CommandContext::~MD3D12CommandContext()
{
	DestroyCommandContext();
}

void MD3D12CommandContext::CreateCommandContext()
{
	ThrowIfFailed(Device->GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(Device->GetD3DDevice()->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));

	ThrowIfFailed(Device->GetD3DDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

	ThrowIfFailed(Device->GetD3DDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
		nullptr, IID_PPV_ARGS(CommandList.GetAddressOf())));

	ThrowIfFailed(CommandList->Close());
}

void MD3D12CommandContext::DestroyCommandContext()
{

}

void MD3D12CommandContext::ResetCommandAllocator()
{
	ThrowIfFailed(CommandAllocator->Reset());
}

void MD3D12CommandContext::ResetCommandList()
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
}

void MD3D12CommandContext::ExcuteCommandList()
{
	ThrowIfFailed(CommandList->Close());

	ID3D12CommandList* CommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);
}

void MD3D12CommandContext::FlushCommandQueue()
{
	CurrentFenceValue++;

	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), CurrentFenceValue));

	if (Fence->GetCompletedValue() < CurrentFenceValue)
	{
		HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);

		ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFenceValue, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void MD3D12CommandContext::EndFrame()
{
	DescriptorCache->Reset();
}

