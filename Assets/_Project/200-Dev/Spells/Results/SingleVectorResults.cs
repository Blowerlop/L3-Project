using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
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