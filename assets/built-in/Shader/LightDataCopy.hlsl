StructuredBuffer<float4> PointBasedLightUploadBUffer : register(t0, space0);
StructuredBuffer<uint> PointBasedLightIndexBUffer : register(t1, space0);
RWStructuredBuffer<float4> PointBasedLightDataBUffer : register(u2, space0);

[numthreads(64, 1, 1)]
void CSMain(uint threadIndex : SV_DispatchThreadID)
{
    uint copyNum = PointBasedLightIndexBUffer[0];
    if(threadIndex >= copyNum)
    {
        return;
    }
    uint copyId = PointBasedLightIndexBUffer[threadIndex + 1];
    PointBasedLightDataBUffer[copyId] = PointBasedLightUploadBUffer[threadIndex];
}