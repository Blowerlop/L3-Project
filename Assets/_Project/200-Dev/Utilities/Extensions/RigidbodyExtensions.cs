using UnityEngine;

namespace _Project._200_Dev.Utilities.Extensions
{
    public static class RigidbodyExtensions
    {
        public static void ResetVelocities(this Rigidbody rigidbody)
        {
            rigidbody.linearVelocity = Vector3.zero;
            rigidbody.angularVelocity = Vector3.zero;
        }
    }
}