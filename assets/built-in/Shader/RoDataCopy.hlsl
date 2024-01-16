StructuredBuffer<float4x4> RoWorldMatrixUploadBuffer : register(t0, space0);
StructuredBuffer<uint> RoWorldMatrixIndexBuffer : register(t1, space0);
RWStructuredBuffer<float4x4> RoWorldMatrixDataBuffer : register(u2, space0);

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