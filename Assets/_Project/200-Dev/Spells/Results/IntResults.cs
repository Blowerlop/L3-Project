using Unity.Netcode;

namespace Project.Spells
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