using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Spells.Results
{
    public struct SingleVectorResults : ICastResult
    {
        public Vector3 VectorProp;
        
        public override string ToString()
        {
            return $"SingleVectorResults: VectorProp: {VectorProp}";
        }

        public void NetworkSerialize<T>(BufferSerializer<T> serializer) where T : IReaderWriter
        {
            serializer.SerializeValue(ref VectorProp);
        }
    }
}