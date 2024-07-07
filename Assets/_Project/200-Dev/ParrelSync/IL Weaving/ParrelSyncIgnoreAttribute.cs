using System;
using System.Diagnostics;

namespace _Project._200_Dev.ParrelSync.IL_Weaving
{
    [Conditional("UNITY_EDITOR")]
    [AttributeUsage(AttributeTargets.Method)]
    public class ParrelSyncIgnoreAttribute : Attribute
    {
    }
}
