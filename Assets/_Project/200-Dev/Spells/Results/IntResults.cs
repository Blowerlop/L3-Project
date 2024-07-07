using Unity.Netcode;

namespace _Project._200_Dev.Spells.Results
{
    public struct IntResults : ICastResult
    {
        public int IntProp;

        public void NetworkSerialize<T>(BufferSerializer<T> serializer) where T : IReaderWriter
        {
            serializer.SerializeValue(ref IntProp);
        }
    }
}