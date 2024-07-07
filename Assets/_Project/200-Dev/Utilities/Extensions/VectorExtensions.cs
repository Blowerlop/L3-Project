using UnityEngine;

namespace _Project._200_Dev.Utilities.Extensions
{
    public enum EAxis
    {
        X,
        Y,
        Z
    }
    
    public static class VectorExtensions
    {
        public static Vector3 ResetAxis(this Vector3 vector3, EAxis axis)
        {
            switch (axis)
            {
                case EAxis.X:
                    vector3.x = 0.0f;
                    break;
                
                case EAxis.Y:
                    vector3.y = 0.0f;
                    break;
                
                case EAxis.Z:
                    vector3.z = 0.0f;
                    break;
            }

            return vector3;
        }
    }
}