using Unity.Netcode;

namespace Project.Spells
{
    public struct EmptyResults : ICastResult
    {
        public override string ToString()
        {
            return "EmptyResults";
        }

        public void NetworkSerialize<T>(BufferSerializer<T> serializer) where T : IReaderWriter { }
    }
}