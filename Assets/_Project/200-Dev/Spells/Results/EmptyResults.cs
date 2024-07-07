using Unity.Netcode;

namespace _Project._200_Dev.Spells.Results
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