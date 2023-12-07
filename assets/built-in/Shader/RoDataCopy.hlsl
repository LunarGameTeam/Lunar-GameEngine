StructuredBuffer<float4x4> RoWorldMatrixUploadBuffer : register(t0, VIEW_SPACE0);
StructuredBuffer<uint> RoWorldMatrixIndexBuffer : register(t1, VIEW_SPACE0);
RWStructuredBuffer<float4> RoWorldMatrixDataBuffer : register(t2, VIEW_SPACE0);

[numthreads(64, 1, 1)]
void CSMain(uint threadIndex : SV_DispatchThreadID)
{
    uint copyNum = RoWorldMatrixIndexBuffer[0];
    if(threadIndex >= copyNum)
    {
        return;
    }
    uint copyId = RoWorldMatrixIndexBuffer[threadIndex + 1];
    RoWorldMatrixDataBuffer[copyId] = RoWorldMatrixUploadBuffer[threadIndex];
}