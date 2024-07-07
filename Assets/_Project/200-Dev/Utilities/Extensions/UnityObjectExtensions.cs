using UnityEngine;

namespace _Project._200_Dev.Utilities.Extensions
{
    public static class UnityObjectExtensions
    {
        public static T IsNull<T>(this T @object) where T : Object
        {
            return @object == null ? null : @object;
        }
    }
}